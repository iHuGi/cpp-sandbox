#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <chrono>

using namespace std;
using json = nlohmann::json;

// ============================================================================
// 1. ENV LOADER CLASS
// ============================================================================
class EnvLoader {
public:
    static string getApiKey(const string& path) {
        ifstream file(path);
        string line;

        if (!file.is_open()) return "";

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
// 2. LIBCURL CALLBACK
// ============================================================================
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* s) {
    size_t total = size * nmemb;
    try { s->append((char*)contents, total); return total; }
    catch (...) { return 0; }
}

// ============================================================================
// 3. ENTRY POINT
// ============================================================================
int main() {
    // -----------------------------------------------------
    // 1. START TIMER AT THE ABSOLUTE BEGINNING
    // -----------------------------------------------------
    auto start_time = chrono::high_resolution_clock::now();

    cout << "--- CONCURRENT WEATHER PIPELINE (ASYNC I/O) ---" << endl;

    string key = EnvLoader::getApiKey(".env");
    if (key.empty()) key = EnvLoader::getApiKey("../.env");
    
    if (key.empty()) {
        cerr << "[!] CRITICAL ABORT: API Key not found." << endl;
        return 1;
    }

    const vector<string> cities = {
        "Aveiro","Beja","Braga","Braganca","Castelo Branco",
        "Coimbra","Evora","Faro","Funchal","Guarda",
        "Leiria","Lisbon","Ponta Delgada","Portalegre","Porto",
        "Santarem","Setubal","Viana do Castelo","Vila Real","Viseu"
    };

    curl_global_init(CURL_GLOBAL_DEFAULT);

    CURLM* multi = curl_multi_init();
    vector<CURL*> handles;
    vector<string> responses(cities.size());
    int still_running = 0;

    // Initialize and attach all requests to the multi-handle
    for (size_t i = 0; i < cities.size(); i++) {
        CURL* c = curl_easy_init();
        if (!c) continue;
        handles.push_back(c);
        
        char* encoded = curl_easy_escape(c, cities[i].c_str(), cities[i].size());
        string url = "https://api.openweathermap.org/data/2.5/weather?q=" + string(encoded) + "&appid=" + key + "&units=metric";
        curl_free(encoded);

        curl_easy_setopt(c, CURLOPT_URL, url.c_str());
        curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(c, CURLOPT_WRITEDATA, &responses[i]);
        curl_easy_setopt(c, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 5L);

        curl_multi_add_handle(multi, c);
    }

    // Fire all requests concurrently (Non-blocking I/O loop)
    curl_multi_perform(multi, &still_running);

    int max_loops = 1000;
    while (still_running && max_loops--) {
        int numfds;
        // Wait for network activity, max 1 second timeout per check
        curl_multi_wait(multi, nullptr, 0, 1000, &numfds);
        curl_multi_perform(multi, &still_running);
    }

    // Clean up the CURL handles
    for (auto c : handles) {
        curl_multi_remove_handle(multi, c);
        curl_easy_cleanup(c);
    }
    curl_multi_cleanup(multi);

    // Parse the populated strings and print the results
    for (size_t i = 0; i < responses.size(); i++) {
        try {
            auto data = json::parse(responses[i]);
            if (data.contains("cod") && data["cod"] == 200) {
                cout << ">> " << string(data["name"]) << " <<" << endl;
                cout << " Temp : " << data["main"]["temp"].get<double>() << " C" << endl;
                cout << " Cond : " << string(data["weather"][0]["main"]) << endl;
                cout << "-----------------------------------" << endl;
            } else {
                cerr << "[!] Failed to fetch data for " << cities[i] << endl;
            }
        } catch (...) {
            cerr << "[!] JSON Parse Error or empty response for " << cities[i] << endl;
        }
    }

    curl_global_cleanup();

    // -----------------------------------------------------
    // 2. STOP TIMER AT THE ABSOLUTE END
    // -----------------------------------------------------
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    cout << "\nTotal C++ End-to-End time: " << duration << " milliseconds." << endl;

    return 0;
}