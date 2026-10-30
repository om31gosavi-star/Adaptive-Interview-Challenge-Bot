#pragma once

#include "common.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <set>
#include <map>

namespace MockInterview {

class AdaptiveEngine {
private:
    int current_elo = 1200;
    int peak_elo = 1200;
    const int K_FACTOR = 48; // Responsive Elo adjustment

public:
    AdaptiveEngine(int startingElo = 1200) : current_elo(startingElo), peak_elo(startingElo) {}

    int getCurrentElo() const { return current_elo; }
    int getPeakElo() const { return peak_elo; }

    Difficulty determineNextDifficulty() const {
        if (current_elo >= 1500) return Difficulty::EXPERT;
        if (current_elo >= 1320) return Difficulty::HARD;
        if (current_elo >= 1120) return Difficulty::MEDIUM;
        return Difficulty::EASY;
    }

    // Evaluates a candidate's answer against a question
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

        // 1. Concept Coverage Evaluation
        int keyMatches = 0;
        for (const auto& concept : question.key_concepts) {
            if (containsConcept(normalizedAnswer, concept)) {
                eval.concepts_covered.push_back(concept);
                keyMatches++;
            } else {
                eval.concepts_missed.push_back(concept);
            }
        }

        // 2. Bonus Concepts
        for (const auto& bonus : question.bonus_concepts) {
            if (containsConcept(normalizedAnswer, bonus)) {
                eval.bonus_covered.push_back(bonus);
            }
        }

        // 3. Depth & Reasoning Analysis
        // Check for analytical connective phrases indicating trade-off / edge-case thinking
        static const std::vector<std::string> analyticalKeywords = {
            "trade-off", "tradeoff", "overhead", "bottleneck", "edge case", "complexity", 
            "amortized", "latency", "throughput", "cache miss", "failure", "in practice", 
            "benchmark", "because", "furthermore", "mitigate", "prevent", "in order to"
        };
        int analyticalSignals = 0;
        for (const auto& kw : analyticalKeywords) {
            if (containsConcept(normalizedAnswer, kw)) {
                analyticalSignals++;
            }
        }

        // Behavioral STAR structure detection if behavioral track
        bool hasStarStructure = false;
        if (question.track == Track::BEHAVIORAL) {
            bool hasS = containsConcept(normalizedAnswer, "situation");
            bool hasT = containsConcept(normalizedAnswer, "task");
            bool hasA = containsConcept(normalizedAnswer, "action");
            bool hasR = containsConcept(normalizedAnswer, "result");
            if ((hasS && hasA && hasR) || (hasS && hasT && hasA)) {
                hasStarStructure = true;
            }
        }

        // 4. Calculate Sub-Scores
        double keyRatio = question.key_concepts.empty() ? 1.0 :
                          static_cast<double>(keyMatches) / question.key_concepts.size();
        
        double bonusRatio = question.bonus_concepts.empty() ? 0.0 :
                            (std::min)(1.0, static_cast<double>(eval.bonus_covered.size()) / 2.0);

        double lengthScore = 0.0;
        size_t wordCount = tokens.size();
        if (wordCount >= 40) lengthScore = 1.0;
        else if (wordCount >= 20) lengthScore = 0.7;
        else if (wordCount >= 10) lengthScore = 0.4;
        else lengthScore = 0.15;

        double analyticalRatio = (std::min)(1.0, analyticalSignals / 3.0);
        if (hasStarStructure) analyticalRatio = (std::min)(1.0, analyticalRatio + 0.35);

        // Weighted raw score (0 - 100)
        double score = (keyRatio * 55.0) + (analyticalRatio * 25.0) + (bonusRatio * 15.0) + (lengthScore * 5.0);

        // Adjust for penalties
        if (hintUsed) score -= 10.0;
        if (clarificationUsed) score -= 2.0;
        if (score < 0.0) score = 0.0;
        if (score > 100.0) score = 100.0;

        eval.score = score;

        // Depth Categorization
        if (score >= 85.0) {
            eval.depth_rating = "Deep Technical Mastery";
        } else if (score >= 60.0) {
            eval.depth_rating = "Competent Working Knowledge";
        } else if (score >= 35.0) {
            eval.depth_rating = "Surface Level Understanding";
        } else {
            eval.depth_rating = "Rudimentary / Incomplete";
        }

