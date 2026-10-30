#pragma once

#include "common.hpp"
#include <string>
#include <vector>
#include <random>

namespace MockInterview {

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
                       "Don't worry about memorizing trivia—focus on your problem-solving process and trade-offs.";
            case PersonaType::SENIOR_ARCHITECT:
                return "Hi " + candidateName + ", great to meet you! I'm Marcus. For our session on '" + trackName + 
                       "', I love seeing how engineers navigate ambiguity and scale. Treat this as an engineering whiteboarding session. "
                       "There are rarely single 'perfect' answers, only well-reasoned trade-offs.";
        }
        return "Welcome to the interview session.";
    }

    std::string getReaction(double score, bool hintUsed, const std::string& keyConceptMissed = "") const {
        if (score >= 85.0) {
            switch (type) {
                case PersonaType::BAR_RAISER:
                    return hintUsed ? "Good recovery after the hint, though try to derive the invariant independently next time." :
                                      "Impressive. You nailed the core invariants and articulated the trade-offs cleanly. Let's raise the bar.";
                case PersonaType::TECH_LEAD:
                    return "Solid answer! That's exactly how we approach this in production systems. Good instinct on edge cases.";
                case PersonaType::SENIOR_ARCHITECT:
                    return "Excellent structural thinking. You captured the architectural essence and articulated the trade-offs nicely.";
            }
        } else if (score >= 60.0) {
            switch (type) {
                case PersonaType::BAR_RAISER:
                    if (!keyConceptMissed.empty()) {
                        return "Decent direction, but you left out critical depth regarding " + keyConceptMissed + ". In a senior loop, that's a key requirement.";
                    }
                    return "You have the high-level intuition, but the technical specifics were somewhat hand-wavy.";
                case PersonaType::TECH_LEAD:
                    return "Good start! You covered the basics, though in a production rollout we'd need tighter bounds on failure modes.";
                case PersonaType::SENIOR_ARCHITECT:
                    return "Fair analysis. You've got the foundational concepts down, but consider what happens when constraints tighten.";
            }
        } else {
            switch (type) {
                case PersonaType::BAR_RAISER:
                    return "That answer was too surface-level. You skipped the underlying mechanics. Let's see if you can calibrate on the next one.";
                case PersonaType::TECH_LEAD:
                    return "Not quite there. That approach would hit serious bottlenecks and edge-case regressions. Keep composure and reflect on the fundamentals.";
                case PersonaType::SENIOR_ARCHITECT:
                    return "A bit fragmented. Take a breath and think from first principles about how the data flows and where the state lives.";
            }
        }
        return "Understood. Moving forward.";
    }

    std::string getHintResponse(const std::string& hintText) const {
        switch (type) {
            case PersonaType::BAR_RAISER:
                return "[Alex nods with a slight frown] 'Fair enough, let me give you a nudge, but note that asking for hints impacts the evaluation: " + hintText + "'";
            case PersonaType::TECH_LEAD:
                return "[Elena smiles constructively] 'No problem! Collaborative problem solving is normal. Consider this hint: " + hintText + "'";
            case PersonaType::SENIOR_ARCHITECT:
                return "[Marcus sketches on a whiteboard] 'Let's look at it from another angle: " + hintText + "'";
        }
        return "Hint: " + hintText;
    }

    std::string getClarificationResponse(const std::string& clarifyText) const {
        return "[Interviewer clarifies]: \"" + clarifyText + "\"";
    }

    std::string getFollowUpIntro() const {
        switch (type) {
            case PersonaType::BAR_RAISER:
                return "Interesting. Let me push you on a specific constraint. I want to test your depth here:";
            case PersonaType::TECH_LEAD:
                return "Building on what you just said—let's suppose our traffic spikes by 100x. Here is my follow-up:";
            case PersonaType::SENIOR_ARCHITECT:
                return "That brings up an intriguing architectural pivot. Consider this follow-up scenario:";
        }
        return "Follow-up question:";
    }

    std::string getFinalVerdictSpeech(Verdict verdict, double overallScore) const {
        std::string scoreStr = std::to_string(static_cast<int>(overallScore));
        switch (verdict) {
            case Verdict::STRONG_HIRE:
                return "Alex, Elena, and Marcus confer: 'Definite Strong Hire (" + scoreStr + 
                       "/100). You demonstrated senior-to-staff engineering command, precision in trade-offs, and deep technical clarity.'";
            case Verdict::HIRE:
                return "The hiring committee review: 'Hire (" + scoreStr + 
                       "/100). You met all target competencies with solid technical intuition and good problem-solving composure.'";
            case Verdict::LEANING_HIRE:
                return "Committee review: 'Leaning Hire (" + scoreStr + 
                       "/100). Solid baseline potential. Sharpen up your edge-case coverage and low-level mechanics for full mastery.'";
            case Verdict::LEANING_NO_HIRE:
                return "Committee review: 'Leaning No-Hire (" + scoreStr + 
                       "/100). Good effort, but several gaps in fundamental systems and complexity reasoning need further preparation.'";
            case Verdict::NO_HIRE:
                return "Committee review: 'No-Hire (" + scoreStr + 
                       "/100). Missing prerequisite conceptual foundations. Review the generated study syllabus and re-attempt!'";
        }
        return "Interview complete.";
    }
};

} // namespace MockInterview
