#pragma once

#include "common.hpp"
#include "ansi_terminal.hpp"
#include "interviewer_persona.hpp"
#include "question_bank.hpp"
#include "adaptive_engine.hpp"
#include "report_generator.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <chrono>
#include <algorithm>
#include <memory>

namespace MockInterview {

class InterviewEngine {
private:
    SessionStatistics stats;
    QuestionBank bank;
    AdaptiveEngine adaptive;
    std::unique_ptr<InterviewerPersona> persona;
    std::set<std::string> askedQuestionIds;
    bool enableTypewriter = true;

public:
    InterviewEngine(bool typewriterAnimation = true) 
        : adaptive(1200), enableTypewriter(typewriterAnimation) {
        stats.start_time = getCurrentTimestampString();
        stats.starting_elo = 1200;
        stats.current_elo = 1200;
        stats.peak_elo = 1200;
    }

    void run() {
        Terminal::clearScreen();
        Terminal::printBanner();

        // 1. Candidate Setup
        setupCandidate();

        // 2. Interviewer Persona Selection
        setupPersona();

        // 3. Track Selection
        setupTrack();

        // 4. Calibration & Greeting
        startSession();

        // 5. Main Interview Loop
        interviewLoop();

        // 6. Wrap Up & Report Generation
        concludeSession();
    }

