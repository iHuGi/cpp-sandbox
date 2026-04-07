#include <iostream>
#include <vector>

using namespace std;

/**
 * Streams contiguous vector payloads to standard output.
 * * @param label Passed by const reference to avoid stack frame overhead and heap allocation of a new string.
 * @param vec Passed by const reference to guarantee immutability and prevent deep copies of heap-allocated arrays.
 */
void print_vector(const string& label, const vector<int>& vec) {
    cout << label;
    // Utilizes a range-based for loop for safe forward iteration across the heap-allocated contiguous block.
    for (int num : vec) {
        cout << num << " ";
    }
    // Flushes the stream buffer.
    cout << endl;
}

/**
 * Reverses a given integer vector into a target vector.
 * * @param array_vec_received Source sequence (read-only reference).
 * @param array_vec_reversed Destination sequence. Must be passed by reference to modify the caller's stack object.
 */
void reverse_array_to_vector(const vector<int>& array_vec_received, vector<int>& array_vec_reversed) {
    int n = array_vec_received.size();
    
    cout << "--- Deconstructing vector of " << n << " elements into the vector ---" << endl;

    // CRITICAL: Pre-allocates heap memory for the destination vector. 
    // This forces a single heap allocation and mitigates $O(N)$ reallocation and copy overhead 
    // triggered when the vector exceeds its underlying capacity.
    array_vec_reversed.reserve(n);

    // Iterates backwards through the source memory block.
    // Note: Reverse traversal degrades hardware prefetcher efficiency slightly, 
    // but pushing back sequentially guarantees cache-friendly, contiguous insertion in the destination block.
    for (int i = n - 1; i >= 0; i--) {
        array_vec_reversed.push_back(array_vec_received[i]);
    }
}

int main() {
    // Instantiates vector objects. 
    // The vector metadata (pointers to begin, end, and capacity) resides on the STACK.
    // The initializer list triggers an allocation of a contiguous integer array on the HEAP.
    vector<int> original_vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Stack-allocated metadata, currently pointing to null or an empty heap block.
    vector<int> reversed_vec;

    // Executes reversal. Modifies reversed_vec via pass-by-reference.
    reverse_array_to_vector(original_vec, reversed_vec);

    cout << "DEBUG Vector: ";
    // Traverses the newly populated heap memory.
    for (int c : reversed_vec) { 
        cout << c << " "; 
    }
    cout << endl;

    cout << "\n[RESULT]" << endl;

    print_vector("Original: ", original_vec);
    print_vector("Reversed: ", reversed_vec);

    // Evaluates in O(1) time complexity. 
    // STL vectors resolve size() via pointer arithmetic (distance between 'end' and 'begin' pointers), 
    // avoiding traversal.
    cout << "\n[LOG] The vector contains " << reversed_vec.size() << " elements." << endl;

    // Program termination. Stack unwinds, calling destructors for both vectors, 
    // which in turn automatically free the associated heap memory.
    return 0;
}