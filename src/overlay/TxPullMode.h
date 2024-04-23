#pragma once

// Copyright 2022 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "util/HashOfHash.h"
#include "util/RandomEvictionCache.h"
#include "xdr/Stellar-overlay.h"

namespace stellar
{

class Peer;
class Application;

// TxPullMode class stores and properly trims incoming advertised transaction
// hashes, and maintains which hashes to retry demanding. In addition, this
// class is responsible for flushing batches of adverts periodically.
//
// When looking for the next tx hash to try,
// we first check the first element in the retry queue. If the retry
// queue is empty, then we look at mIncomingTxHashes and pop the first element.
// Both mIncomingTxHashes and mTxHashesToRetry are FIFO.

class TxPullMode
{
  private:
    Application& mApp;

    std::deque<std::pair<Hash, VirtualClock::time_point>> mIncomingTxHashes;
    std::list<Hash> mTxHashesToRetry;

    // Cache seen hashes for a bit to avoid re-broadcasting the same data
    // transaction hash -> ledger number
    RandomEvictionCache<Hash, uint32_t> mAdvertHistory;
    TxAdvertVector mOutgoingTxHashes;
    VirtualTimer mAdvertTimer;
    std::weak_ptr<Peer> mWeakPeer;

    void rememberHash(Hash const& hash, uint32_t ledgerSeq);
    void flushAdvert();
    void startAdvertTimer();

  public:
    TxPullMode(Application& app, std::weak_ptr<Peer> peer);

    // Total transaction hashes to process including demand retries.
    size_t size() const;
    // Pop the next advert hash to process, size() must be > 0
    std::pair<Hash, std::optional<VirtualClock::time_point>>
    popIncomingAdvert();
    // Queue up a transaction hash to advertise to neighbours
    void queueOutgoingAdvert(Hash const& txHash);
    // Queue up a transaction hash from a neighbour to try demanding
    void queueIncomingAdvert(TxAdvertVector const& hash, uint32_t seq);
    // Queue up transaction hashes to retry demanding. Note: `list` becomes
    // empty after this functions is called
    void retryIncomingAdvert(std::list<Hash>& list);
    // Compute maximum number of hashes in an advert based on network limits
    size_t getMaxAdvertSize() const;

    bool seenAdvert(Hash const& hash);
    void clearBelow(uint32_t ledgerSeq);
    void shutdown();

#ifdef BUILD_TESTS
    size_t
    outgoingSize() const
    {
        return mOutgoingTxHashes.size();
    }
#endif

    static int64_t getOpsFloodLedger(size_t maxOps, double rate);
};
}
