#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

/**
 * @brief Parses and extracts telemetry data from a local CSV database.
 * The engine isolates each row and splits the string using a comma delimiter 
 * to recreate the data structure in memory.
 */
void read_weather_database() {
    string csv_filename = "../weather_database.csv";
    
    // Initializes the Input File Stream to access the database in read-only mode
    ifstream csv_file(csv_filename);

    if (!csv_file.is_open()) {
        cerr << "[!] CRITICAL: Could not open " << csv_filename << " for reading." << endl;
        return;
    }

    cout << "\n--- READING WEATHER DATABASE ---\n";

    string line;
    bool is_header = true;

    // Iterates through the file line by line until the End Of File (EOF) is reached
    while (getline(csv_file, line)) {
        
        // Bypasses the initial header row to prevent parsing string titles as core data
        if (is_header) {
            is_header = false;
            continue;
        }

        // Converts the raw string line into an in-memory stream for targeted extraction
        stringstream ss(line);
        
        // Temporary memory allocations to store isolated column data
        string id, city, temp_str, condition, lat_str, lon_str, timestamp;

        // Extracts substrings delimited by commas, mapping them to the respective variables
        getline(ss, id, ',');
        getline(ss, city, ',');
        getline(ss, temp_str, ',');
        getline(ss, condition, ',');
        getline(ss, lat_str, ',');
        getline(ss, lon_str, ',');
        getline(ss, timestamp, ','); 

        // Outputs the parsed telemetry to the console interface
        cout << "ID: " << id
             << " | [DB LOG] City: " << city
             << " | Temp: " << temp_str << " C"
             << " | Condition: " << condition
             << " | Timestamp: " << timestamp << endl;
    }

    // Closes the file stream to release the OS lock and free system memory
    csv_file.close();
    
    cout << "--- EOF REACHED (End Of File) ---\n";
}

/**
 * @brief Program entry point.
 * Delegates execution to the CSV reading engine.
 */
int main() {
    // Bootstraps the data analyzer pipeline
    read_weather_database();
    
    return 0;
}