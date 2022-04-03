#pragma once
#include <proxygen/lib/http/session/HTTPTransaction.h>

namespace Server {
class RequestHandler;

class TransactionHandler : public proxygen::HTTPTransactionHandler {
 public:
	explicit TransactionHandler(RequestHandler &parent);
	void setTransaction(proxygen::HTTPTransaction *txn) noexcept override;
	void detachTransaction() noexcept override;
	void onHeadersComplete(std::unique_ptr<proxygen::HTTPMessage> msg) noexcept override;
	void onBody(std::unique_ptr<folly::IOBuf> chain) noexcept override;
	void onTrailers(std::unique_ptr<proxygen::HTTPHeaders> trailers) noexcept override;
	void onEOM() noexcept override;
	void onUpgrade(proxygen::UpgradeProtocol protocol) noexcept override;
	void onError(const proxygen::HTTPException &error) noexcept override;
	void onEgressPaused() noexcept override;
	void onEgressResumed() noexcept override;
 private:
	RequestHandler &parent;
};
}
