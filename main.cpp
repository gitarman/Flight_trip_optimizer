#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>

#include "flight.hpp"
#include "planner.hpp"

using namespace std;

void printRoute(const string& title, const vector<FlightPtr>& route) {
    cout << "--- " << title << " ---" << endl;
    if (route.empty()) {
        cout << "No route found." << endl;
        return;
    }

    long long total_cost = 0;
    for (const auto& flight : route) {
        cout << "  Flight " << flight->flight_no
             << ": " << flight->start_city << " (" << flight->departure_time << ")"
             << " -> " << flight->end_city << " (" << flight->arrival_time << ")"
             << " [Cost: " << flight->fare << "]" << endl;
        total_cost += flight->fare;
    }
    cout << "  Total Flights: " << route.size() << endl;
    cout << "  Total Cost: " << total_cost << endl;
    cout << "  Final Arrival: " << route.back()->arrival_time << endl;
    cout << "---------------------------------" << endl << endl;
}

int main() {
    string file_path = "C.txt";
    ifstream input_file(file_path);
    if (!input_file.is_open()) {
        cerr << "Error: Could not open file " << file_path << endl;
        return 1;
    }

    string line;
    int n, m;
    if (!getline(input_file, line)) { cerr << "Bad input\n"; return 1; }
    stringstream ss(line);
    ss >> n >> m;

    vector<Flight> flights_data;
    for (int i = 0; i < m; ++i) {
        if (!getline(input_file, line)) break;
        stringstream s2(line);
        int u, v, cost, dep, arr;
        s2 >> u >> v >> cost >> dep >> arr;
        flights_data.emplace_back(i, u, dep, v, arr, cost);
    }
    input_file.close();
    cout << "Successfully parsed " << flights_data.size() << " flights." << endl;

    auto st = chrono::high_resolution_clock::now();
    Planner planner(flights_data);
    auto et = chrono::high_resolution_clock::now();
    cout << "Planner built in " << chrono::duration_cast<chrono::milliseconds>(et - st).count() << " ms.\n\n";

    int start_city = 0;
    int destination = 5;
    long long t1 = 0;
    long long t2 = 3000000000000LL;

    cout << "Testing routes for start_city=" << start_city << ", destination=" << destination << "\n\n";

    auto r1 = planner.least_flights_earliest_route(start_city, destination, t1, t2);
    printRoute("Task 1: Least Flights Earliest Route", r1);

    auto r2 = planner.cheapest_route(start_city, destination, t1, t2);
    printRoute("Task 2: Cheapest Route", r2);

    auto r3 = planner.least_flights_cheapest_route(start_city, destination, t1, t2);
    printRoute("Task 3: Least Flights Cheapest Route", r3);

    return 0;
}
