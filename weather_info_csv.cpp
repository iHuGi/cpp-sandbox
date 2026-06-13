// NOTE: LINUX CRON JOBS ONLY WORKING WITH ABSOLUTE PATHS

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Add your absolute paths below
const string file_path_env = "/home/hugo_azevedo/cpp_train/.env";
const string file_path = "/home/hugo_azevedo/cpp_train/weather_database.csv";

// ---------------------------------------------------------
// TIMESTAMP GENERATOR
// ---------------------------------------------------------
/**
 * @brief Retrieves the current system time and formats it for database insertion.
 * Converts the raw chronometer data into a standard SQL DATETIME string format.
 * @return string The formatted timestamp (YYYY-MM-DD HH:MM:SS).
 */
string get_current_timestamp() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// ---------------------------------------------------------
// ENV FILE PARSER
// ---------------------------------------------------------
/**
 * @brief Parses a local .env file to safely extract the target API key.
 * Scans the configuration file line by line to locate and sanitize the required credential.
 * @return string The sanitized API key, or an empty string if the key is not found.
 */
string load_api_key_from_env() {
    // ifstream file(".env"); // Relative Path problematic with CRON
    ifstream file(file_path_env); // Absolute path for CRON
    string line;
    
    if (!file.is_open()) {
        cerr << "[!] File Error: The system could not locate the .env file!" << endl;
        return "";
    }

    while (getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != string::npos) {
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);

            key.erase(key.find_last_not_of(" \t") + 1);

            if (key == "WEATHER_API") {
                size_t first = value.find_first_not_of(" \t\"");
                size_t last = value.find_last_not_of(" \t\"");
                if (first != string::npos && last != string::npos) {
                    return value.substr(first, (last - first + 1));
                }
            }
        }
    }
    return "";
}

// ---------------------------------------------------------
// LIBCURL CALLBACK FUNCTION
// ---------------------------------------------------------
/**
 * @brief Processes incoming network data streams from libcurl.
 * Appends raw byte chunks received over HTTP into an allocated in-memory string buffer.
 * @param contents Pointer to the delivered data chunk.
 * @param size Size of a single byte element.
 * @param nmemb Number of bytes delivered in the current network packet.
 * @param s Pointer to the destination string buffer.
 * @return size_t The exact number of bytes successfully appended. Returns 0 on memory allocation failure.
 */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    } catch(bad_alloc& e) {
        return 0;
    }
}

// ---------------------------------------------------------
// AUTO-INCREMENT ENGINE
// ---------------------------------------------------------
/**
 * @brief Scans the database to find the last inserted ID and generates the next serial sequence.
 * @param filename The path to the target CSV database.
 * @return int The next available auto-increment ID. Returns 1 if the database is new or empty.
 */
int get_next_serial_id(const string& filename) {
    ifstream file(filename);
    
    if (!file.is_open()) return 1;

    string line;
    string last_line;
    bool is_first_line = true;

    while (getline(file, line)) {
        if (line.empty()) continue;
        if (is_first_line) {
            is_first_line = false;
            continue;
        }
        last_line = line; 
    }
    
    file.close();

    if (last_line.empty()) return 1;

    stringstream ss(last_line);
    string last_id_str;
    getline(ss, last_id_str, ',');

    try {
        return stoi(last_id_str) + 1;
    } catch (...) {
        return 1;
    }
}

// ---------------------------------------------------------
// MAIN PIPELINE
// ---------------------------------------------------------
/**
 * @brief Program entry point.
 * Orchestrates the full ETL (Extract, Transform, Load) pipeline by connecting to the
 * OpenWeather API, parsing the JSON telemetry, and appending records to a persistent CSV database.
 */
int main() {
    vector<string> cities = {
        "Lisbon", "Porto", "London", "Paris", "Berlin",
        "Madrid", "Rome", "New York", "Barcelona", "Miami"
    };

    // string csv_filename = "../weather_database.csv"; // Relative Path problematic with CRON
    string csv_filename = file_path; // Absolute path for CRON

    cout << "--- WEATHER ETL PIPELINE BOOTING UP ---" << endl;
    cout << "Loading API Key from secure .env file..." << endl;
    
    string api_key = load_api_key_from_env();
    
    if (api_key.empty()) {
        cerr << "[!] CRITICAL: WEATHER_API variable not found or empty." << endl;
        cerr << "--- SHUTDOWN ---" << endl;
        return 1;
    }
    cout << "API Key Loaded Successfully." << endl;

    // --- CSV DATABASE INITIALIZATION ---
    bool is_new_file = false;
    
    ifstream check_file(csv_filename);
    if (!check_file.is_open() || check_file.peek() == ifstream::traits_type::eof()) {
        is_new_file = true;
    }
    check_file.close();

    ofstream csv_file(csv_filename, ios::app);
    if (!csv_file.is_open()) {
        cerr << "[!] CRITICAL: Could not open " << csv_filename << " for writing." << endl;
        return 1;
    }

    if (is_new_file) {
        cout << "[SYS] Initializing new CSV database with headers." << endl;
        csv_file << "ID,City,Temperature_C,Condition,Latitude,Longitude,Data_Inserted\n";
    }

    cout << "Initializing HTTP Client..." << endl;
    cout << "-----------------------------------" << endl << endl;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();

    if(curl) {
        
        // OPTIMIZATION: Fetches the starting sequence ID once, before the network loop begins
        int current_id = get_next_serial_id(csv_filename);

        for(const auto& city : cities) {
            
            char* encoded_city = curl_easy_escape(curl, city.c_str(), city.length());
            string url = "http://api.openweathermap.org/data/2.5/weather?q=" + string(encoded_city) + "&appid=" + api_key + "&units=metric";
            curl_free(encoded_city);
            
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            string response_string;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

            CURLcode res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                cerr << "[!] Network Failure processing " << city << ": " << curl_easy_strerror(res) << endl;
            } else {
                try {
                    auto data = json::parse(response_string);
                    
                    if (data.contains("cod") && data["cod"] == 200) {
                        
                        string city_name = data["name"];
                        double temp = data["main"]["temp"];
                        string condition = data["weather"][0]["main"];
                        double lon = data["coord"]["lon"];
                        double lat = data["coord"]["lat"];
                        
                        string timestamp = get_current_timestamp();

                        // Appends the telemetry row directly into the database stream
                        csv_file << current_id << ","
                                 << city_name << ","
                                 << temp << ","
                                 << condition << ","
                                 << lat << ","
                                 << lon << ","
                                 << timestamp << "\n";

                        cout << "[LOG] Ingested " << city_name << " into database. (ID: " << current_id << ")" << endl;
                        
                        // Increments the Primary Key in memory for the next loop iteration (Saves disk I/O)
                        current_id++;
                        
                    } else {
                        string error_msg = data.value("message", "Unknown API error");
                        cerr << "[!] API Error for " << city << ": " << error_msg << endl;
                    }
                } catch (json::parse_error& e) {
                    cerr << "[!] JSON Parse Crash for " << city << ": " << e.what() << endl;
                }
            }
        }
        
        // Flushes the stream buffer and releases the operating system lock
        csv_file.close();
        curl_easy_cleanup(curl);
    }
    
    // Purges global libcurl states
    curl_global_cleanup();
    
    cout << "\n--- PIPELINE SHUTDOWN COMPLETE ---" << endl;

    return 0;
}