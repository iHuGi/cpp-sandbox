#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>

using namespace std;
using json = nlohmann::json;

// ============================================================================
// 1. ENV LOADER CLASS
// ============================================================================
class EnvLoader {
public:
    /**
     * @brief Parses a local .env file to safely extract the target API key.
     * * Since C++ lacks a native dotenv library, this function acts as a lightweight
     * file reader. It scans the file line by line, identifies key-value pairs,
     * and sanitizes the output by removing whitespace and quotation marks.
     * * @param path The filepath to the .env configuration file.
     * @return string The sanitized API key, or an empty string if the key is not found.
     */
    static string getApiKey(const string& path) {
        ifstream file(path);
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
};

// ============================================================================
// 2. DATABASE MANAGER CLASS
// ============================================================================
class DatabaseManager {
    ofstream file;
public:
    /**
     * @brief Scans the CSV database to find the last inserted ID.
     * * Reads through the entire file to locate the final row and extracts
     * its primary key, incrementing it by 1 for the next insertion.
     * * @param path The filepath to the CSV database.
     * @return int The next available auto-increment ID. Returns 1 if empty/new.
     */
    int getNextId(const string& path) {
        ifstream infile(path);
        
        if (!infile.is_open()) return 1;

        string line;
        string last_line;
        bool is_first_line = true;

        while (getline(infile, line)) {
            if (line.empty()) continue;
            if (is_first_line) {
                is_first_line = false;
                continue;
            }
            last_line = line;
        }
        
        infile.close();

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

    /**
     * @brief Opens the database file in append mode and initializes headers.
     * * Checks if the file already exists before opening. If it is a brand new
     * file, it automatically injects the standard CSV headers.
     * * @param path The filepath to the CSV database.
     * @return true If the file was successfully opened and is ready for writing.
     * @return false If the system failed to open the file.
     */
    bool open(const string& path) {
        bool exists = ifstream(path).good();
        file.open(path, ios::app);
        if (!exists && file.is_open()) {
            file << "ID,City,Temperature_C,Condition,Lat,Lon,Timestamp\n";
        }
        return file.is_open();
    }
    
    /**
     * @brief Appends a single row of formatted telemetry data into the CSV file.
     * * @param id The auto-incremented primary key.
     * @param city The name of the scraped city.
     * @param temp The current temperature in Celsius.
     * @param cond The textual description of the weather (e.g., Clouds, Clear).
     * @param lat Geographical latitude.
     * @param lon Geographical longitude.
     * @param time Formatted system timestamp of the data ingestion.
     */
    void writeRow(int id, const string& city, double temp, const string& cond, double lat, double lon, const string& time) {
        if (file.is_open()) {
            file << id << "," << city << "," << temp << "," << cond << "," << lat << "," << lon << "," << time << "\n";
        }
    }
    
    /**
     * @brief Safely flushes the buffer and releases the file lock from the OS.
     */
    void close() { if(file.is_open()) file.close(); }
};

// ============================================================================
// 3. ORCHESTRATOR (PIPELINE) CLASS
// ============================================================================
class WeatherPipeline {
    string apiKey;
    vector<string> cities;

    /**
     * @brief libcurl callback function to process incoming HTTP streams.
     * * @param contents Pointer to the delivered data chunk.
     * @param size Size of a single byte element.
     * @param nmemb Number of bytes delivered in the payload.
     * @param s Pointer to our destination string buffer.
     * @return size_t The exact number of bytes safely appended to the buffer.
     */
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* s) {
        size_t newLength = size * nmemb;
        try {
            s->append((char*)contents, newLength);
            return newLength;
        } catch(bad_alloc& e) {
            return 0;
        }
    }

    /**
     * @brief Retrieves and formats the current system time.
     * * @return string Formatted timestamp (YYYY-MM-DD HH:MM:SS) for SQL/CSV insertion.
     */
    string get_timestamp() {
        auto now = chrono::system_clock::now();
        time_t now_c = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&now_c), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

public:
    /**
     * @brief Constructs a new Weather Pipeline instance.
     * @param key The authenticated OpenWeather API Key.
     * @param target_cities A vector containing the names of the cities to track.
     */
    WeatherPipeline(const string& key, const vector<string>& target_cities) : apiKey(key), cities(target_cities) {}

    /**
     * @brief Executes the complete ETL (Extract, Transform, Load) cycle.
     * * Connects to the database, initializes the libcurl network environment,
     * iterates through the target cities, parses JSON responses, and commits
     * the data to the persistent storage.
     * * @param csv_path The destination path for the database file.
     */
    void run(const string& csv_path) {
        DatabaseManager db;
        int current_id = db.getNextId(csv_path);
        
        if (!db.open(csv_path)) {
            cerr << "[!] Failed to open database!" << endl;
            return;
        }

        curl_global_init(CURL_GLOBAL_DEFAULT);
        CURL* curl = curl_easy_init();

        if(curl) {
            for (const auto& city : cities) {
                string url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";
                string response;
                
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                
                if (curl_easy_perform(curl) == CURLE_OK) {
                    try {
                        auto data = json::parse(response);
                        db.writeRow(current_id++, data["name"], data["main"]["temp"], data["weather"][0]["main"],
                                    data["coord"]["lat"], data["coord"]["lon"], get_timestamp());
                        cout << "[LOG] Ingested: " << city << " (ID: " << current_id - 1 << ")" << endl;
                    } catch (json::parse_error& e) {
                        cerr << "[!] JSON Error for " << city << ": " << e.what() << endl;
                    }
                }
            }
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
        db.close();
    }
};

// ============================================================================
// 4. ENTRY POINT
// ============================================================================
int main() {
    const string path_env = "../.env";
    const string path_csv = "../weather_database.csv";

    // 1. Load the secure API key from the environment configuration file
    string key = EnvLoader::getApiKey(path_env);
    if (key.empty()) {
        cerr << "[!] CRITICAL: API Key not found." << endl;
        return 1;
    }

    // 2. Define the target array of cities to be tracked by the pipeline
    vector<string> cities = {
        "Lisbon", "Porto", "London", "Paris", "Berlin",
        "Madrid", "Rome", "New York", "Barcelona", "Miami"
    };

    // 3. Instantiate the engine via Dependency Injection (passing credentials and data)
    WeatherPipeline pipeline(key, cities);
    
    // 4. Execute the ETL pipeline to fetch telemetry and write to the database
    pipeline.run(path_csv);
    
    return 0;
}