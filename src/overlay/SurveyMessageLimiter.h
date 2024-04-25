#pragma once

// Copyright 2019 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "overlay/StellarXDR.h" // IWYU pragma: keep
#include "util/UnorderedMap.h"
#include <functional>
#include <map>

namespace stellar
{
class Application;

/*
The SurveyMessageLimiter module manages survey message traffic through specific
filtering policies:
 * It validates ledger numbers of survey messages, restricting messages to a
predefined ledger range to maintain relevance.
  * It enforces a cap on the number of survey requests a node can handle (via
`mMaxRequestLimit`)
  * It implements duplication checks for `Surveyor-Surveyed` pairs
*/

class SurveyMessageLimiter
{
  public:
    SurveyMessageLimiter(Application& app, uint32_t numLedgersBeforeIgnore,
                         uint32_t maxRequestLimit);

    // we pass in validation functions that are run if the rate limiter
    // determines the message is valid. We do this so signatures (an expensive
    // task) will only be validated if we know that we aren't going to throw the
    // message away
    bool addAndValidateRequest(SurveyRequestMessage const& request,
                               std::function<bool()> onSuccessValidation);
    bool recordAndValidateResponse(SurveyResponseMessage const& response,
                                   std::function<bool()> onSuccessValidation);
    void clearOldLedgers(uint32_t lastClosedledgerSeq);

  private:
    bool surveyLedgerNumValid(uint32_t ledgerNum);

    typedef UnorderedMap<NodeID /*surveyedNodeId*/, bool /*responseSeen*/>
        SurveyedMap;
    typedef UnorderedMap<NodeID /*surveyorNodeId*/, SurveyedMap> SurveyorMap;

    std::map<uint32_t /*ledgerNum*/, SurveyorMap> mRecordMap;

    // We filter survey messages out if the difference between
    // currentLedgerNum and ledgerNum on the message is greater than this number
    uint32_t const mNumLedgersBeforeIgnore;

    // Number of requests we allow for a (surveyorNodeId, ledger) pair before we
    // start rate limiting
    uint32_t const mMaxRequestLimit;

    Application& mApp;
};
}
