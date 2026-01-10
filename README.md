# Cache Simulator

A C-based simulation of a **Dynamic CPU Cache** implementing **N-Way Set Associative Mapping** and the **Least Recently Used (LRU)** replacement policy. This project demonstrates understanding of computer architecture concepts including memory hierarchy, cache conflicts, and write-back policies.

## 📂 Project Structure

This repository contains three different implementations of the cache logic:

### 1. Interactive Simulator (`cache_interactive.c`)
* **Mode:** Interactive (User Input).
* **Function:** Allows you to manually type memory addresses (in Hex) to see if they result in a HIT or MISS.
* **Use Case:** Great for step-by-step debugging or understanding how specific addresses map to sets and tags.

### 2. Trace Simulator (`cache_trace_basic.c`)
* **Mode:** Automated (File Input).
* **Function:** Reads a sequence of memory accesses from `trace.txt`.
* **Logic:** Implements standard LRU eviction but treats all accesses as "Reads" (does not track dirty bits).

### 3. Advanced Trace Simulator (`cache_trace_writeback.c`)
* **Mode:** Automated (File Input).
* **Feature:** Adds **Write-Back Policy** support.
* **Logic:**
    * Tracks a **Dirty Bit** for every cache line.
    * Distinguishes between Read (`R`) and Write (`W`) operations from the trace file.
    * Simulates "Writing back to RAM" when a dirty block is evicted.
# 4. Dynamic Cache Simulator

This version of the simulator introduces **Dynamic Memory Allocation** and **Command Line Arguments**, allowing the user to configure the cache geometry (Number of Sets and Associativity) at runtime without recompiling.

## 📝 Overview
In this phase, we moved away from hardcoded array sizes (e.g., `my_cache[4096]`) and replaced them with pointers and `malloc`. The simulator now acts like a configurable hardware controller that generates the cache structure based on user specifications.

### Key Features
* **Dynamic Arrays:** Uses `malloc` to allocate exact memory for Sets and Lines based on input.
* **Runtime Configuration:** Accepts command-line arguments for `Sets` and `Ways`.
* **Universal Indexing:** Automatically calculates `index_bits` and `tag_bits` for any cache size using bitwise logic.
* **Safety Mechanisms:** Includes input validation (`argc` checks) and proper pointer management.

## 🔧 How to Build & Run

### 1. Compilation
The code now requires the Standard Library (`stdlib.h`) for memory management.
```bash
gcc dynamic_cache_sim.c -o cache_sim

Run the program by specifying the Number of Sets and the Associativity (Ways).

./cache_sim <num_sets> <associativity>

./cache_sim 1024 1
```
# 📊 Performance Analysis
To validate the simulator, I conducted an A/B test using a "Thrashing Trace" (a memory access pattern designed to cause conflicts). I compared a basic Direct-Mapped cache against a modern Set-Associative design.

**Test Parameters:**
* **Trace:** Repeated access to 4 conflicting addresses (0x1000, 0x2000, 0x3000, 0x4000).
* **Cache Size:** Constant (1024 lines total).
* **Block Size:** 64 Bytes.

| Configuration | Hit Rate | AMAT (Cycles) | Engineering Conclusion |
| :--- | :--- | :--- | :--- |
| **Direct Mapped** (1-Way) | 16% | **85.0** | **Failed.** The cache suffered from extreme thrashing. Every new address evicted the previous one, forcing the CPU to fetch from RAM constantly. |
| **4-Way Set Associative** | 85% | **16.0** | **Optimal.** The 4-way design allowed all 4 conflicting addresses to coexist in the same set. After the initial "Cold Start" misses, the CPU operated at near-L1 speeds. |

### 🧠 Key Takeaways
1.  **Associativity Matters:** Increasing associativity from 1-Way to 4-Way reduced the Average Memory Access Time (AMAT) by **81%**.
2.  **Hit Rate is Misleading:** A 16% Hit Rate doesn't sound "too bad" until you calculate AMAT. The penalty for misses is so high (100 cycles) that even a small number of misses destroys system performance.
3.  **Dynamic Config:** The simulator's ability to reconfigure geometry at runtime (using pointers and bitwise masks) allows for rapid design space exploration without recompiling.

   
## ⚙️ Cache Specifications

The simulator is configured with the following architecture:

| Parameter | Value |
| :--- | :--- |
| **Total Cache Size** | 1 MB (1,048,576 Bytes) |
| **Associativity** | 4-Way Set Associative |
| **Number of Sets** | 4096 Sets ($2^{12}$) |
| **Block Size** | 64 Bytes ($2^6$) |
| **Replacement Policy** | LRU (Least Recently Used) |
| **Write Policy** | Write-Back (in advanced version) |

### Address Breakdown (32-bit)
* **Offset:** 6 bits
* **Index:** 12 bits
* **Tag:** 14 bits

## 🚀 How to Run

### Prerequisites
You need a C compiler (GCC) installed.

### 1. Compiling
Run the command corresponding to the version you want to test:

```bash
# For the Interactive Version
gcc cache_interactive.c -o cache_interactive

# For the Trace Version (Standard)
gcc cache_trace_basic.c -o cache_trace

# For the Advanced Version (With Dirty Bits)
gcc cache_trace_writeback.c -o cache_advanced
```
2. Running
Interactive Mode:

```Bash

./cache_interactive
# Then type addresses like: 0x12345678
Trace Mode: (Ensure you have a trace.txt file in the same folder formatted as R 0xAddress)
```
```Bash

./cache_advanced
📊 Sample Output
When running the simulator, you will see detailed logs of the cache behavior:

Plaintext

checking address: 0x7ff00010  | Tag : 8191  | index : 0x0
Result MISS ! (Updating new address in 0)
-> Mark Dirty : No
Current Hit Rate: 0.00% (0/1)

checking address: 0x7ff00010  | Tag : 8191  | index : 0x0
Result HIT !
-> Mark Dirty: YES
Current Hit Rate: 50.00% (1/2)

...

Result: MISS (Cache Full - Evicting Way 2)
  -> WRITING BACK dirty block to RAM... (Slow!)
```
🛠️ Technology Stack
Language: C

Concepts: Bitwise Operations (Masking/Shifting), Structs, File I/O, Pointers.