    // Automated Demo Mode for immediate validation and demonstration
    void runAutomatedDemo(Track track = Track::MODERN_CPP, PersonaType personaType = PersonaType::BAR_RAISER) {
        Terminal::clearScreen();
        Terminal::printBanner();

        stats.candidate_name = "Candidate Demo";
        stats.track = track;
        persona = std::make_unique<InterviewerPersona>(personaType);
        stats.persona = personaType;
        enableTypewriter = false;

        std::cout << Terminal::Color::BRIGHT_GREEN << Terminal::Color::BOLD
                  << ">>> RUNNING AUTOMATED AI MOCK INTERVIEW BENCHMARK SIMULATION <<<\n"
                  << Terminal::Color::RESET;
        std::cout << "Track: " << trackToString(track) << " | Persona: " << personaName(personaType) << "\n\n";

        // Simulated questions & realistic technical answers demonstrating adaptive difficulty scaling
        struct DemoTurn {
            std::string answer;
            bool useHint;
        };

        std::map<std::string, std::string> demoAnswerMap = {
            // Modern C++
            {"CPP_EASY_1", "RAII binds resource allocation to object lifetime. When unique_ptr is constructed on the stack, "
                           "its destructor automatically triggers delete when leaving scope, ensuring exception safety and stack unwinding. "
                           "std::unique_ptr enforces exclusive ownership by deleting its copy constructor and only allowing move semantics. "
                           "It has zero runtime overhead compared to a raw pointer and supports custom deleters."},
            {"CPP_MED_1", "Lvalues have persistent memory addresses, while rvalues are temporary expressions or expiring objects (xvalues). "
                          "std::move is an unconditional static_cast<T&&> to an rvalue reference that enables resource stealing instead of copying. "
                          "In templates, universal references (T&&) combine with reference collapsing rules. std::forward<T> implements perfect forwarding, "
                          "casting to rvalue only if the original caller supplied an rvalue."},
            {"CPP_HARD_1", "Dynamic dispatch uses a virtual method table (vtable) and a hidden vptr pointer inside the object memory layout. "
                           "When ptr->func() is called, it performs an indirect function call indexed through vptr with an offset. "
                           "A non-virtual destructor in a base class leads to undefined behavior, object slicing, and resource leaks when deleted via base pointer, "
                           "because only the base destructor is invoked instead of dynamic dispatch to the derived destructor."},
            {"CPP_EXP_1", "std::memory_order_acquire and std::memory_order_release establish a synchronized happens-before relationship "
                          "without the full sequential consistency penalty of seq_cst, while relaxed atomics only guarantee atomicity. "
                          "False sharing occurs when multiple threads concurrently write to distinct variables located on the exact same 64-byte CPU cache line, "
                          "causing severe MESI cache invalidation ping-pong. We resolve it using alignas(64) or hardware_destructive_interference_size padding."},
            // DSA
            {"DSA_EASY_1", "A hash map uses a hash function to map keys to bucket indices, achieving average O(1) amortized lookup. "
                           "Separate chaining stores a linked list at each bucket, while open addressing uses linear probing in array slots. "
                           "When the load factor exceeds 0.75, a rehash doubles bucket capacity. Catastrophic collisions degrade lookup to O(N)."},
            {"DSA_MED_1", "BFS finds the shortest path in unweighted graphs in linear O(V + E) time. "
                          "Dijkstra's algorithm is required for weighted graphs with non-negative edges, using a priority queue min-heap in O((V + E) log V) time. "
                          "Dijkstra cannot handle negative weight edges because its greedy relaxation invariant is violated; Bellman-Ford is needed instead."},
            {"DSA_HARD_1", "An LRU cache uses a Doubly Linked List paired with a Hash Map to achieve strict O(1) get and put operations. "
                           "The doubly linked list maintains recency with dummy head and tail sentinel nodes. The hash map stores key to node pointers. "
                           "On get, the node is spliced to head in O(1). On put, if capacity is reached, we evict the tail node in O(1). "
                           "For thread safety, we can use std::shared_mutex or fine-grained bucket locks."},
            {"DSA_EXP_1", "Exact in-memory hash maps and min-heaps exceed RAM limits at 100M events/sec. "
                          "We utilize a Count-Min Sketch with bounded memory and d independent hash functions to estimate frequency, "
                          "paired with a bounded min-heap tracking candidate heavy hitters or Space-Saving lossy counting for sub-linear Top-K streaming."},
            // System Design
            {"SYS_EASY_1", "Horizontal scaling adds more instances behind a load balancer, while vertical scaling upgrades CPU and RAM on a single machine. "
                           "Load balancers use round robin or least connections with active health checks to eliminate single points of failure. "
                           "Horizontal auto-scaling requires stateless application servers, offloading session state to a distributed store like Redis."},
            {"SYS_MED_1", "Cache-aside reads from cache and backfills from DB on miss. Write-through updates cache and DB synchronously. "
                          "Write-back buffers writes in cache and asynchronously flushes to DB. "
                          "A cache stampede or thundering herd occurs when a high-traffic key expires, overwhelming the DB. "
                          "We mitigate this using distributed mutex locks, SingleFlight request deduplication, and probabilistic early expiration (XFetch)."},
            {"SYS_HARD_1", "B+ trees store balanced pages on disk and excel at point lookups and range queries, but suffer from random I/O and page splits on writes. "
                           "Log-Structured Merge (LSM) trees optimize for heavy writes by sequentially appending to a Write-Ahead Log (WAL) and buffering in an in-memory MemTable. "
                           "Flushed immutable SSTables are written sequentially. To speed up reads, LSM trees use Bloom Filters and background compaction to merge runs."},
            {"SYS_EXP_1", "At 10M RPS, centralized synchronous Redis round-trips cause severe network bottlenecks and race conditions. "
                          "We implement a two-tier hybrid model: local token buckets in server memory pre-fetch token batches asynchronously using Redis Lua scripts. "
                          "Between multi-region data centers, limits synchronize via eventual consistency with gossip or Kafka, defaulting to fail-open under failure."},
            // Behavioral
            {"BEH_EASY_1", "Situation: At my previous team, we faced a major disagreement on migrating our backend to gRPC vs REST. "
                           "Task: As senior engineer, I needed to resolve the technical impasse and align the team without emotional friction. "
                           "Action: I drafted an objective RFC rubric, built a 2-day prototype benchmark measuring serialization latency and throughput, and shared data openly. "
                           "Result: The team unanimously agreed with the benchmark findings, adopted gRPC, and cut inter-service latency by 45%."},
            {"BEH_MED_1", "Situation: A critical P0 incident caused connection pool exhaustion and elevated error rates in production. "
                          "Task: Rapidly contain the blast radius and restore system availability under strict SLAs. "
                          "Action: I initiated an immediate automated rollback rather than attempting risky live debugging, restored traffic within 11 minutes, "
                          "and conducted a blameless post-mortem using the 5 Whys to diagnose leak conditions. "
                          "Result: We deployed circuit breakers and automated canary thresholds that prevented any recurrence."},
            {"BEH_HARD_1", "Situation: Aggressive commercial deadlines collided with legacy technical debt on our primary billing pipeline. "
                           "Task: Deliver the Q4 enterprise feature set while mitigating catastrophic outage risks. "
                           "Action: I negotiated with the product manager by framing tech debt in business terms (churn and SLA risk), "
                           "isolated the legacy shortcuts behind explicit interface abstractions with feature flags, and scheduled a dedicated 2-week hardening sprint. "
                           "Result: We hit the revenue deadline on schedule, and subsequent feature velocity increased by 30%."}
        };

        std::cout << persona->getGreeting(stats.candidate_name, stats.track) << "\n\n";

        const size_t BENCHMARK_ROUNDS = 3;
        for (size_t i = 0; i < BENCHMARK_ROUNDS; ++i) {
            Difficulty currentDiff = adaptive.determineNextDifficulty();
            Question q = pickNextQuestion(currentDiff);

            std::cout << Terminal::Color::BRIGHT_BLUE << "────────────────────────────────────────────────────────────────────────\n" << Terminal::Color::RESET;
            std::cout << Terminal::Color::BRIGHT_CYAN << Terminal::Color::BOLD
                      << "ROUND " << (i + 1) << " | TOPIC: " << q.topic 
                      << " | DIFFICULTY: " << difficultyToString(q.difficulty) 
                      << " | ELO: " << adaptive.getCurrentElo() << "\n" << Terminal::Color::RESET;
            std::cout << Terminal::Color::BRIGHT_WHITE << Terminal::Color::BOLD << q.title << "\n\n" << Terminal::Color::RESET;
            std::cout << q.prompt << "\n\n";

            std::string answer = "I approach this from first principles, considering trade-offs, edge cases, and performance constraints.";
            auto it = demoAnswerMap.find(q.id);
            if (it != demoAnswerMap.end()) {
                answer = it->second;
            }

            std::cout << Terminal::Color::BRIGHT_YELLOW << ">>> Candidate Answer Submission:\n" << Terminal::Color::RESET;
            std::cout << answer << "\n\n";

            double responseTime = 28.0 + (i * 10.0);
            AnswerEvaluation eval = adaptive.evaluateAnswer(q, answer, responseTime, false, false, stats.persona);

            displayEvaluationResult(eval);
            recordEvaluation(eval);
        }

        concludeSession();
    }

private:
    void setupCandidate() {
        std::cout << Terminal::Color::BRIGHT_WHITE << Terminal::Color::BOLD 
                  << "Enter Candidate Name: " << Terminal::Color::RESET;
        std::string name;
        std::getline(std::cin, name);
        if (name.empty()) name = "Engineer";
        stats.candidate_name = name;
        std::cout << "\n";
    }

