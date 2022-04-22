#include "Server/RequestHanlerFactory.h"
#include <proxygen/lib/http/HTTPConstants.h>
#include <proxygen/lib/utils/URL.h>
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
	spdlog::logger& logger;
	TransactionHandler serverHandler;
	std::unique_ptr<proxygen::HTTPMessage> request;
	balancer::Redirector& redirector;
};

BalancerRequestHandler::BalancerRequestHandler(folly::HHWheelTimer* timer, balancer::Redirector& redir)
		: serverHandler(*this), redirector(redir), logger(LoggerContainer::Get()) {
}

void BalancerRequestHandler::onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
	request = std::move(headers);
	proxygen::URL url(request->getURL());
	folly::SocketAddress addr;
	std::string redirectionURL = redirector.getNextRedirectURL(request->getDstIP(), request->getPath());
	logger.info(redirectionURL);
	logger.info("Method String: " + request->getMethodString());
	logger.info("Path: " + request->getPath());
	logger.info("Query String: " + request->getQueryString());
	logger.info("Dst Port: " + request->getDstPort());
	auto head = request->getHeaders();
	head.forEach([&](const std::string& header, const std::string& val) {
		logger.debug(redirectionURL + "| " + header + ": " + val);
	});
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
