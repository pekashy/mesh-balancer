
#include <memory>
#include <proxygen/lib/http/HTTPConstants.h>
#include <Server/RequestHandler.h>
#include <proxygen/lib/utils/URL.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <glog/logging.h>

namespace server {
RequestHandler::RequestHandler(folly::HHWheelTimer *timer, balancer::Redirector &redir)
		: serverHandler(*this), redirector(redir) {
	LOG(INFO) << "Created request handler";
}

void RequestHandler::onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
	request = std::move(headers);
	proxygen::URL url(request->getURL());

	folly::SocketAddress addr;
	std::string redirectionURL = redirector.GetNextRedirectURL();
	LOG(INFO) << "Redirecting to " << redirectionURL;

	proxygen::ResponseBuilder(downstream_)
			.status(307, "Balanced")
			.header<std::string>("Location", redirectionURL)
			.sendWithEOM();
}
void RequestHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {

}

void RequestHandler::onUpgrade(proxygen::UpgradeProtocol prot) noexcept {

}

void RequestHandler::onEOM() noexcept {

}

void RequestHandler::requestComplete() noexcept {

}

void RequestHandler::onError(proxygen::ProxygenError err) noexcept {

}

void RequestHandlerFactory::onServerStart(folly::EventBase *evb) noexcept {

}

void RequestHandlerFactory::onServerStop() noexcept {

}

RequestHandlerFactory::RequestHandlerFactory(balancer::Redirector &redir)
		: redirector(redir) {
}

proxygen::RequestHandler *RequestHandlerFactory::onRequest(proxygen::RequestHandler *handler,
																													 proxygen::HTTPMessage *message) noexcept {
	return new RequestHandler(timer->timer.get(), redirector);
}
}
