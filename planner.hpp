#pragma once

#include <vector>
#include <map>
#include <string>
#include "flight.hpp" // Include our Flight definition

/**
 * @class Planner
 * @brief Manages flight data and provides routing algorithms.
 */
class Planner {
private:
    int num_cities;
    std::vector<FlightPtr> all_flights;
    std::map<int, std::vector<FlightPtr>> adj; // city -> outgoing flights

    // Helper to reconstruct the path (parents: child_flight_no -> parent FlightPtr)
    std::vector<FlightPtr> reconstructPath(
        std::map<int, FlightPtr>& parents,
        FlightPtr end_flight);

public:
    Planner(const std::vector<Flight>& flights);

    std::vector<FlightPtr> least_flights_earliest_route(
        int start_city, int end_city, long long t1, long long t2);

    std::vector<FlightPtr> cheapest_route(
        int start_city, int end_city, long long t1, long long t2);

    std::vector<FlightPtr> least_flights_cheapest_route(
        int start_city, int end_city, long long t1, long long t2);
};
