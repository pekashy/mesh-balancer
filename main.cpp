#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <folly/SocketAddress.h>
#include <Logger.h>
#include "Server/RequestHanlerFactory.h"
#include <Config.h>

proxygen::HTTPServerOptions CreateServerOptions(balancer::Redirector& redirector, const Config& config) {
  proxygen::HTTPServerOptions options;
  options.supportsConnect = true;
  options.threads = static_cast<size_t>(config.nThreads);
  options.shutdownOn = {SIGINT, SIGTERM};
  options.handlerFactories =
      proxygen::RequestHandlerChain().addThen<server::RequestHandlerFactory>(redirector, config).build();
  return options;
}

int parseLoglevel(char* argp) {
  std::string arg = argp;
  int loglevel = 3;
  try {
    std::size_t pos;
    loglevel = std::stoi(arg, &pos);
    if (pos < arg.size()) {
      std::cerr << "Trailing characters after number: " << arg << '\n';
    }
    if (loglevel < 0 || loglevel > 5) {
      throw std::runtime_error("Invalid loglevel");
    }
  } catch (std::invalid_argument const& ex) {
    throw std::runtime_error("Invalid number");
  } catch (std::out_of_range const& ex) {
    throw std::runtime_error("Invalid loglevel");
  }
  return loglevel;
}

int main(int argc, char* argv[]) {
  using namespace proxygen;
  using namespace folly;
  if (argc < 3) {
    return -1;
  }
  int loglevel = 3;
  try {
    loglevel = parseLoglevel(argv[2]);
  } catch (std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    return -1;
  }

  auto logger = LoggerContainer::Get(loglevel);
  logger.info("Started!");

  std::unique_ptr<Config> config;
  try {
    config = std::make_unique<Config>((std::string(argv[1])));
  } catch (std::runtime_error& e) {
    logger.error(e.what());
    return 1;
  }

  std::vector<HTTPServer::IPConfig> IPs = {
      {SocketAddress(config->ip, config->redirectorPort, true), HTTPServer::Protocol::HTTP}, {
          SocketAddress(config->ip, config->recorderPort, true), HTTPServer::Protocol::HTTP},};
  balancer::Redirector redirector(config->initialHostsETAs, config->hosts, config->windowSize);
  auto options = CreateServerOptions(redirector, *config);
  HTTPServer server(std::move(options));
  server.bind(IPs);
  std::thread t([&]() { server.start(); });
  t.join();
}
