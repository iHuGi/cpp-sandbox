#include <iostream>
#include <vector>
#include <string>

using namespace std;

/**
 * @brief Validates the structural integrity of parentheses within a given expression.
 * * This algorithm utilizes a Last-In-First-Out (LIFO) stack data structure to ensure 
 * that all brackets '()', '[]', and '{}' are properly paired and correctly nested.
 * * @param expression The string containing the sequence of characters to evaluate.
 * @return true If all parentheses are perfectly matched and closed.
 * @return false If there are unmatched, improperly ordered, or dangling parentheses.
 */
bool isValidParentheses(string expression) {
    vector<char> bracketStack;

    // Iterates through the expression character by character to evaluate brackets
    for (int i = 0; i < expression.length(); i++) {
        char current = expression[i];

        // 1. Opening brackets are pushed onto the stack for deferred evaluation
        if (current == '(' || current == '[' || current == '{') {
            bracketStack.push_back(current);
        } 
        // 2. Closing brackets trigger the validation logic against the top of the stack
        else if (current == ')' || current == ']' || current == '}') {
            
            // A closing bracket with an empty stack indicates a dangling closing element. 
            // The expression is immediately marked as invalid.
            if (bracketStack.empty()) {
                return false;
            }

            // Retrieves the most recently opened bracket for comparison
            char lastOpened = bracketStack.back();
            
            // Evaluates if the current closing bracket corresponds to the last opened bracket
            if ((current == ')' && lastOpened == '(') ||
                (current == ']' && lastOpened == '[') ||
                (current == '}' && lastOpened == '{')) {
                
                // The pair is successfully resolved. The opening bracket is popped 
                // from the stack to allow subsequent evaluations.
                bracketStack.pop_back(); 
            } else {
                // A mismatch in bracket types (e.g., closing a '[' with a '}') constitutes a syntax error.
                return false;
            }
        }
    }

    // 3. Post-iteration validation: A completely empty stack confirms that all 
    // opened brackets were successfully paired and popped. 
    return bracketStack.empty(); // returns true
}

int main() {
    // Define a suite of test cases to validate edge conditions and nominal execution
    string test1 = "{[()]}";       // Expected: Valid
    string test2 = "{[()]";        // Expected: Invalid (Missing terminal '}')
    string test3 = "[(])";         // Expected: Invalid (Improper nesting order)
    string test4 = "a + (b * c)";  // Expected: Valid (Ignores non-bracket characters)

    cout << "--- System Diagnostic: Parentheses Validator ---" << endl;
    cout << "Test 1 (" << test1 << "): " << (isValidParentheses(test1) ? "Valid" : "Invalid") << endl;
    cout << "Test 2 (" << test2 << "): " << (isValidParentheses(test2) ? "Valid" : "Invalid") << endl;
    cout << "Test 3 (" << test3 << "): " << (isValidParentheses(test3) ? "Valid" : "Invalid") << endl;
    cout << "Test 4 (" << test4 << "): " << (isValidParentheses(test4) ? "Valid" : "Invalid") << endl;

    return 0;
}