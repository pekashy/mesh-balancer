#include <folly/io/async/EventBaseManager.h>
#include <folly/portability/GFlags.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <folly/SocketAddress.h>
#include <Server/RequestHandler.h>
#include <Logger.h>

DEFINE_int32(http_port, 11002, "");
DEFINE_string(ip, "0.0.0.0", "");
DEFINE_int32(threads,
						 0,
						 "");

proxygen::HTTPServerOptions CreateServerOptions(balancer::Redirector& redirector) {
	proxygen::HTTPServerOptions options;
	options.supportsConnect = true;
	options.threads = static_cast<size_t>(FLAGS_threads);
	options.shutdownOn = {SIGINT, SIGTERM};
	options.handlerFactories =
			proxygen::RequestHandlerChain().addThen<server::RequestHandlerFactory>(redirector).build();
	return options;
}

int main(int argc, char *argv[]) {
	using namespace proxygen;
	using namespace folly;
	auto logger = LoggerContainer::Get();
	logger.info("Started!");
	std::vector<HTTPServer::IPConfig> IPs = {
			{SocketAddress(FLAGS_ip, FLAGS_http_port, true), HTTPServer::Protocol::HTTP},
	};
	balancer::Redirector redirector;
	auto options = CreateServerOptions(redirector);
	HTTPServer server(std::move(options));
	server.bind(IPs);
	// Start HTTPServer mainloop in a separate thread
	std::thread t([&]() { server.start(); });

	t.join();
}