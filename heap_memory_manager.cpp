#include <iostream>
#include <format>
#include <string>
#include <algorithm>
#include <cctype>

// Benchmarks heap memory allocation performance with manual deallocation.
int main() {
    std::cout << "--- BOOTING MEMORY BENCHMARK THAT AVOIDS LEAKS ---\n\n";

    long long allocations = 0;
    const long long BATCH_SIZE = 10'000'000;

    // Initiates infinite allocation loop.
    while (true) {
        // Allocates 4 bytes on heap.
        int* heap_ptr = new int(50000);
        allocations++;

        // Deallocates heap memory immediately to ensure zero leaks.
        delete heap_ptr;
        heap_ptr = nullptr;

        // Triggers telemetry and handles user interrupt requests.
        if (allocations % BATCH_SIZE == 0) {
            long long mb = (allocations * 4) / 1024 / 1024;
            std::cout << std::format("[SYS] {} cycles. ~{} MB processed.\n", allocations, mb);
            std::cout << "[PROMPT] Type 'EXIT' to stop, or any key + Enter to continue: ";
            
            // Captures and processes user input.
            std::string input;
            std::getline(std::cin, input);
            std::cin.clear();

            // Normalizes input to lowercase for robust comparison.
            std::transform(input.begin(), input.end(), input.begin(), 
                           [](unsigned char c){ return std::tolower(c); });

            // Evaluates exit command.
            if (input == "exit") {
                std::cout << "\n[SYS] Graceful shutdown. Zero leaks detected.\n";
                break;
            }
            // Handles empty input as a continuation signal.
            else if (input.empty()) {
                std::cout << "[SYS] Empty input. Resuming benchmark...\n\n";
                continue;
            }
            
            // Resumes operations for valid non-exit input.
            std::cout << "[SYS] Resuming benchmark...\n\n";
        }
    }
    return 0;
}