    void setupPersona() {
        Terminal::printDivider("SELECT INTERVIEWER PERSONA");
        std::cout << "1. " << Terminal::Color::BRIGHT_CYAN << "Alex Vance (FAANG Bar Raiser)" << Terminal::Color::RESET 
                  << " - Rigorous, edge cases, Big-O proofs, deep systems fundamentals\n";
        std::cout << "2. " << Terminal::Color::BRIGHT_GREEN << "Elena Rostova (Staff Tech Lead)" << Terminal::Color::RESET 
                  << " - Pragmatic, production maintainability, testing, real-world trade-offs\n";
        std::cout << "3. " << Terminal::Color::BRIGHT_YELLOW << "Marcus Sterling (Principal Architect)" << Terminal::Color::RESET 
                  << " - Exploratory, high-level system decoupling, architecture trade-offs\n\n";
        std::cout << "Enter selection (1-3, default 1): ";

        std::string input;
        std::getline(std::cin, input);
        int choice = 1;
        if (!input.empty()) {
            try { choice = std::stoi(input); } catch (...) { choice = 1; }
        }

        PersonaType pType = PersonaType::BAR_RAISER;
        if (choice == 2) pType = PersonaType::TECH_LEAD;
        else if (choice == 3) pType = PersonaType::SENIOR_ARCHITECT;

        persona = std::make_unique<InterviewerPersona>(pType);
        stats.persona = pType;
        std::cout << "\nSelected: " << Terminal::Color::BOLD << personaName(pType) << Terminal::Color::RESET << "\n\n";
    }

