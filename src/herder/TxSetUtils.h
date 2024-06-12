// Copyright 2022 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "herder/TxSetFrame.h"
#include "util/UnorderedMap.h"
#include "xdr/Stellar-types.h"
#include <ledger/LedgerHashUtils.h>
#include <tuple>

namespace stellar
{

class AccountTransactionQueue
{
  public:
    AccountTransactionQueue(
        std::vector<TransactionFrameBasePtr> const& accountTxs);

    TransactionFrameBasePtr getTopTx() const;
    bool empty() const;
    void popTopTx();

  private:
    std::deque<TransactionFrameBasePtr> mTxs;
    uint32_t mNumOperations = 0;
};

class TxSetUtils
{
  public:
    static bool hashTxSorter(TransactionFrameBasePtr const& tx1,
                             TransactionFrameBasePtr const& tx2);

    static TxSetTransactions
    sortTxsInHashOrder(TxSetTransactions const& transactions);

    static std::vector<std::shared_ptr<AccountTransactionQueue>>
    buildAccountTxQueues(TxSetTransactions const& txs);

    // Returns transactions from a TxSet that are invalid. If
    // returnEarlyOnFirstInvalidTx is true, return immediately if an invalid
    // transaction is found (instead of finding all of them), this is useful for
    // checking if a TxSet is valid.
    static TxSetTransactions
    getInvalidTxList(TxSetTransactions const& txs, Application& app,
                     uint64_t lowerBoundCloseTimeOffset,
                     uint64_t upperBoundCloseTimeOffset);

    static TxSetTransactions trimInvalid(TxSetTransactions const& txs,
                                         Application& app,
                                         uint64_t lowerBoundCloseTimeOffset,
                                         uint64_t upperBoundCloseTimeOffset,
                                         TxSetTransactions& invalidTxs);
}; // class TxSetUtils
} // namespace stellar
