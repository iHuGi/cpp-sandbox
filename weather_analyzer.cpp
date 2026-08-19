#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <format>

using namespace std;

int main () {
    // ---------------------------------------------------------
    // 1. CONFIGURATION & I/O
    // ---------------------------------------------------------
    const string file_path = "../weather_database.csv";
    ifstream file(file_path);

    // Guard clause: Validates if the file stream was successfully acquired
    if (!file.is_open()) {
        cerr << "[!] CRITICAL ERROR: No file was found in the specified path." << endl;
        return 1;
    }

    string line;
    // Discard the CSV header to prevent mathematical parsing errors
    getline(file, line);

    // ---------------------------------------------------------
    // 2. DATA STRUCTURES
    // ---------------------------------------------------------
    // Hash map for O(1) lookups.
    // Key = City Name | Value = Pair of {Total Temperature, Record Count}
    unordered_map<string, pair<double, int>> city_stats;
    int row_count = 0;

    // ---------------------------------------------------------
    // 3. ETL PIPELINE: EXTRACTION & TRANSFORMATION
    // ---------------------------------------------------------
    while(getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string id, city, temp_str, condition, lat, lon, timestamp;

        // Parse CSV columns using the comma delimiter
        getline(ss, id, ',');
        getline(ss, city, ',');
        getline(ss, temp_str, ',');
        getline(ss, condition, ',');
        getline(ss, lat, ',');
        getline(ss, lon, ',');
        getline(ss, timestamp, ',');

        try {
            // Transform string telemetry into a 64-bit float
            double temp = stod(temp_str);

            // Accumulate data natively in the Hash Map
            city_stats[city].first += temp;  // Accumulate total temperature
            city_stats[city].second += 1;    // Increment record count for the specific city
            row_count++;
        } 
        catch (const exception& e) {
            cerr << "[!] Parsing fatal error at line: " << line
                 << " | row_count: " << row_count
                 << " | Details: " << e.what() << endl;
            continue; // Ensure the pipeline survives malformed rows
        }
    }

    // Release the OS file lock as the extraction phase is complete
    file.close();

    // ---------------------------------------------------------
    // 4. DATA ANALYTICS & PRESENTATION
    // ---------------------------------------------------------
    cout << "========================================" << endl;
    cout << "   WEATHER DATA ANALYTICS ENGINE V1.0   " << endl;
    cout << "========================================" << endl;
    cout << format("[SYS] Memory loaded with {} telemetry rows.\n\n", row_count);
    
    cout << "--- GLOBAL AVERAGE TEMPERATURES ---" << endl;

    // Iterate through the Hash Map using C++17 Structured Bindings
    for (const auto& [city_name, data] : city_stats) {
        
        // Calculate the accurate mean temperature
        double avg_temp = data.first / data.second;
        
        // Output formatting: Left-align city name (12 chars), format temp to 2 decimals
        cout << format(">> {:<12} : {:.2f} °C (based on {} records)\n", city_name, avg_temp, data.second);
    }
    
    cout << "-----------------------------------" << endl;

    return 0;
}