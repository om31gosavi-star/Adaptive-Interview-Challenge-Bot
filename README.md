# 🤖 Adaptive AI Mock Interview Simulation Bot (C++ & Web)

[![Deploy with Vercel](https://vercel.com/button)](https://vercel.com/new/clone?repository-url=https://github.com/om31gosavi-star/Adaptive-Interview-Challenge-Bot)
[![Live Demo](https://img.shields.io/badge/Live%20Demo-GitHub%20Pages-brightgreen?style=for-the-badge&logo=github)](https://om31gosavi-star.github.io/Adaptive-Interview-Challenge-Bot/)

> 🌐 **Live Web Application:** [https://om31gosavi-star.github.io/Adaptive-Interview-Challenge-Bot/](https://om31gosavi-star.github.io/Adaptive-Interview-Challenge-Bot/)  
> ⚡ **Deploy to Vercel with 1-Click:** [https://vercel.com/new/clone?repository-url=https://github.com/om31gosavi-star/Adaptive-Interview-Challenge-Bot](https://vercel.com/new/clone?repository-url=https://github.com/om31gosavi-star/Adaptive-Interview-Challenge-Bot)

A high-performance, adaptive mock interview simulation engine engineered in modern C++ (C++17) with a companion interactive Web Application. Designed to simulate realistic technical screening and bar-raiser loops at top tech companies (FAANG/Tier-1), featuring real-time **Elo-based difficulty adaptation**, **multi-track question banks**, **interactive interviewer personas**, **dynamic follow-up probing**, and automated **dual report generation** (Markdown and Interactive HTML).

---

## 🌟 Key Features

### 1. 🧠 Dynamic Adaptive Difficulty Engine (Elo Rating)
- **Candidate Skill Rating:** Baseline begins at 1200 (Mid-level). Adjusts dynamically using a customized Elo rating algorithm after every answer:
  $$E_{\text{expected}} = \frac{1}{1 + 10^{(R_{\text{question}} - R_{\text{candidate}}) / 400}}$$
  $$R_{\text{new}} = R_{\text{old}} + K \times (\text{ActualScore} - E_{\text{expected}})$$
- **Difficulty Transitions:** The bot dynamically transitions between **EASY** (1000), **MEDIUM** (1250), **HARD** (1500), and **EXPERT** (1750) based on real-time performance.
- **Multi-Factor Semantic NLP Evaluation:** Evaluates answers across:
  - **Key Invariants Coverage (55%)**: Core technical terminology, data structures, algorithms, and mechanics.
  - **Analytical Depth & Trade-offs (25%)**: Detection of edge cases, latency/throughput considerations, memory/cache implications, failure modes, and architectural trade-offs.
  - **Advanced Nuances & Insights (15%)**: Bonus deep-dive concepts (e.g. `xvalue`, `reference collapsing`, `MESI cache line invalidation`, `SingleFlight`, `Bloom filters`).
  - **Structure & Articulation (5%)**: Ratio of signal to noise, including STAR framework detection (Situation, Task, Action, Result) for behavioral questions.

### 2. 🎯 Multi-Track Technical & Behavioral Domains
- **Track 1: Modern C++ & Low-Level Systems Programming**
  - RAII, Smart Pointer internals (`std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr`)
  - Move Semantics, Rvalues, and Perfect Forwarding (`std::move`, `std::forward`, reference collapsing)
  - Polymorphic Memory Layout, `vptr` and `vtable` dynamic dispatch, virtual destructors
  - C++ Memory Model, `std::atomic`, Memory Ordering (`relaxed`, `acquire-release`, `seq_cst`), CPU Cache Line False Sharing & `alignas(64)`
- **Track 2: Data Structures, Algorithms & Problem Solving**
  - Hash Map collision resolution (Separate Chaining vs Open Addressing, load factors, amortized $O(1)$)
  - Shortest Path algorithms (BFS vs Dijkstra with Min-Priority Queue, handling negative weights with Bellman-Ford)
  - LRU Cache Design ($O(1)$ get/put combining Doubly Linked List with Hash Map, concurrency safety)
  - Real-time High-throughput Streaming Top-K (Count-Min Sketch, Space-Saving / Lossy Counting, bounded memory)
- **Track 3: System Design & Scalability**
  - Horizontal vs Vertical Scaling, Load Balancing algorithms (Round Robin, Least Connections, IP Hash)
  - Caching Strategies (Cache-Aside, Write-Through, Write-Back) and Preventing Cache Stampede / Thundering Herd
  - Database Storage Engines: B+ Trees (InnoDB/Postgres) vs Log-Structured Merge (LSM) Trees (RocksDB/Cassandra)
  - Distributed Rate Limiter at 10,000,000 RPS (Token Bucket, Redis Lua scripts, cross-DC synchronization, race conditions)
- **Track 4: Behavioral & Engineering Leadership (STAR Method)**
  - Technical conflict resolution & data-driven alignment
  - High-severity incident triage (P0 containment, rollback, blameless post-mortem)
  - Balancing product deadlines with architectural technical debt
- **Track 5: Full Mock Loop**
  - Comprehensive multi-round tech company onsite simulation.

### 3. 🎭 Distinctive Interviewer Personas
- **Alex Vance (FAANG Bar Raiser):** Rigorous, zero-fluff, demands Big-O proofs, edge cases, and hardware-level mechanics.
- **Elena Rostova (Staff Tech Lead):** Pragmatic, emphasizes code safety, testing, maintainability, and production error resilience.
- **Marcus Sterling (Principal Architect):** Systems thinker, explores architectural decoupling, trade-offs, and scalability under ambiguity.

### 4. ⚡ Dynamic Adaptive Follow-up Probing
- When an answer is close but lacks depth or misses an edge case, the interviewer dynamically interrupts with a targeted follow-up question ("Building on what you said, how would you handle a sudden 100x traffic spike?").
- Evaluates follow-ups in-context and records them in the final evaluation transcript.

### 5. 💻 Rich Interactive Console UI
- Full ANSI color graphics, visual difficulty badges, score meters, and live progress bars.
- In-interview live commands:
  - `[answer text]` : Submit your technical answer.
  - `/hint` : Request a hint from the interviewer (slight calibration penalty).
  - `/clarify` : Request scope/constraint clarification.
  - `/stats` : View real-time Elo rating, round count, and topic averages.
  - `/skip` : Skip question if stuck.
  - `/finish` : Conclude early and generate reports.

### 6. 📊 Automated Dual Report Generation
At the end of every interview, two reports are generated in `reports/`:
1. **Markdown Report (`interview_report_<Name>.md`)**:
   - Executive Summary table with final Hiring Committee Verdict (`STRONG HIRE`, `HIRE`, `LEANING HIRE`, etc.).
   - Topic competency radar breakdown.
   - Complete round-by-round transcript with candidate answer, concept coverage, strengths, and critique.
   - Actionable study syllabus with tailored next steps.
2. **Interactive HTML Dashboard (`interview_report_<Name>.html`)**:
   - Modern glassmorphism dark UI with metric cards.
   - Visual topic competency progress bars.
   - Interactive question cards with color-coded concept pills (`✔ Covered`, `✘ Missed`, `★ Advanced`).
   - Printable and shareable.

---

## 📁 Repository Structure

```
c:\Chatbot
├── include/
│   ├── common.hpp              # Core data models, Enums, Question & Evaluation structures
│   ├── ansi_terminal.hpp       # Windows console UTF-8 setup, ANSI styling & visual widgets
│   ├── interviewer_persona.hpp # Interviewer personas, dynamic reactions & speech generators
│   ├── question_bank.hpp       # Comprehensive question bank across all tracks and difficulties
│   ├── adaptive_engine.hpp     # Elo rating calculator, NLP concept matcher, depth analyzer
│   ├── report_generator.hpp    # Markdown and interactive HTML report generation engines
│   └── interview_engine.hpp    # Session state machine, interactive loop & demo benchmarks
├── src/
│   └── main.cpp                # Application entry point & command line parser
├── reports/                    # Output directory for generated .md and .html reports
├── build.bat                   # 1-click build script for MSVC (cl.exe)
├── run.bat                     # 1-click launch script
└── README.md                   # Full documentation and usage guide
```

---

## 🚀 Quick Start & Build Instructions

### Prerequisites
- Windows 10/11
- Visual Studio Build Tools / MSVC (2019/2022/2026) with C++17 support

### Compiling
Run the automated build batch file:
```cmd
build.bat
```
This automatically locates your MSVC environment (`vcvars64.bat`) and builds with optimization:
```cmd
cl /nologo /std:c++17 /EHsc /O2 /W4 /utf-8 /DNOMINMAX /DWIN32_LEAN_AND_MEAN /Iinclude src\main.cpp /Fe:MockInterviewBot.exe
```

---

## 🎮 Running the Simulation

### 1. Interactive Interview Mode
Launch the bot to interview interactively with the AI persona:
```cmd
run.bat
```
*(or run `.\MockInterviewBot.exe` directly)*

You will be prompted to:
1. Enter your name
2. Select your interviewer persona (Alex, Elena, or Marcus)
3. Select your interview track (Modern C++, DSA, System Design, Behavioral, or Full Mock Loop)
4. Answer questions while monitoring your live adaptive score and Elo rating!

### 2. Automated Benchmark Simulations
Run automated simulations across any track to test evaluation and report generation:
```cmd
# Modern C++ & Systems Track
.\MockInterviewBot.exe --demo

# Data Structures & Algorithms Track
.\MockInterviewBot.exe --demo dsa

# System Design & Scalability Track
.\MockInterviewBot.exe --demo sys

# Behavioral & Leadership Track
.\MockInterviewBot.exe --demo beh
```

---

## 📈 Sample Generated Reports
After completion, open the generated HTML report in any web browser:
```cmd
start reports/interview_report_Candidate_Demo.html
```
Or view the Markdown transcript:
```cmd
type reports/interview_report_Candidate_Demo.md
```
