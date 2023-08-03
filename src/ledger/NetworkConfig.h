#pragma once

// Copyright 2023 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "ledger/LedgerTxn.h"
#include "main/Config.h"
#include <cstdint>
#include <deque>
#ifdef ENABLE_NEXT_PROTOCOL_VERSION_UNSAFE_FOR_PRODUCTION
#include "rust/RustBridge.h"
#endif

namespace stellar
{

class Application;

// Defines the initial values of the network configuration
// settings that are applied during the protocol version upgrade.
// These values should never be changed after the protocol upgrade
// happens - any further changes should be performed separately via
// config upgrade mechanism.
struct InitialSorobanNetworkConfig
{
    // Contract size settings
    static constexpr uint32_t MAX_CONTRACT_SIZE = 64 * 1024; // 64KB

    // Contract data settings
    static constexpr uint32_t MAX_CONTRACT_DATA_KEY_SIZE_BYTES = 300;
    static constexpr uint32_t MAX_CONTRACT_DATA_ENTRY_SIZE_BYTES =
        64 * 1024; // 64KB

    // Compute settings
    static constexpr int64_t TX_MAX_INSTRUCTIONS = 100'000'000;
    static constexpr int64_t LEDGER_MAX_INSTRUCTIONS = 10 * TX_MAX_INSTRUCTIONS;
    static constexpr int64_t FEE_RATE_PER_INSTRUCTIONS_INCREMENT =
        100; // 0.2 XLM/max tx
    static constexpr uint32_t MEMORY_LIMIT = 100 * 1024 * 1024; // 100MB

    // Ledger access settings
    static constexpr uint32_t TX_MAX_READ_LEDGER_ENTRIES = 40;
    static constexpr uint32_t TX_MAX_READ_BYTES = 200 * 1024;
    static constexpr uint32_t TX_MAX_WRITE_LEDGER_ENTRIES = 20;
    static constexpr uint32_t TX_MAX_WRITE_BYTES = 100 * 1024;
    static constexpr uint32_t LEDGER_MAX_READ_LEDGER_ENTRIES =
        10 * TX_MAX_READ_LEDGER_ENTRIES;
    static constexpr uint32_t LEDGER_MAX_READ_BYTES = 10 * TX_MAX_READ_BYTES;
    static constexpr uint32_t LEDGER_MAX_WRITE_LEDGER_ENTRIES =
        10 * TX_MAX_WRITE_LEDGER_ENTRIES;
    static constexpr uint32_t LEDGER_MAX_WRITE_BYTES = 10 * TX_MAX_WRITE_BYTES;
    static constexpr int64_t FEE_READ_LEDGER_ENTRY = 5'000;   // 0.02 XLM/max tx
    static constexpr int64_t FEE_WRITE_LEDGER_ENTRY = 20'000; // 0.04 XLM/max tx
    static constexpr int64_t FEE_READ_1KB = 1'000;            // 0.02 XLM/max tx
    static constexpr int64_t BUCKET_LIST_TARGET_SIZE_BYTES =
        30LL * 1024 * 1024 * 1024; // 30 GB
    static constexpr int64_t BUCKET_LIST_FEE_1KB_BUCKET_LIST_LOW =
        1'000; // 0.01 XLM/max tx
    static constexpr int64_t BUCKET_LIST_FEE_1KB_BUCKET_LIST_HIGH =
        10'000; // 0.1 XLM/max tx
    // No growth fee initially to make sure fees are accessible
    static constexpr uint32_t BUCKET_LIST_WRITE_FEE_GROWTH_FACTOR = 1;

    static constexpr uint64_t BUCKET_LIST_SIZE_WINDOW_SAMPLE_SIZE =
        30; // 30 day average

    // Historical data settings
    static constexpr int64_t FEE_HISTORICAL_1KB = 100; // 0.001 XLM/max tx

    // Bandwidth settings
    static constexpr uint32_t TX_MAX_SIZE_BYTES = 100 * 1024;
    static constexpr uint32_t LEDGER_MAX_TRANSACTION_SIZES_BYTES =
        10 * TX_MAX_SIZE_BYTES;
    static constexpr int64_t FEE_TRANSACTION_SIZE_1KB =
        2'000; // 0.02 XLM/max tx

