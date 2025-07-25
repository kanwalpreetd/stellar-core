// Copyright 2017 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "util/asio.h"

#include "database/Database.h"
#include "herder/TxSetFrame.h"
#include "invariant/Invariant.h"
#include "invariant/InvariantDoesNotHold.h"
#include "invariant/InvariantManager.h"
#include "ledger/LedgerTxn.h"
#include "ledger/test/LedgerTestUtils.h"
#include "main/Application.h"
#include "test/Catch2.h"
#include "test/TestAccount.h"
#include "test/TestUtils.h"
#include "test/test.h"

#include <fmt/format.h>

using namespace stellar;

namespace InvariantTests
{

class TestInvariant : public Invariant
{
  public:
    TestInvariant(int id, bool shouldFail)
        : Invariant(true), mInvariantID(id), mShouldFail(shouldFail)
    {
    }

    // if id < 0, generate prefix that will match any invariant
    static std::string
    toString(int id, bool fail)
    {
        if (id < 0)
        {
            return fmt::format("TestInvariant{}", fail ? "Fail" : "Succeed");
        }
        else
        {
            return fmt::format("TestInvariant{}{}", fail ? "Fail" : "Succeed",
                               id);
        }
    }

    virtual std::string
    getName() const override
    {
        return toString(mInvariantID, mShouldFail);
    }

    virtual std::string
    checkOnBucketApply(
        std::shared_ptr<LiveBucket const> bucket, uint32_t oldestLedger,
        uint32_t newestLedger,
        std::unordered_set<LedgerKey> const& shadowedKeys) override
    {
        return mShouldFail ? "fail" : "";
    }

    virtual std::string
    checkAfterAssumeState(uint32_t newestLedger) override
    {
        return mShouldFail ? "fail" : "";
    }

    virtual std::string
    checkOnOperationApply(Operation const& operation,
                          OperationResult const& result,
                          LedgerTxnDelta const& ltxDelta,
                          std::vector<ContractEvent> const& events) override
    {
        return mShouldFail ? "fail" : "";
    }

  private:
    int mInvariantID;
    bool mShouldFail;
};
}

using namespace InvariantTests;

TEST_CASE("no duplicate register", "[invariant]")
{
    VirtualClock clock;
    Config cfg = getTestConfig();
    cfg.INVARIANT_CHECKS = {};
    Application::pointer app = createTestApplication(clock, cfg);

    app->getInvariantManager().registerInvariant<TestInvariant>(0, true);
    REQUIRE_THROWS_AS(
        app->getInvariantManager().registerInvariant<TestInvariant>(0, true),
        std::runtime_error);
}

TEST_CASE("no duplicate enable", "[invariant]")
{
    VirtualClock clock;
    Config cfg = getTestConfig();
    cfg.INVARIANT_CHECKS = {};
    Application::pointer app = createTestApplication(clock, cfg);

    app->getInvariantManager().registerInvariant<TestInvariant>(0, true);
    app->getInvariantManager().enableInvariant(
        TestInvariant::toString(0, true));
    REQUIRE_THROWS_AS(app->getInvariantManager().enableInvariant(
                          TestInvariant::toString(0, true)),
                      std::runtime_error);
}

TEST_CASE("only enable registered invariants", "[invariant]")
{
    VirtualClock clock;
    Config cfg = getTestConfig();
    cfg.INVARIANT_CHECKS = {};
    Application::pointer app = createTestApplication(clock, cfg);

    app->getInvariantManager().registerInvariant<TestInvariant>(0, true);
    app->getInvariantManager().enableInvariant(
        TestInvariant::toString(0, true));
    REQUIRE_THROWS_AS(app->getInvariantManager().enableInvariant("WrongName"),
                      std::runtime_error);
}

TEST_CASE("enable registered invariants regex", "[invariant]")
{
    VirtualClock clock;
    Config cfg = getTestConfig();
    cfg.INVARIANT_CHECKS = {};
    Application::pointer app = createTestApplication(clock, cfg);

    const int nbInvariants = 3;
    for (int i = 0; i < nbInvariants; i++)
    {
        app->getInvariantManager().registerInvariant<TestInvariant>(i, true);
    }
    app->getInvariantManager().registerInvariant<TestInvariant>(nbInvariants,
                                                                false);

    app->getInvariantManager().enableInvariant(
        TestInvariant::toString(-1, true) + ".*");
    auto e = app->getInvariantManager().getEnabledInvariants();
    std::sort(e.begin(), e.end());

    REQUIRE(e.size() == nbInvariants);
    for (int i = 0; i < nbInvariants; i++)
    {
        REQUIRE(e[i] == TestInvariant::toString(i, true));
    }
}

