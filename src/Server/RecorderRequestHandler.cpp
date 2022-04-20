#include "Server/RequestHanlerFactory.h"
#include <proxygen/lib/http/HTTPConstants.h>
#include <proxygen/lib/utils/URL.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <Logger.h>
#include <memory>

namespace server {
class RecorderRequestHandler
		: public proxygen::RequestHandler {
 public:
	explicit RecorderRequestHandler(folly::HHWheelTimer *timer, balancer::Redirector &redir);
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
	balancer::Redirector &redirector;
};

RecorderRequestHandler::RecorderRequestHandler(folly::HHWheelTimer *timer, balancer::Redirector &redir)
		: serverHandler(*this), redirector(redir), logger(LoggerContainer::Get()) {
}

void RecorderRequestHandler::onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
	request = std::move(headers);
	proxygen::URL url(request->getURL());
	logger.info("Recorded: " + request->getURL());
	auto head = request->getHeaders();
	head.forEach([&] (const std::string& header, const std::string& val) {
		logger.debug(request->getURL() + "| " + header + ": " + val);
	});
	proxygen::ResponseBuilder(downstream_)
			.status(200, "Recorded")
			.sendWithEOM();
}
void RecorderRequestHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {

}

void RecorderRequestHandler::onUpgrade(proxygen::UpgradeProtocol prot) noexcept {

}

void RecorderRequestHandler::onEOM() noexcept {

}

void RecorderRequestHandler::requestComplete() noexcept {

}

void RecorderRequestHandler::onError(proxygen::ProxygenError err) noexcept {

}


proxygen::RequestHandler* RequestHandlerFactory::createRecorderRequestHandler(proxygen::RequestHandler *handler,
																											 proxygen::HTTPMessage *message) noexcept {
	return new RecorderRequestHandler(timer->timer.get(), redirector);
}

}