#include <iostream>
#include <vector>
#include <ranges>
#include <format>
#include <numeric>
#include <limits>

int main () {
    
    int number_chosen = 0;

    // Keep asking the user until a valid number is provided
    while (true) {
        std::cout << "Choose a number (minimum 10): ";
        
        // std::cin >> returns true if it successfully read a valid integer.
        // It returns false if it hits a string, a decimal, or an overflow (too big for int).
        if (std::cin >> number_chosen) {
            
            // Input is a valid int, now we check our logic rule
            if (number_chosen >= 10) {
                break; // Exit the loop, we are good to go!
            } else {
                std::cout << "You must choose at least 10 to guarantee we have 5 evens and 5 odds! Try again.\n\n";
            }
            
        } else {
            // The input failed (was a string, character, or overflowed the int limit)
            std::cout << "Invalid input! Please enter a valid whole number that fits within integer limits.\n\n";
            
            // 1. Clear the error state so std::cin works again
            std::cin.clear();
            
            // 2. Ignore/flush all the garbage characters left in the input buffer until the next new line
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    // Create the vector with the chosen size
    std::vector<int> numbers(number_chosen);

    // Fill the vector sequentially starting from 1
    std::iota(numbers.begin(), numbers.end(), 1);

    std::cout << std::format("\nWe have {} numbers to analyze!\n", numbers.size());
    std::cout << "-------------------------------------------\n";

    // Pipeline for the odd numbers
    auto modern_view_odds = numbers 
        | std::views::filter([](int n) { return n % 2 != 0; })
        | std::views::transform([](int n) { return n * n; })
        | std::views::take(5);

    std::cout << "The squares of the first 5 odd numbers:\n";

    for (int odd : modern_view_odds) {
        std::cout << std::format("-> {}\n", odd);
    }

    // Pipeline for the even numbers
    auto modern_view_evens = numbers 
        | std::views::filter([](int n) { return n % 2 == 0; })
        | std::views::transform([](int n) { return n * n; })
        | std::views::take(5);
        
    std::cout << "\nThe squares of the first 5 even numbers:\n";

    for (int even : modern_view_evens) {
        std::cout << std::format("-> {}\n", even);
    }

    return 0;
}