    // Contract events settings
    static constexpr uint32_t TX_MAX_CONTRACT_EVENTS_SIZE_BYTES = 10 * 1024;
    static constexpr int64_t FEE_CONTRACT_EVENTS_SIZE_1KB = 200;

    // State expiration settings
    // 1 year in ledgers
    static constexpr uint32_t MAXIMUM_ENTRY_LIFETIME = 6'312'000;

    // Live until level 6
    static constexpr uint32_t MINIMUM_PERSISTENT_ENTRY_LIFETIME = 4096;
    static constexpr uint32_t MINIMUM_TEMP_ENTRY_LIFETIME = 16;

    static constexpr uint32_t AUTO_BUMP_NUM_LEDGERS = 0;

    static constexpr uint64_t EVICTION_SCAN_SIZE = 1;
    static constexpr uint32_t MAX_ENTRIES_TO_EXPIRE = 1;

    // Rent payment of a write fee per ~25 days.
    static constexpr int64_t PERSISTENT_RENT_RATE_DENOMINATOR = 252'480;
    // Rent payment of a write fee per ~250 days.
    static constexpr int64_t TEMP_RENT_RATE_DENOMINATOR = 2'524'800;

    // General execution settings
    static constexpr uint32_t LEDGER_MAX_TX_COUNT = 10;
};

// Defines the minimum values allowed for the network configuration
// settings during upgrades. An upgrade that does not follow the minimums
// will be rejected.
struct MinimumSorobanNetworkConfig
{
    static constexpr uint32_t TX_MAX_READ_LEDGER_ENTRIES = 5;
    static constexpr uint32_t TX_MAX_READ_BYTES = 5000;

    static constexpr uint32_t TX_MAX_WRITE_LEDGER_ENTRIES = 2;
    static constexpr uint32_t TX_MAX_WRITE_BYTES = 5000;

    static constexpr uint32_t LEDGER_MAX_READ_LEDGER_ENTRIES = 5;
    static constexpr uint32_t LEDGER_MAX_READ_BYTES = 5000;
    static constexpr uint32_t LEDGER_MAX_WRITE_LEDGER_ENTRIES = 2;
    static constexpr uint32_t LEDGER_MAX_WRITE_BYTES = 5000;

    static constexpr uint32_t TX_MAX_SIZE_BYTES = 10000;
    static constexpr uint32_t LEDGER_MAX_TX_SIZE_BYTES = 10000;

    static constexpr uint32_t TX_MAX_INSTRUCTIONS = 5'000'000;
    static constexpr uint32_t LEDGER_MAX_INSTRUCTIONS = 5'000'000;
    static constexpr uint32_t MEMORY_LIMIT = 5'000'000;

    static constexpr uint32_t MAX_CONTRACT_DATA_KEY_SIZE_BYTES = 500;
    static constexpr uint32_t MAX_CONTRACT_DATA_ENTRY_SIZE_BYTES = 5000;
    static constexpr uint32_t MAX_CONTRACT_SIZE = 5000;

    static constexpr uint32_t MINIMUM_PERSISTENT_ENTRY_LIFETIME = 100;
    static constexpr uint32_t MAXIMUM_ENTRY_LIFETIME = 259200; // 15 days
};

// Wrapper for the contract-related network configuration.
class SorobanNetworkConfig
{
  public:
    // Creates the initial contract configuration entries for protocol v20.
    // This should happen once during the correspondent protocol version
    // upgrade.
    static void createLedgerEntriesForV20(AbstractLedgerTxn& ltx,
                                          Application& app);
    // Test-only function that initializes contract network configuration
    // bypassing the normal upgrade process (i.e. when genesis ledger starts not
    // at v1)
    static void
    initializeGenesisLedgerForTesting(uint32_t genesisLedgerProtocol,
                                      AbstractLedgerTxn& ltx, Application& app);

    void loadFromLedger(AbstractLedgerTxn& ltx, uint32_t configMaxProtocol,
                        uint32_t protocolVersion);
    // Maximum allowed size of the contract Wasm that can be uploaded (in
    // bytes).
    uint32_t maxContractSizeBytes() const;
    // Maximum allowed size of a `LedgerKey::CONTRACT_DATA` (in bytes).
    uint32_t maxContractDataKeySizeBytes() const;
    // Maximum allowed size of a `LedgerEntry::CONTRACT_DATA` (in bytes).
    uint32_t maxContractDataEntrySizeBytes() const;

