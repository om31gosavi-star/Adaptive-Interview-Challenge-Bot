/**
 * ============================================================================
 * Project: Adaptive Interview Challenge (AI Mock Interview Simulation Bot)
 * Language: Modern C++ (C++17)
 * Author: Om Gosavi (om31gosavi-star)
 * Description: A full-featured, self-contained AI Mock Interview Simulation Bot
 *              featuring dynamic Elo difficulty adaptation, multi-persona
 *              interviewers (FAANG Bar Raiser, Staff Tech Lead, Architect),
 *              multi-track question banks (C++, DSA, System Design, Behavioral),
 *              real-time concept coverage NLP scoring, and dual report export.
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <cmath>
#include <cctype>
#include <fstream>
#include <thread>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace MockInterview {

// ============================================================================
// 1. DATA MODELS & ENUMS
// ============================================================================

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD,
    EXPERT
};

inline std::string difficultyToString(Difficulty d) {
    switch (d) {
        case Difficulty::EASY:   return "EASY";
        case Difficulty::MEDIUM: return "MEDIUM";
        case Difficulty::HARD:   return "HARD";
        case Difficulty::EXPERT: return "EXPERT";
    }
    return "MEDIUM";
}

inline int difficultyRatingBase(Difficulty d) {
    switch (d) {
        case Difficulty::EASY:   return 1000;
        case Difficulty::MEDIUM: return 1250;
        case Difficulty::HARD:   return 1500;
        case Difficulty::EXPERT: return 1750;
    }
    return 1250;
}

enum class Track {
    MODERN_CPP,
    ALGORITHMS_DSA,
    SYSTEM_DESIGN,
    BEHAVIORAL,
    FULL_MOCK_LOOP
};

inline std::string trackToString(Track t) {
    switch (t) {
        case Track::MODERN_CPP:     return "Modern C++ & Low-Level Systems";
        case Track::ALGORITHMS_DSA: return "Data Structures & Algorithms";
        case Track::SYSTEM_DESIGN:  return "System Design & Scalability";
        case Track::BEHAVIORAL:     return "Behavioral & Leadership (STAR)";
        case Track::FULL_MOCK_LOOP: return "Full Mock Loop (Multi-Round)";
    }
    return "General Tech";
}

enum class PersonaType {
    BAR_RAISER,       // Alex Vance: Rigorous, algorithmic precision, edge cases
    TECH_LEAD,        // Elena Rostova: Pragmatic, production maintainability, testing
    SENIOR_ARCHITECT  // Marcus Sterling: Systems thinker, trade-offs, architecture
};

inline std::string personaName(PersonaType p) {
    switch (p) {
        case PersonaType::BAR_RAISER:       return "Alex Vance (FAANG Bar Raiser)";
        case PersonaType::TECH_LEAD:        return "Elena Rostova (Staff Tech Lead)";
        case PersonaType::SENIOR_ARCHITECT: return "Marcus Sterling (Principal Architect)";
    }
    return "Interviewer";
}

enum class Verdict {
    STRONG_HIRE,
    HIRE,
    LEANING_HIRE,
    LEANING_NO_HIRE,
    NO_HIRE
};

inline std::string verdictToString(Verdict v) {
    switch (v) {
        case Verdict::STRONG_HIRE:     return "STRONG HIRE (Senior / Staff Level)";
        case Verdict::HIRE:            return "HIRE (Target Level Met)";
        case Verdict::LEANING_HIRE:    return "LEANING HIRE (Solid baseline with minor gaps)";
        case Verdict::LEANING_NO_HIRE: return "LEANING NO HIRE (Needs more depth)";
        case Verdict::NO_HIRE:         return "NO HIRE (Significant gaps in core concepts)";
    }
    return "NEUTRAL";
}

struct Question {
    std::string id;
    Track track;
    Difficulty difficulty;
    std::string topic;
    std::string title;
    std::string prompt;
    std::string hint;
    std::string clarification;
    std::vector<std::string> key_concepts;
    std::vector<std::string> bonus_concepts;
    std::vector<std::string> follow_up_prompts;
    std::string ideal_answer_summary;
    std::string common_pitfalls;
};

struct AnswerEvaluation {
    std::string question_id;
    std::string question_title;
    Difficulty difficulty;
    std::string topic;
    std::string user_answer;
    double score = 0.0;
    int old_elo = 1200;
    int new_elo = 1200;
    std::vector<std::string> concepts_covered;
    std::vector<std::string> concepts_missed;
    std::vector<std::string> bonus_covered;
    std::string depth_rating;
    std::string feedback_strengths;
    std::string feedback_improvements;
    std::string interviewer_comment;
    double response_time_seconds = 0.0;
    bool hint_used = false;
    bool clarification_used = false;
    bool was_follow_up = false;
};

struct SessionStatistics {
    std::string candidate_name;
    Track track;
    PersonaType persona;
    std::string start_time;
    double total_duration_seconds = 0.0;
    int starting_elo = 1200;
    int current_elo = 1200;
    int peak_elo = 1200;
    double overall_score = 0.0;
    Verdict verdict = Verdict::LEANING_HIRE;
    std::map<std::string, double> topic_scores;
    std::map<Difficulty, int> difficulty_count;
    std::vector<AnswerEvaluation> evaluations;
    int hints_used_total = 0;
};

inline std::string getCurrentTimestampString() {
    auto now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm buf{};
#if defined(_MSC_VER)
    localtime_s(&buf, &in_time_t);
#else
    std::tm* tm_ptr = std::localtime(&in_time_t);
    if (tm_ptr) {
        buf = *tm_ptr;
    }
#endif
    std::ostringstream ss;
    ss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

inline std::string sanitizeFilename(const std::string& input) {
    std::string res;
    for (char c : input) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-') {
            res += c;
        } else if (c == ' ') {
            res += '_';
        }
    }
    return res.empty() ? "candidate" : res;
}

// ============================================================================
// 2. ANSI TERMINAL & VISUAL FORMATTING
// ============================================================================

namespace Terminal {
    namespace Color {
        inline const std::string RESET          = "\033[0m";
        inline const std::string BOLD           = "\033[1m";
        inline const std::string DIM            = "\033[2m";
        inline const std::string RED            = "\033[31m";
        inline const std::string GREEN          = "\033[32m";
        inline const std::string YELLOW         = "\033[33m";
        inline const std::string BLUE           = "\033[34m";
        inline const std::string MAGENTA        = "\033[35m";
        inline const std::string CYAN           = "\033[36m";
        inline const std::string WHITE          = "\033[37m";
        inline const std::string BRIGHT_BLACK   = "\033[90m";
        inline const std::string BRIGHT_RED     = "\033[91m";
        inline const std::string BRIGHT_GREEN   = "\033[92m";
        inline const std::string BRIGHT_YELLOW  = "\033[93m";
        inline const std::string BRIGHT_BLUE    = "\033[94m";
        inline const std::string BRIGHT_MAGENTA = "\033[95m";
        inline const std::string BRIGHT_CYAN    = "\033[96m";
        inline const std::string BRIGHT_WHITE   = "\033[97m";
    }

    inline void initConsole() {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif
    }

    inline void clearScreen() {
        std::cout << "\033[2J\033[1;1H";
    }

    inline void printBanner() {
        std::cout << Color::BRIGHT_CYAN << Color::BOLD;
        std::cout << R"(
  █████╗ ██████╗  █████╗ ██████╗ ████████╗██╗██╗   ██╗███████╗
 ██╔══██╗██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██║██║   ██║██╔════╝
 ███████║██║  ██║███████║██████╔╝   ██║   ██║██║   ██║█████╗  
 ██╔══██║██║  ██║██╔══██║██╔═══╝    ██║   ██║╚██╗ ██╔╝██╔══╝  
 ██║  ██║██████╔╝██║  ██║██║        ██║   ██║ ╚████╔╝ ███████╗
 ╚═╝  ╚═╝╚═════╝ ╚═╝  ╚═╝╚═╝        ╚═╝   ╚═╝  ╚═══╝  ╚══════╝
        )" << Color::RESET;

        std::cout << Color::BRIGHT_MAGENTA << Color::BOLD
                  << "   >>> AI MOCK INTERVIEW SIMULATION BOT (C++ ENGINE) <<<\n"
                  << Color::RESET;
        std::cout << Color::DIM
                  << "   Dynamic Elo Scaling | Multi-Persona | Adaptive Probing | C++17 Core\n"
                  << Color::RESET;
        std::cout << Color::BRIGHT_BLACK
                  << "   ------------------------------------------------------------------------\n\n"
                  << Color::RESET;
    }

    inline void printDivider(const std::string& label = "") {
        std::cout << Color::BRIGHT_BLACK << "───";
        if (!label.empty()) {
            std::cout << " " << Color::BRIGHT_YELLOW << Color::BOLD << label << " " << Color::RESET << Color::BRIGHT_BLACK;
            int remaining = 70 - static_cast<int>(label.length()) - 5;
            if (remaining < 5) remaining = 5;
            for (int i = 0; i < remaining; ++i) std::cout << "─";
        } else {
            for (int i = 0; i < 68; ++i) std::cout << "─";
        }
        std::cout << Color::RESET << "\n";
    }

    inline std::string scoreStars(double score) {
        std::ostringstream ss;
        int stars = static_cast<int>((score / 100.0) * 5.0 + 0.5);
        if (stars > 5) stars = 5;
        if (stars < 0) stars = 0;
        ss << Color::BRIGHT_YELLOW;
        for (int i = 0; i < 5; ++i) {
            if (i < stars) ss << "★ ";
            else ss << Color::DIM << "☆ " << Color::BRIGHT_YELLOW;
        }
        ss << Color::RESET;
        return ss.str();
    }
} // namespace Terminal

// ============================================================================
// 3. INTERVIEWER PERSONAS
// ============================================================================

class InterviewerPersona {
public:
    PersonaType type;
    std::string name;
    std::string role;
    std::string tagline;
    std::string avatar;

    InterviewerPersona(PersonaType t) : type(t) {
        switch (t) {
            case PersonaType::BAR_RAISER:
                name = "Alex Vance";
                role = "FAANG Principal Bar Raiser";
                tagline = "Rigorous, precision-focused, deep-dive on complexities & edge cases.";
                avatar = "[ALEX - BAR RAISER]";
                break;
            case PersonaType::TECH_LEAD:
                name = "Elena Rostova";
                role = "Staff Tech Lead & Production Architect";
                tagline = "Pragmatic, values maintainability, observability, and real-world trade-offs.";
                avatar = "[ELENA - TECH LEAD]";
                break;
            case PersonaType::SENIOR_ARCHITECT:
                name = "Marcus Sterling";
                role = "VP of Architecture & Systems Mentor";
                tagline = "Collaborative systems thinker, explores decoupling, failure modes, and scalability.";
                avatar = "[MARCUS - ARCHITECT]";
                break;
        }
    }

    std::string getGreeting(const std::string& candidateName, Track track) const {
        std::string trackName = trackToString(track);
        switch (type) {
            case PersonaType::BAR_RAISER:
                return "Hello " + candidateName + ". I'm Alex Vance. In this round on '" + trackName + 
                       "', I evaluate algorithmic rigor, fundamental principles, and edge-case handling. "
                       "Be precise, mention Big-O complexities where appropriate, and explain your technical rationale.";
            case PersonaType::TECH_LEAD:
                return "Welcome, " + candidateName + "! I'm Elena. Today we're diving into '" + trackName + "'. "
                       "I care about how you think through real-world systems, code safety, and production maintainability. "
                       "Focus on your problem-solving process and trade-offs.";
            case PersonaType::SENIOR_ARCHITECT:
                return "Hi " + candidateName + ", great to meet you! I'm Marcus. For our session on '" + trackName + 
                       "', I love seeing how engineers navigate ambiguity and scale. Treat this as an engineering whiteboarding session.";
        }
        return "Welcome to the interview session.";
    }

    std::string getHintResponse(const std::string& hintText) const {
        return "[" + name + " provides guidance]: \"" + hintText + "\" (Note: using hints applies a slight calibration adjustment).";
    }

    std::string getClarificationResponse(const std::string& clarifyText) const {
        return "[" + name + " clarifies]: \"" + clarifyText + "\"";
    }

    std::string getFinalVerdictSpeech(Verdict verdict, double overallScore) const {
        std::string scoreStr = std::to_string(static_cast<int>(overallScore));
        switch (verdict) {
            case Verdict::STRONG_HIRE:
                return "Committee consensus: 'Strong Hire (" + scoreStr + "/100). Demonstrated senior-level command, trade-off clarity, and systems precision.'";
            case Verdict::HIRE:
                return "Committee consensus: 'Hire (" + scoreStr + "/100). Met target competencies with solid technical intuition and problem-solving composure.'";
            case Verdict::LEANING_HIRE:
                return "Committee consensus: 'Leaning Hire (" + scoreStr + "/100). Solid baseline potential. Sharpen edge-case coverage and low-level mechanics.'";
            case Verdict::LEANING_NO_HIRE:
                return "Committee consensus: 'Leaning No-Hire (" + scoreStr + "/100). Good effort, but notable gaps in systems depth need further preparation.'";
            case Verdict::NO_HIRE:
                return "Committee consensus: 'No-Hire (" + scoreStr + "/100). Foundational concepts missing. Review the generated report syllabus and re-attempt!'";
        }
        return "Interview complete.";
    }
};

// ============================================================================
// 4. QUESTION BANK
// ============================================================================

class QuestionBank {
private:
    std::vector<Question> questions;

public:
    QuestionBank() {
        populateQuestions();
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
        // Modern C++ Track
        questions.push_back({
            "CPP_EASY_1", Track::MODERN_CPP, Difficulty::EASY, "Resource Management (RAII)",
            "RAII and Smart Pointers vs Raw Pointers",
            "Explain the concept of RAII in C++. How does std::unique_ptr enforce exclusive ownership compared to raw pointers, and what is its runtime overhead?",
            "Think about destructor cleanup on scope exit, delete operator, and zero-cost abstractions.",
            "Assume C++11 and later. Focus on single-object ownership semantics.",
            {"raii", "destructor", "unique_ptr", "ownership", "scope", "delete", "zero overhead"},
            {"custom deleter", "stack unwinding", "noexcept", "move only"},
            {"How does std::unique_ptr behave if you attempt to copy it instead of moving it?"},
            "RAII ties resource lifecycle to object lifetime. Destructors guarantee release upon leaving scope.",
            "Forgetting exception safety or claiming unique_ptr has high memory overhead."
        });

        questions.push_back({
            "CPP_MED_1", Track::MODERN_CPP, Difficulty::MEDIUM, "Memory Management & Semantics",
            "Move Semantics, Rvalues, and std::forward",
            "Explain the difference between lvalues and rvalues in Modern C++. What problem does std::move solve, and how does perfect forwarding with std::forward<T> prevent unnecessary copies in templates?",
            "Consider value categories (lvalue vs rvalue/xvalue), universal references (T&&), and reference collapsing.",
            "Focus on standard C++14/17 semantics and function template argument deduction.",
            {"move semantics", "lvalue", "rvalue", "std::move", "std::forward", "perfect forwarding", "universal reference"},
            {"xvalue", "prvalue", "reference collapsing", "noexcept move"},
            {"Why should a move constructor almost always be marked noexcept with std::vector?"},
            "Lvalues have addresses; rvalues are expiring temporaries. std::move casts to rvalue reference.",
            "Thinking std::move moves bytes at runtime rather than being a compile-time static_cast."
        });

        questions.push_back({
            "CPP_HARD_1", Track::MODERN_CPP, Difficulty::HARD, "Object Model & Polymorphism",
            "Virtual Method Table (vtable), Memory Layout & Destructors",
            "Describe the internal memory layout of a C++ polymorphic class. How does dynamic dispatch work via vtable and vptr? Why is a non-virtual destructor in a base class dangerous when deleting via base pointer?",
            "Think about pointer offsets, hidden vptr pointer added to the object, and undefined behavior.",
            "Assume standard 64-bit architecture with common Itanium/MSVC ABI layout.",
            {"vtable", "vptr", "dynamic dispatch", "virtual destructor", "undefined behavior", "offset", "memory layout"},
            {"multiple inheritance", "thunk", "typeinfo", "rtti"},
            {"What happens to object size when the first virtual function is added to an empty class?"},
            "Vtable stores function pointers; objects hold hidden vptr (8 bytes). Non-virtual destructor leaks derived state.",
            "Confusing compile-time overloading with runtime dynamic dispatch."
        });

        // DSA Track
        questions.push_back({
            "DSA_EASY_1", Track::ALGORITHMS_DSA, Difficulty::EASY, "Hash Tables & Complexity",
            "Hash Map Collision Resolution & Amortized Complexity",
            "How does a Hash Table achieve O(1) average time complexity? Compare Separate Chaining with Open Addressing, and explain what causes worst-case O(N) performance.",
            "Discuss hash functions, uniform distribution, load factor, and rehashing.",
            "Assume a standard key-value map like std::unordered_map.",
            {"hash function", "collision", "separate chaining", "open addressing", "load factor", "amortized", "o(1)"},
            {"robin hood hashing", "tombstone", "rehash"},
            {"What happens if all keys hash to the exact same bucket?"},
            "Hash function maps keys to buckets. Chaining uses linked lists; open addressing probes slots. Resize occurs at threshold.",
            "Failing to explain load factor or how catastrophic collisions degrade lookup."
        });

        questions.push_back({
            "DSA_MED_1", Track::ALGORITHMS_DSA, Difficulty::MEDIUM, "Graph Algorithms & Traversals",
            "Shortest Path: Dijkstra's Algorithm vs BFS",
            "Under what conditions would you choose BFS over Dijkstra's Algorithm? What is the time complexity of Dijkstra with a min-priority queue, and how do you handle negative edge weights?",
            "Think about unweighted vs weighted graphs, edge relaxations, and why Dijkstra fails on negative cycles (Bellman-Ford).",
            "State vertices as V and edges as E.",
            {"bfs", "dijkstra", "priority queue", "unweighted", "weighted", "o((v + e) log v)", "negative weight", "bellman-ford"},
            {"greedy", "relaxation", "fibonacci heap"},
            {"Why can't Dijkstra add a large constant to edge weights to eliminate negative weights?"},
            "BFS finds shortest paths in unweighted graphs in O(V + E). Dijkstra uses min-heap greedy relaxation in O((V+E)log V).",
            "Believing Dijkstra works correctly on negative edge weights."
        });

        questions.push_back({
            "DSA_HARD_1", Track::ALGORITHMS_DSA, Difficulty::HARD, "Advanced System & Algorithm Design",
            "LRU Cache: O(1) Get and Put Design",
            "Design an LRU (Least Recently Used) Cache with strict O(1) time complexity for both get and put operations. Describe the exact data structures you would combine and how node eviction works.",
            "Consider combining a doubly linked list with a hash map.",
            "Target standard capacity N, avoiding O(N) scans.",
            {"doubly linked list", "hash map", "o(1)", "eviction", "head", "tail", "mutex", "splice"},
            {"dummy head tail", "shared_mutex", "read write lock"},
            {"Why is a singly linked list insufficient for true O(1) arbitrary node eviction?"},
            "Combine Doubly Linked List (recency order) with Hash Map (key -> node pointer) for O(1) get, put, and eviction.",
            "Attempting to use a single array or queue which requires O(N) shifting on cache hits."
        });

        // System Design Track
        questions.push_back({
            "SYS_EASY_1", Track::SYSTEM_DESIGN, Difficulty::EASY, "Scaling Fundamentals",
            "Horizontal vs Vertical Scaling and Load Balancing",
            "Explain the difference between horizontal and vertical scaling. What role does a Load Balancer play, and how do Round Robin, Least Connections, and IP Hash algorithms differ?",
            "Consider single points of failure, stateless web tiers, and health checks.",
            "Focus on web architecture scaling from a single server to a multi-instance cluster.",
            {"horizontal scaling", "vertical scaling", "load balancer", "round robin", "least connections", "stateless"},
            {"consistent hashing", "sticky session", "health check"},
            {"Why is session state stored in local server RAM an anti-pattern for horizontal scaling?"},
            "Vertical scales single machine; horizontal adds nodes behind load balancer. Requires stateless app tier.",
            "Failing to mention that horizontal scaling requires stateless app servers."
        });

        questions.push_back({
            "SYS_MED_1", Track::SYSTEM_DESIGN, Difficulty::MEDIUM, "Distributed Caching",
            "Caching Strategies & Preventing Cache Stampede",
            "Compare Cache-Aside, Write-Through, and Write-Back caching patterns. What is a Cache Stampede (Thundering Herd) when a hot key expires, and what architectural mechanisms prevent it?",
            "Think about distributed mutex locks, SingleFlight request deduplication, and probabilistic early expiration (XFetch).",
            "Assume a high-traffic system with 50,000 requests/sec.",
            {"cache-aside", "write-through", "write-back", "cache stampede", "thundering herd", "mutex", "ttl", "singleflight"},
            {"xfetch", "bloom filter", "redis"},
            {"How does probabilistic early expiration prevent cache stampede without distributed locks?"},
            "Cache-aside queries cache, backfills from DB. Stampede occurs on hot key expiration. Mitigated by mutex locks and SingleFlight.",
            "Claiming Write-Through is faster for writes than Write-Back."
        });

        // Behavioral Track
        questions.push_back({
            "BEH_EASY_1", Track::BEHAVIORAL, Difficulty::EASY, "Technical Disagreements",
            "Resolving an Architectural Disagreement in a Team",
            "Tell me about a time you had a strong technical disagreement with a peer or lead on a design or technology choice. Walk me through using Situation, Task, Action, and Result (STAR).",
            "Use the STAR framework explicitly. Highlight data-driven evaluation, benchmarks, or prototypes.",
            "Focus on professional collaboration and 'disagree and commit' principles.",
            {"situation", "task", "action", "result", "disagreement", "trade-off", "benchmark", "compromise"},
            {"disagree and commit", "rfc", "data driven"},
            {"What would you do if after presenting your data, the lead still insisted on their approach?"},
            "STAR structure: Situation, Task, Action (benchmarks, objective RFC), Result (alignment and consensus).",
            "Saying 'I convinced them I was right' without showing collaborative listening or empirical data."
        });
    }
};

// ============================================================================
// 5. ADAPTIVE ENGINE (ELO & NLP SCORER)
// ============================================================================

class AdaptiveEngine {
private:
    int current_elo = 1200;
    int peak_elo = 1200;
    const int K_FACTOR = 48;

public:
    AdaptiveEngine(int startingElo = 1200) : current_elo(startingElo), peak_elo(startingElo) {}

    int getCurrentElo() const { return current_elo; }
    int getPeakElo() const { return peak_elo; }

    Difficulty determineNextDifficulty() const {
        if (current_elo >= 1500) return Difficulty::EXPERT;
        if (current_elo >= 1320) return Difficulty::HARD;
        if (current_elo >= 1150) return Difficulty::MEDIUM;
        return Difficulty::EASY;
    }

    AnswerEvaluation evaluateAnswer(const Question& question,
                                   const std::string& rawAnswer,
                                   double responseTimeSeconds,
                                   bool hintUsed,
                                   bool clarificationUsed,
                                   const PersonaType persona) {
        AnswerEvaluation eval;
        eval.question_id = question.id;
        eval.question_title = question.title;
        eval.difficulty = question.difficulty;
        eval.topic = question.topic;
        eval.user_answer = rawAnswer;
        eval.response_time_seconds = responseTimeSeconds;
        eval.hint_used = hintUsed;
        eval.clarification_used = clarificationUsed;
        eval.old_elo = current_elo;

        std::string normalizedAnswer = normalizeText(rawAnswer);
        std::vector<std::string> tokens = tokenize(normalizedAnswer);

        // Concept Coverage
        int keyMatches = 0;
        for (const auto& concept : question.key_concepts) {
            if (containsConcept(normalizedAnswer, concept)) {
                eval.concepts_covered.push_back(concept);
                keyMatches++;
            } else {
                eval.concepts_missed.push_back(concept);
            }
        }

        for (const auto& bonus : question.bonus_concepts) {
            if (containsConcept(normalizedAnswer, bonus)) {
                eval.bonus_covered.push_back(bonus);
            }
        }

        // Depth & Reasoning signals
        static const std::vector<std::string> analyticalKeywords = {
            "trade-off", "tradeoff", "overhead", "bottleneck", "edge case", "complexity", 
            "amortized", "latency", "throughput", "cache miss", "failure", "benchmark", "because"
        };
        int analyticalSignals = 0;
        for (const auto& kw : analyticalKeywords) {
            if (containsConcept(normalizedAnswer, kw)) analyticalSignals++;
        }

        double keyRatio = question.key_concepts.empty() ? 1.0 :
                          static_cast<double>(keyMatches) / question.key_concepts.size();
        double bonusRatio = question.bonus_concepts.empty() ? 0.0 :
                            (std::min)(1.0, static_cast<double>(eval.bonus_covered.size()) / 2.0);

        double lengthScore = tokens.size() >= 40 ? 1.0 : (tokens.size() >= 20 ? 0.7 : 0.4);
        double analyticalRatio = (std::min)(1.0, analyticalSignals / 3.0);

        double score = (keyRatio * 55.0) + (analyticalRatio * 25.0) + (bonusRatio * 15.0) + (lengthScore * 5.0);
        if (hintUsed) score -= 10.0;
        if (clarificationUsed) score -= 2.0;
        if (score < 5.0) score = 5.0;
        if (score > 100.0) score = 100.0;
        eval.score = score;

        if (score >= 80.0) eval.depth_rating = "Deep Technical Mastery";
        else if (score >= 55.0) eval.depth_rating = "Competent Working Knowledge";
        else eval.depth_rating = "Surface Level Understanding";

        // Elo Update
        updateElo(eval, question.difficulty);

        // Feedback
        generateFeedback(eval, question, persona);

        return eval;
    }

    static Verdict calculateFinalVerdict(double overallScore, int finalElo) {
        if (finalElo >= 1480 && overallScore >= 80.0) return Verdict::STRONG_HIRE;
        if (finalElo >= 1320 && overallScore >= 65.0) return Verdict::HIRE;
        if (finalElo >= 1150 && overallScore >= 50.0) return Verdict::LEANING_HIRE;
        if (finalElo >= 1050) return Verdict::LEANING_NO_HIRE;
        return Verdict::NO_HIRE;
    }

private:
    void updateElo(AnswerEvaluation& eval, Difficulty diff) {
        int questionRating = difficultyRatingBase(diff);
        double expectedScore = 1.0 / (1.0 + std::pow(10.0, (questionRating - current_elo) / 400.0));
        double actualScore = eval.score / 100.0;
        int eloChange = static_cast<int>(std::round(K_FACTOR * (actualScore - expectedScore)));
        current_elo += eloChange;
        if (current_elo < 600) current_elo = 600;
        if (current_elo > 2400) current_elo = 2400;
        if (current_elo > peak_elo) peak_elo = current_elo;
        eval.new_elo = current_elo;
    }

    void generateFeedback(AnswerEvaluation& eval, const Question& q, const PersonaType persona) {
        std::ostringstream strengths;
        std::ostringstream improvements;

        if (!eval.concepts_covered.empty()) {
            strengths << "Accurately identified and articulated: ";
            for (size_t i = 0; i < eval.concepts_covered.size(); ++i) {
                strengths << "'" << eval.concepts_covered[i] << "'";
                if (i + 1 < eval.concepts_covered.size()) strengths << ", ";
            }
            strengths << ". ";
        }
        if (!eval.bonus_covered.empty()) {
            strengths << "Demonstrated senior-level depth by discussing: ";
            for (size_t i = 0; i < eval.bonus_covered.size(); ++i) {
                strengths << "'" << eval.bonus_covered[i] << "'";
                if (i + 1 < eval.bonus_covered.size()) strengths << ", ";
            }
            strengths << ". ";
        }
        if (strengths.str().empty()) strengths << "Attempted the challenge and engaged with the problem.";
        eval.feedback_strengths = strengths.str();

        if (!eval.concepts_missed.empty()) {
            improvements << "To elevate your answer, integrate key concepts: ";
            for (size_t i = 0; i < eval.concepts_missed.size(); ++i) {
                improvements << "'" << eval.concepts_missed[i] << "'";
                if (i + 1 < eval.concepts_missed.size()) improvements << ", ";
            }
            improvements << ". ";
        }
        if (!q.common_pitfalls.empty()) improvements << "Pitfall to watch for: " << q.common_pitfalls << " ";
        eval.feedback_improvements = improvements.str();

        std::string missed = eval.concepts_missed.empty() ? "" : eval.concepts_missed.front();
        if (persona == PersonaType::BAR_RAISER) {
            if (eval.score >= 80.0) eval.interviewer_comment = "Alex [Bar Raiser]: 'Very rigorous. You demonstrated the mechanical precision expected from a senior candidate.'";
            else if (eval.score >= 55.0) eval.interviewer_comment = "Alex [Bar Raiser]: 'You had the broad intuition, but missed depth on " + (missed.empty() ? "edge cases" : missed) + ".'";
            else eval.interviewer_comment = "Alex [Bar Raiser]: 'That lacked technical precision. You need to anchor your answers in concrete complexity.'";
        } else if (persona == PersonaType::TECH_LEAD) {
            if (eval.score >= 80.0) eval.interviewer_comment = "Elena [Tech Lead]: 'Great job. You focused on practical trade-offs and code safety.'";
            else if (eval.score >= 55.0) eval.interviewer_comment = "Elena [Tech Lead]: 'Good practical instinct! Consider how that handles failure modes under load.'";
            else eval.interviewer_comment = "Elena [Tech Lead]: 'In production, that shortcut would cause unexpected regressions.'";
        } else {
            if (eval.score >= 80.0) eval.interviewer_comment = "Marcus [Architect]: 'Elegant structural thinking. You balanced decoupling and scaling constraints nicely.'";
            else if (eval.score >= 55.0) eval.interviewer_comment = "Marcus [Architect]: 'A solid start. Remember that every architecture is a compromise.'";
            else eval.interviewer_comment = "Marcus [Architect]: 'The components were disjointed. Step back and consider how data and failure flow through the system.'";
        }
    }

    static std::string normalizeText(const std::string& input) {
        std::string result;
        result.reserve(input.size());
        for (char c : input) {
            if (std::isalnum(static_cast<unsigned char>(c)) || c == ' ' || c == '_' || c == '-' || c == '+') {
                result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            } else {
                result += ' ';
            }
        }
        return result;
    }

    static std::vector<std::string> tokenize(const std::string& text) {
        std::vector<std::string> tokens;
        std::istringstream iss(text);
        std::string token;
        while (iss >> token) tokens.push_back(token);
        return tokens;
    }

    static bool containsConcept(const std::string& text, const std::string& concept) {
        std::string normConcept = normalizeText(concept);
        if (normConcept.empty()) return false;
        return text.find(normConcept) != std::string::npos;
    }
};

// ============================================================================
// 6. REPORT GENERATOR
// ============================================================================

class ReportGenerator {
public:
    static std::string generateMarkdownReport(const SessionStatistics& stats, const std::string& outputDir = "reports") {
        std::string safeName = sanitizeFilename(stats.candidate_name);
        std::string filename = outputDir + "/interview_report_" + safeName + ".md";

        std::ofstream file(filename);
        if (!file.is_open()) return "";

        file << "# 🎯 AI Mock Interview Performance Evaluation Report\n\n";
        file << "> **Session Timestamp:** " << stats.start_time << "  \n";
        file << "> **Candidate Name:** " << stats.candidate_name << "  \n";
        file << "> **Track:** " << trackToString(stats.track) << "  \n";
        file << "> **Interviewer:** " << personaName(stats.persona) << "  \n\n";
        file << "## 🏆 Executive Summary & Verdict\n\n";
        file << "| Metric | Value |\n| :--- | :--- |\n";
        file << "| **Final Verdict** | **" << verdictToString(stats.verdict) << "** |\n";
        file << "| **Overall Calibration Score** | **" << std::fixed << std::setprecision(1) << stats.overall_score << " / 100** |\n";
        file << "| **Final Adaptive Elo** | **" << stats.current_elo << "** (Peak: " << stats.peak_elo << ") |\n";
        file << "| **Total Rounds** | " << stats.evaluations.size() << " |\n\n";

        file << "## 📝 Round-by-Round Breakdown\n\n";
        int roundNum = 1;
        for (const auto& eval : stats.evaluations) {
            file << "### Round " << roundNum++ << ": " << eval.question_title << " `[" << difficultyToString(eval.difficulty) << "]`\n\n";
            file << "- **Score:** " << std::fixed << std::setprecision(1) << eval.score << "/100\n";
            file << "- **Elo Adjustment:** `" << eval.old_elo << " -> " << eval.new_elo << "`\n";
            file << "- **Candidate Response:** " << eval.user_answer << "\n";
            file << "- **Interviewer Critique:** *" << eval.interviewer_comment << "*\n\n";
        }

        file.close();
        return filename;
    }

    static std::string generateHtmlReport(const SessionStatistics& stats, const std::string& outputDir = "reports") {
        std::string safeName = sanitizeFilename(stats.candidate_name);
        std::string filename = outputDir + "/interview_report_" + safeName + ".html";

        std::ofstream file(filename);
        if (!file.is_open()) return "";

        file << "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Interview Report</title>"
             << "<style>body{font-family:sans-serif;background:#0f172a;color:#f8fafc;padding:30px;max-width:900px;margin:auto;}"
             << ".card{background:#1e293b;border-radius:12px;padding:20px;margin-bottom:16px;border:1px solid #334155;}"
             << ".badge{display:inline-block;padding:6px 14px;border-radius:999px;font-weight:700;background:#0284c7;color:#fff;}"
             << "</style></head><body>"
             << "<h1>🎯 AI Mock Interview Performance Report</h1>"
             << "<p>Candidate: <strong>" << stats.candidate_name << "</strong> | Track: " << trackToString(stats.track) << "</p>"
             << "<div class='badge'>" << verdictToString(stats.verdict) << "</div>"
             << "<h3>Overall Score: " << std::fixed << std::setprecision(1) << stats.overall_score << "% | Final Elo: " << stats.current_elo << "</h3>";

        for (const auto& eval : stats.evaluations) {
            file << "<div class='card'>"
                 << "<h4>" << eval.question_title << " [" << difficultyToString(eval.difficulty) << "]</h4>"
                 << "<p><strong>Score:</strong> " << eval.score << "/100 | <strong>Elo:</strong> " << eval.old_elo << " ➔ " << eval.new_elo << "</p>"
                 << "<p><em>\"" << eval.user_answer << "\"</em></p>"
                 << "<p style='color:#38bdf8;'>" << eval.interviewer_comment << "</p>"
                 << "</div>";
        }

        file << "</body></html>";
        file.close();
        return filename;
    }
};

// ============================================================================
// 7. INTERVIEW CONTROLLER & STATE MACHINE
// ============================================================================

class InterviewEngine {
private:
    SessionStatistics stats;
    QuestionBank bank;
    AdaptiveEngine adaptive;
    std::unique_ptr<InterviewerPersona> persona;
    std::set<std::string> askedQuestionIds;

public:
    InterviewEngine(bool /*unused*/ = true) : adaptive(1200) {
        stats.start_time = getCurrentTimestampString();
        stats.starting_elo = 1200;
        stats.current_elo = 1200;
        stats.peak_elo = 1200;
    }

    void run() {
        Terminal::clearScreen();
        Terminal::printBanner();

        // 1. Candidate Name
        std::cout << Terminal::Color::BRIGHT_WHITE << Terminal::Color::BOLD << "Enter Candidate Name: " << Terminal::Color::RESET;
        std::string name;
        std::getline(std::cin, name);
        if (name.empty()) name = "Senior Engineer";
        stats.candidate_name = name;

        // 2. Persona Selection
        Terminal::printDivider("SELECT INTERVIEWER PERSONA");
        std::cout << "1. " << Terminal::Color::BRIGHT_CYAN << "Alex Vance (FAANG Bar Raiser)" << Terminal::Color::RESET << " - Rigorous, edge cases, Big-O proofs\n"
                  << "2. " << Terminal::Color::BRIGHT_GREEN << "Elena Rostova (Staff Tech Lead)" << Terminal::Color::RESET << " - Pragmatic, production maintainability\n"
                  << "3. " << Terminal::Color::BRIGHT_YELLOW << "Marcus Sterling (Principal Architect)" << Terminal::Color::RESET << " - Architecture trade-offs & scale\n\n"
                  << "Enter selection (1-3, default 1): ";
        std::string pInput;
        std::getline(std::cin, pInput);
        int pChoice = 1;
        if (!pInput.empty()) { try { pChoice = std::stoi(pInput); } catch (...) { pChoice = 1; } }
        PersonaType pType = (pChoice == 2) ? PersonaType::TECH_LEAD : ((pChoice == 3) ? PersonaType::SENIOR_ARCHITECT : PersonaType::BAR_RAISER);
        persona = std::make_unique<InterviewerPersona>(pType);
        stats.persona = pType;

        // 3. Track Selection
        Terminal::printDivider("SELECT INTERVIEW TRACK");
        std::cout << "1. " << Terminal::Color::BRIGHT_CYAN << "Modern C++ & Low-Level Systems" << Terminal::Color::RESET << "\n"
                  << "2. " << Terminal::Color::BRIGHT_GREEN << "Data Structures & Algorithms" << Terminal::Color::RESET << "\n"
                  << "3. " << Terminal::Color::BRIGHT_YELLOW << "System Design & Scalability" << Terminal::Color::RESET << "\n"
                  << "4. " << Terminal::Color::BRIGHT_MAGENTA << "Behavioral & Leadership (STAR)" << Terminal::Color::RESET << "\n\n"
                  << "Enter selection (1-4, default 1): ";
        std::string tInput;
        std::getline(std::cin, tInput);
        int tChoice = 1;
        if (!tInput.empty()) { try { tChoice = std::stoi(tInput); } catch (...) { tChoice = 1; } }
        switch (tChoice) {
            case 2: stats.track = Track::ALGORITHMS_DSA; break;
            case 3: stats.track = Track::SYSTEM_DESIGN; break;
            case 4: stats.track = Track::BEHAVIORAL; break;
            default: stats.track = Track::MODERN_CPP; break;
        }

        // 4. Greeting
        Terminal::printDivider("INTERVIEW CALIBRATION");
        std::cout << Terminal::Color::BOLD << persona->avatar << ":\n" << Terminal::Color::RESET;
        std::cout << "\"" << persona->getGreeting(stats.candidate_name, stats.track) << "\"\n\n";

        // 5. Rounds Loop
        const int MAX_ROUNDS = 4;
        for (int r = 1; r <= MAX_ROUNDS; ++r) {
            Difficulty targetDiff = adaptive.determineNextDifficulty();
            Question q = pickNextQuestion(targetDiff);

            Terminal::printDivider("ROUND " + std::to_string(r) + " / " + std::to_string(MAX_ROUNDS));
            std::cout << Terminal::Color::BRIGHT_CYAN << "Topic: " << q.topic 
                      << "  |  Difficulty: [" << difficultyToString(q.difficulty) 
                      << "]  |  Current Elo: " << adaptive.getCurrentElo() << "\n\n" << Terminal::Color::RESET;

            std::cout << Terminal::Color::BRIGHT_WHITE << Terminal::Color::BOLD << q.title << "\n" << Terminal::Color::RESET;
            std::cout << q.prompt << "\n\n";

            bool hintUsed = false;
            std::string answer = "";
            auto startTime = std::chrono::steady_clock::now();

            while (true) {
                std::cout << Terminal::Color::BRIGHT_GREEN << "Your Answer (/hint, /skip, /finish): \n> " << Terminal::Color::RESET;
                std::string line;
                std::getline(std::cin, line);

                if (line == "/hint") {
                    hintUsed = true;
                    stats.hints_used_total++;
                    std::cout << "\n" << Terminal::Color::BRIGHT_YELLOW << persona->getHintResponse(q.hint) << "\n\n" << Terminal::Color::RESET;
                } else if (line == "/skip") {
                    answer = "Skipped question.";
                    break;
                } else if (line == "/finish") {
                    concludeSession();
                    return;
                } else if (!line.empty()) {
                    answer = line;
                    break;
                }
            }

            auto endTime = std::chrono::steady_clock::now();
            double elapsedSeconds = std::chrono::duration<double>(endTime - startTime).count();

            AnswerEvaluation eval = adaptive.evaluateAnswer(q, answer, elapsedSeconds, hintUsed, false, stats.persona);
            displayEvaluation(eval);
            recordEvaluation(eval);
        }

        concludeSession();
    }

    void runAutomatedDemo(Track track, PersonaType personaType) {
        Terminal::clearScreen();
        Terminal::printBanner();
        stats.candidate_name = "Candidate Demo";
        stats.track = track;
        persona = std::make_unique<InterviewerPersona>(personaType);
        stats.persona = personaType;

        std::cout << Terminal::Color::BRIGHT_GREEN << Terminal::Color::BOLD
                  << ">>> RUNNING AUTOMATED BENCHMARK DEMO (" << trackToString(track) << ") <<<\n\n"
                  << Terminal::Color::RESET;

        std::map<std::string, std::string> demoAnswers = {
            {"CPP_EASY_1", "RAII binds resource allocation to object lifetime. When unique_ptr is constructed on the stack, its destructor guarantees deterministic cleanup on scope exit, ensuring exception safety with zero runtime overhead compared to raw pointers."},
            {"CPP_MED_1", "Lvalues have persistent memory addresses; rvalues are temporary expressions. std::move casts to rvalue reference to enable resource pilfering. std::forward preserves value category during template argument deduction."},
            {"CPP_HARD_1", "Virtual dispatch uses a virtual method table (vtable) and a hidden vptr inside the object memory layout. A non-virtual base destructor leads to undefined behavior when deleted via base pointer."},
            {"DSA_EASY_1", "Hash tables use hash functions to map keys to bucket indices in O(1) amortized time. Separate chaining stores linked lists at buckets, while open addressing uses linear probing. Rehash doubles bucket capacity."},
            {"DSA_MED_1", "BFS operates in linear O(V + E) for unweighted graphs. Dijkstra requires a min-priority queue in O((V + E) log V) for non-negative weighted graphs; Bellman-Ford is needed for negative weights."},
            {"DSA_HARD_1", "LRU cache combines a Doubly Linked List for access recency with a Hash Map for O(1) key-to-node lookups and tail eviction."},
            {"SYS_EASY_1", "Horizontal scaling adds more instances behind a load balancer using round robin or least connections. Requires stateless application servers offloading state to Redis."},
            {"SYS_MED_1", "Cache-aside queries cache and backfills on miss. Cache stampede occurs when hot keys expire, mitigated with distributed mutexes, SingleFlight, and early expiration."},
            {"BEH_EASY_1", "Situation: Team disagreed on REST vs gRPC. Task: Resolve impasse objectively. Action: Built a 2-day benchmark measuring serialization overhead and latency. Result: Adopted gRPC and cut latency by 45%."}
        };

        for (int i = 0; i < 3; ++i) {
            Difficulty targetDiff = adaptive.determineNextDifficulty();
            Question q = pickNextQuestion(targetDiff);

            Terminal::printDivider("ROUND " + std::to_string(i + 1) + " | DIFFICULTY: [" + difficultyToString(q.difficulty) + "]");
            std::cout << Terminal::Color::BRIGHT_WHITE << Terminal::Color::BOLD << q.title << "\n\n" << Terminal::Color::RESET;
            std::cout << q.prompt << "\n\n";

            std::string ans = demoAnswers.count(q.id) ? demoAnswers[q.id] : "I analyze this from first principles, considering trade-offs and complexity bounds.";
            std::cout << Terminal::Color::BRIGHT_YELLOW << "Candidate Answer Submission:\n" << ans << "\n\n" << Terminal::Color::RESET;

            AnswerEvaluation eval = adaptive.evaluateAnswer(q, ans, 25.0, false, false, stats.persona);
            displayEvaluation(eval);
            recordEvaluation(eval);
        }

        concludeSession();
    }

