// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Unobtanium Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "validationinterface.h"

#include <boost/signals2/signal.hpp>

struct MainSignalsInstance {
  /** Notifies listeners of updated block chain tip */
  boost::signals2::signal<void (const CBlockIndex *, const CBlockIndex *, bool fInitialDownload)> UpdatedBlockTip;
  /** TODO add these: */
  //boost::signals2::signal<void (const CTransactionRef &)> TransactionAddedToMempool;
  //boost::signals2::signal<void (const std::shared_ptr<const CBlock> &, const CBlockIndex *pindex, const std::vector<CTransactionRef>&)> BlockConnected;
  //boost::signals2::signal<void (const std::shared_ptr<const CBlock> &)> BlockDisconnected;
  //boost::signals2::signal<void (const CBlockIndex *, const std::shared_ptr<const CBlock>&)> NewPoWValidBlock;
  /** Notifies listeners of updated transaction data (transaction, and optionally the block it is found in. */
  boost::signals2::signal<void (const CTransaction &, const CBlock *)> SyncTransaction;
  /** Notifies listeners of an erased transaction (currently disabled, requires transaction replacement). */
  boost::signals2::signal<void (const uint256 &)> EraseTransaction;
  /** Notifies listeners of an updated transaction without new data (for now: a coinbase potentially becoming visible). */
  boost::signals2::signal<void (const uint256 &)> UpdatedTransaction;
  /** Notifies listeners of a new active block chain. */
  boost::signals2::signal<void (const CBlockLocator &)> SetBestChain;
  /** Notifies listeners about an inventory item being seen on the network. */
  boost::signals2::signal<void (const uint256 &)> Inventory;
  /** Tells listeners to broadcast their data. */
  boost::signals2::signal<void (int64_t nBestBlockTime)> Broadcast;
  /** Notifies listeners of a block validation result */
  boost::signals2::signal<void (const CBlock&, const CValidationState&)> BlockChecked;
  /** Notifies listeners that a key for mining is required (coinbase) */
  boost::signals2::signal<void (std::shared_ptr<CReserveScript>&)> ScriptForMining;
  /** Notifies listeners that a block has been successfully mined */
  boost::signals2::signal<void (const uint256 &)> BlockFound;
}

static CMainSignals g_signals;

CMainSignals::CMainSignals() {
    m_internals.reset(new MainSignalsInstance());
}

CMainSignals& GetMainSignals()
{
    return g_signals;
}

void RegisterValidationInterface(CValidationInterface* pwalletIn) {
    g_signals.m_internals->UpdatedBlockTip.connect(boost::bind(&CValidationInterface::UpdatedBlockTip, pwalletIn, _1, _2, _3));
    g_signals.m_internals->SyncTransaction.connect(boost::bind(&CValidationInterface::SyncTransaction, pwalletIn, _1, _2));
    g_signals.m_internals->EraseTransaction.connect(boost::bind(&CValidationInterface::EraseFromWallet, pwalletIn, _1));
    g_signals.m_internals->UpdatedTransaction.connect(boost::bind(&CValidationInterface::UpdatedTransaction, pwalletIn, _1));
    g_signals.m_internals->SetBestChain.connect(boost::bind(&CValidationInterface::SetBestChain, pwalletIn, _1));
    g_signals.m_internals->Inventory.connect(boost::bind(&CValidationInterface::Inventory, pwalletIn, _1));
    g_signals.m_internals->Broadcast.connect(boost::bind(&CValidationInterface::ResendWalletTransactions, pwalletIn, _1));
    g_signals.m_internals->BlockChecked.connect(boost::bind(&CValidationInterface::BlockChecked, pwalletIn, _1, _2));
    g_signals.m_internals->ScriptForMining.connect(boost::bind(&CValidationInterface::GetScriptForMining, pwalletIn, _1));
    g_signals.m_internals->BlockFound.connect(boost::bind(&CValidationInterface::ResetRequestCount, pwalletIn, _1));
    /** TODO Add these later */
    //g_signals.m_internals->TransactionAddedToMempool.connect(boost::bind(&CValidationInterface::TransactionAddedToMempool, pwalletIn, _1));
    //g_signals.m_internals->BlockConnected.connect(boost::bind(&CValidationInterface::BlockConnected, pwalletIn, _1, _2, _3));
    //g_signals.m_internals->BlockDisconnected.connect(boost::bind(&CValidationInterface::BlockDisconnected, pwalletIn, _1));
    //g_signals.m_internals->NewPoWValidBlock.connect(boost::bind(&CValidationInterface::NewPoWValidBlock, pwalletIn, _1, _2));
}

