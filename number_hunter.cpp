#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <limits>
#include <chrono> // Modern time library for high-precision clocks

using namespace std;
using namespace std::chrono; // Avoids typing std::chrono:: repeatedly

/**
 * @brief Compares the player's guess with the secret number.
 * @param playerGuess The number guessed by the player (passed as a read-only value).
 * @param secretNumber The target number to guess (passed as a read-only value).
 * @return string A hint ("Too Low", "Too High") or the winning message.
 */
string check_guess(const int playerGuess, const int secretNumber) {
    if (playerGuess < secretNumber) {
        return "Too Low!";
    } else if (playerGuess > secretNumber) {
        return "Too High!";
    } else {
        return "Damn Right! :)";
    }
}

int main() {
    // ---------------------------------------------------------
    // SYSTEM BOOT & RANDOM SEED SETUP
    // ---------------------------------------------------------
    srand(time(0));
    time_t currentTime = time(0);
    char* readableDate = ctime(&currentTime);

    cout << "--- SYSTEM BOOT ---" << endl;
    cout << "Current Date & Time: " << readableDate;
    cout << "-------------------" << endl << endl;
    // ---------------------------------------------------------
    
    // Generates a target number from 1 to 100. 
    // The modulo (%) chops the massive int down to a range of 0-99, and we add 1 to avoid rolling a 0.
    int secretNumber = rand() % 100 + 1;
    
    int playerGuess = 0;
    int attempts = 0;

    cout << "--- THE NUMBER HUNTER ---" << endl;
    cout << "I've locked in a secret number between 1 and 100. Let's see what you've got." << endl;

    // --- START THE CLOCK ---
    auto start_time = high_resolution_clock::now();

    do {
        cout << "\nEnter your guess: ";
        cin >> playerGuess;

        // --- INPUT VALIDATION ---
        if (cin.fail()) {
            cin.clear(); // Reset the internal error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Flush the garbage characters from the buffer
            
            attempts++; // Penalize the player for trying to break the game!
            cout << "Invalid input! Please type an actual number. (And yes, that counts as an attempt!)" << endl;
            continue; // Skip the rest of the loop and ask again
        }

        attempts++;

        string resultMessage = check_guess(playerGuess, secretNumber);
        cout << resultMessage << endl;

    } while (playerGuess != secretNumber);

    // --- STOP THE CLOCK ---
    auto stop_time = high_resolution_clock::now();

    // Calculate the difference in seconds using a 'double' (decimal)
    // to format the output nicely (e.g., 5.43 seconds) instead of raw microseconds.
    duration<double> time_taken = stop_time - start_time;

    cout << "\n======================================" << endl;
    cout << " WINNER WINNER CHICKEN DINNER! " << endl;
    cout << " Total Attempts : " << attempts << endl;
    cout << " Time Elapsed   : " << time_taken.count() << " seconds" << endl;
    cout << "======================================" << endl;

    return 0;
}