#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <folly/SocketAddress.h>
#include <Logger.h>
#include "Server/RequestHanlerFactory.h"
#include "Server/Params.h"

proxygen::HTTPServerOptions CreateServerOptions(balancer::Redirector& redirector) {
	proxygen::HTTPServerOptions options;
	options.supportsConnect = true;
	options.threads = static_cast<size_t>(THREADS);
	options.shutdownOn = {SIGINT, SIGTERM};
	options.handlerFactories =
			proxygen::RequestHandlerChain().addThen<server::RequestHandlerFactory>(redirector).build();
	return options;
}

int main(int argc, char* argv[]) {
	using namespace proxygen;
	using namespace folly;
	auto logger = LoggerContainer::Get();
	logger.info("Started!");
	std::vector<HTTPServer::IPConfig> IPs = {
			{SocketAddress(SERVER_IP, HTTP_PORT_REDIRECT, true), HTTPServer::Protocol::HTTP}, {
					SocketAddress(SERVER_IP, HTTP_PORT_RECORD, true), HTTPServer::Protocol::HTTP},};
	balancer::Redirector redirector
			({{"http://0.0.0.0:30001", 0}, {"http://0.0.0.0:30002", 0}, {"http://0.0.0.0:30003", 0}, {"http://0.0.0.0:30004"
																																																, 0}},
			 {{"http://0.0.0.0:30001"}, {"http://0.0.0.0:30002"}, {"http://0.0.0.0:30003"}, {"http://0.0.0.0:30004"}},
			 100);
	auto options = CreateServerOptions(redirector);
	HTTPServer server(std::move(options));
	server.bind(IPs);
	// Start HTTPServer mainloop in a separate thread
	std::thread t([&]() { server.start(); });

	t.join();
}