        // Elo Rating Update
        updateElo(eval, question.difficulty);

        // Construct Constructive Feedback
        generateFeedback(eval, question, persona);

        return eval;
    }

    bool shouldTriggerFollowUp(const AnswerEvaluation& eval, const Question& q) const {
        if (q.follow_up_prompts.empty()) return false;
        // Trigger follow-up if:
        // 1. Candidate is close but missed 1 or 2 specific concepts (score 45 - 80)
        // 2. Candidate answered exceptionally well on Hard/Expert (score >= 88)
        if (eval.score >= 45.0 && eval.score <= 80.0 && !eval.concepts_missed.empty()) {
            return true;
        }
        if (eval.score >= 88.0 && (q.difficulty == Difficulty::HARD || q.difficulty == Difficulty::EXPERT)) {
            return true;
        }
        return false;
    }

    std::string selectFollowUpPrompt(const Question& q, const AnswerEvaluation& eval) const {
        if (q.follow_up_prompts.empty()) return "";
        if (eval.score >= 85.0 && q.follow_up_prompts.size() > 1) {
            // Give the second (often deeper) follow-up
            return q.follow_up_prompts[1];
        }
        return q.follow_up_prompts[0];
    }

    static Verdict calculateFinalVerdict(double overallScore, int finalElo) {
        if (finalElo >= 1520 && overallScore >= 82.0) {
            return Verdict::STRONG_HIRE;
        } else if (finalElo >= 1340 && overallScore >= 70.0) {
            return Verdict::HIRE;
        } else if (finalElo >= 1180 && overallScore >= 55.0) {
            return Verdict::LEANING_HIRE;
        } else if (finalElo >= 1050 && overallScore >= 40.0) {
            return Verdict::LEANING_NO_HIRE;
        } else {
            return Verdict::NO_HIRE;
        }
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

        // Strengths
        if (!eval.concepts_covered.empty()) {
            strengths << "Accurately identified and articulated: ";
            for (size_t i = 0; i < eval.concepts_covered.size(); ++i) {
                strengths << "'" << eval.concepts_covered[i] << "'";
                if (i + 1 < eval.concepts_covered.size()) strengths << ", ";
            }
            strengths << ". ";
        }
        if (!eval.bonus_covered.empty()) {
            strengths << "Demonstrated senior-level nuance by bringing up: ";
            for (size_t i = 0; i < eval.bonus_covered.size(); ++i) {
                strengths << "'" << eval.bonus_covered[i] << "'";
                if (i + 1 < eval.bonus_covered.size()) strengths << ", ";
            }
            strengths << ". ";
        }
        if (strengths.str().empty()) {
            strengths << "Attempted the question and maintained engagement.";
        }
        eval.feedback_strengths = strengths.str();

        // Improvements
        if (!eval.concepts_missed.empty()) {
            improvements << "To level up this answer, integrate key concepts: ";
            for (size_t i = 0; i < eval.concepts_missed.size(); ++i) {
                improvements << "'" << eval.concepts_missed[i] << "'";
                if (i + 1 < eval.concepts_missed.size()) improvements << ", ";
            }
            improvements << ". ";
        }
        if (!q.common_pitfalls.empty()) {
            improvements << "Common pitfall to avoid: " << q.common_pitfalls << " ";
        }
        if (eval.hint_used) {
            improvements << "Note: Relying on hints incurs a slight penalty in senior calibration loops.";
        }
        if (improvements.str().empty()) {
            improvements << "Outstanding response. Keep this level of clarity and depth across subsequent rounds.";
        }
        eval.feedback_improvements = improvements.str();

        // Persona Reaction
        std::string missed = eval.concepts_missed.empty() ? "" : eval.concepts_missed.front();
        eval.interviewer_comment = getPersonaComment(persona, eval.score, eval.hint_used, missed);
    }

