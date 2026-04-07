#include <iostream>
#include <vector> // Pulling in std::vector for dynamic contiguous memory allocation on the Heap

using namespace std;

/*
 * MODULE: even_odds processor
 * PASS BY REFERENCE (&): Vectors are passed by reference to avoid O(N) copy overhead.
 * This ensures the original data structures are mutated in-place, conserving memory.
 */
void even_odds(int limit, vector<int>& even_vec, vector<int>& odd_vec) {
    cout << "--- Starting analysis up to " << limit << " ---" << endl;

    for (int i = 1; i <= limit; i++) {
        
        if (i % 2 == 0) {
            // push_back automatically handles Heap memory reallocation.
            // Data is kept contiguous to maintain Cache Locality.
            even_vec.push_back(i);
        } else {
            odd_vec.push_back(i);
        }
    }
    cout << "--- Vectors populated successfully. ---" << endl;
}

/*
 * V2 PRACTICE MODULE
 * Encapsulated execution logic to maintain a clean main() entry point.
 */
void run_v2() {
    int target_limit_v2 = 200;

    // Allocating new vector objects in the current stack frame.
    vector<int> evens_v2;
    vector<int> odds_v2;

    even_odds(target_limit_v2, evens_v2, odds_v2);

    cout << "\n[V2] Proof of execution: " << endl;
    // .size() is an O(1) operation.
    cout << "[V2] Total even numbers stored: " << evens_v2.size() << endl;
    cout << "[V2] Total odd numbers stored: " << odds_v2.size() << endl;

    // Range-based for loops benefit from vector contiguous memory and L1 cache prefetching.
    cout << "[V2] Even numbers are: ";
    for (int num : evens_v2) {
        cout << num << " ";
    }
    cout << endl;

    cout << "[V2] Odds numbers are: ";
    for (int num : odds_v2) {
        cout << num << " ";
    }
    cout << endl;
}

/*
 * MAIN EXECUTION THREAD
 * The Operating System passes control to this function upon execution.
 */
int main() {
    int target_limit = 100;
    
    // Instantiating empty vectors. Memory footprint remains minimal until allocation occurs.
    vector<int> evens;
    vector<int> odds;

    // Passing references to the core logic engine to ensure in-place mutation.
    even_odds(target_limit, evens, odds);

    cout << "\n[V1] Proof of execution:" << endl;
    // Retrieving the exact element count via O(1) size() method.
    cout << "[V1] Total even numbers stored: " << evens.size() << endl;
    cout << "[V1] Total odd numbers stored: " << odds.size() << endl;
    
    // Iterating through contiguous memory and dumping data to standard output.
    cout << "[V1] Even numbers are: ";
    for (int num : evens) {
        cout << num << " ";
    }
    cout << endl;

    cout << "[V1] Odd numbers are: ";
    for (int num : odds) {
        cout << num << " ";
    }
    cout << endl;

    cout << "\n=========================================" << endl;
    cout << "INITIATING V2 PRACTICE RUN..." << endl;
    cout << "=========================================\n" << endl;
    
    // Calling the modularized V2 function to keep the main thread clean.
    run_v2();

    // Returning 0 signals a zero-error termination to the Linux kernel.
    return 0;
}