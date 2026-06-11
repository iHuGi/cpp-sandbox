#include <iostream>
#include <vector>
#include <functional>
#include <cassert>

/**
 * @brief Performs aggregate calculations on adjacent elements within a vector.
 * Iterates through the input data and applies the provided operation to consecutive pairs.
 * Returns 0 if the vector size is insufficient for adjacent operations.
 */
int adjacent_math_engine(const std::vector<int>& data, const std::function<int(int, int)>& operation) {
    if (data.size() < 2) return 0;

    int total_aggregate = 0;
    for (size_t i = 0; i < data.size() - 1; ++i) {
        total_aggregate += operation(data[i], data[i + 1]);
    } 
    return total_aggregate;
}

/**
 * @brief Executes the unit test suite.
 * Validates the math engine across various arithmetic operations and edge cases.
 * Aborts execution if any assertion fails.
 */
void run_unit_tests() {
    std::cout << "[TESTS] Booting unit tests...\n";

    // Mock datasets for engine validation
    std::vector<int> data_standard {1, 2, 3, 4}; // (1+2) + (2+3) + (3+4) = 3 + 5 + 7 = 15
    std::vector<int> data_sub {100, 85, 60};     // (100-85) + (85-60) = 15 + 25 = 40
    std::vector<int> data_mul {2, 3, 4};         // (2*3) + (3*4) = 6 + 12 = 18
    std::vector<int> data_div {10, 2, 0, 5};     // (10/2) + (2/0) + (0/5) = 5 + 0 + 0 = 5
    std::vector<int> data_empty {};              // Ensures guard clause stability

    // Functional operators (Lambdas)
    auto add_op      = [](int a, int b) { return a + b; };
    auto subtract_op = [](int a, int b) { return a - b; };
    auto multiply_op = [](int a, int b) { return a * b; };
    // Prevents division by zero by returning 0 if denominator is invalid
    auto divide_op   = [](int a, int b) { return (b != 0) ? (a / b) : 0; }; 

    // TEST 1: Validates basic additive aggregation
    std::cout << " -> Running Test 1: Addition...\n";
    assert(adjacent_math_engine(data_standard, add_op) == 15);

    // TEST 2: Validates subtractive aggregation
    std::cout << " -> Running Test 2: Subtraction...\n";
    assert(adjacent_math_engine(data_sub, subtract_op) == 40);

    // TEST 3: Validates multiplicative aggregation
    std::cout << " -> Running Test 3: Multiplication...\n";
    assert(adjacent_math_engine(data_mul, multiply_op) == 18);

    // TEST 4: Validates division safety checks
    std::cout << " -> Running Test 4: Division by Zero Safety...\n";
    assert(adjacent_math_engine(data_div, divide_op) == 5);

    // TEST 5: Validates empty vector handling
    std::cout << " -> Running Test 5: Empty Vector Guard Clause...\n";
    assert(adjacent_math_engine(data_empty, add_op) == 0);

    // TEST 6: Validates single element vector handling
    std::cout << " -> Running Test 6: Single Element Guard Clause...\n";
    assert(adjacent_math_engine({42}, multiply_op) == 0);

    std::cout << "[TESTS] All assertions passed successfully!.\n\n";
}

/**
 * @brief Program entry point.
 * Delegates execution to the unit test suite.
 */
int main() {
    run_unit_tests();
    return 0;
}