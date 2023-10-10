#pragma once

// Copyright 2014 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "overlay/FlowControl.h"
#include "overlay/Peer.h"
#include <deque>
#include <random>

/*
Another peer out there that we are connected to
*/

namespace stellar
{
// [testing] Peer that communicates via byte-buffer delivery events queued in
// in-process io_contexts.
//
// NB: Do not construct one of these directly; instead, construct a connected
// pair of them wrapped in a LoopbackPeerConnection that explicitly manages the
// lifecycle of the connection.

class LoopbackPeer : public Peer
{
  private:
    std::weak_ptr<LoopbackPeer> mRemote;
    std::deque<TimestampedMessage> mOutQueue; // sending queue
    std::queue<xdr::msg_ptr> mInQueue;        // receiving queue

    bool mCorked{false};
    bool mStraggling{false};
    size_t mMaxQueueDepth{0};

    bool mDamageCert{false};
    bool mDamageAuth{false};
    std::bernoulli_distribution mDuplicateProb{0.0};
    std::bernoulli_distribution mReorderProb{0.0};
    std::bernoulli_distribution mDamageProb{0.0};
    std::bernoulli_distribution mDropProb{0.0};

    struct Stats
    {
        size_t messagesDuplicated{0};
        size_t messagesReordered{0};
        size_t messagesDamaged{0};
        size_t messagesDropped{0};

        size_t bytesDelivered{0};
        size_t messagesDelivered{0};
    };

    Stats mStats;

    void sendMessage(xdr::msg_ptr&& xdrBytes) override;
    AuthCert getAuthCert() override;

    void processInQueue();
    void recvMessage(xdr::msg_ptr const& xdrBytes);

    std::string mDropReason;

  public:
    virtual ~LoopbackPeer()
    {
    }
    LoopbackPeer(Application& app, PeerRole role);

    static std::pair<std::shared_ptr<LoopbackPeer>,
                     std::shared_ptr<LoopbackPeer>>
    initiate(Application& app, Application& otherApp);

    void drop(std::string const& reason, DropDirection dropDirection,
              DropMode dropMode) override;

    void deliverOne();
    void deliverAll();
    void dropAll();
    size_t getBytesQueued() const;
    size_t getMessagesQueued() const;

    virtual void scheduleRead() override;

    Stats const& getStats() const;

    bool getCorked() const;
    void setCorked(bool c);

    bool getStraggling() const;
    void setStraggling(bool s);

    size_t getMaxQueueDepth() const;
    void setMaxQueueDepth(size_t sz);

    double getDamageProbability() const;
    void setDamageProbability(double d);

    bool getDamageCert() const;
    void setDamageCert(bool d);

    bool getDamageAuth() const;
    void setDamageAuth(bool d);

    double getDropProbability() const;
    void setDropProbability(double d);

    double getDuplicateProbability() const;
    void setDuplicateProbability(double d);

    double getReorderProbability() const;
    void setReorderProbability(double d);

    void clearInAndOutQueues();

    size_t
    getTxQueueByteCount() const
    {
        return mFlowControl->getTxQueueByteCountForTesting();
    }

    std::string
    getDropReason() const
    {
        return mDropReason;
    }

    std::array<std::deque<FlowControl::QueuedOutboundMessage>, 4>&
    getQueues()
    {
        return getFlowControl()->getQueuesForTesting();
    }

    uint64_t
    getOutboundCapacity()
    {
        return getFlowControl()->getCapacity()->getOutboundCapacity();
    }

    bool checkCapacity(std::shared_ptr<LoopbackPeer> otherPeer) const;

    std::string getIP() const override;

    using Peer::recvMessage;
    using Peer::sendAuth;
    using Peer::sendAuthenticatedMessage;
    using Peer::sendMessage;

    friend class LoopbackPeerConnection;
};

/**
 * Testing class for managing a simulated network connection between two
 * LoopbackPeers.
 */
class LoopbackPeerConnection
{
    std::shared_ptr<LoopbackPeer> mInitiator;
    std::shared_ptr<LoopbackPeer> mAcceptor;

  public:
    LoopbackPeerConnection(Application& initiator, Application& acceptor);
    ~LoopbackPeerConnection();
    std::shared_ptr<LoopbackPeer> getInitiator() const;
    std::shared_ptr<LoopbackPeer> getAcceptor() const;
};
}
