#pragma once

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/lib/http/HTTPConnector.h>
#include "TransactionHandler.h"
#include <Balancer/Redirector.h>
#include <spdlog/logger.h>

namespace server {
class RequestHandlerFactory : public proxygen::RequestHandlerFactory {
 public:
	RequestHandlerFactory(balancer::Redirector &redirector);
	void onServerStart(folly::EventBase *evb) noexcept override;
	void onServerStop() noexcept override;
	proxygen::RequestHandler *onRequest(proxygen::RequestHandler *handler,
																			proxygen::HTTPMessage *message) noexcept override;
 private:
	struct TimerWrapper {
		folly::HHWheelTimer::UniquePtr timer;
	};
	folly::ThreadLocal<TimerWrapper> timer;
	balancer::Redirector &redirector;
};

class RequestHandler
		: public proxygen::RequestHandler {
 public:
	explicit RequestHandler(folly::HHWheelTimer *timer, balancer::Redirector &redir);
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
}