    void setupTrack() {
        Terminal::printDivider("SELECT INTERVIEW TRACK");
        std::cout << "1. " << Terminal::Color::BRIGHT_CYAN << "Modern C++ & Low-Level Systems" << Terminal::Color::RESET 
                  << " (RAII, Move Semantics, vtables, Atomics, Memory Model)\n";
        std::cout << "2. " << Terminal::Color::BRIGHT_GREEN << "Data Structures & Algorithms" << Terminal::Color::RESET 
                  << " (Hash Maps, Graphs/Dijkstra, LRU, Stream Top-K)\n";
        std::cout << "3. " << Terminal::Color::BRIGHT_YELLOW << "System Design & Scalability" << Terminal::Color::RESET 
                  << " (Load Balancing, Cache Stampede, B+ vs LSM, 10M RPS Rate Limiter)\n";
        std::cout << "4. " << Terminal::Color::BRIGHT_MAGENTA << "Behavioral & Leadership" << Terminal::Color::RESET 
                  << " (STAR Method, Conflict Resolution, P0 Incident, Tech Debt)\n";
        std::cout << "5. " << Terminal::Color::BRIGHT_WHITE << "Full Mock Loop (Comprehensive Mixed Tracks)" << Terminal::Color::RESET << "\n\n";
        std::cout << "Enter selection (1-5, default 1): ";

        std::string input;
        std::getline(std::cin, input);
        int choice = 1;
        if (!input.empty()) {
            try { choice = std::stoi(input); } catch (...) { choice = 1; }
        }

        switch (choice) {
            case 1: stats.track = Track::MODERN_CPP; break;
            case 2: stats.track = Track::ALGORITHMS_DSA; break;
            case 3: stats.track = Track::SYSTEM_DESIGN; break;
            case 4: stats.track = Track::BEHAVIORAL; break;
            case 5: stats.track = Track::FULL_MOCK_LOOP; break;
            default: stats.track = Track::MODERN_CPP; break;
        }

        std::cout << "\nTrack Activated: " << Terminal::Color::BOLD << trackToString(stats.track) << Terminal::Color::RESET << "\n\n";
    }

    void startSession() {
        Terminal::printDivider("INTERVIEW CALIBRATION");
        std::cout << Terminal::Color::BRIGHT_BLACK << "Starting Elo Rating: " 
                  << Terminal::Color::BRIGHT_CYAN << "1200" 
                  << Terminal::Color::BRIGHT_BLACK << " (Mid-Level Engineer Baseline)\n\n" 
                  << Terminal::Color::RESET;

        std::string greeting = persona->getGreeting(stats.candidate_name, stats.track);
        std::cout << Terminal::Color::BOLD << persona->avatar << ":\n" << Terminal::Color::RESET;
        Terminal::typeWriter("\"" + greeting + "\"\n\n", 4, enableTypewriter);

        std::cout << Terminal::Color::BRIGHT_BLACK 
                  << "Commands during questions: [your answer] | /hint | /clarify | /stats | /skip | /finish\n\n"
                  << Terminal::Color::RESET;

        std::cout << Terminal::Color::DIM << "Press Enter to start the first question..." << Terminal::Color::RESET;
        std::string dummy;
        std::getline(std::cin, dummy);
        std::cout << "\n";
    }

