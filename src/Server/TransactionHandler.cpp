#include <Server/TransactionHandler.h>
#include <Server/RequestHandler.h>

namespace Server {

TransactionHandler::TransactionHandler(RequestHandler &parent) : parent(parent) {
}

void TransactionHandler::setTransaction(proxygen::HTTPTransaction *txn) noexcept {

}

void TransactionHandler::detachTransaction() noexcept {

}

void TransactionHandler::onHeadersComplete(std::unique_ptr<proxygen::HTTPMessage> msg) noexcept {

}

void TransactionHandler::onBody(std::unique_ptr<folly::IOBuf> chain) noexcept {

}

void TransactionHandler::onTrailers(std::unique_ptr<proxygen::HTTPHeaders> trailers) noexcept {

}

void TransactionHandler::onEOM() noexcept {

}

void TransactionHandler::onUpgrade(proxygen::UpgradeProtocol protocol) noexcept {

}

void TransactionHandler::onError(const proxygen::HTTPException &error) noexcept {

}

void TransactionHandler::onEgressPaused() noexcept {

}

void TransactionHandler::onEgressResumed() noexcept {

}
}