private:
    Question pickNextQuestion(Difficulty diff) {
        auto list = bank.getQuestionsByTrackAndDifficulty(stats.track, diff);
        for (const auto& q : list) {
            if (!askedQuestionIds.count(q.id)) {
                askedQuestionIds.insert(q.id);
                return q;
            }
        }
        auto all = bank.getQuestionsByTrack(stats.track);
        for (const auto& q : all) {
            if (!askedQuestionIds.count(q.id)) {
                askedQuestionIds.insert(q.id);
                return q;
            }
        }
        return all.empty() ? Question{} : all.front();
    }

    void displayEvaluation(const AnswerEvaluation& eval) {
        Terminal::printDivider("EVALUATION");
        std::cout << "Calibration Score: " << Terminal::scoreStars(eval.score) 
                  << " " << std::fixed << std::setprecision(1) << eval.score << "/100  |  Elo: "
                  << eval.old_elo << " -> " << eval.new_elo << " (" << (eval.new_elo >= eval.old_elo ? "+" : "") << (eval.new_elo - eval.old_elo) << ")\n\n";
        
        if (!eval.concepts_covered.empty()) {
            std::cout << Terminal::Color::BRIGHT_GREEN << "Covered: " << Terminal::Color::RESET;
            for (const auto& c : eval.concepts_covered) std::cout << "[" << c << "] ";
            std::cout << "\n";
        }
        if (!eval.concepts_missed.empty()) {
            std::cout << Terminal::Color::BRIGHT_RED << "Missed: " << Terminal::Color::RESET;
            for (const auto& m : eval.concepts_missed) std::cout << "[" << m << "] ";
            std::cout << "\n";
        }
        std::cout << "\n" << Terminal::Color::BRIGHT_CYAN << eval.interviewer_comment << "\n\n" << Terminal::Color::RESET;
    }

    void recordEvaluation(const AnswerEvaluation& eval) {
        stats.evaluations.push_back(eval);
        stats.total_duration_seconds += eval.response_time_seconds;
        stats.current_elo = eval.new_elo;
        if (eval.new_elo > stats.peak_elo) stats.peak_elo = eval.new_elo;
    }

    void concludeSession() {
        Terminal::printDivider("FINAL SCORECARD");
        if (stats.evaluations.empty()) return;

        double sum = 0.0;
        for (const auto& e : stats.evaluations) sum += e.score;
        stats.overall_score = sum / stats.evaluations.size();
        stats.verdict = AdaptiveEngine::calculateFinalVerdict(stats.overall_score, stats.current_elo);

        std::cout << "Candidate: " << stats.candidate_name << "\n"
                  << "Track: " << trackToString(stats.track) << "\n"
                  << "Overall Score: " << std::fixed << std::setprecision(1) << stats.overall_score << " / 100\n"
                  << "Final Adaptive Elo: " << stats.current_elo << " (Peak: " << stats.peak_elo << ")\n\n"
                  << "Verdict: " << Terminal::Color::BOLD << verdictToString(stats.verdict) << Terminal::Color::RESET << "\n\n"
                  << persona->avatar << ": \"" << persona->getFinalVerdictSpeech(stats.verdict, stats.overall_score) << "\"\n\n";

        ReportGenerator::generateMarkdownReport(stats);
        ReportGenerator::generateHtmlReport(stats);
        std::cout << Terminal::Color::BRIGHT_GREEN << "✔ Reports saved in reports/ folder (Markdown & HTML)\n" << Terminal::Color::RESET;
    }
};

} // namespace MockInterview

