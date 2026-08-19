#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <format>
#include <random>

// Validates whether a given 64-bit unsigned integer is a prime number.
bool is_prime(uint64_t n) {
    if (n <= 1) return false;
    // 2 and 3 are prime numbers.
    if (n <= 3) return true; 
    // Prime numbers only have 2 divisors: 1 and themselves. 
    // Eliminates multiples of 2 and 3 early for performance.
    if (n % 2 == 0 || n % 3 == 0) return false; 
    
    // Validates the remaining numbers starting from 25.
    for (uint64_t i = 5; i * i <= n; i += 6) { 
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

int main () {
    std::cout << "Booting up C++ Concurrent Engine... \n";
    
    // Initializes the C++20 stop mechanism to broadcast cancellation signals across all threads.
    std::stop_source global_kill_switch;
    std::stop_token stoken = global_kill_switch.get_token();

    // Shared global scoreboard. Uses CPU-level memory locks to prevent race conditions.
    std::atomic<uint64_t> winning_prime{0};

    // Lambda function defining the specific ETL/Search workload for each concurrent thread.
    auto worker_task = [&winning_prime, &global_kill_switch](std::stop_token st, int thread_id, uint64_t start_range) {
        
        // 'current' tracks the specific number this thread is evaluating.
        uint64_t current = start_range;

        // Continuously polls the stop token; exits loop gracefully if another thread broadcasts a stop request.
        while (!st.stop_requested()) {
            
            // Checks if the number is prime and ends in 99999 (simulating heavy computation).
            if(is_prime(current) && current % 100000 == 99999) {
                
                // The 'NULL' equivalent. If the scoreboard is 0, no thread has claimed victory yet.
                uint64_t expected = 0;

                // Thread-safe COALESCE operation at the hardware level.
                // Asks the CPU: "If winning_prime == expected (0), UPDATE it to the current massive number."
                // Locks the memory bus so only one thread can successfully execute this transaction.
                if(winning_prime.compare_exchange_strong(expected, current)) {
                    std::cout << std::format("\n>> [Thread {}] HIT THE JACKPOT: {} <<\n", thread_id, current);

                    // Transaction successful. Broadcasts a halt signal to shut down all other active threads.
                    // Flips the global flag, causing peer threads to evaluate !st.stop_requested() as false.
                    global_kill_switch.request_stop(); 
                }
                
                // Terminates the thread's execution. The race is over for this thread.
                return; 
            }
            // Increments the thread's personal counter to test the next number.
            current++;
        }
        
        // Executes only if the thread was gracefully halted by the global kill switch.
        std::cout << std::format("[Thread {}] Kill switch activated. Halting search safely.\n", thread_id);
    };

    // Dynamic vector container to manage the lifecycle of the CPU hardware threads.
    std::vector<std::jthread> threads;

    std::cout << "Deploying concurrent worker threads with randomized start lines...\n";

    // Dynamically queries the OS for the maximum number of logical cores (SMT/Hyper-Threading).
    unsigned int hardware_threads = std::thread::hardware_concurrency();
    std::cout << "Detected " << hardware_threads << " hardware threads available.\n";

    // Initializes the hardware-level random number generator for pure mathematical chaos.
    std::random_device rd;
    std::mt19937_64 gen(rd());
    
    // Configures a distribution filter to constrain the raw chaos into a specific mathematical boundary.
    std::uniform_int_distribution<uint64_t> dist(10'000'000'000ULL, 50'000'000'000ULL);

    // Spawns the threads, placing each in a race to find the first valid prime.
    for (unsigned int i = 0; i < hardware_threads; ++i) {
        // Generates a completely random starting block for the specific thread.
        uint64_t random_start = dist(gen);
        threads.emplace_back(worker_task, stoken, i + 1, random_start);
    }

    return 0;
}