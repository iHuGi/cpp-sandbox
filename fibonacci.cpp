#include <iostream>
#include <vector>
#include <concepts>

// This requires gnu++20 or c++20
template <std::integral T>
std::vector<T> generate_fibonacci(T n) {
    std::vector<T> fib;

    // Handle edge case: return empty vector if n is 0 or less
    if (n <= 0) return fib;

    // Push 0 into the vector, as it's the required starting number of the sequence
    fib.push_back(0);

    // If n is 1, return the vector now since the sequence stops here
    if (n == 1) return fib;

    // Otherwise, push 1, which is the second required number for the sequence
    fib.push_back(1);

    // Loop to generate the rest of the Fibonacci numbers up to n.
    // We calculate the next number by adding the two preceding numbers 
    // in the sequence (the last and second-to-last values).
    for (T i = 2; i < n; i++) {
        fib.push_back(fib[i - 1] + fib[i - 2]);
    }

    return fib;
}

int main() {
    // Define how many numbers of the Fibonacci sequence we want to generate
    int number = 15;

    // Call our template function. 
    // 'auto' is smart enough to figure out that 'seq' should be a std::vector<int>
    auto seq = generate_fibonacci(number);

    // Print a header for our output
    std::cout << "First " << number << " numbers of the Fibonacci sequence:\n";

    // Modern range-based for loop (acts exactly like a 'foreach' in C# or Java).
    // Using 'const auto&' is best practice here:
    // - 'auto' figures out the type (int).
    // - '&' (reference) means we look directly at the number in memory instead of making a slow copy of it.
    // - 'const' protects the number so we don't accidentally modify it while printing.
    for (const auto& num : seq) {
        std::cout << num << " ";
    }

    // Print a final newline character to keep the terminal clean
    std::cout << '\n';

    return 0;
}