    void interviewLoop() {
        const int MAX_ROUNDS = 5;
        int roundCount = 0;

        while (roundCount < MAX_ROUNDS) {
            roundCount++;
            Difficulty targetDiff = adaptive.determineNextDifficulty();
            Question q = pickNextQuestion(targetDiff);

            Terminal::printDivider("ROUND " + std::to_string(roundCount) + " / " + std::to_string(MAX_ROUNDS));
            
            std::cout << Terminal::Color::BRIGHT_CYAN << "Topic: " << Terminal::Color::BOLD << q.topic 
                      << Terminal::Color::RESET << "  |  Difficulty: ";
            
            std::string diffColor = Terminal::Color::BRIGHT_GREEN;
            if (q.difficulty == Difficulty::MEDIUM) diffColor = Terminal::Color::BRIGHT_YELLOW;
            else if (q.difficulty == Difficulty::HARD) diffColor = Terminal::Color::BRIGHT_MAGENTA;
            else if (q.difficulty == Difficulty::EXPERT) diffColor = Terminal::Color::BRIGHT_RED;

            std::cout << diffColor << Terminal::Color::BOLD << "[" << difficultyToString(q.difficulty) << "]" 
                      << Terminal::Color::RESET << "  |  Current Elo: " << Terminal::Color::BRIGHT_WHITE 
                      << adaptive.getCurrentElo() << Terminal::Color::RESET << "\n\n";

            std::cout << Terminal::Color::BOLD << Terminal::Color::BRIGHT_WHITE << q.title << "\n" << Terminal::Color::RESET;
            std::cout << Terminal::Color::RESET << q.prompt << "\n\n";

            // Candidate Interaction Loop
            bool hintUsed = false;
            bool clarificationUsed = false;
            std::string finalAnswer = "";
            auto startTime = std::chrono::steady_clock::now();

            while (true) {
                std::cout << Terminal::Color::BRIGHT_GREEN << "Your Answer (/hint, /clarify, /finish): \n> " << Terminal::Color::RESET;
                std::string line;
                std::getline(std::cin, line);

                if (line == "/hint") {
                    hintUsed = true;
                    stats.hints_used_total++;
                    std::cout << "\n" << Terminal::Color::BRIGHT_YELLOW << persona->getHintResponse(q.hint) << "\n\n" << Terminal::Color::RESET;
                    continue;
                } else if (line == "/clarify") {
                    clarificationUsed = true;
                    std::cout << "\n" << Terminal::Color::BRIGHT_BLUE << persona->getClarificationResponse(q.clarification) << "\n\n" << Terminal::Color::RESET;
                    continue;
                } else if (line == "/stats") {
                    displayLiveStats();
                    continue;
                } else if (line == "/skip") {
                    std::cout << Terminal::Color::YELLOW << "[Question skipped by candidate]\n\n" << Terminal::Color::RESET;
                    finalAnswer = "Skipped question. Insufficient knowledge.";
                    break;
                } else if (line == "/finish" || line == "/quit") {
                    std::cout << Terminal::Color::BRIGHT_MAGENTA << "Concluding interview session upon candidate request.\n\n" << Terminal::Color::RESET;
                    return;
                } else {
                    if (line.empty()) {
                        std::cout << Terminal::Color::DIM << "(Please enter a substantive answer, or type /hint for help)\n" << Terminal::Color::RESET;
                        continue;
                    }
                    finalAnswer = line;
                    break;
                }
            }

            auto endTime = std::chrono::steady_clock::now();
            double elapsedSeconds = std::chrono::duration<double>(endTime - startTime).count();

            // Evaluate with Adaptive Engine
            AnswerEvaluation eval = adaptive.evaluateAnswer(q, finalAnswer, elapsedSeconds, hintUsed, clarificationUsed, stats.persona);

            displayEvaluationResult(eval);
            recordEvaluation(eval);

            // Check if Dynamic Follow-up Probing should trigger
            if (adaptive.shouldTriggerFollowUp(eval, q)) {
                std::string followUpPrompt = adaptive.selectFollowUpPrompt(q, eval);
                triggerFollowUp(q, followUpPrompt);
            }

            std::cout << Terminal::Color::DIM << "Press Enter to proceed to the next round..." << Terminal::Color::RESET;
            std::string dummy;
            std::getline(std::cin, dummy);
            std::cout << "\n";
        }
    }

    void triggerFollowUp(const Question& parentQuestion, const std::string& followUpPrompt) {
        Terminal::printDivider("DYNAMIC ADAPTIVE FOLLOW-UP");
        std::cout << Terminal::Color::BRIGHT_MAGENTA << Terminal::Color::BOLD 
                  << persona->avatar << " " << persona->getFollowUpIntro() << "\n\n" 
                  << Terminal::Color::RESET;
        std::cout << Terminal::Color::BRIGHT_WHITE << "\"" << followUpPrompt << "\"\n\n" << Terminal::Color::RESET;

        std::cout << Terminal::Color::BRIGHT_GREEN << "Your Follow-up Response: \n> " << Terminal::Color::RESET;
        std::string followUpAnswer;
        std::getline(std::cin, followUpAnswer);
        if (followUpAnswer.empty()) followUpAnswer = "No follow-up response provided.";

        // Evaluate follow-up
        Question tempQ = parentQuestion;
        tempQ.title = parentQuestion.title + " (Follow-up)";
        tempQ.prompt = followUpPrompt;

        AnswerEvaluation eval = adaptive.evaluateAnswer(tempQ, followUpAnswer, 15.0, false, false, stats.persona);
        eval.was_follow_up = true;

        displayEvaluationResult(eval);
        recordEvaluation(eval);
    }

