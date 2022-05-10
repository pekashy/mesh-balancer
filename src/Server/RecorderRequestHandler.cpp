#include "Server/RequestHanlerFactory.h"
#include <proxygen/lib/http/HTTPConstants.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <Logger.h>
#include <memory>

namespace server {
class RecorderRequestHandler
    : public proxygen::RequestHandler {
 public:
  explicit RecorderRequestHandler(folly::HHWheelTimer* timer, balancer::Redirector& redir);
  void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;
  void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;
  void onUpgrade(proxygen::UpgradeProtocol prot) noexcept override;
  void onEOM() noexcept override;
  void requestComplete() noexcept override;
  void onError(proxygen::ProxygenError err) noexcept override;
 private:
  void logRequest(const std::string& redirectionURL) const;

  TransactionHandler serverHandler;
  std::unique_ptr<proxygen::HTTPMessage> request;
  balancer::Redirector& redirector;
  spdlog::logger& logger;
};

RecorderRequestHandler::RecorderRequestHandler(folly::HHWheelTimer* timer, balancer::Redirector& redir)
    : serverHandler(*this), redirector(redir), logger(LoggerContainer::Get()) {
}

void RecorderRequestHandler::logRequest(const std::string& requestID) const {
  logger.debug(requestID);
  logger.debug("Method String: " + request->getMethodString());
  logger.debug("Path: " + request->getPath());
  logger.debug("Query String: " + request->getQueryString());
  logger.debug("Dst Port: " + request->getDstPort());
  auto head = request->getHeaders();
  head.forEach([&](const std::string& header, const std::string& val) {
    logger.debug(requestID + " | " + header + ": " + val);
  });
}

void RecorderRequestHandler::onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
  request = std::move(headers);
  auto requestID = request->getHeaders().getSingleOrEmpty<std::string>("x-request-id");

  if (requestID.empty()) {
    proxygen::ResponseBuilder(downstream_)
        .status(400, "Request ID not found")
        .sendWithEOM();
    return;
  }
  logRequest(requestID);
  redirector.finishRequest(requestID);

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

proxygen::RequestHandler* RequestHandlerFactory::createRecorderRequestHandler(proxygen::RequestHandler* handler,
                                                                              proxygen::HTTPMessage* message) noexcept {
  return new RecorderRequestHandler(timer->timer.get(), redirector);
}

}