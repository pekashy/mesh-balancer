#include "Server/RequestHanlerFactory.h"
#include "Server/Params.h"
#include <Logger.h>

namespace server {
RequestHandlerFactory::RequestHandlerFactory(balancer::Redirector &redir)
		: redirector(redir) {
}

void RequestHandlerFactory::onServerStart(folly::EventBase *evb) noexcept {

}

void RequestHandlerFactory::onServerStop() noexcept {

}

proxygen::RequestHandler *RequestHandlerFactory::onRequest(proxygen::RequestHandler *handler,
																													 proxygen::HTTPMessage *message) noexcept {
	LoggerContainer::Get().info(message->getDstPort());
	auto dstPort = std::stoi(message->getDstPort());
	if(dstPort == HTTP_PORT_RECORD) {
		return createRecorderRequestHandler(handler, message);
	}
	return createBalancerRequestHandler(handler, message);
}

}