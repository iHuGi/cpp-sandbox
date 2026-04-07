#include <iostream>
#include <vector>
#include <string>

using namespace std;

/*
 * Writes the characters of a string into a vector in reverse order.
 * The 'text' string is passed by constant reference (const&) to prevent copying
 * and ensure read-only access. The 'char_vec' vector is passed by reference (&)
 * to avoid O(N) copy overhead and allow direct mutation of the original memory buffer.
 */
void reverse_to_vector(const string& text, vector<char>& char_vec) {
    size_t n = text.length();
    
    cout << "--- Deconstructing string of " << n << " bytes into the vector ---" << endl;

    // Pre-allocate memory since we know the final size.
    // This avoids reallocations and copying during push_back.
    char_vec.reserve(n);

    // Iterate the string in reverse order and append each character to the vector (O(N)).
    for (int i = n - 1; i >= 0; i--) {
        char_vec.push_back(text[i]);
    }
}

/*
 * MAIN
 */
int main() {
    // std::string object lives on the stack.
    // Small strings may use SSO (Small String Optimization) and avoid heap allocation.
    string original = "Hugo Azevedo";
    
    // Vector object is on the stack, internal buffer will be allocated on the heap.
    vector<char> reversed_chars;

    reverse_to_vector(original, reversed_chars);

    // Iterating contiguous memory is cache-friendly.
    cout << "DEBUG Vector: ";
    for (char c : reversed_chars) {
        cout << c; 
    }
    cout << endl;

    // Construct a new string from the vector range.
    string final_string(reversed_chars.begin(), reversed_chars.end());

    cout << "\n[RESULT]" << endl;
    cout << "Original: " << original << endl;
    cout << "Reversed: " << final_string << endl;

    // size() is O(1)
    cout << "\n[LOG] The vector contains " << reversed_chars.size() << " elements." << endl;

    return 0;
}