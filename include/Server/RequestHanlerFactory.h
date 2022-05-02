#pragma once
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/lib/http/HTTPConnector.h>
#include "TransactionHandler.h"
#include <Balancer/Redirector.h>
#include <spdlog/logger.h>
#include <Config.h>

namespace server {
class RequestHandlerFactory : public proxygen::RequestHandlerFactory {
 public:
	RequestHandlerFactory(balancer::Redirector& redirector, const Config& config);
	void onServerStart(folly::EventBase* evb) noexcept override;
	void onServerStop() noexcept override;
	proxygen::RequestHandler* onRequest(proxygen::RequestHandler* handler,
																			proxygen::HTTPMessage* message) noexcept override;
 private:
	proxygen::RequestHandler* createRecorderRequestHandler(proxygen::RequestHandler* handler,
																												 proxygen::HTTPMessage* message) noexcept;
	proxygen::RequestHandler* createBalancerRequestHandler(proxygen::RequestHandler* handler,
																												 proxygen::HTTPMessage* message) noexcept;
	struct TimerWrapper {
		folly::HHWheelTimer::UniquePtr timer;
	};
	folly::ThreadLocal<TimerWrapper> timer;
	balancer::Redirector& redirector;
	Config config;
};

}
