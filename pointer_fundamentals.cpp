#include <iostream>
#include <format>

/**
 * Demonstrates the fundamentals of memory addresses and pointer manipulation.
 * This program illustrates how to access RAM locations and modify data 
 * indirectly via dereferencing.
 */

int main() {
    // Initializes a standard integer on the stack.
    int account_balance = 1000;
    
    // Declares a pointer and assigns it the memory address of the variable.
    // The '&' operator retrieves the physical address in RAM.
    int* ptr = &account_balance;

    std::cout << "--- Memory Inspection ---\n";
    
    // Outputs the current value and its physical location.
    std::cout << std::format("Variable value: {}\n", account_balance);
    std::cout << std::format("Memory address (&account_balance): {}\n", (void*)&account_balance);
    std::cout << std::format("Pointer value (stored address): {}\n", (void*)ptr);

    std::cout << "\n--- Direct RAM Manipulation ---\n";

    // Dereferences the pointer to modify the original variable's value.
    // The '*' operator directs the CPU to the address stored in 'ptr'.
    *ptr = 5000;

    // Validates that the original variable was updated without direct access.
    std::cout << std::format("New variable value: {}\n", account_balance);
    std::cout << "Success: Variable state updated via memory address manipulation.\n";

    return 0;
}