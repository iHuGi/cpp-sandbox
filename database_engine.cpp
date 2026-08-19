#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

/**
 * @brief In-memory data structure representing a single database record.
 * Contains the user's name, fiscal code (NIF), and phone number.
 */
struct User {
    std::string name;
    std::string fiscal_code;
    std::string phone;
};

/**
 * @brief Validates a Portuguese fiscal code (NIF).
 * @param fiscal_code A constant reference to the string containing the NIF.
 * @return true if the string is exactly 9 characters long and fully numeric.
 * @return false otherwise.
 */
bool is_valid_fiscal_code(const std::string& fiscal_code) {
    if (fiscal_code.length() != 9) return false; // len needs to be exactly 9 in Portugal
    return std::all_of(fiscal_code.begin(), fiscal_code.end(), ::isdigit); // all characters must be digits
}

/**
 * @brief Validates a Portuguese phone number.
 * @param phone A constant reference to the string containing the phone number.
 * @return true if the string is exactly 14 characters, starts with "00351", and is fully numeric.
 * @return false otherwise.
 */
bool is_phone_valid(const std::string& phone) {
    if (phone.length() != 14) return false; // len needs to be exactly 14 in Portugal
    if (phone.substr(0, 5) != "00351") return false; // needs to start with 00351 in Portugal
    return std::all_of(phone.begin(), phone.end(), ::isdigit); // all characters must be digits
}

/**
 * @brief Main database engine loop. 
 * Handles user input parsing, memory allocation into the vector, and data validation loops.
 */
void run_database_engine() {
    std::vector<User> database;
    std::string command;
    
    std::cout << "--- BARE-METAL DATABASE ENGINE ---\n";

    while(true) {
        // UI Refresh: The prompt is reprinted on every new loop iteration
        std::cout << "[SYS] Engine ready. Use '+' to add users or 'exit' to dump memory.\n";
        std::cout << "> ";
        std::getline(std::cin, command);

        // Remove ALL spaces (The "Erase-Remove Idiom")
        command.erase(std::remove_if(command.begin(), command.end(), ::isspace), command.end());

        // Convert to lowercase in-place
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);

        if (command == "+") {
            User u;

            // Name
            std::cout << "  Name: ";
            std::getline(std::cin, u.name);

            // Fiscal code validation loop
            while (true) {
                std::cout << "  Fiscal Code (9 digits): ";
                std::getline(std::cin, u.fiscal_code);
                if (is_valid_fiscal_code(u.fiscal_code)) break;
                std::cerr << "  [!] Error: Invalid fiscal code. Only 9-digit numbers are allowed.\n";
            }

            // Phone validation loop
            while (true) {
                std::cout << "  Phone (14 digits, starting with 00351): ";
                std::getline(std::cin, u.phone);
                if (is_phone_valid(u.phone)) break;
                std::cerr << "  [!] Error: Invalid phone number. Must be 14 digits and start with 00351.\n";
            }

            // Save in the database vector
            database.push_back(u);
            std::cout << "  [SYS] User added successfully. > db.size(): " << database.size() << "\n\n";
        }
        else if (command == "exit") {
            std::cout << "\n--- DUMPING DATABASE MEMORY ---\n";
            
            // Range-based for loop with const reference for zero-copy memory access
            for (const auto& user: database) {
                std::cout << "Name: " << user.name << "\n";
                std::cout << "Fiscal Code: " << user.fiscal_code << "\n";
                std::cout << "Phone: " << user.phone << "\n";
                std::cout << "-----------------------------\n";
            }
            std::cout << "[SYS] Memory dump complete. Exiting...\n";
            break; // break the loop to exit the program
        }
        else {
            std::cerr << "  [!] Error: Invalid command. Use '+' or 'exit'.\n\n";
        }
    }
    
    return; // Explicit return to exit the void function cleanly
}

/**
 * @brief Program entry point. Bootstraps the database engine.
 * @return 0 upon successful execution and shutdown.
 */
int main() {
    // The main function delegates all logic and memory management to the engine
    run_database_engine();
    
    return 0;
}