// ============================================================================
// 8. MAIN ENTRY POINT
// ============================================================================

void printHelp() {
    std::cout << "Adaptive AI Mock Interview Simulation Bot (C++)\n\n"
              << "Usage:\n"
              << "  MockInterviewBot.exe                 Interactive Interview Mode\n"
              << "  MockInterviewBot.exe --demo          Run benchmark demo (C++ Track)\n"
              << "  MockInterviewBot.exe --demo dsa      Run benchmark demo (DSA Track)\n"
              << "  MockInterviewBot.exe --demo sys      Run benchmark demo (System Design Track)\n"
              << "  MockInterviewBot.exe --demo beh      Run benchmark demo (Behavioral Track)\n"
              << "  MockInterviewBot.exe --help          Show this help manual\n";
}

int main(int argc, char* argv[]) {
    try {
        MockInterview::Terminal::initConsole();

        std::string mode = "";
        std::string subTrack = "cpp";

        if (argc > 1) {
            mode = argv[1];
            if (argc > 2) subTrack = argv[2];
        }

        if (mode == "--help" || mode == "-h") {
            printHelp();
            return 0;
        }

        MockInterview::InterviewEngine engine;

        if (mode == "--demo" || mode == "-d") {
            MockInterview::Track selectedTrack = MockInterview::Track::MODERN_CPP;
            if (subTrack == "dsa") selectedTrack = MockInterview::Track::ALGORITHMS_DSA;
            else if (subTrack == "sys" || subTrack == "system") selectedTrack = MockInterview::Track::SYSTEM_DESIGN;
            else if (subTrack == "beh" || subTrack == "behavioral") selectedTrack = MockInterview::Track::BEHAVIORAL;

            engine.runAutomatedDemo(selectedTrack, MockInterview::PersonaType::BAR_RAISER);
            return 0;
        }

        // Standard interactive mode
        engine.run();

    } catch (const std::exception& ex) {
        std::cerr << "\n[Fatal Exception]: " << ex.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[Unknown Fatal Error]\n";
        return 1;
    }

    return 0;
}
