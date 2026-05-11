#include <iostream>
#include <vector>
#include <ranges>
#include <concepts>
#include <format>

// Represents a single data record extracted from a temperature sensor.
struct SensorData {
    int id;
    double temperature_celsius;
    bool is_active;
};

// Main entry point for the data pipeline application.
int main () {
    // Initializes a mock dataset of sensor telemetry.
    std::vector<SensorData> dataset = {
        {1, 22.5, true}, {2, 28.1, false}, {3, 19.8, true},
        {4, 30.5, true}, {5, 25.0, true}, {6, -5.0, false},
        {7, 40.0, true}, {8, 35.5, true}, {9, -15.0, true}
    };

    // Constructs a lazy evaluation pipeline using C++20 Ranges.
    // Filters for active sensors with temperatures exceeding 20.0 Celsius, 
    // and transforms the qualifying values into Fahrenheit.
    auto processed_data = dataset
        | std::views::filter([](const SensorData& d) { return d.is_active; })
        | std::views::filter([](const SensorData& d) { return d.temperature_celsius > 20.0; })
        | std::views::transform([](const SensorData& d) {
            return d.temperature_celsius * 1.8  + 32;
        });

    // Outputs the dataset header to the standard output stream.
    std::cout << "Temp processed (in Fahrenheit):\n";

    // Sequentially evaluates the pipeline and formats each result to two decimal places.
    for (double temp : processed_data) {
        std::cout << std::format("- {:.2f} °F\n", temp);
    }

    return 0;
}