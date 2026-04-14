#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// ---------------------------------------------------------
// ENV FILE PARSER
// ---------------------------------------------------------
/**
 * @brief Parses a local .env file to safely extract the target API key.
 * * Since C++ lacks a native dotenv library, this function acts as a lightweight 
 * file reader. It scans the file line by line, identifies key-value pairs, 
 * and sanitizes the output by removing whitespace and quotation marks.
 * * @return string The sanitized API key, or an empty string if the key is not found.
 */
string load_api_key_from_env() {
    ifstream file(".env"); // The program opens the .env file in the current working directory
    string line;
    
    if (!file.is_open()) {
        cerr << "[!] File Error: The system could not locate the .env file!" << endl;
        return "";
    }

    // The loop reads the file line by line until the end of the document
    while (getline(file, line)) {
        // The program searches the current line for the assignment operator (=)
        size_t pos = line.find('=');
        
        // If 'pos' is NOT equal to string::npos, it means the '=' was successfully found
        if (pos != string::npos) {
            
            // The string is split into two parts based on the position of the '='
            // substr(0, pos) starts at index 0 and extracts 'pos' amount of characters (The Key)
            string key = line.substr(0, pos);
            
            // substr(pos + 1) starts one character after the '=' and grabs the rest of the line (The Value)
            string value = line.substr(pos + 1);

            // The program trims trailing spaces from the key to ensure an accurate match
            key.erase(key.find_last_not_of(" \t") + 1);

            // The system checks if the extracted key matches the required target
            if (key == "WEATHER_API") {
                
                // The program searches for the first and last characters in the value 
                // that are NOT spaces (\t) or quotation marks (\")
                size_t first = value.find_first_not_of(" \t\"");
                size_t last = value.find_last_not_of(" \t\"");
                
                // If the value is not empty or composed entirely of junk characters...
                if (first != string::npos && last != string::npos) {
                    // The system extracts only the valid characters.
                    // 'last - first + 1' calculates the exact length of the clean string.
                    return value.substr(first, (last - first + 1));
                }
            }
        }
    }
    return ""; // Returns an empty string if the file finishes and the key was never found
}

// ---------------------------------------------------------
// LIBCURL CALLBACK FUNCTION
// ---------------------------------------------------------
/**
 * @brief Appends incoming data chunks from libcurl into a target string buffer.
 * * libcurl does not return the entire HTTP response at once. Instead, the library 
 * triggers this callback function multiple times as data arrives over the network.
 * * @param contents Pointer to the delivered data chunk in system memory.
 * @param size Size of one data item (usually 1 byte).
 * @param nmemb Number of items (total bytes in this specific chunk).
 * @param s Pointer to the target std::string where the final JSON payload is stored.
 * @return size_t The exact number of bytes safely processed by the application.
 */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* s) {
    size_t newLength = size * nmemb; 
    
    try {
        // The program accesses the target string via pointer and appends the new chunk
        s->append((char*)contents, newLength);
        
        // The function returns the exact byte count back to libcurl to confirm successful storage
        return newLength; 
        
    } catch(bad_alloc& e) {
        // If the system runs out of RAM, returning 0 triggers a safe libcurl abort
        return 0; 
    }
}

int main() {
    // ---------------------------------------------------------
    // CONFIGURATION & SETUP
    // ---------------------------------------------------------
    vector<string> cities = {
        "Lisbon", "Porto", "London", "Paris", "Berlin", 
        "Madrid", "Rome", "New York", "Barcelona", "Miami"
    };

    cout << "--- WEATHER PIPELINE BOOTING UP ---" << endl;
    cout << "Loading API Key from secure .env file..." << endl;
    
    // The program attempts to read the local .env file
    string api_key = load_api_key_from_env();
    
    // Failsafe: The application aborts immediately if the key is missing to prevent network errors
    if (api_key.empty()) {
        cerr << "[!] CRITICAL: WEATHER_API variable not found or empty." << endl;
        cerr << "--- SHUTDOWN ---" << endl;
        return 1;
    }

    cout << "API Key Loaded Successfully." << endl;
    cout << "Initializing HTTP Client..." << endl;
    cout << "-----------------------------------" << endl << endl;

    // The libcurl environment is initialized globally before any requests are made
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();

    // The program verifies that the curl handle was successfully created in memory
    if(curl) {
        // --- START THE ETL LOOP ---
        for(const auto& city : cities) {
            
            // ---------------------------------------------------------
            // URL ENCODING & SETUP
            // ---------------------------------------------------------
            
            // The libcurl engine safely encodes special characters (e.g., spaces to "%20")
            // The length is explicitly provided to prevent reading past the end of the string
            char* encoded_city = curl_easy_escape(curl, city.c_str(), city.length());
            
            // The dynamic API endpoint is constructed using the safely encoded city name
            string url = "http://api.openweathermap.org/data/2.5/weather?q=" + string(encoded_city) + "&appid=" + api_key + "&units=metric";
            
            // Memory Management: The dynamically allocated char array is manually freed to prevent RAM leaks
            curl_free(encoded_city);
            
            // The curl handle is configured with the target URL
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            
            // A string buffer is created in memory to hold the eventual JSON payload
            string response_string; 
            
            // The custom WriteCallback function is assigned to handle incoming data
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            
            // The memory address of the string buffer is passed so the callback knows where to dump the payload
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

            // --- EXECUTION PHASE ---
            // The engine connects to the internet, executes the GET request, and triggers the callback
            CURLcode res = curl_easy_perform(curl);

            // --- ERROR HANDLING & PARSING ---
            if(res != CURLE_OK) {
                // Catches network drops or internal curl errors
                cerr << "[!] Network Failure processing " << city << ": " << curl_easy_strerror(res) << endl;
            } else {
                try {
                    // The raw string payload is converted into a searchable JSON object
                    auto data = json::parse(response_string);
                    
                    // The program checks the internal API status code to ensure a valid response
                    if (data.contains("cod") && data["cod"] == 200) {
                        
                        // Telemetry metrics are extracted from the JSON tree
                        string city_name = data["name"];
                        double temp = data["main"]["temp"];
                        string condition = data["weather"][0]["main"];
                        double lon = data["coord"]["lon"];
                        double lat = data["coord"]["lat"];

                        // The formatted metrics are printed to the console interface
                        cout << ">> " << city_name << " <<" << endl;
                        cout << " Temperature : " << temp << " C" << endl;
                        cout << " Condition   : " << condition << endl;
                        cout << " Coordinates : (Lat: " << lat << ", Lon: " << lon << ")" << endl;
                        cout << "-----------------------------------" << endl;
                    } else {
                        // Catches API-side errors, such as typing a city name that doesn't exist
                        string error_msg = data.value("message", "Unknown API error");
                        cerr << "[!] API Error for " << city << ": " << error_msg << endl;
                    }
                } catch (json::parse_error& e) {
                    // Catches scenarios where the payload is corrupt or returned HTML instead of JSON
                    cerr << "[!] JSON Parse Crash for " << city << ": " << e.what() << endl;
                }
            }
        } // The loop resets and moves to the next city in the vector

        // The specific curl handle is destroyed to free up system resources
        curl_easy_cleanup(curl);
    }
    
    // The entire libcurl environment undergoes a complete and safe shutdown
    curl_global_cleanup();
    
    cout << "\n--- PIPELINE SHUTDOWN COMPLETE ---" << endl;
    return 0;
}