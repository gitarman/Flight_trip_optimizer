#pragma once

#include <memory>

/**
 * @struct Flight
 * @brief Represents a single flight (the data).
 */
struct Flight {
    const int flight_no;
    const int start_city;
    const int departure_time;
    const int end_city;
    const int arrival_time;
    const int fare;

    // Constructor
    Flight(int no, int start, int dep, int end, int arr, int f)
        : flight_no(no), start_city(start), departure_time(dep),
          end_city(end), arrival_time(arr), fare(f) {}
};

// Shared pointer alias
using FlightPtr = std::shared_ptr<Flight>;