void UnregisterValidationInterface(CValidationInterface* pwalletIn) {
    g_signals.m_internals->BlockFound.disconnect(boost::bind(&CValidationInterface::ResetRequestCount, pwalletIn, _1));
    g_signals.m_internals->ScriptForMining.disconnect(boost::bind(&CValidationInterface::GetScriptForMining, pwalletIn, _1));
    g_signals.m_internals->BlockChecked.disconnect(boost::bind(&CValidationInterface::BlockChecked, pwalletIn, _1, _2));
    g_signals.m_internals->Broadcast.disconnect(boost::bind(&CValidationInterface::ResendWalletTransactions, pwalletIn, _1));
    g_signals.m_internals->Inventory.disconnect(boost::bind(&CValidationInterface::Inventory, pwalletIn, _1));
    g_signals.m_internals->SetBestChain.disconnect(boost::bind(&CValidationInterface::SetBestChain, pwalletIn, _1));
    g_signals.m_internals->UpdatedTransaction.disconnect(boost::bind(&CValidationInterface::UpdatedTransaction, pwalletIn, _1));
    g_signals.m_internals->EraseTransaction.disconnect(boost::bind(&CValidationInterface::EraseFromWallet, pwalletIn, _1));
    g_signals.m_internals->SyncTransaction.disconnect(boost::bind(&CValidationInterface::SyncTransaction, pwalletIn, _1, _2));
    g_signals.m_internals->UpdatedBlockTip.disconnect(boost::bind(&CValidationInterface::UpdatedBlockTip, pwalletIn, _1, _2, _3));
    /** TODO: Add these later*/
    //g_signals.m_internals->TransactionAddedToMempool.disconnect(boost::bind(&CValidationInterface::TransactionAddedToMempool, pwalletIn, _1));
    //g_signals.m_internals->BlockConnected.disconnect(boost::bind(&CValidationInterface::BlockConnected, pwalletIn, _1, _2, _3));
    //g_signals.m_internals->BlockDisconnected.disconnect(boost::bind(&CValidationInterface::BlockDisconnected, pwalletIn, _1));
    //g_signals.m_internals->NewPoWValidBlock.disconnect(boost::bind(&CValidationInterface::NewPoWValidBlock, pwalletIn, _1, _2));
}

void UnregisterAllValidationInterfaces() {
    g_signals.m_internals->BlockFound.disconnect_all_slots();
    g_signals.m_internals->ScriptForMining.disconnect_all_slots();
    g_signals.m_internals->BlockChecked.disconnect_all_slots();
    g_signals.m_internals->Broadcast.disconnect_all_slots();
    g_signals.m_internals->Inventory.disconnect_all_slots();
    g_signals.m_internals->SetBestChain.disconnect_all_slots();
    g_signals.m_internals->EraseTransaction.disconnect_all_slots();
    g_signals.m_internals->UpdatedTransaction.disconnect_all_slots();
    g_signals.m_internals->SyncTransaction.disconnect_all_slots();
    g_signals.m_internals->UpdatedBlockTip.disconnect_all_slots();
    /** TODO Add these later */
    //g_signals.m_internals->TransactionAddedToMempool.disconnect_all_slots();
    //g_signals.m_internals->BlockConnected.disconnect_all_slots();
    //g_signals.m_internals->BlockDisconnected.disconnect_all_slots();
    //g_signals.m_internals->NewPoWValidBlock.disconnect_all_slots();
}

void CMainSignals::UpdatedBlockTip(const CBlockIndex *pindexNew, const CBlockIndex *pindexFork, bool fInitialDownload) {
    m_internals->UpdatedBlockTip(pindexNew, pindexFork, fInitialDownload);
}

void CMainSignals::ScriptForMining(std::shared_ptr<CReserveScript> &Script) {
   m_internals->ScriptForMining(Script);
}

void CMainSignals::BlockFound(const uint256 &hash) {
    m_internals->BlockFound(hash);
}

void CMainSignals::EraseTransaction(const uint256 &hash) {
    m_internals->EraseTransaction(hash);
}

/** TODO Add these later */
/*
void CMainSignals::TransactionAddedToMempool(const CTransactionRef &ptx) {
    m_internals->TransactionAddedToMempool(ptx);
}

void CMainSignals::BlockConnected(const std::shared_ptr<const CBlock> &pblock, const CBlockIndex *pindex, const std::vector<CTransactionRef>& vtxConflicted) {
    m_internals->BlockConnected(pblock, pindex, vtxConflicted);
}

void CMainSignals::BlockDisconnected(const std::shared_ptr<const CBlock> &pblock) {
    m_internals->BlockDisconnected(pblock);
}
*/

void CMainSignals::SetBestChain(const CBlockLocator &locator) {
    m_internals->SetBestChain(locator);
}

void CMainSignals::Inventory(const uint256 &hash) {
    m_internals->Inventory(hash);
}

void CMainSignals::Broadcast(int64_t nBestBlockTime) {//, CConnman* connman) {
    m_internals->Broadcast(nBestBlockTime);//, connman);
}

void CMainSignals::BlockChecked(const CBlock& block, const CValidationState& state) {
    m_internals->BlockChecked(block, state);
}

/** TODO Add NewPoWValidBlock Later */
/*
void CMainSignals::NewPoWValidBlock(const CBlockIndex *pindex, const std::shared_ptr<const CBlock> &block) {
    m_internals->NewPoWValidBlock(pindex, block);
}
*/
void SyncWithWallets(const CTransaction &tx, const CBlock *pblock) {
    g_signals.SyncTransaction(tx, pblock);
}
