#include <iostream>
#include <vector>
#include <functional>
#include <format>

/**
 * @brief Iterates through a vector and applies a dynamic mathematical operation to adjacent pairs.
 * * This engine utilizes a sliding window approach to process elements pairwise. 
 * It implements the Higher-Order Function pattern by accepting a functional dependency.
 * * @param data A constant reference to the std::vector containing the dataset.
 * @param operation A std::function (lambda) that takes two integers and returns an integer result.
 * @return int The accumulated sum of all operations performed on adjacent pairs. Returns 0 if data size < 2.
 */
int adjacent_math_engine(const std::vector<int>& data, const std::function<int(int, int)>& operation) {
    // Guard clause: Early exit if there are not enough elements to form a pair
    if (data.size() < 2) {
        return 0;
    }

    int total_aggregate = 0;

    // Iteration stops at size - 1 to strictly prevent out-of-bounds memory access on (i + 1)
    for (size_t i = 0; i < data.size() - 1; ++i) {
        
        // Execute the injected operation against the current element and its immediate neighbor
        int step_result = operation(data[i], data[i + 1]);
        
        // Telemetry logging for system state validation
        std::cout << std::format("[SYS] Processing pair ({}, {}) -> Result: {}\n", data[i], data[i+1], step_result);

        // Accumulate the step result into the global aggregate
        total_aggregate += step_result;
    } 

    return total_aggregate;
}

int main () {
    std::cout << "--- BOOTING ADJACENT MATH ENGINE ---\n\n";

    // 1. Data Initialization
    
    // Dataset for Multiplication and Addition (Ascending)
    std::vector<int> data_standard {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Dataset for Subtraction (Descending with varied mathematical gaps)
    std::vector<int> data_subtraction {100, 85, 60, 45, 20, 15, 5, 2, 0};
    
    // Dataset for Division (Strict multiples to ensure clean integer division without truncation)
    // NOTE: Utilizes native integer division.
    std::vector<int> data_division {1024, 512, 128, 64, 16, 8, 2, 1};
    
    std::cout << "[SYS] Standard dataset loaded: {1, ..., 10}\n";
    std::cout << "[SYS] Subtraction dataset loaded: {100, 85, 60, 45, 20, 15, 5, 2, 0}\n";
    std::cout << "[SYS] Division dataset loaded: {1024, 512, 128, 64, 16, 8, 2, 1}\n\n";

    // 2. Functional Dependencies (Lambdas)
    auto multiply_op = [](int a, int b) { return a * b; };
    auto add_op      = [](int a, int b) { return a + b; };
    auto subtract_op = [](int a, int b) { return a - b; };
    
    // Safety check implemented to prevent undefined behavior (division by zero)
    auto divide_op   = [](int a, int b) { return b != 0 ? a / b : 0; };

    // 3. Engine Execution Phase
    std::cout << "--- PERFORMING MULTIPLICATION ---\n";
    int multiplication_result = adjacent_math_engine(data_standard, multiply_op);
    std::cout << std::format(">> Total aggregate of multiplication: {}\n\n", multiplication_result);

    std::cout << "--- PERFORMING ADDITION ---\n";
    int addition_result = adjacent_math_engine(data_standard, add_op);
    std::cout << std::format(">> Total aggregate of addition: {}\n\n", addition_result);

    std::cout << "--- PERFORMING SUBTRACTION ---\n";
    int subtraction_result = adjacent_math_engine(data_subtraction, subtract_op);
    std::cout << std::format(">> Total aggregate of subtraction: {}\n\n", subtraction_result);

    std::cout << "--- PERFORMING DIVISION ---\n";
    int division_result = adjacent_math_engine(data_division, divide_op);
    std::cout << std::format(">> Total aggregate of division: {}\n\n", division_result);

    return 0;
}