    std::string getPersonaComment(PersonaType persona, double score, bool hintUsed, const std::string& missed) {
        if (persona == PersonaType::BAR_RAISER) {
            if (score >= 85.0) {
                return hintUsed ? "Alex [Bar Raiser]: 'Good recovery after the hint, though I expect a senior candidate to derive this independently.'" :
                                  "Alex [Bar Raiser]: 'Very rigorous. You demonstrated the mechanical precision I expect from a senior engineer.'";
            }
            if (score >= 60.0) return "Alex [Bar Raiser]: 'You had the broad intuition, but missed depth on " + (missed.empty() ? "edge cases" : missed) + ". In our bar raiser rounds, that makes a big difference.'";
            return "Alex [Bar Raiser]: 'That lacked technical precision. You need to anchor your answers in concrete complexity and systems behavior.'";
        } else if (persona == PersonaType::TECH_LEAD) {
            if (score >= 85.0) return "Elena [Tech Lead]: 'Great job. You focused on practical trade-offs and code safety. Exactly how we want our team to operate.'";
            if (score >= 60.0) return "Elena [Tech Lead]: 'Good practical instinct! Consider how that handles failure modes and maintainability under load.'";
            return "Elena [Tech Lead]: 'In production, that shortcut would cause unexpected regressions or outages. Always consider the boundary failure conditions.'";
        } else {
            if (score >= 85.0) return "Marcus [Architect]: 'Elegant structural thinking. You balanced decoupling and scaling constraints very smoothly.'";
            if (score >= 60.0) return "Marcus [Architect]: 'A solid start. Remember that every architecture is a compromise—be explicit about what you are sacrificing.'";
            return "Marcus [Architect]: 'The components were disjointed. Step back and consider how data, state, and failure flow through the system.'";
        }
    }

    static std::string normalizeText(const std::string& input) {
        std::string result;
        result.reserve(input.size());
        for (char c : input) {
            if (std::isalnum(static_cast<unsigned char>(c)) || c == ' ' || c == '_' || c == '-' || c == '+' || c == '(' || c == ')') {
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
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

    static bool containsConcept(const std::string& text, const std::string& concept) {
        std::string normConcept = normalizeText(concept);
        if (normConcept.empty()) return false;

        // Check exact substring match
        if (text.find(normConcept) != std::string::npos) {
            return true;
        }

        // Synonyms & Aliases
        static const std::map<std::string, std::vector<std::string>> synonyms = {
            {"unique_ptr", {"unique ptr", "smart pointer", "std::unique_ptr", "unique pointer"}},
            {"raii", {"resource acquisition is initialization", "destructor cleanup", "automatic cleanup"}},
            {"move semantics", {"std::move", "rvalue reference", "move constructor", "pilfer resource"}},
            {"std::forward", {"forward", "perfect forwarding", "forwarding reference", "universal reference"}},
            {"vtable", {"virtual table", "vptr", "dynamic dispatch", "virtual method table"}},
            {"o(1)", {"constant time", "o (1)", "order 1", "constant complexity"}},
            {"o((v + e) log v)", {"v log v", "e log v", "log v", "heap complexity"}},
            {"dijkstra", {"dijkstra's", "priority queue graph", "min-heap shortest path"}},
            {"bfs", {"breadth first search", "breadth-first", "level by level"}},
            {"hash map", {"hash table", "unordered_map", "hashmap", "dictionary"}},
            {"doubly linked list", {"doubly linked", "bidirectional list", "dll"}},
            {"b+ tree", {"b plus tree", "b-tree", "b tree"}},
            {"lsm tree", {"lsm", "log structured merge", "sstable", "memtable"}},
            {"bloom filter", {"bloom filters", "probabilistic filter", "false positive filter"}},
            {"count-min sketch", {"count min", "cms", "probabilistic counter", "heavy hitters"}},
            {"token bucket", {"token-bucket", "leaky bucket", "rate limit algorithm"}},
            {"single point of failure", {"spof", "single point"}},
            {"load balancer", {"load balancing", "alb", "nlb", "reverse proxy"}},
            {"cache stampede", {"thundering herd", "cache dogpiling", "stampede"}},
            {"situation", {"the situation", "context was", "at my previous"}},
            {"action", {"i took the action", "my action", "i implemented", "i led"}},
            {"result", {"the result was", "as a result", "outcome was", "impact was"}}
        };

        auto it = synonyms.find(normConcept);
        if (it != synonyms.end()) {
            for (const auto& syn : it->second) {
                if (text.find(syn) != std::string::npos) {
                    return true;
                }
            }
        }
        return false;
    }
};

} // namespace MockInterview
