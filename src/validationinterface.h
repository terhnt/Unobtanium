// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Unobtanium Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef UNOBTANIUM_VALIDATIONINTERFACE_H
#define UNOBTANIUM_VALIDATIONINTERFACE_H

#include <memory>

#include "primitives/transaction.h" // CTransaction(Ref)

class CBlock;
class CBlockIndex;
struct CBlockLocator;
class CReserveScript;
class CTransaction;
class CValidationInterface;
class CValidationState;
class uint256;

// These functions dispatch to one or all registered wallets

/** Register a wallet to receive updates from core */
void RegisterValidationInterface(CValidationInterface* pwalletIn);
/** Unregister a wallet from core */
void UnregisterValidationInterface(CValidationInterface* pwalletIn);
/** Unregister all wallets from core */
void UnregisterAllValidationInterfaces();
/** Push an updated transaction to all registered wallets */
//void SyncWithWallets(const CTransaction& tx, const CBlock* pblock = NULL);

class CValidationInterface {
protected:
    /** Notifies listeners of updated block chain tip */
    virtual void UpdatedBlockTip(const CBlockIndex *pindexNew, const CBlockIndex *pindexFork, bool fInitialDownload) {}
    /** Notifies listeners of a transaction having been added to mempool. */
    // virtual void TransactionAddedToMempool(const CTransactionRef &ptxn) {}
    /**
     * Notifies listeners of a block being connected.
     * Provides a vector of transactions evicted from the mempool as a result.
     */
    //virtual void BlockConnected(const std::shared_ptr<const CBlock> &block, const CBlockIndex *pindex, const std::vector<CTransactionRef> &txnConflicted) {}
    /** Notifies listeners of a block being disconnected */
    //virtual void BlockDisconnected(const std::shared_ptr<const CBlock> &block) {}
    /** Notifies listeners of updated transaction data (transaction, and optionally the block it is found in. */
    //virtual void SyncTransaction(const CTransaction &tx, CBlock *block) {} //    boost::signals2::signal<void (const CTransaction &, const CBlock *)> SyncTransaction;
    /** Notifies listeners of an erased transaction (currently disabled, requires transaction replacement). */
    virtual void EraseFromWallet(const uint256 &hash) {}
    /** Notifies listeners of an updated transaction without new data (for now: a coinbase potentially becoming visible). */
    virtual void UpdatedTransaction(const uint256 &hash) {}
    /** Notifies listeners of a the active block chain. */
    virtual void SetBestChain(const CBlockLocator &locator) {}
    /** Notifies listeners about an inventory item being seen on the network. */
    virtual void Inventory(const uint256 &hash) {} //boost::signals2::signal<void (const uint256 &)> Inventory;
    /** Tells listeners to broadcast their data. */
    //boost::signals2::signal<void ()> Broadcast;
    //virtual void ResendWalletTransactions(int64_t nBestBlockTime, CConnman* connman) {}
    virtual void ResendWalletTransactions(int64_t nBestBlockTime) {} //?
    /** Notifies listeners of a block validation result */
    virtual void BlockChecked(const CBlock&, const CValidationState&) {} //boost::signals2::signal<void (const CBlock&, const CValidationState&)> BlockChecked;
    /** Notifies listeners that a key for mining is required (coinbase) */
    virtual void GetScriptForMining(std::shared_ptr<CReserveScript>&) {} //boost::signals2::signal<void (std::shared_ptr<CReserveScript>&)> ScriptForMining;
    /** Notifies listeners that a block has been successfully mined */
    virtual void ResetRequestCount(const uint256 &hash) {} //boost::signals2::signal<void (const uint256 &)> BlockFound;
    /**
     * Notifies listeners that a block which builds directly on our current tip
     * has been received and connected to the headers tree, though not validated yet */
    //virtual void NewPoWValidBlock(const CBlockIndex *pindex, const std::shared_ptr<const CBlock>& block) {};
    friend void ::RegisterValidationInterface(CValidationInterface*);
    friend void ::UnregisterValidationInterface(CValidationInterface*);
    friend void ::UnregisterAllValidationInterfaces();
};

struct MainSignalsInstance;
class CMainSignals {
private:
    std::unique_ptr<MainSignalsInstance> m_internals;

    friend void ::RegisterValidationInterface(CValidationInterface*);
    friend void ::UnregisterValidationInterface(CValidationInterface*);
    friend void ::UnregisterAllValidationInterfaces();
public:
    CMainSignals();

    void UpdatedBlockTip(const CBlockIndex *, const CBlockIndex *, bool fInitialDownload);
    //void TransactionAddedToMempool(const CTransactionRef &);
    //void BlockConnected(const std::shared_ptr<const CBlock> &, const CBlockIndex *pindex, const std::vector<CTransactionRef> &);
    //void BlockDisconnected(const std::shared_ptr<const CBlock> &);
    void UpdatedTransaction(const uint256 &);
    void SetBestChain(const CBlockLocator &);
    void Inventory(const uint256 &);
    void EraseTransaction(const uint256 &);
    void ScriptForMining(std::shared_ptr<CReserveScript>&);
    void BlockFound(const uint256 &);
    //void Broadcast(int64_t nBestBlockTime, CConnman* connman);
    void Broadcast(int64_t nBestBlockTime); //??
    void BlockChecked(const CBlock&, const CValidationState&);
    //void NewPoWValidBlock(const CBlockIndex *, const std::shared_ptr<const CBlock>&);
    void SyncTransaction(const CTransaction &tx, const CBlock *pblock);
};

CMainSignals& GetMainSignals();

#endif // UNOBTANIUM_VALIDATIONINTERFACE_H
