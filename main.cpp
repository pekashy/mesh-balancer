//
// Created by pekashy on 21.02.2022.
//
#include <folly/io/async/EventBaseManager.h>
#include <folly/portability/GFlags.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <folly/portability/GFlags.h>
#include <folly/SocketAddress.h>

DEFINE_int32(http_port, 11001, "Port to listen on with HTTP protocol");
DEFINE_string(ip, "localhost", "IP/Hostname to bind to");
DEFINE_int32(threads,
	0,
	"Number of threads to listen on. Numbers <= 0 "
	"will use the number of cores on this machine.");


int main(int argc, char* argv[]) {
	using namespace proxygen;
	using namespace folly;

	FLAGS_log_dir = "../log";
	google::InitGoogleLogging(argv[0]);
	HTTPServerOptions options;
	options.supportsConnect = true;
	options.threads = static_cast<size_t>(FLAGS_threads);
	options.shutdownOn = {SIGINT, SIGTERM};

	std::vector<HTTPServer::IPConfig> IPs = {
		{SocketAddress(FLAGS_ip, FLAGS_http_port, true), HTTPServer::Protocol::HTTP},
	};
	HTTPServer server(std::move(options));
	server.bind(IPs);

	// Start HTTPServer mainloop in a separate thread
	std::thread t([&]() { server.start(); });

	t.join();
}