TEST_CASE("onBucketApply fail succeed", "[invariant]")
{
    {
        VirtualClock clock;
        Config cfg = getTestConfig();
        cfg.INVARIANT_CHECKS = {};
        Application::pointer app = createTestApplication(clock, cfg);

        app->getInvariantManager().registerInvariant<TestInvariant>(0, true);
        app->getInvariantManager().enableInvariant(
            TestInvariant::toString(0, true));

        auto bucket = std::make_shared<LiveBucket>();
        uint32_t ledger = 1;
        uint32_t level = 0;
        bool isCurr = true;
        REQUIRE_THROWS_AS(app->getInvariantManager().checkOnBucketApply(
                              bucket, ledger, level, isCurr, {}),
                          InvariantDoesNotHold);
    }

    {
        VirtualClock clock;
        Config cfg = getTestConfig();
        cfg.INVARIANT_CHECKS = {};
        Application::pointer app = createTestApplication(clock, cfg);

        app->getInvariantManager().registerInvariant<TestInvariant>(0, false);
        app->getInvariantManager().enableInvariant(
            TestInvariant::toString(0, false));

        auto bucket = std::make_shared<LiveBucket>();
        uint32_t ledger = 1;
        uint32_t level = 0;
        bool isCurr = true;
        REQUIRE_NOTHROW(app->getInvariantManager().checkOnBucketApply(
            bucket, ledger, level, isCurr, {}));
    }
}

TEST_CASE("onOperationApply fail succeed", "[invariant]")
{
    VirtualClock clock;
    Config cfg = getTestConfig();
    Application::pointer app = createTestApplication(clock, cfg);

    OperationResult res;
    SECTION("Fail")
    {
        app->getInvariantManager().registerInvariant<TestInvariant>(0, true);
        app->getInvariantManager().enableInvariant(
            TestInvariant::toString(0, true));

        LedgerTxn ltx(app->getLedgerTxnRoot());
        REQUIRE_THROWS_AS(app->getInvariantManager().checkOnOperationApply(
                              {}, res, ltx.getDelta(), {}),
                          InvariantDoesNotHold);
    }
    SECTION("Succeed")
    {
        app->getInvariantManager().registerInvariant<TestInvariant>(0, false);
        app->getInvariantManager().enableInvariant(
            TestInvariant::toString(0, false));

        LedgerTxn ltx(app->getLedgerTxnRoot());
        REQUIRE_NOTHROW(app->getInvariantManager().checkOnOperationApply(
            {}, res, ltx.getDelta(), {}));
    }
}

TEST_CASE_VERSIONS("EventsAreConsistentWithEntryDiffs invariant", "[invariant]")
{
    auto invariantTest = [](bool enableInvariant) {
        VirtualClock clock;
        auto cfg = getTestConfig(0);
        if (enableInvariant)
        {
            cfg.INVARIANT_CHECKS = {"EventsAreConsistentWithEntryDiffs"};
            cfg.EMIT_CLASSIC_EVENTS = true;
            cfg.BACKFILL_STELLAR_ASSET_EVENTS = true;
        }
        else
        {
            cfg.INVARIANT_CHECKS = {};
        }

        auto app = createTestApplication(clock, cfg);

        // set up world
        auto root = app->getRoot();
        LedgerTxn ltx(app->getLedgerTxnRoot());
        auto ltxe = loadAccount(ltx, root->getPublicKey());
        REQUIRE(ltxe.current().data.type() == ACCOUNT);
        ltxe.current().data.account().balance -= 1;

        OperationResult res;
        if (enableInvariant)
        {
            REQUIRE_THROWS_AS(app->getInvariantManager().checkOnOperationApply(
                                  {}, res, ltx.getDelta(), {}),
                              InvariantDoesNotHold);
        }
        else
        {
            REQUIRE_NOTHROW(app->getInvariantManager().checkOnOperationApply(
                {}, res, ltx.getDelta(), {}));
        }
    };

    invariantTest(true);
    invariantTest(false);
}