    // Compute settings for contracts (instructions and memory).
    // Maximum instructions per ledger
    int64_t ledgerMaxInstructions() const;
    // Maximum instructions per transaction
    int64_t txMaxInstructions() const;
    // Cost of 10000 instructions
    int64_t feeRatePerInstructionsIncrement() const;
    // Memory limit per transaction.
    uint32_t txMemoryLimit() const;

    // Ledger access settings for contracts.
    // Maximum number of ledger entry read operations per ledger
    uint32_t ledgerMaxReadLedgerEntries() const;
    // Maximum number of bytes that can be read per ledger
    uint32_t ledgerMaxReadBytes() const;
    // Maximum number of ledger entry write operations per ledger
    uint32_t ledgerMaxWriteLedgerEntries() const;
    // Maximum number of bytes that can be written per ledger
    uint32_t ledgerMaxWriteBytes() const;
    // Maximum number of ledger entry read operations per transaction
    uint32_t txMaxReadLedgerEntries() const;
    // Maximum number of bytes that can be read per transaction
    uint32_t txMaxReadBytes() const;
    // Maximum number of ledger entry write operations per transaction
    uint32_t txMaxWriteLedgerEntries() const;
    // Maximum number of bytes that can be written per transaction
    uint32_t txMaxWriteBytes() const;
    // Fee per ledger entry read
    int64_t feeReadLedgerEntry() const;
    // Fee per ledger entry write
    int64_t feeWriteLedgerEntry() const;
    // Fee for reading 1KB
    int64_t feeRead1KB() const;
    // Fee for writing 1KB
    int64_t feeWrite1KB() const;

    // Historical data (pushed to core archives) settings for contracts.
    // Fee for storing 1KB in archives
    int64_t feeHistorical1KB() const;

    // Contract events settings.
    // Maximum size of contract events produced by a transaction.
    uint32_t txMaxContractEventsSizeBytes() const;
    // Fee for generating 1KB of contract events.
    int64_t feeContractEventsSize1KB() const;

    // Bandwidth related data settings for contracts
    // Maximum size in bytes to propagate per ledger
    uint32_t ledgerMaxTransactionSizesBytes() const;
    // Maximum size in bytes for a transaction
    uint32_t txMaxSizeBytes() const;
    // Fee for propagating 1KB of data
    int64_t feeTransactionSize1KB() const;

    // General execution ledger settings
    uint32_t ledgerMaxTxCount() const;

    // Number of samples in slidign window
    uint32_t getBucketListSizeSnapshotPeriod() const;

    // If currLedger is a ledger when we should snapshot, add a new snapshot to
    // the sliding window and write it to disk.
    void maybeSnapshotBucketListSize(uint32_t currLedger,
                                     AbstractLedgerTxn& ltx, Application& app);

    // Returns the average of all BucketList size snapshots in the sliding
    // window.
    uint64_t getAverageBucketListSize() const;

#ifdef BUILD_TESTS
    uint32_t& maxContractDataKeySizeBytes();
    uint32_t& maxContractDataEntrySizeBytes();

    void setBucketListSnapshotPeriodForTesting(uint32_t period);
    std::deque<uint64_t> const& getBucketListSizeWindowForTesting() const;
#endif

#ifdef ENABLE_NEXT_PROTOCOL_VERSION_UNSAFE_FOR_PRODUCTION
    // Cost model parameters of the Soroban host
    ContractCostParams const& cpuCostParams() const;
    ContractCostParams const& memCostParams() const;

    static bool isValidCostParams(ContractCostParams const& params);

    CxxFeeConfiguration rustBridgeFeeConfiguration() const;
    CxxRentFeeConfiguration rustBridgeRentFeeConfiguration() const;

    // State expiration settings
    StateExpirationSettings const& stateExpirationSettings() const;
#ifdef BUILD_TESTS
    StateExpirationSettings& stateExpirationSettings();
#endif
#endif

  private:
    static constexpr uint32_t BUCKETLIST_SIZE_SNAPSHOT_PERIOD =
        17280; // 1 day, in ledgers

