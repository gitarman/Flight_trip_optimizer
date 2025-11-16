#pragma once
#include <bits/stdc++.h>
#include "flight.hpp"
using namespace std;
using ll = long long;

class Planner {
private:
    vector<Flight> flights;              // list of flights
    vector<vector<int>> adj;             // adj[city] → list of outgoing flight indices

    vector<int> reconstructPath(const vector<int>& parent, int end_index);

public:
    Planner(const vector<Flight>& flights_data);

    vector<int> least_flights_earliest_route(int start_city, int end_city, ll t1, ll t2);

    vector<int> cheapest_route(int start_city, int end_city, ll t1, ll t2);

    vector<int> least_flights_cheapest_route(int start_city, int end_city, ll t1, ll t2);
};

