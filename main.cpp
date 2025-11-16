#include <bits/stdc++.h>
#include "planner.hpp"
using namespace std;
#define ll long long
// helper to print final route
void printRoute(const vector<int>& route, const vector<Flight>& flights, const string& title) {
    cout << "\n" << title << "\n";
    if (route.empty()) {
        cout << "  No route found.\n";
        return;
    }

    ll total = 0;
    for (int idx : route) {
        const Flight &f = flights[idx];
        cout << "  Flight " << f.id << ": " << f.start_city 
             << " (" << f.dep << ") -> " 
             << f.end_city << " (" << f.arr << ")"
             << " [Cost: " << f.fare << "]\n";
        total += f.fare;
    }
    cout << "  Total flights: " << route.size() << endl;
    cout << "  Total cost: " << total << endl;
    cout << "  Final arrival: " << flights[route.back()].arr << endl;
}

int main() {
    ifstream in("C.txt");
    int n, m;
    in >> n >> m;

    vector<Flight> flights;
    flights.reserve(m);

    for (int i = 0; i < m; ++i) {
        int u, v, cost, dep, arr;
        in >> u >> v >> cost >> dep >> arr;
        flights.push_back(Flight(i, u, dep, v, arr, cost));
    }
    in.close();

    Planner planner(flights);

    int start_city = 0, end_city = 5;
    ll t1 = 0, t2 = 3000000000000LL;

    auto r1 = planner.least_flights_earliest_route(start_city, end_city, t1, t2);
    printRoute(r1, flights, "Task 1: Least Flights + Earliest Arrival");

    auto r2 = planner.cheapest_route(start_city, end_city, t1, t2);
    printRoute(r2, flights, "Task 2: Cheapest Route");

    auto r3 = planner.least_flights_cheapest_route(start_city, end_city, t1, t2);
    printRoute(r3, flights, "Task 3: Least Flights + Cheapest");

    return 0;
}

