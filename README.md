# Naka Core ($NKC)

**Naka Core** is an independent, decentralized, Proof-of-Work (PoW) cryptocurrency built from scratch in C++. Inspired by the foundational principles of Bitcoin, Naka Core focuses on absolute digital scarcity, transparent economic rules, and cryptographic security.

## 🚀 Core Economic Specifications
* **Ticker:** $NKC (Naka Core)
* **Max Supply:** 20,000,000 NKC
* **Consensus Algorithm:** SHA-256 Proof-of-Work (PoW)
* **Block Interval:** Regulated background mining
* **Halving Mechanism:** Automatic reward reduction schedule to enforce long-term scarcity.

## 🛠️ Technical Architecture
* **Language:** Modern C++
* **Storage:** Binary-persisted blockchain ledger (`./data/chain.dat`)
* **Networking:** P2P Node server architecture (default port: `8333`)
* **Subsystems:** Integrated Mempool, UTXO/Block Validation, and CLI Daemon control.

## 💻 Getting Started & Compilation

### Prerequisites
* A Linux environment (or WSL on Windows 11)
* CMake (version 3.10 or higher)
* A C++ compiler supporting C++17 (e.g., `g++`)

### Build Instructions
## 🕹️ CLI Commands
Once inside the `nakacored` daemon console, you can use:
* `status` - Show node health, height, and miner balance.
* `balance` - Check total $NKC accumulated.
* `height` - Display current blockchain height.
* `mempool` - View pending transactions.
* `help` - List all available commands.
* `exit` - Shut down the daemon safely.

---
*Built with passion for decentralized systems.*
