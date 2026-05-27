#include <iostream>
#include <format>

// Demonstrates bare-metal array traversal using raw Pointer Arithmetic.
// Proves that C-style arrays are just contiguous memory blocks under the hood.
int main() {
    std::cout << "--- Booting Pointer Math Radar ---\n\n";

    // Allocates a contiguous block of memory for 5 integers (5 * 4 bytes = 20 bytes total).
    int secure_data[5] = {100, 200, 300, 400, 500};

    // The array name 'secure_data' is implicitly a raw pointer to the first element's address.
    int* radar_ptr = secure_data; 

    // Accesses the first value in memory (Index 0: 100) using the base pointer.
    std::cout << "[System] Array allocated in memory.\n";
    std::cout << std::format("Base Address (radar_ptr): {}\n", (void*)radar_ptr);
    std::cout << std::format("Base Value (*radar_ptr): {}\n\n", *radar_ptr);

    // --- POINTER ARITHMETIC ---
    std::cout << "--- Executing Mathematical Memory Jumps ---\n";
    
    // Jumps +1 element forward. 
    // The CPU knows an 'int' is 4 bytes, so it automatically shifts exactly 4 bytes in RAM.
    // Effectively accesses the value at Index 1 (200).
    std::cout << std::format("Jump +1 Address: {}\n", (void*)(radar_ptr + 1));
    std::cout << std::format("Jump +1 Value (*(radar_ptr + 1)): {}\n\n", *(radar_ptr + 1));

    // Jumps +3 elements forward (12 bytes).
    // Effectively accesses the value at Index 3 (400).
    std::cout << std::format("Jump +3 Address: {}\n", (void*)(radar_ptr + 3));
    std::cout << std::format("Jump +3 Value (*(radar_ptr + 3)): {}\n\n", *(radar_ptr + 3));

    // --- POINTER WALKING ---
    std::cout << "--- Walking the Pointer ---\n";
    
    // Physically moves the pointer forward in memory using the ++ operator.
    // Bypasses the need for standard array indices (e.g., secure_data[i]).
    // HARDWARE OPTIMIZATION: Sequential memory access maximizes CPU L1 Cache hits 
    // and eliminates the overhead of index bounds checking.
    for (int i = 0; i < 5; ++i) {
        std::cout << std::format("Scanning address {}: Found Value {}\n", (void*)radar_ptr, *radar_ptr);
        
        // Re-assigns the pointer to the next 4-byte memory block.
        radar_ptr++; 
    }

    std::cout << "\n--- Radar Scan Complete ---\n";

    return 0;
}