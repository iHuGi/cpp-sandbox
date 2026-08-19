#include <iostream>
#include <format>

/**
 * Updates a financial balance safely via direct memory pointer manipulation.
 * Validates the pointer against nullptr to prevent memory access violations (Segfaults).
 */
void apply_adjustment(int* balance_ptr, int adjustment) {
    // Validates the memory address. Aborts if the pointer does not point to a valid location.
    if (balance_ptr == nullptr) {
        std::cout << "[Error] Null pointer detected. Aborting memory operation.\n";
        return;
    }

    // Dereferences the pointer to modify the actual value stored in RAM.
    *balance_ptr += adjustment;
    
    // Outputs the raw hexadecimal memory address and the adjustment made.
    // Casts to (void*) to ensure the raw address is printed correctly by the formatter.
    std::cout << std::format("[System] Memory at {} updated by {}.\n", (void*)balance_ptr, adjustment);
}

int main () {
    std::cout << "--- Advanced Memory Operations ---\n";

    // --- Scenario 1: Valid Memory Access ---
    // Initializes a standard integer variable and a pointer holding its memory address.
    int primary_account = 3000;
    int* active_ptr = &primary_account;

    std::cout << std::format("Initial Balance: {}\n", primary_account);
    
    // Passes the memory address to the adjustment function.
    apply_adjustment(active_ptr, 1000);
    
    std::cout << std::format("Updated Balance: {}\n\n", primary_account);

    // --- Scenario 2: Null Pointer Handling (Danger Zone) ---
    // Initializes a pointer explicitly to 'nullptr' (points to address 0).
    int* danger_ptr = nullptr;

    std::cout << "Attempting to access an uninitialized (null) pointer...\n";

    // Passes the null pointer to the function to test the safety guard.
    apply_adjustment(danger_ptr, 1000);

    // Confirms the program survived the null pointer without crashing.
    std::cout << "--- Execution finished safely because the function did its job ---\n";

    return 0;
}