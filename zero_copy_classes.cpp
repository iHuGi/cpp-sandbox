#include <iostream>
#include <vector>
#include <chrono>
#include <format>

// Blueprint for the dataset. Secures and manages the massive RAM allocation.
class ETLPayload {
private:
    // Encapsulated data vault. Cannot be modified directly from the outside.
    std::vector<double> massive_dataset;

public:
    // Constructor: Allocates 10 million elements in memory instantly upon object creation.
    ETLPayload() {
        std::cout << "[System] Constructing ETLPayload Object...\n";
        massive_dataset.assign(10'000'000, 3.14159);
        std::cout << "[System] 80MB payload securely allocated in RAM.\n\n";
    }

    // Exposes a read-only (const) reference to the data for zero-copy access.
    const std::vector<double>& get_data() const {
        return massive_dataset; 
    }
};

// Data processing engine. Demonstrates the performance impact of memory handling.
class ETLProcessor {
public:
    // ANTI-PATTERN: Pass-by-value. 
    // Forces the CPU to create a complete 80MB clone of the object before processing.
    double process_by_value(ETLPayload payload) {
        double sum = 0;
        for (double val: payload.get_data()) {
            sum += val;
        }
        return sum;
    }

    // ENTERPRISE STANDARD: Pass-by-const-reference.
    // Zero-copy architecture. Reads directly from the original memory address.
    double process_by_reference(const ETLPayload& payload) {
        double sum = 0;
        for (double val: payload.get_data()) {
            sum += val;
        }
        return sum;
    }
};

int main() {
    std::cout << "--- Booting Zero-Copy ETL Engine ---\n";

    // Instantiating the objects into live RAM.
    ETLPayload main_payload;
    ETLProcessor data_engine;

    // --- TEST 1: PASS BY VALUE ---
    // Measures the execution time, including the massive memory duplication bottleneck.
    auto start_value = std::chrono::high_resolution_clock::now();

    double result_value = data_engine.process_by_value(main_payload);

    auto end_value = std::chrono::high_resolution_clock::now();
    auto duration_value = std::chrono::duration_cast<std::chrono::milliseconds>(end_value - start_value);

    std::cout << "[WARNING] Pass-by-Value execution\n";
    std::cout << std::format("Result: {}\n", result_value);
    std::cout << std::format("Execution time was: {} ms \n", duration_value.count());

    // --- TEST 2: PASS BY REFERENCE ---
    // Measures the execution time using the zero-copy memory address.
    auto start_reference = std::chrono::high_resolution_clock::now();

    double result_reference = data_engine.process_by_reference(main_payload);

    auto end_reference = std::chrono::high_resolution_clock::now();
    auto duration_reference = std::chrono::duration_cast<std::chrono::milliseconds>(end_reference - start_reference);

    std::cout << "\n[OPTIMIZED] Pass-by-Reference execution\n";
    std::cout << std::format("Result: {}\n", result_reference);
    std::cout << std::format("Execution time was: {} ms \n", duration_reference.count());

    return 0;
}