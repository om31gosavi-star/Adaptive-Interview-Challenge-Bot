#pragma once

#include "common.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>

namespace MockInterview {

class ReportGenerator {
public:
    static std::string generateMarkdownReport(const SessionStatistics& stats, const std::string& outputDir = "reports") {
        std::string safeName = sanitizeFilename(stats.candidate_name);
        std::string filename = outputDir + "/interview_report_" + safeName + ".md";

        std::ofstream file(filename);
        if (!file.is_open()) {
            return "";
        }

        file << "# 🎯 AI Mock Interview Performance Evaluation Report\n\n";
        file << "> **Session Timestamp:** " << stats.start_time << "  \n";
        file << "> **Candidate Name:** " << stats.candidate_name << "  \n";
        file << "> **Track:** " << trackToString(stats.track) << "  \n";
        file << "> **Interviewer Persona:** " << personaName(stats.persona) << "  \n";
        file << "> **Total Duration:** " << std::fixed << std::setprecision(1) 
             << (stats.total_duration_seconds / 60.0) << " minutes\n\n";

        file << "---\n\n";
        file << "## 🏆 Executive Summary & Hiring Verdict\n\n";
        file << "| Metric | Value |\n";
        file << "| :--- | :--- |\n";
        file << "| **Final Verdict** | **" << verdictToString(stats.verdict) << "** |\n";
        file << "| **Overall Calibration Score** | **" << std::fixed << std::setprecision(1) << stats.overall_score << " / 100** |\n";
        file << "| **Starting Elo Rating** | " << stats.starting_elo << " |\n";
        file << "| **Final Adaptive Elo** | **" << stats.current_elo << "** (Peak: " << stats.peak_elo << ") |\n";
        file << "| **Total Questions Answered** | " << stats.evaluations.size() << " |\n";
        file << "| **Hints / Clarifications Used** | " << stats.hints_used_total << " |\n\n";

        // Topic Breakdown
        file << "## 📊 Topic & Competency Breakdown\n\n";
        file << "| Topic | Average Score | Performance Tier |\n";
        file << "| :--- | :---: | :--- |\n";
        for (const auto& pair : stats.topic_scores) {
            std::string tier = "Mastery (L5/L6)";
            if (pair.second < 45.0) tier = "Needs Foundational Study";
            else if (pair.second < 65.0) tier = "Working Knowledge (L3/L4)";
            else if (pair.second < 85.0) tier = "Solid Competency (L4/L5)";
            
            file << "| " << pair.first << " | " 
                 << std::fixed << std::setprecision(1) << pair.second << "% | " 
                 << tier << " |\n";
        }
        file << "\n---\n\n";

        // Detailed Question-by-Question Transcript
        file << "## 📝 Detailed Round-by-Round Breakdown\n\n";
        int roundNum = 1;
        for (const auto& eval : stats.evaluations) {
            file << "### Round " << roundNum++ << ": " << eval.question_title 
                 << " `[" << difficultyToString(eval.difficulty) << "]`\n\n";
            file << "- **Topic:** " << eval.topic << "\n";
            file << "- **Round Score:** **" << std::fixed << std::setprecision(1) << eval.score << " / 100**\n";
            file << "- **Elo Adjustment:** `" << eval.old_elo << " -> " << eval.new_elo 
                 << " (" << (eval.new_elo >= eval.old_elo ? "+" : "") << (eval.new_elo - eval.old_elo) << ")`\n";
            file << "- **Depth Assessment:** *" << eval.depth_rating << "*\n";
            file << "- **Response Time:** " << std::fixed << std::setprecision(1) << eval.response_time_seconds << "s\n";
            if (eval.hint_used) file << "- **Note:** Candidate requested an architectural hint.\n";
            file << "\n";

            file << "#### 💬 Candidate's Response:\n";
            file << "> " << eval.user_answer << "\n\n";

            file << "#### 🔍 Concept Coverage:\n";
            if (!eval.concepts_covered.empty()) {
                file << "- **Covered Key Invariants (✅):** ";
                for (size_t i = 0; i < eval.concepts_covered.size(); ++i) {
                    file << "`" << eval.concepts_covered[i] << "`" << (i + 1 < eval.concepts_covered.size() ? ", " : "");
                }
                file << "\n";
            }
            if (!eval.concepts_missed.empty()) {
                file << "- **Missed Key Nuances (❌):** ";
                for (size_t i = 0; i < eval.concepts_missed.size(); ++i) {
                    file << "`" << eval.concepts_missed[i] << "`" << (i + 1 < eval.concepts_missed.size() ? ", " : "");
                }
                file << "\n";
            }
            if (!eval.bonus_covered.empty()) {
                file << "- **Advanced Insights (🌟):** ";
                for (size_t i = 0; i < eval.bonus_covered.size(); ++i) {
                    file << "`" << eval.bonus_covered[i] << "`" << (i + 1 < eval.bonus_covered.size() ? ", " : "");
                }
                file << "\n";
            }
            file << "\n";

            file << "#### 🤖 Interviewer Evaluation & Critique:\n";
            file << "- **Strengths:** " << eval.feedback_strengths << "\n";
            file << "- **Growth Areas:** " << eval.feedback_improvements << "\n";
            file << "- **Interviewer Note:** *" << eval.interviewer_comment << "*\n\n";
            file << "---\n\n";
        }

        // Recommendations & Syllabus
        file << "## 📚 Targeted Next-Steps & Recommended Study Plan\n\n";
        file << "Based on the nuances missed during this simulation, focus your preparation on:\n\n";
        
        bool anyMissed = false;
        std::set<std::string> uniqueMissed;
        for (const auto& eval : stats.evaluations) {
            for (const auto& m : eval.concepts_missed) {
                uniqueMissed.insert(m);
            }
        }

        if (!uniqueMissed.empty()) {
            anyMissed = true;
            for (const auto& concept : uniqueMissed) {
                file << "- [ ] **Deepen mastery on:** `" << concept << "` (explore hardware trade-offs, standard library specs, and edge failure modes).\n";
            }
        }
        if (!anyMissed) {
            file << "- [x] **Exceptional performance across all tested domains!** Continue practicing Staff-level distributed systems and low-latency cache benchmarking.\n";
        }

        file << "\n*Generated automatically by AI Mock Interview Simulation Engine (C++)*\n";
        file.close();

        return filename;
    }

