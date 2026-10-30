#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace MockInterview {

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
        case Track::BEHAVIORAL:     return "Behavioral & Leadership";
        case Track::FULL_MOCK_LOOP: return "Full Mock Loop (Multi-Round)";
    }
    return "General Tech";
}

enum class PersonaType {
    BAR_RAISER,      // Alex: Rigorous, asks edge cases, Big O, low tolerance for hand-waving
    TECH_LEAD,       // Elena: Pragmatic, focuses on trade-offs, testing, observability, maintenance
    SENIOR_ARCHITECT // Marcus: Systems thinker, mentoring tone, focuses on decoupling & patterns
};

inline std::string personaName(PersonaType p) {
    switch (p) {
        case PersonaType::BAR_RAISER:      return "Alex Vance (FAANG Bar Raiser)";
        case PersonaType::TECH_LEAD:       return "Elena Rostova (Staff Tech Lead)";
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
        case Verdict::STRONG_HIRE:     return "STRONG HIRE (L5/L6 Senior Level)";
        case Verdict::HIRE:            return "HIRE (Target Level Met)";
        case Verdict::LEANING_HIRE:    return "LEANING HIRE (Promising with minor gaps)";
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
    std::vector<std::string> key_concepts;       // Must-have technical keywords/concepts
    std::vector<std::string> bonus_concepts;     // Advanced concepts that elevate the answer
    std::vector<std::string> follow_up_prompts;  // Dynamic follow-ups
    std::string ideal_answer_summary;
    std::string common_pitfalls;
};

struct AnswerEvaluation {
    std::string question_id;
    std::string question_title;
    Difficulty difficulty;
    std::string topic;
    std::string user_answer;
    double score = 0.0;                       // 0.0 - 100.0
    int old_elo = 1200;
    int new_elo = 1200;
    std::vector<std::string> concepts_covered;
    std::vector<std::string> concepts_missed;
    std::vector<std::string> bonus_covered;
    std::string depth_rating;                 // "Surface Level", "Competent", "Mastery"
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
    std::map<std::string, double> topic_scores; // Topic -> Average Score
    std::map<Difficulty, int> difficulty_count;  // Questions answered per difficulty
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

} // namespace MockInterview
