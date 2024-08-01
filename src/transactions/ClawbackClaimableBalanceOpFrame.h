#pragma once

// Copyright 2021 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "transactions/OperationFrame.h"

namespace stellar
{

class AbstractLedgerTxn;

class ClawbackClaimableBalanceOpFrame : public OperationFrame
{
    ClawbackClaimableBalanceResult&
    innerResult(OperationResult& res) const
    {
        return res.tr().clawbackClaimableBalanceResult();
    }

    ClawbackClaimableBalanceOp const& mClawbackClaimableBalance;

  public:
    ClawbackClaimableBalanceOpFrame(Operation const& op,
                                    TransactionFrame const& parentTx);

    bool isOpSupported(LedgerHeader const& header) const override;

    bool doApply(Application& app, AbstractLedgerTxn& ltx,
                 Hash const& sorobanBasePrngSeed, OperationResult& res,
                 std::shared_ptr<SorobanTxData> sorobanData) const override;
    bool doCheckValid(uint32_t ledgerVersion,
                      OperationResult& res) const override;
    void
    insertLedgerKeysToPrefetch(UnorderedSet<LedgerKey>& keys) const override;

    static ClawbackClaimableBalanceResultCode
    getInnerCode(OperationResult const& res)
    {
        return res.tr().clawbackClaimableBalanceResult().code();
    }
};
}
