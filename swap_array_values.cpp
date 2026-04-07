#include <iostream>
#include <vector>

using namespace std;

/**
 * Streams contiguous vector payloads to standard output.
 *
 * @param label Passed by const reference to avoid stack frame overhead and heap allocation of a new string.
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
 * Reverses a given integer vector in-place using a two-pointer technique.
 * This approach guarantees an O(N) time complexity (specifically N/2 operations) 
 * and an optimal O(1) auxiliary space complexity.
 *
 * @param vec Destination sequence. Must be passed by reference to modify the 
 * caller's underlying heap-allocated memory directly, avoiding a costly deep copy.
 */
void reverse_vector_in_place(vector<int>& vec) {
    // Initializes the left boundary. Vector elements are stored contiguously in memory, 
    // allowing O(1) random access via pointer arithmetic under the hood.
    int left = 0;
    
    // Calculates the right boundary. Subtracting 1 avoids out-of-bounds memory access 
    // since arrays use zero-based indexing. vector::size() is an O(1) operation.
    int right = vec.size() - 1;
    
    cout << "--- Reversing vector of " << vec.size() << " elements in-place ---" << endl;

    // Iterates until the two pointers meet or cross in the middle. 
    // This efficiently handles both even and odd-sized contiguous memory blocks.
    while (left < right) {
        // Allocates a temporary variable on the stack (or within a CPU register) 
        // to hold the value during the swap operation.
        int temp = vec[left];
        
        // Overwrites the memory address at the 'left' index with the payload from 'right'.
        vec[left] = vec[right];
        
        // Deposits the stored temporary value into the memory address at the 'right' index.
        vec[right] = temp;
        
        // Translates the logical pointers inward. This access pattern remains 
        // highly cache-friendly since the memory addresses are accessed predictably.
        left++;
        right--;
    }
}

int main() {
    // Instantiates the vector object. 
    // The vector metadata (pointers to begin, end, and capacity) resides on the STACK.
    // The initializer list triggers an allocation of a contiguous integer array on the HEAP.
    vector<int> my_vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Passes by const reference; guarantees no mutation of the heap data during read.
    print_vector("Before Swap: ", my_vec);

    // Executes the in-place reversal. Mutates my_vec directly via pass-by-reference.
    // No new heap allocations occur during this function call, optimizing memory usage.
    reverse_vector_in_place(my_vec);

    print_vector("After Swap:  ", my_vec);

    // Program termination. Stack unwinds, calling the destructor for my_vec, 
    // which in turn automatically frees the associated heap memory to prevent memory leaks.
    return 0;
}