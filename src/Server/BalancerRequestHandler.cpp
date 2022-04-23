#include "Server/RequestHanlerFactory.h"
#include <proxygen/lib/http/HTTPConstants.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <Logger.h>
#include <memory>

namespace server {
class BalancerRequestHandler
		: public proxygen::RequestHandler {
 public:
	explicit BalancerRequestHandler(folly::HHWheelTimer* timer, balancer::Redirector& redir);
	void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;
	void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;
	void onUpgrade(proxygen::UpgradeProtocol prot) noexcept override;
	void onEOM() noexcept override;
	void requestComplete() noexcept override;
	void onError(proxygen::ProxygenError err) noexcept override;
 private:
	void logRequest(const std::string& redirectionURL, const std::string& requestID) const;

	spdlog::logger& logger;
	TransactionHandler serverHandler;
	std::unique_ptr<proxygen::HTTPMessage> request;
	balancer::Redirector& redirector;
};

BalancerRequestHandler::BalancerRequestHandler(folly::HHWheelTimer* timer, balancer::Redirector& redir)
		: serverHandler(*this), redirector(redir), logger(LoggerContainer::Get()) {
}

void BalancerRequestHandler::logRequest(const std::string& redirectionURL, const std::string& requestID) const {
	logger.debug(redirectionURL);
	logger.debug("Method String: " + request->getMethodString());
	logger.debug("Path: " + request->getPath());
	logger.debug("Query String: " + request->getQueryString());
	logger.debug("Dst Port: " + request->getDstPort());
	auto head = request->getHeaders();
	head.forEach([&](const std::string& header, const std::string& val) {
		logger.debug(redirectionURL + "| " + header + ": " + val);
	});
}

void BalancerRequestHandler::onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
	request = std::move(headers);
	auto requestID = request->getHeaders().getSingleOrEmpty<std::string>("x-request-id");

	if (requestID.empty()) {
		proxygen::ResponseBuilder(downstream_)
				.status(400, "Request ID not found")
				.sendWithEOM();
		return;
	}
	std::string redirectionURL = redirector.getNextRedirectURL(requestID, request->getPath());
	logRequest(redirectionURL, requestID);

	proxygen::ResponseBuilder(downstream_)
			.status(302, "Balanced")
			.header<std::string>("Location", redirectionURL)
			.sendWithEOM();
}
void BalancerRequestHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {

}

void BalancerRequestHandler::onUpgrade(proxygen::UpgradeProtocol prot) noexcept {

}

void BalancerRequestHandler::onEOM() noexcept {

}

void BalancerRequestHandler::requestComplete() noexcept {

}

void BalancerRequestHandler::onError(proxygen::ProxygenError err) noexcept {

}

proxygen::RequestHandler* RequestHandlerFactory::createBalancerRequestHandler(proxygen::RequestHandler* handler,
																																							proxygen::HTTPMessage* message) noexcept {
	return new BalancerRequestHandler(timer->timer.get(), redirector);
}
}