    static std::string generateHtmlReport(const SessionStatistics& stats, const std::string& outputDir = "reports") {
        std::string safeName = sanitizeFilename(stats.candidate_name);
        std::string filename = outputDir + "/interview_report_" + safeName + ".html";

        std::ofstream file(filename);
        if (!file.is_open()) {
            return "";
        }

        std::string verdictBadgeColor = "#22c55e"; // green
        if (stats.verdict == Verdict::LEANING_NO_HIRE) verdictBadgeColor = "#f59e0b"; // yellow
        else if (stats.verdict == Verdict::NO_HIRE) verdictBadgeColor = "#ef4444"; // red

        file << R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AI Mock Interview Report - )" << stats.candidate_name << R"(</title>
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700;800&family=JetBrains+Mono:wght@400;500;600&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg: #090d16;
            --surface: #111827;
            --surface-card: #1a2234;
            --border: #26334d;
            --primary: #38bdf8;
            --primary-glow: rgba(56, 189, 248, 0.25);
            --accent: #818cf8;
            --success: #22c55e;
            --warning: #f59e0b;
            --danger: #ef4444;
            --text-main: #f1f5f9;
            --text-dim: #94a3b8;
            --text-code: #38bdf8;
        }

        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, sans-serif;
            background-color: var(--bg);
            color: var(--text-main);
            line-height: 1.6;
            padding: 30px 20px;
        }
        .container {
            max-width: 1080px;
            margin: 0 auto;
        }
        header {
            background: linear-gradient(135deg, rgba(30, 41, 59, 0.9), rgba(15, 23, 42, 0.95));
            border: 1px solid var(--border);
            border-radius: 16px;
            padding: 35px;
            margin-bottom: 30px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.4);
            position: relative;
            overflow: hidden;
        }
        header::before {
            content: '';
            position: absolute;
            top: 0; left: 0; right: 0; height: 4px;
            background: linear-gradient(90deg, #38bdf8, #818cf8, #c084fc);
        }
        .header-top {
            display: flex;
            justify-content: space-between;
            align-items: flex-start;
            flex-wrap: wrap;
            gap: 20px;
        }
        .title h1 {
            font-size: 28px;
            font-weight: 800;
            background: linear-gradient(135deg, #f8fafc, #94a3b8);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            margin-bottom: 6px;
        }
        .title p {
            color: var(--text-dim);
            font-size: 14px;
        }
        .verdict-badge {
            background: )" << verdictBadgeColor << R"(20;
            border: 1px solid )" << verdictBadgeColor << R"(;
            color: )" << verdictBadgeColor << R"(;
            padding: 10px 20px;
            border-radius: 999px;
            font-size: 15px;
            font-weight: 700;
            letter-spacing: 0.5px;
            display: inline-flex;
            align-items: center;
            gap: 8px;
        }
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        .stat-card {
            background: var(--surface);
            border: 1px solid var(--border);
            border-radius: 14px;
            padding: 22px;
            display: flex;
            flex-direction: column;
            justify-content: space-between;
            box-shadow: 0 4px 15px rgba(0,0,0,0.2);
            position: relative;
        }
        .stat-card .label {
            font-size: 13px;
            font-weight: 500;
            color: var(--text-dim);
            text-transform: uppercase;
            letter-spacing: 0.8px;
            margin-bottom: 8px;
        }
        .stat-card .value {
            font-size: 32px;
            font-weight: 800;
            color: #fff;
        }
        .stat-card .sub {
            font-size: 12px;
            color: var(--text-dim);
            margin-top: 6px;
        }
        .section-title {
            font-size: 20px;
            font-weight: 700;
            margin: 35px 0 20px 0;
            display: flex;
            align-items: center;
            gap: 12px;
            color: #fff;
        }
        .section-title::before {
            content: '';
            display: inline-block;
            width: 4px;
            height: 22px;
            background: var(--primary);
            border-radius: 2px;
        }
        .topic-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
            gap: 16px;
            margin-bottom: 30px;
        }
        .topic-card {
            background: var(--surface);
            border: 1px solid var(--border);
            border-radius: 12px;
            padding: 18px;
        }
        .topic-header {
            display: flex;
            justify-content: space-between;
            font-weight: 600;
            font-size: 14px;
            margin-bottom: 10px;
        }
        .bar-bg {
            background: rgba(255,255,255,0.06);
            height: 8px;
            border-radius: 4px;
            overflow: hidden;
        }
        .bar-fill {
            height: 100%;
            background: linear-gradient(90deg, #38bdf8, #818cf8);
            border-radius: 4px;
        }
        .question-card {
            background: var(--surface);
            border: 1px solid var(--border);
            border-radius: 14px;
            margin-bottom: 24px;
            overflow: hidden;
            box-shadow: 0 4px 15px rgba(0,0,0,0.2);
        }
        .q-header {
            background: var(--surface-card);
            padding: 18px 24px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-bottom: 1px solid var(--border);
            flex-wrap: wrap;
            gap: 10px;
        }
        .q-title {
            font-weight: 700;
            font-size: 16px;
            color: #fff;
        }
        .badge {
            font-size: 11px;
            font-weight: 700;
            padding: 4px 10px;
            border-radius: 6px;
            text-transform: uppercase;
        }
        .badge-diff {
            background: #3b82f622;
            color: #60a5fa;
            border: 1px solid #3b82f655;
        }
        .badge-score {
            background: #22c55e22;
            color: #4ade80;
            border: 1px solid #22c55e55;
        }
        .q-body {
            padding: 24px;
        }
        .user-answer {
            background: #0d131f;
            border-left: 3px solid var(--primary);
            padding: 16px;
            border-radius: 0 8px 8px 0;
            font-family: 'JetBrains Mono', monospace;
            font-size: 13.5px;
            color: #cbd5e1;
            margin-bottom: 20px;
            white-space: pre-wrap;
        }
        .pill-container {
            display: flex;
            flex-wrap: wrap;
            gap: 8px;
            margin: 12px 0 20px 0;
        }
        .pill {
            font-size: 12px;
            font-weight: 600;
            padding: 4px 12px;
            border-radius: 999px;
            display: inline-flex;
            align-items: center;
            gap: 6px;
        }
        .pill-covered {
            background: #15803d22;
            color: #86efac;
            border: 1px solid #16a34a44;
        }
        .pill-missed {
            background: #b91c1c22;
            color: #fca5a5;
            border: 1px solid #dc262644;
        }
        .pill-bonus {
            background: #7e22ce22;
            color: #d8b4fe;
            border: 1px solid #9333ea44;
        }
        .feedback-box {
            background: rgba(255,255,255,0.02);
            border: 1px solid rgba(255,255,255,0.06);
            border-radius: 10px;
            padding: 16px;
            margin-top: 15px;
        }
        .feedback-box h4 {
            font-size: 13px;
            text-transform: uppercase;
            letter-spacing: 0.5px;
            margin-bottom: 6px;
            color: var(--text-dim);
        }
        .persona-quote {
            font-style: italic;
            color: #93c5fd;
            border-left: 2px solid #60a5fa;
            padding-left: 12px;
            margin-top: 12px;
            font-size: 13.5px;
        }
        footer {
            text-align: center;
            padding: 40px 0 20px 0;
            color: var(--text-dim);
            font-size: 13px;
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <div class="header-top">
                <div class="title">
                    <h1>)" << stats.candidate_name << R"( &mdash; AI Mock Interview Evaluation</h1>
                    <p>)" << trackToString(stats.track) << " &bull; Conducted by " << personaName(stats.persona) << " &bull; " << stats.start_time << R"(</p>
                </div>
                <div class="verdict-badge">
                    <span>&#10004;</span> )" << verdictToString(stats.verdict) << R"(
                </div>
            </div>
        </header>

        <div class="stats-grid">
            <div class="stat-card">
                <div class="label">Calibration Score</div>
                <div class="value">)" << std::fixed << std::setprecision(1) << stats.overall_score << R"(%</div>
                <div class="sub">Weighted Multi-Factor Metric</div>
            </div>
            <div class="stat-card">
                <div class="label">Final Elo Rating</div>
                <div class="value">)" << stats.current_elo << R"(</div>
                <div class="sub">Peak Achieved: )" << stats.peak_elo << R"( (Started at )" << stats.starting_elo << R"()</div>
            </div>
            <div class="stat-card">
                <div class="label">Total Rounds</div>
                <div class="value">)" << stats.evaluations.size() << R"(</div>
                <div class="sub">Adaptive Probing Active</div>
            </div>
            <div class="stat-card">
                <div class="label">Total Time</div>
                <div class="value">)" << std::fixed << std::setprecision(1) << (stats.total_duration_seconds / 60.0) << R"(m</div>
                <div class="sub">Hints Used: )" << stats.hints_used_total << R"(</div>
            </div>
        </div>

        <h3 class="section-title">Topic Competency Matrix</h3>
        <div class="topic-grid">)";

        for (const auto& pair : stats.topic_scores) {
            file << R"(
            <div class="topic-card">
                <div class="topic-header">
                    <span>)" << pair.first << R"(</span>
                    <span style="color: var(--primary);">)" << static_cast<int>(pair.second) << R"(%</span>
                </div>
                <div class="bar-bg">
                    <div class="bar-fill" style="width: )" << static_cast<int>((std::min)(100.0, pair.second)) << R"(%;"></div>
                </div>
            </div>)";
        }

        file << R"(
        </div>

        <h3 class="section-title">Round-by-Round Interview Transcripts & Analysis</h3>)";

        int qIndex = 1;
        for (const auto& eval : stats.evaluations) {
            file << R"(
        <div class="question-card">
            <div class="q-header">
                <div class="q-title">Round )" << qIndex++ << ": " << eval.question_title << R"(</div>
                <div style="display: flex; gap: 8px;">
                    <span class="badge badge-diff">)" << difficultyToString(eval.difficulty) << R"(</span>
                    <span class="badge badge-score">Score: )" << std::fixed << std::setprecision(0) << eval.score << R"(/100</span>
                </div>
            </div>
            <div class="q-body">
                <div style="font-size: 13px; color: var(--text-dim); margin-bottom: 8px;">Candidate Technical Submission:</div>
                <div class="user-answer">)" << eval.user_answer << R"(</div>

                <div style="font-size: 13px; font-weight: 600; color: #fff;">Concepts Evaluated:</div>
                <div class="pill-container">)";

            for (const auto& c : eval.concepts_covered) {
                file << R"(<span class="pill pill-covered">&#10003; )" << c << "</span>";
            }
            for (const auto& m : eval.concepts_missed) {
                file << R"(<span class="pill pill-missed">&#10007; Missed: )" << m << "</span>";
            }
            for (const auto& b : eval.bonus_covered) {
                file << R"(<span class="pill pill-bonus">&#9733; Advanced: )" << b << "</span>";
            }

            file << R"(
                </div>

                <div class="feedback-box">
                    <h4>Depth Assessment: <span style="color: #fff;">)" << eval.depth_rating << R"(</span> &bull; Response Time: )" 
                 << std::fixed << std::setprecision(1) << eval.response_time_seconds << R"(s</h4>
                    <p style="font-size: 14px; margin-bottom: 6px;"><strong>Strengths:</strong> )" << eval.feedback_strengths << R"(</p>
                    <p style="font-size: 14px;"><strong>Recommended Addition:</strong> )" << eval.feedback_improvements << R"(</p>
                    <div class="persona-quote">)" << eval.interviewer_comment << R"(</div>
                </div>
            </div>
        </div>)";
        }

        file << R"(
        <footer>
            Adaptive AI Mock Interview Simulation Engine &bull; Developed in Modern C++ &bull; 
            Exported on )" << stats.start_time << R"(
        </footer>
    </div>
</body>
</html>)";

        file.close();
        return filename;
    }
};

} // namespace MockInterview
