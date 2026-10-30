#pragma once

#include "common.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <random>

namespace MockInterview {

class QuestionBank {
private:
    std::vector<Question> questions;

public:
    QuestionBank() {
        populateQuestions();
    }

    const std::vector<Question>& getAllQuestions() const {
        return questions;
    }

    std::vector<Question> getQuestionsByTrack(Track track) const {
        std::vector<Question> result;
        for (const auto& q : questions) {
            if (track == Track::FULL_MOCK_LOOP || q.track == track) {
                result.push_back(q);
            }
        }
        return result;
    }

    std::vector<Question> getQuestionsByTrackAndDifficulty(Track track, Difficulty diff) const {
        std::vector<Question> result;
        for (const auto& q : questions) {
            bool trackMatch = (track == Track::FULL_MOCK_LOOP || q.track == track);
            if (trackMatch && q.difficulty == diff) {
                result.push_back(q);
            }
        }
        return result;
    }

private:
    void populateQuestions() {
        // =========================================================================
        // TRACK 1: MODERN C++ & LOW-LEVEL SYSTEMS PROGRAMMING
        // =========================================================================

        // CPP-1: EASY
        questions.push_back({
            "CPP_EASY_1",
            Track::MODERN_CPP,
            Difficulty::EASY,
            "Resource Management (RAII)",
            "RAII and Smart Pointers vs Raw Pointers",
            "Explain the concept of RAII (Resource Acquisition Is Initialization) in C++. "
            "How does std::unique_ptr enforce exclusive ownership compared to raw pointers, and what is its runtime overhead?",
            "Think about destructor invocation when going out of scope, delete operator, and zero-overhead abstractions.",
            "Assume C++11 and later. Focus on single-object ownership semantics and stack unwinding.",
            {"raii", "destructor", "unique_ptr", "ownership", "scope", "delete", "zero overhead"},
            {"custom deleter", "stack unwinding", "noexcept", "move only", "make_unique"},
            {"How does std::unique_ptr behave if you attempt to copy it instead of moving it?",
             "What happens if an exception is thrown before the unique_ptr goes out of scope?"},
            "RAII ties resource lifecycle to object lifetime. When an object is constructed on the stack, it acquires resources; "
            "its destructor guarantees deterministic release upon leaving scope (even during exceptions). std::unique_ptr provides "
            "zero-cost exclusive ownership by deleting its copy constructor and only enabling move semantics. In typical implementations, "
            "it has zero memory and CPU overhead compared to a raw pointer.",
            "Forgetting to mention exception safety/stack unwinding, or claiming unique_ptr has high memory overhead."
        });

        // CPP-2: MEDIUM
        questions.push_back({
            "CPP_MED_1",
            Track::MODERN_CPP,
            Difficulty::MEDIUM,
            "Memory Management & Semantics",
            "Move Semantics, Rvalues, and std::forward",
            "Explain the difference between lvalues and rvalues in Modern C++. "
            "What problem does std::move solve, and how does perfect forwarding with std::forward<T> prevent unnecessary copies in templates?",
            "Consider value categories (lvalue, prvalue, xvalue), universal/forwarding references (T&&), and std::remove_reference.",
            "Focus on standard C++14/17 semantics and function template argument deduction.",
            {"move semantics", "lvalue", "rvalue", "std::move", "std::forward", "perfect forwarding", "universal reference", "cast"},
            {"xvalue", "prvalue", "reference collapsing", "noexcept move", "steals resource"},
            {"Why should a move constructor almost always be marked noexcept, particularly when interacting with std::vector?",
             "Does std::move actually move any bytes or allocate anything at runtime?"},
            "Lvalues have identifiable memory addresses and persistent identity, while rvalues are temporary values or expiring objects (xvalues). "
            "std::move is an unconditional cast to an rvalue reference (static_cast<T&&>), enabling resources (like heap buffers) to be pilfered "
            "rather than duplicated. In templates, universal references (T&&) combine with reference collapsing rules (e.g., & + && -> &). "
            "std::forward<T> selectively casts to rvalue only if the argument was passed as an rvalue, preserving exact value category.",
            "Thinking std::move performs actual data transfer at runtime rather than being a compile-time static_cast."
        });

        // CPP-3: HARD
        questions.push_back({
            "CPP_HARD_1",
            Track::MODERN_CPP,
            Difficulty::HARD,
            "Object Model & Polymorphism",
            "Virtual Method Table (vtable), Memory Layout & Destructors",
            "Describe the internal memory layout of a C++ polymorphic class. "
            "How does dynamic dispatch work via vtable and vptr? Why is a non-virtual destructor in a base class dangerous when deleting via base pointer?",
            "Think about pointer offsets, hidden vptr pointer added to the object, multiple inheritance, and undefined behavior.",
            "Assume standard 64-bit architecture with common Itanium/MSVC ABI layout.",
            {"vtable", "vptr", "dynamic dispatch", "virtual destructor", "undefined behavior", "offset", "memory layout", "slicing"},
            {"multiple inheritance", "thunk", "typeinfo", "rtti", "cache miss", "indirect call"},
            {"What happens to the object size when the first virtual function is added to an empty class?",
             "How does multiple inheritance alter the number of vptrs inside a derived instance?"},
            "When a class declares a virtual function, the compiler generates a virtual method table (vtable) storing function pointers and embeds "
            "a virtual pointer (vptr) inside each object instance (typically taking 8 bytes on 64-bit systems). During a call (e.g. ptr->func()), "
            "an indirect call jumps to vptr[index]. If a base class destructor is non-virtual and an object is deleted through Base*, "
            "only Base's destructor executes, leaking derived resources and invoking undefined behavior.",
            "Confusing compile-time overloading with runtime dynamic dispatch, or forgetting the vptr memory footprint."
        });

        // CPP-4: EXPERT
        questions.push_back({
            "CPP_EXP_1",
            Track::MODERN_CPP,
            Difficulty::EXPERT,
            "Concurrency & Memory Model",
            "C++ Memory Ordering, Atomics and Cache Coherence",
            "Explain the difference between std::memory_order_relaxed, std::memory_order_acquire, std::memory_order_release, and std::memory_order_seq_cst. "
            "How does hardware-level cache line false sharing occur, and how do you resolve it in high-performance C++ multithreading?",
            "Think about happens-before relationships, synchronization fences, CPU store buffers, cache line bouncing (MESI protocol), and alignas(64).",
            "Focus on the C++11 memory model and modern x86/ARM multi-core architectures.",
            {"memory_order_acquire", "memory_order_release", "relaxed", "seq_cst", "false sharing", "cache line", "alignas", "atomic", "happens-before"},
            {"mesi", "hardware fences", "store buffer", "instruction reordering", "lock-free", "double-checked locking"},
            {"Why is x86 TSO (Total Store Order) often hiding acquire-release bugs that immediately blow up on weakly-ordered ARM chips?",
             "What is the exact cache line size on modern consumer x86_64 CPUs, and how do you align a struct to avoid cache contention?"},
            "Acquire-release semantics guarantee that memory writes before a release operation become visible to any thread completing a matching acquire operation, "
            "preventing compiler/CPU instruction reordering across the boundary without the heavy penalty of sequential consistency (seq_cst). "
            "False sharing occurs when independent threads concurrently modify distinct variables that reside within the same 64-byte L1 CPU cache line, "
            "causing repeated invalidation and cache ping-ponging via MESI/MOESI cache coherence protocols. It is resolved by padding variables or using alignas(hardware_destructive_interference_size).",
            "Assuming relaxed atomics synchronize other memory operations, or not knowing cache lines are typically 64 bytes."
        });

        // =========================================================================
        // TRACK 2: DATA STRUCTURES & ALGORITHMS & PROBLEM SOLVING
        // =========================================================================

        // DSA-1: EASY
        questions.push_back({
            "DSA_EASY_1",
            Track::ALGORITHMS_DSA,
            Difficulty::EASY,
            "Hash Tables & Complexity",
            "Hash Map Collision Resolution & Amortized Complexity",
            "How does a Hash Table achieve O(1) average time complexity? "
            "Compare Separate Chaining with Open Addressing (e.g. Linear Probing), and explain what causes worst-case O(N) performance.",
            "Discuss hash functions, uniform distribution, load factor, rehash / resize, and hash collisions.",
            "Assume a standard key-value map implementation like std::unordered_map.",
            {"hash function", "collision", "separate chaining", "open addressing", "load factor", "amortized", "o(1)", "bucket"},
            {"robin hood hashing", "tombstone", "rehash", "quadratic probing", "worst case o(n)"},
            {"What happens to lookup performance if every single inserted key produces the exact same hash code?",
             "How does resizing affect the capacity and what is the amortized cost per insertion?"},
            "A hash function maps keys to array bucket indices. Separate chaining stores a linked list (or red-black tree) at each bucket, "
            "whereas Open Addressing stores items directly in array slots, probing forward upon collision. Average lookup is O(1) assuming "
            "a uniform hash and low load factor (typically 0.7-0.75). When load factor exceeds threshold, a rehash doubles bucket capacity. "
            "Worst-case O(N) occurs when catastrophic collisions degrade all buckets into a single linear chain.",
            "Not explaining load factor or failing to contrast chaining with open addressing."
        });

        // DSA-2: MEDIUM
        questions.push_back({
            "DSA_MED_1",
            Track::ALGORITHMS_DSA,
            Difficulty::MEDIUM,
            "Graph Algorithms & Traversals",
            "Shortest Path: Dijkstra's Algorithm vs Breadth-First Search (BFS)",
            "Under what conditions would you choose BFS over Dijkstra's Algorithm to find the shortest path in a graph? "
            "What is the time complexity of Dijkstra when implemented with a min-priority queue (binary heap vs Fibonacci heap), and how do you handle negative edge weights?",
            "Think about unweighted vs weighted graphs, edge relaxations, greedy choices, and why Dijkstra fails on negative cycles (Bellman-Ford).",
            "State vertices as V and edges as E in adjacency list representation.",
            {"bfs", "dijkstra", "priority queue", "unweighted", "weighted", "o((v + e) log v)", "negative weight", "bellman-ford"},
            {"greedy", "relaxation", "fibonacci heap", "dag topological", "shortest path tree"},
            {"Why can't Dijkstra simply add a large positive constant to all edge weights to fix negative edge weights?",
             "In an unweighted grid with millions of nodes, how does bidirectional BFS improve search space?"},
            "BFS finds the shortest path in unweighted graphs in linear O(V + E) time by expanding uniformly level-by-level. "
            "Dijkstra is required for non-negatively weighted graphs, using a min-heap to greedily expand the closest unvisited vertex in O((V + E) log V) time. "
            "Dijkstra cannot handle negative edge weights because its greedy invariant assumes once a vertex is visited, its distance is final; "
            "graphs with negative edge weights require Bellman-Ford (O(V * E)) or SPFA.",
            "Believing Dijkstra works on negative edges or claiming BFS works correctly on non-uniform edge weights."
        });

        // DSA-3: HARD
        questions.push_back({
            "DSA_HARD_1",
            Track::ALGORITHMS_DSA,
            Difficulty::HARD,
            "Advanced System & Algorithm Design",
            "LRU Cache: O(1) Get and Put Design",
            "Design an LRU (Least Recently Used) Cache with strict O(1) time complexity for both get(key) and put(key, value) operations. "
            "Describe the exact data structures you would combine, how node eviction works at capacity, and how you would make it thread-safe.",
            "Consider combining a doubly linked list with a hash map, pointer references, node splicing, and lock granularity.",
            "Target standard capacity N, avoiding O(N) shifts or scans.",
            {"doubly linked list", "hash map", "o(1)", "eviction", "head", "tail", "mutex", "splice", "concurrency"},
            {"dummy head tail", "shared_mutex", "read write lock", "fine grained locking", "lru vs lfu"},
            {"Why is a singly linked list insufficient to achieve true O(1) removal of an arbitrary node in an LRU?",
             "How would a read-write lock (std::shared_mutex) improve concurrent read throughput?"},
            "An LRU cache combines a Doubly Linked List with a Hash Map. The Doubly Linked List maintains access recency order (most recently used at head, "
            "least recently used at tail with dummy sentinel nodes). The Hash Map maps key -> DoublyLinkedList::NodeIterator/Pointer, allowing O(1) lookup. "
            "On get(key), we find the node in O(1) and splice it to the head in O(1). On put(key, value), we update or insert at head; if capacity is exceeded, "
            "we evict the tail node in O(1) and erase its key from the hash map. Thread safety can use std::shared_mutex or segmented striped locks.",
            "Attempting to use a single array or queue which requires O(N) shifting on cache hits."
        });

        // DSA-4: EXPERT
        questions.push_back({
            "DSA_EXP_1",
            Track::ALGORITHMS_DSA,
            Difficulty::EXPERT,
            "High Scale Streaming & Algorithms",
            "Streaming Top-K & Heavy Hitters at Scale",
            "Imagine a real-time stream of 100 million incoming events per second with billions of unique keys. "
            "You need to compute the Top-K most frequent items in a sliding window with bounded memory. "
            "Compare an exact Min-Heap approach with approximate probabilistic algorithms like Count-Min Sketch or Space-Saving (Lossy Counting).",
            "Discuss memory constraints, hash collisions, sub-linear space, frequency estimation bounds, and CMS depth/width (epsilon, delta).",
            "Assume memory is strictly limited to 64MB, preventing a complete hash map of all keys.",
            {"count-min sketch", "heavy hitters", "min-heap", "probabilistic", "space-saving", "hash functions", "bounded memory", "sliding window"},
            {"hyperloglog", "lossy counting", "count sketch", "point query", "stream processing", "turnstile model"},
            {"How do you mathematically determine the number of hash functions (depth) and table width (columns) for an error rate epsilon with probability 1 - delta?",
             "How do you prune old counts when sliding the time window forward in a real-time stream?"},
            "An exact hash map + min-heap requires O(U) space where U is unique keys, blowing past memory limits under high cardinality. "
            "Probabilistic streaming algorithms solve this in O(log(1/delta) / epsilon) space. Count-Min Sketch utilizes a 2D array of d hash functions across w buckets; "
            "updates increment d cells, and queries return the minimum counter to eliminate negative errors. Combined with a bounded min-heap tracking candidate heavy hitters "
            "or the Space-Saving algorithm (Misra-Gries generalization), we guarantee epsilon-accurate Top-K retrieval within small megabytes of RAM.",
            "Suggesting an exact in-memory hash map without addressing the multi-gigabyte memory explosion."
        });

        // =========================================================================
        // TRACK 3: SYSTEM DESIGN & SCALABILITY
        // =========================================================================

        // SYS-1: EASY
        questions.push_back({
            "SYS_EASY_1",
            Track::SYSTEM_DESIGN,
            Difficulty::EASY,
            "High Availability & Scaling Fundamentals",
            "Horizontal vs Vertical Scaling and Load Balancing",
            "Explain the difference between horizontal and vertical scaling. "
            "What role does a Load Balancer play in a distributed system, and how do Round Robin, Least Connections, and IP Hash algorithms differ?",
            "Consider single point of failure (SPOF), stateless web tiers, session stickiness, and health checks.",
            "Focus on web architecture scaling from a single server to a multi-instance cluster.",
            {"horizontal scaling", "vertical scaling", "load balancer", "round robin", "least connections", "stateless", "health checks", "single point of failure"},
            {"consistent hashing", "layer 4 vs layer 7", "sticky session", "dns load balancing", "auto scaling"},
            {"Why is maintaining session state in server memory an anti-pattern for horizontal auto-scaling?",
             "What happens when a backend instance fails health checks?"},
            "Vertical scaling increases CPU/RAM on a single machine (bounded by hardware limits and SPOF risk). Horizontal scaling adds more independent "
            "nodes behind a Load Balancer (providing elasticity and high availability). Layer 4/Layer 7 Load Balancers distribute ingress traffic using algorithms "
            "like Round Robin (simple equal turn), Least Connections (routes to lowest active workload), or IP Hash (session stickiness). Horizontal scaling requires "
            "a stateless application tier, delegating session persistence to shared stores like Redis.",
            "Failing to mention that horizontal scaling requires stateless app servers or ignoring health check mechanics."
        });

        // SYS-2: MEDIUM
        questions.push_back({
            "SYS_MED_1",
            Track::SYSTEM_DESIGN,
            Difficulty::MEDIUM,
            "Distributed Caching & Concurrency",
            "Caching Strategies & Preventing Cache Stampede",
            "Compare Cache-Aside, Write-Through, and Write-Back caching patterns. "
            "What is a Cache Stampede (Thundering Herd) when a hot key expires, and what architectural mechanisms prevent it under high concurrency?",
            "Think about cache hit/miss flows, DB write bottlenecks, distributed mutex/locks, SingleFlight pattern, and probabilistic early expiration (XFetch).",
            "Assume a high-traffic e-commerce system with 50,000 requests/sec reading identical hot catalog records.",
            {"cache-aside", "write-through", "write-back", "cache stampede", "thundering herd", "mutex", "distributed lock", "ttl", "singleflight"},
            {"xfetch", "probabilistic early expiration", "redis", "read-through", "cache penetration", "bloom filter"},
            {"How does probabilistic early expiration (XFetch algorithm) prevent cache stampede without requiring a distributed lock?",
             "What risk does Write-Back caching introduce during unexpected node crashes?"},
            "In Cache-Aside, the application queries cache; on miss, it loads from DB and backfills cache. In Write-Through, writes update cache and DB synchronously. "
            "In Write-Back, writes update cache immediately and asynchronously drain to DB (high write speed, but risk of data loss on crash). "
            "A Cache Stampede occurs when a high-traffic hot key expires, causing thousands of concurrent requests to simultaneously hammer the database. "
            "Mitigations include: (1) Distributed Mutex (only 1 thread regenerates cache while others wait), (2) SingleFlight in-process deduplication, "
            "and (3) Probabilistic early expiration (as TTL approaches zero, background workers pre-fetch data before expiration).",
            "Claiming Write-Through is faster for writes than Write-Back, or having no solution for thundering herd."
        });

        // SYS-3: HARD
        questions.push_back({
            "SYS_HARD_1",
            Track::SYSTEM_DESIGN,
            Difficulty::HARD,
            "Database Storage Engines & Indexing",
            "B+ Tree vs Log-Structured Merge (LSM) Tree Storage Engines",
            "Compare B+ Tree storage engines (PostgreSQL, MySQL InnoDB) with Log-Structured Merge (LSM) Trees (RocksDB, Cassandra, ScyllaDB). "
            "Why are LSM trees superior for write-heavy workloads, and what are their read trade-offs (Bloom Filters, Compaction)?",
            "Discuss random I/O vs sequential write append, write amplification, SSTables, MemTable, Write-Ahead Log (WAL), and compaction overhead.",
            "Focus on SSD/NVMe read/write characteristics and page splits.",
            {"b+ tree", "lsm tree", "sstable", "memtable", "wal", "compaction", "sequential write", "random i/o", "bloom filter", "write amplification"},
            {"write-ahead log", "innodb", "rocksdb", "level-based compaction", "point lookup", "page split"},
            {"Why does a B+ Tree suffer high write amplification on random inserts compared to an append-only WAL + MemTable?",
             "How do Bloom Filters rescue read performance in an LSM tree across multiple SSTables?"},
            "B+ Trees maintain balanced page hierarchies on disk; random inserts cause costly in-place page updates, random disk I/O, and page splits. "
            "LSM Trees optimize writes by appending sequentially to a Write-Ahead Log (WAL) and buffering in an in-memory sorted MemTable (SkipList/RB-Tree). "
            "Once full, MemTable is flushed as an immutable SSTable on disk in sequential I/O. However, reads must search MemTable and multiple SSTable levels. "
            "To mitigate read amplification, LSM engines use Bloom Filters to skip SSTables that definitely don't contain the key and perform background Compaction "
            "to merge sorted runs, trading CPU/disk bandwidth for fast O(1) writes.",
            "Not understanding that LSM writes are sequential appends or missing the role of Bloom filters in read mitigation."
        });

        // SYS-4: EXPERT
        questions.push_back({
            "SYS_EXP_1",
            Track::SYSTEM_DESIGN,
            Difficulty::EXPERT,
            "Distributed Consensus & Architecture",
            "Distributed Rate Limiter at 10 Million RPS & Race Conditions",
            "Design a globally distributed Rate Limiter handling 10,000,000 requests per second across multiple data centers. "
            "Evaluate Token Bucket, Leaky Bucket, and Sliding Window Counter. How do you handle race conditions, synchronization latency between regions, and Redis cluster limits?",
            "Discuss local memory token pre-allocation (batching), eventual consistency vs strict limits, Redis Lua scripts, token synchronization overhead, and fallback modes.",
            "Consider cross-continental network latency (100ms) making synchronous cross-DC locking impossible.",
            {"token bucket", "sliding window counter", "race condition", "redis", "lua script", "batching", "eventual consistency", "multi-region", "rate limit"},
            {"local token bucket", "redis cluster", "leaky bucket", "single point of failure", "fail open vs fail close", "clock drift"},
            {"Why does calling Redis INCR and EXPIRE in two separate network trips introduce a race condition and potential key leakage?",
             "How does a local token batching algorithm (pre-fetching 1000 tokens locally) reduce central Redis load by 99.9%?"},
            "At 10M RPS, making a remote Redis round-trip for every request incurs massive latency and saturates the cluster. "
            "The architecture uses a two-tier hybrid model: Edge instances run local Token Buckets in C++ memory that reserve batches of tokens "
            "(e.g. 500-1000 tokens) asynchronously from regional Redis clusters using atomic Lua scripts or Redis cell. "
            "Between global data centers, regional rate limits are statically or dynamically allocated (e.g. 40% US-East, 30% US-West, 30% EU), "
            "synchronizing aggregate consumption every 500ms via gossip or Kafka. If the cache layer degrades, the system defaults to 'fail-open' "
            "to protect availability while logging anomalies.",
            "Proposing a single centralized Redis instance with synchronous per-request round-trips for 10M RPS."
        });

        // =========================================================================
        // TRACK 4: BEHAVIORAL & ENGINEERING LEADERSHIP (STAR METHOD)
        // =========================================================================

        // BEH-1: EASY
        questions.push_back({
            "BEH_EASY_1",
            Track::BEHAVIORAL,
            Difficulty::EASY,
            "Collaboration & Technical Disagreements",
            "Resolving an Architectural Disagreement in a Team",
            "Tell me about a time you had a strong technical disagreement with a peer or lead on a design or technology choice. "
            "Walk me through the Situation, Task, Action, and Result (STAR). How did you prevent personal friction and reach an aligned decision?",
            "Use the STAR framework explicitly: Situation, Task, Action, Result. Highlight data-driven evaluation, benchmarks, or prototypes.",
            "Focus on professional collaboration, empathy, and 'disagree and commit' principles.",
            {"situation", "task", "action", "result", "disagreement", "trade-offs", "benchmark", "compromise", "prototype", "alignment"},
            {"disagree and commit", "rfc", "data driven", "post-mortem", "mentorship", "empathy"},
            {"What would you do if after presenting your benchmark data, the lead still insisted on their approach?",
             "How did you verify that the chosen solution met production SLAs after deployment?"},
            "A structured STAR response: Situation (a concrete engineering project facing a bifurcation between two solutions, e.g. gRPC vs REST), "
            "Task (responsible for ensuring high throughput and team velocity), Action (de-escalated emotional bias by proposing an objective rubric, "
            "built a 2-day proof-of-concept benchmark measuring latency and serialization overhead, documented findings in an RFC), "
            "Result (aligned the team around empirical data, shipped on time, established a reusable design review standard).",
            "Saying 'I convinced them I was right' without showing collaborative listening, data-driven prototyping, or compromise."
        });

        // BEH-2: MEDIUM
        questions.push_back({
            "BEH_MED_1",
            Track::BEHAVIORAL,
            Difficulty::MEDIUM,
            "Incident Management & Resilience",
            "Handling a High-Severity Production Incident (P0)",
            "Describe a time when a critical bug or outage occurred under your watch in production. "
            "How did you triage the issue, communicate with stakeholders during the crisis, and lead the subsequent blameless post-mortem?",
            "Focus on containment first (rollback, traffic reroute) before debugging, blameless culture, 5 Whys, and systemic remediation.",
            "Structure using the STAR framework.",
            {"situation", "task", "action", "result", "incident", "rollback", "mitigation", "post-mortem", "blameless", "root cause", "monitoring"},
            {"5 whys", "blast radius", "telemetry", "circuit breaker", "runbook", "action items"},
            {"During an active outage, how do you decide whether to roll back immediately versus attempting a live patch?",
             "What concrete automated guardrail did you install to guarantee that specific bug never recurs?"},
            "STAR outline: Situation (a P0 outage triggered by an unhandled edge case or connection pool exhaustion causing error spikes), "
            "Task (triage and mitigate immediately while keeping executive stakeholders informed), Action (prioritized blast radius containment "
            "by triggering automated rollback instead of risky live hotpatching, communicated on status channels with clear ETAs, collected core dumps and logs), "
            "Result (restored service in 14 minutes, conducted a blameless post-mortem using the '5 Whys', added synthetic canary testing and automated rate limiting).",
            "Focusing on finger-pointing or diving into hours of live root-cause debugging while users are still experiencing downtime."
        });

        // BEH-3: HARD
        questions.push_back({
            "BEH_HARD_1",
            Track::BEHAVIORAL,
            Difficulty::HARD,
            "Technical Debt & Delivery Pressure",
            "Balancing Ruthless Deadlines with Technical Debt & Quality",
            "Tell me about a project where aggressive product deadlines collided with significant technical debt or architectural shortcuts. "
            "How did you negotiate scope with product managers, make deliberate architectural trade-offs, and pay down the debt later?",
            "Showcase negotiation, risk quantification, feature flags, phased delivery, and technical debt tracking.",
            "Highlight business empathy while safeguarding system reliability.",
            {"situation", "task", "action", "result", "deadline", "technical debt", "trade-off", "scope", "product manager", "risk", "feature flag"},
            {"phased rollout", "sla", "debt sprint", "refactor", "scalability", "business impact"},
            {"How do you communicate the risk of technical debt in dollar or SLA terms that non-technical business leaders understand?",
             "How did you ensure that 'temporary' shortcuts didn't become permanent production liabilities?"},
            "STAR outline: Situation (a major market-driven release date with high revenue impact clashed with legacy brittle codebase), "
            "Task (deliver critical customer value without triggering cascading outages or burning out the team), Action (quantified risks into product terms "
            "— uptime and churn risk — negotiated MVP scope reduction, isolated technical shortcuts behind explicit abstraction interfaces and feature flags, "
            "and scheduled a dedicated 2-week hardening sprint right after launch), Result (hit the critical market milestone, subsequent refactoring took 60% less time "
            "due to modular boundaries, and team velocity increased by 25%).",
            "Refusing to compromise with product, or conversely, accumulating toxic tech debt with zero plan or interfaces to refactor."
        });
    }
};

} // namespace MockInterview