    void displayEvaluationResult(const AnswerEvaluation& eval) {
        std::cout << "\n";
        Terminal::printDivider("ROUND EVALUATION");

        std::cout << "Calibration Score: " << Terminal::scoreStars(eval.score) 
                  << "  " << Terminal::Color::BRIGHT_WHITE << Terminal::Color::BOLD 
                  << std::fixed << std::setprecision(1) << eval.score << " / 100" << Terminal::Color::RESET;

        // Elo Change
        int diff = eval.new_elo - eval.old_elo;
        std::string eloColor = (diff >= 0) ? Terminal::Color::BRIGHT_GREEN : Terminal::Color::BRIGHT_RED;
        std::cout << "  |  Elo: " << eloColor << eval.old_elo << " -> " << eval.new_elo 
                  << " (" << (diff >= 0 ? "+" : "") << diff << ")" << Terminal::Color::RESET << "\n\n";

        // Concepts Breakdown
        if (!eval.concepts_covered.empty()) {
            std::cout << Terminal::Color::BRIGHT_GREEN << "Key Concepts Covered (✔): " << Terminal::Color::RESET;
            for (const auto& c : eval.concepts_covered) {
                std::cout << Terminal::Color::GREEN << "[" << c << "] " << Terminal::Color::RESET;
            }
            std::cout << "\n";
        }
        if (!eval.concepts_missed.empty()) {
            std::cout << Terminal::Color::BRIGHT_RED << "Nuances Missed (✘): " << Terminal::Color::RESET;
            for (const auto& m : eval.concepts_missed) {
                std::cout << Terminal::Color::RED << "[" << m << "] " << Terminal::Color::RESET;
            }
            std::cout << "\n";
        }
        if (!eval.bonus_covered.empty()) {
            std::cout << Terminal::Color::BRIGHT_MAGENTA << "Advanced Insights (★): " << Terminal::Color::RESET;
            for (const auto& b : eval.bonus_covered) {
                std::cout << Terminal::Color::MAGENTA << "[" << b << "] " << Terminal::Color::RESET;
            }
            std::cout << "\n";
        }

        std::cout << "\n" << Terminal::Color::BOLD << persona->avatar << ":\n" << Terminal::Color::RESET;
        std::cout << Terminal::Color::BRIGHT_CYAN << "\"" << eval.interviewer_comment << "\"\n\n" << Terminal::Color::RESET;

        std::cout << Terminal::Color::BRIGHT_BLACK << "Feedback: " << Terminal::Color::RESET << eval.feedback_improvements << "\n\n";
    }

    void recordEvaluation(const AnswerEvaluation& eval) {
        stats.evaluations.push_back(eval);
        stats.total_duration_seconds += eval.response_time_seconds;
        stats.current_elo = eval.new_elo;
        if (eval.new_elo > stats.peak_elo) stats.peak_elo = eval.new_elo;
        stats.difficulty_count[eval.difficulty]++;

        // Update topic score
        double currentTopicScore = stats.topic_scores[eval.topic];
        if (currentTopicScore == 0.0) {
            stats.topic_scores[eval.topic] = eval.score;
        } else {
            stats.topic_scores[eval.topic] = (currentTopicScore + eval.score) / 2.0;
        }
    }

    void displayLiveStats() {
        std::cout << "\n" << Terminal::Color::BRIGHT_BLUE << "--- Current Live Session Stats ---\n" << Terminal::Color::RESET;
        std::cout << "Candidate: " << stats.candidate_name << "\n";
        std::cout << "Current Elo: " << adaptive.getCurrentElo() << " (Peak: " << adaptive.getPeakElo() << ")\n";
        std::cout << "Questions Answered: " << stats.evaluations.size() << "\n";
        std::cout << "Hints Used: " << stats.hints_used_total << "\n";
        std::cout << "-----------------------------------\n\n";
    }

