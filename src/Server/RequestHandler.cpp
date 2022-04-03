
#include <memory>
#include <proxygen/lib/http/HTTPConstants.h>
#include <Server/RequestHandler.h>
#include <proxygen/lib/utils/URL.h>
#include <proxygen/httpserver/ResponseBuilder.h>

namespace Server {
RequestHandler::RequestHandler(folly::HHWheelTimer *timer)
		: serverHandler(*this) {
}

void RequestHandler::onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
	request = std::move(headers);
	proxygen::URL url(request->getURL());

	folly::SocketAddress addr;
	LOG(INFO) << "Redirecting to 0.0.0.0:8001";
	proxygen::ResponseBuilder(downstream_)
			.status(307, "Balanced")
			.header<std::string>("Location", "http://0.0.0.0:30000/bench")
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

proxygen::RequestHandler *RequestHandlerFactory::onRequest(proxygen::RequestHandler *handler,
																													 proxygen::HTTPMessage *message) noexcept {
	return new RequestHandler(timer->timer.get());
}
}
