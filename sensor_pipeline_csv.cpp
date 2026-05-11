#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <ranges>
#include <format>

// Represents a single data record extracted from a temperature sensor.
struct SensorData {
    int id;
    double temperature_celsius;
    bool is_active;
};

// Reads a CSV file and populates a vector of SensorData structures.
// Expects the format: id,temperature_celsius,is_active (e.g., 1,22.5,1 or 1,22.5,true).
std::vector<SensorData> load_data_from_csv(const std::string& filepath) {
    std::vector<SensorData> dataset;
    std::ifstream file(filepath);
    std::string line;

    // Terminates the file reading operation if the file cannot be opened.
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << filepath << "\n";
        return dataset;
    }

    // Ignores the first line containing the CSV column headers.
    std::getline(file, line);

    // Iterates through each subsequent line of the CSV file.
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id_str, temp_str, active_str;

        // Extracts substrings delimited by commas.
        if (std::getline(ss, id_str, ',') && 
            std::getline(ss, temp_str, ',') && 
            std::getline(ss, active_str, ',')) {
            
            SensorData data;
            data.id = std::stoi(id_str);
            data.temperature_celsius = std::stod(temp_str);
            
            // Evaluates both numeric and textual boolean representations.
            data.is_active = (active_str == "1" || active_str == "true");

            dataset.push_back(data);
        }
    }

    // Closes the file stream to release system resources.
    file.close();
    return dataset;
}

// Example to RUN via LINUX CLI if Binary is in bin folder: ./sensor_pipeline_csv ../sensors.csv
// Main entry point for the data pipeline application.
// Expects the CSV file path to be passed as a command-line argument.
int main(int argc, char* argv[]) {
    // Validates that the user provided exactly one argument (the file path).
    // argc is 1 if only the program name is executed, so argc must be at least 2.
    if (argc != 2) {
        std::cerr << "Usage Error: Missing file path.\n";
        std::cerr << "Command syntax: " << argv[0] << " <path_to_csv_file>\n";
        return 1;
    }

    // Dynamically loads the dataset using the provided command-line argument.
    std::string csv_file = argv[1];
    std::vector<SensorData> dataset = load_data_from_csv(csv_file);

    // Exits the application gracefully if no data is available for processing.
    if (dataset.empty()) {
        std::cout << "No data to process. Exiting.\n";
        return 1;
    }

    // Constructs a lazy evaluation pipeline using C++20 Ranges.
    // Filters for active sensors with temperatures exceeding 20.0 Celsius, 
    // and transforms the qualifying values into Fahrenheit.
    auto processed_data = dataset
        | std::views::filter([](const SensorData& d) { return d.is_active; })
        | std::views::filter([](const SensorData& d) { return d.temperature_celsius > 20.0; })
        | std::views::transform([](const SensorData& d) {
            return d.temperature_celsius * 1.8 + 32.0;
        });

    std::cout << "Temperatures processed (in Fahrenheit):\n";

    // Sequentially evaluates the pipeline and outputs the formatted results.
    for (double temp : processed_data) {
        std::cout << std::format("- {:.2f} °F\n", temp);
    }

    return 0;
}