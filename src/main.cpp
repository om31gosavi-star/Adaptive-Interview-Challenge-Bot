#if defined(__has_include)
  #if __has_include("../include/interview_engine.hpp")
    #include "../include/common.hpp"
    #include "../include/ansi_terminal.hpp"
    #include "../include/interview_engine.hpp"
  #elif __has_include("include/interview_engine.hpp")
    #include "include/common.hpp"
    #include "include/ansi_terminal.hpp"
    #include "include/interview_engine.hpp"
  #else
    #include "common.hpp"
    #include "ansi_terminal.hpp"
    #include "interview_engine.hpp"
  #endif
#else
  #include "../include/common.hpp"
  #include "../include/ansi_terminal.hpp"
  #include "../include/interview_engine.hpp"
#endif

#include <iostream>
#include <string>
#include <vector>

void printHelp() {
    std::cout << "Adaptive AI Mock Interview Simulation Bot (C++)\n";
    std::cout << "Usage:\n";
    std::cout << "  MockInterviewBot.exe                 Interactive Interview Mode\n";
    std::cout << "  MockInterviewBot.exe --demo          Run automated benchmark simulation (C++ Track)\n";
    std::cout << "  MockInterviewBot.exe --demo dsa      Run automated benchmark simulation (DSA Track)\n";
    std::cout << "  MockInterviewBot.exe --demo sys      Run automated benchmark simulation (System Design Track)\n";
    std::cout << "  MockInterviewBot.exe --help          Show this usage manual\n";
}

int main(int argc, char* argv[]) {
    try {
        MockInterview::Terminal::initConsole();

        std::string mode = "";
        std::string subTrack = "cpp";

        if (argc > 1) {
            mode = argv[1];
            if (argc > 2) {
                subTrack = argv[2];
            }
        }

        if (mode == "--help" || mode == "-h") {
            printHelp();
            return 0;
        }

        MockInterview::InterviewEngine engine(true);

        if (mode == "--demo" || mode == "-d") {
            MockInterview::Track selectedTrack = MockInterview::Track::MODERN_CPP;
            if (subTrack == "dsa") {
                selectedTrack = MockInterview::Track::ALGORITHMS_DSA;
            } else if (subTrack == "sys" || subTrack == "system") {
                selectedTrack = MockInterview::Track::SYSTEM_DESIGN;
            } else if (subTrack == "beh" || subTrack == "behavioral") {
                selectedTrack = MockInterview::Track::BEHAVIORAL;
            }
            engine.runAutomatedDemo(selectedTrack, MockInterview::PersonaType::BAR_RAISER);
            return 0;
        }

        // Standard interactive mode
        engine.run();

    } catch (const std::exception& ex) {
        std::cerr << "\n[Fatal Exception Encountered]: " << ex.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[Unknown Fatal Error Encountered]\n";
        return 1;
    }

    return 0;
}