    void loadMaxContractSize(AbstractLedgerTxn& ltx);
    void loadMaxContractDataKeySize(AbstractLedgerTxn& ltx);
    void loadMaxContractDataEntrySize(AbstractLedgerTxn& ltx);
    void loadComputeSettings(AbstractLedgerTxn& ltx);
    void loadLedgerAccessSettings(AbstractLedgerTxn& ltx);
    void loadHistoricalSettings(AbstractLedgerTxn& ltx);
    void loadContractEventsSettings(AbstractLedgerTxn& ltx);
    void loadBandwidthSettings(AbstractLedgerTxn& ltx);
    void loadCpuCostParams(AbstractLedgerTxn& ltx);
    void loadMemCostParams(AbstractLedgerTxn& ltx);
    void loadStateExpirationSettings(AbstractLedgerTxn& ltx);
    void loadExecutionLanesSettings(AbstractLedgerTxn& ltx);
    void loadBucketListSizeWindow(AbstractLedgerTxn& ltx);
    void computeWriteFee(uint32_t configMaxProtocol, uint32_t protocolVersion);
    // If newSize is different than the current BucketList size sliding window,
    // update the window. If newSize < currSize, pop entries off window. If
    // newSize > currSize, add as many copies of the current BucketList size to
    // window until it has newSize entries.
    void maybeUpdateBucketListWindowSize(AbstractLedgerTxn& ltx);

#ifdef ENABLE_NEXT_PROTOCOL_VERSION_UNSAFE_FOR_PRODUCTION
    void writeBucketListSizeWindow(AbstractLedgerTxn& ltxRoot) const;
    void updateBucketListSizeAverage();
#endif

    uint32_t mMaxContractSizeBytes{};
    uint32_t mMaxContractDataKeySizeBytes{};
    uint32_t mMaxContractDataEntrySizeBytes{};

    // Compute settings for contracts (instructions and memory).
    int64_t mLedgerMaxInstructions{};
    int64_t mTxMaxInstructions{};
    int64_t mFeeRatePerInstructionsIncrement{};
    uint32_t mTxMemoryLimit{};

    // Ledger access settings for contracts.
    uint32_t mLedgerMaxReadLedgerEntries{};
    uint32_t mLedgerMaxReadBytes{};
    uint32_t mLedgerMaxWriteLedgerEntries{};
    uint32_t mLedgerMaxWriteBytes{};
    uint32_t mLedgerMaxTxCount{};
    uint32_t mTxMaxReadLedgerEntries{};
    uint32_t mTxMaxReadBytes{};
    uint32_t mTxMaxWriteLedgerEntries{};
    uint32_t mTxMaxWriteBytes{};
    int64_t mFeeReadLedgerEntry{};
    int64_t mFeeWriteLedgerEntry{};
    int64_t mFeeRead1KB{};
    int64_t mFeeWrite1KB{};
    int64_t mBucketListTargetSizeBytes{};
    int64_t mWriteFee1KBBucketListLow{};
    int64_t mWriteFee1KBBucketListHigh{};
    uint32_t mBucketListWriteFeeGrowthFactor{};

    // Historical data (pushed to core archives) settings for contracts.
    int64_t mFeeHistorical1KB{};

    // Contract events settings.
    uint32_t mTxMaxContractEventsSizeBytes{};
    int64_t mFeeContractEvents1KB{};

    // Bandwidth related data settings for contracts
    uint32_t mLedgerMaxTransactionsSizeBytes{};
    uint32_t mTxMaxSizeBytes{};
    int64_t mFeeTransactionSize1KB{};

    // FIFO queue, push_back/pop_front
    std::deque<uint64_t> mBucketListSizeSnapshots;
    uint64_t mAverageBucketListSize{0};

#ifdef BUILD_TESTS
    std::optional<uint32_t> mBucketListSnapshotPeriodForTesting;
#endif

#ifdef ENABLE_NEXT_PROTOCOL_VERSION_UNSAFE_FOR_PRODUCTION
    // Host cost params
    ContractCostParams mCpuCostParams{};
    ContractCostParams mMemCostParams{};

    // State expiration settings
    StateExpirationSettings mStateExpirationSettings{};

#endif
};

}