    Question pickNextQuestion(Difficulty diff) {
        auto candidates = bank.getQuestionsByTrackAndDifficulty(stats.track, diff);
        for (const auto& q : candidates) {
            if (askedQuestionIds.find(q.id) == askedQuestionIds.end()) {
                askedQuestionIds.insert(q.id);
                return q;
            }
        }
        // Fallback: any unasked question in track
        auto allTrack = bank.getQuestionsByTrack(stats.track);
        for (const auto& q : allTrack) {
            if (askedQuestionIds.find(q.id) == askedQuestionIds.end()) {
                askedQuestionIds.insert(q.id);
                return q;
            }
        }
        // If all asked, recycle first
        return allTrack.empty() ? bank.getAllQuestions().front() : allTrack.front();
    }

    void concludeSession() {
        Terminal::clearScreen();
        Terminal::printDivider("FINAL INTERVIEW SCORECARD");

        if (stats.evaluations.empty()) {
            std::cout << "No rounds were evaluated in this session.\n";
            return;
        }

        double totalScoreSum = 0.0;
        for (const auto& e : stats.evaluations) {
            totalScoreSum += e.score;
        }
        stats.overall_score = totalScoreSum / stats.evaluations.size();
        stats.verdict = AdaptiveEngine::calculateFinalVerdict(stats.overall_score, stats.current_elo);

        std::cout << Terminal::Color::BRIGHT_CYAN << Terminal::Color::BOLD
                  << "   CANDIDATE: " << stats.candidate_name << "\n"
                  << "   INTERVIEW TRACK: " << trackToString(stats.track) << "\n"
                  << "   INTERVIEWER: " << personaName(stats.persona) << "\n\n"
                  << Terminal::Color::RESET;

        std::cout << "   " << Terminal::Color::BOLD << "Overall Calibration Score: " 
                  << Terminal::Color::BRIGHT_WHITE << std::fixed << std::setprecision(1) << stats.overall_score << " / 100 " 
                  << Terminal::scoreStars(stats.overall_score) << "\n";

        std::cout << "   " << Terminal::Color::BOLD << "Final Adaptive Elo: " 
                  << Terminal::Color::BRIGHT_GREEN << stats.current_elo 
                  << Terminal::Color::DIM << " (Peak: " << stats.peak_elo << ", Baseline: " << stats.starting_elo << ")\n\n"
                  << Terminal::Color::RESET;

        std::cout << "   " << Terminal::Color::BOLD << "Hiring Committee Verdict: \n" 
                  << Terminal::Color::RESET;

        std::string verdictColor = Terminal::Color::BRIGHT_GREEN;
        if (stats.verdict == Verdict::LEANING_NO_HIRE) verdictColor = Terminal::Color::BRIGHT_YELLOW;
        else if (stats.verdict == Verdict::NO_HIRE) verdictColor = Terminal::Color::BRIGHT_RED;

        std::cout << "   " << verdictColor << Terminal::Color::BOLD 
                  << ">>> " << verdictToString(stats.verdict) << " <<<\n\n" 
                  << Terminal::Color::RESET;

        std::cout << "   " << Terminal::Color::BOLD << persona->avatar << " Remarks:\n" << Terminal::Color::RESET;
        std::cout << "   " << Terminal::Color::BRIGHT_CYAN << "\"" 
                  << persona->getFinalVerdictSpeech(stats.verdict, stats.overall_score) 
                  << "\"\n\n" << Terminal::Color::RESET;

        Terminal::printDivider("GENERATING COMPREHENSIVE REPORTS");

        std::string mdPath = ReportGenerator::generateMarkdownReport(stats);
        std::string htmlPath = ReportGenerator::generateHtmlReport(stats);

        std::cout << Terminal::Color::BRIGHT_GREEN << "✔ Markdown Report successfully generated: \n  -> " 
                  << Terminal::Color::BOLD << mdPath << "\n" << Terminal::Color::RESET;

        std::cout << Terminal::Color::BRIGHT_CYAN << "✔ Interactive HTML Dashboard generated: \n  -> " 
                  << Terminal::Color::BOLD << htmlPath << "\n\n" << Terminal::Color::RESET;

        std::cout << Terminal::Color::BRIGHT_WHITE 
                  << "You can open the HTML report in your browser to view the visual skill matrix, "
                  << "charts, and detailed feedback cards!\n"
                  << Terminal::Color::RESET;

        Terminal::printDivider();
    }
};

} // namespace MockInterview
