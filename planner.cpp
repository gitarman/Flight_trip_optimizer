#include<bits/stdc++.h>
#include "planner.hpp"
#include <queue>
#include <map>
#include <limits>
#include <algorithm>

// --- Constructor ---
Planner::Planner(const std::vector<Flight>& flights) {
    int max_city = 0;
    all_flights.reserve(flights.size());
    for (const auto &f : flights) {
        FlightPtr fp = std::make_shared<Flight>(f);
        all_flights.push_back(fp);
        adj[f.start_city].push_back(fp);
        if (f.start_city > max_city) max_city = f.start_city;
        if (f.end_city > max_city) max_city = f.end_city;
    }
    num_cities = max_city + 1;
    // ensure map entries exist (optional)
    for (int i = 0; i < num_cities; ++i) {
        if (!adj.count(i)) adj[i] = std::vector<FlightPtr>();
    }

    // Optional: sort outgoing flights by departure_time to make traversal deterministic
    for (auto &p : adj) {
        std::sort(p.second.begin(), p.second.end(),
                  [](const FlightPtr &a, const FlightPtr &b){
                      return a->departure_time < b->departure_time;
                  });
    }
}

// --- Path reconstruction (parents: child_flight_no -> parent FlightPtr) ---
std::vector<FlightPtr> Planner::reconstructPath(
    std::map<int, FlightPtr>& parents,
    FlightPtr end_flight) {

    std::vector<FlightPtr> path;
    FlightPtr cur = end_flight;
    while (cur) {
        path.push_back(cur);
        auto it = parents.find(cur->flight_no);
        if (it == parents.end()) break;
        cur = it->second;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// -------------------- Task 1 --------------------
// Fewest flights, tie-breaker earliest arrival
std::vector<FlightPtr> Planner::least_flights_earliest_route(
    int start_city, int end_city, long long t1, long long t2) {

    std::queue<FlightPtr> q;
    std::map<int, FlightPtr> parents;      // child flight_no -> parent flight
    std::map<int, int> usedFlights;        // flight_no -> number of flights to reach it
    FlightPtr best = nullptr;
    int minFlights = std::numeric_limits<int>::max();

    // seed with flights from start_city that satisfy times
    for (const auto &f : adj[start_city]) {
        if (f->departure_time >= t1 && f->arrival_time <= t2) {
            q.push(f);
            parents[f->flight_no] = nullptr;
            usedFlights[f->flight_no] = 1;
        }
    }

    while (!q.empty()) {
        FlightPtr cur = q.front(); q.pop();
        int cnt = usedFlights[cur->flight_no];
        if (cnt > minFlights) continue; // prune longer paths

        if (cur->end_city == end_city) {
            if (cnt < minFlights ||
               (cnt == minFlights && (!best || cur->arrival_time < best->arrival_time))) {
                best = cur;
                minFlights = cnt;
            }
            continue;
        }

        for (const auto &nxt : adj[cur->end_city]) {
            if (nxt->departure_time >= (cur->arrival_time + 20) &&
                nxt->arrival_time <= t2 &&
                !usedFlights.count(nxt->flight_no)) {
                usedFlights[nxt->flight_no] = cnt + 1;
                parents[nxt->flight_no] = cur;
                q.push(nxt);
            }
        }
    }

    if (!best) return {};
    return reconstructPath(parents, best);
}

// -------------------- Task 2 --------------------
// Cheapest route (Dijkstra over flights). State = (cumulative_cost, flight_ptr)
std::vector<FlightPtr> Planner::cheapest_route(
    int start_city, int end_city, long long t1, long long t2) {

    using State = std::pair<long long, FlightPtr>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    std::map<int, long long> bestCostForFlight;   // flight_no -> best cost to reach that flight
    std::map<int, FlightPtr> parents;             // child flight_no -> parent flight
    FlightPtr best = nullptr;
    long long bestCost = std::numeric_limits<long long>::max();

    // seed
    for (const auto &f : adj[start_city]) {
        if (f->departure_time >= t1 && f->arrival_time <= t2) {
            pq.push({f->fare, f});
            bestCostForFlight[f->flight_no] = f->fare;
            parents[f->flight_no] = nullptr;
        }
    }

    while (!pq.empty()) {
        State s = pq.top(); pq.pop();
        long long cost = s.first;
        FlightPtr cur = s.second;

        // stale entry check
        if (bestCostForFlight.count(cur->flight_no) && cost != bestCostForFlight[cur->flight_no])
            continue;

        if (cur->end_city == end_city) {
            if (cost < bestCost) {
                bestCost = cost;
                best = cur;
            }
            // continue to pop other equally cheap candidates that might give different parents
            continue;
        }

        for (const auto &nxt : adj[cur->end_city]) {
            if (nxt->departure_time >= (cur->arrival_time + 20) &&
                nxt->arrival_time <= t2) {
                long long nc = cost + nxt->fare;
                if (!bestCostForFlight.count(nxt->flight_no) || nc < bestCostForFlight[nxt->flight_no]) {
                    bestCostForFlight[nxt->flight_no] = nc;
                    parents[nxt->flight_no] = cur;
                    pq.push({nc, nxt});
                }
            }
        }
    }

    if (!best) return {};
    return reconstructPath(parents, best);
}

// -------------------- Task 3 --------------------
// Fewest flights first, among them cheapest.
// We'll run a multi-criteria Dijkstra where state is (flights_used, cost, flight_ptr)
// Compare by flights, then cost. We track best known (flights,cost) per flight node to avoid
// pruning valid alternatives incorrectly.
std::vector<FlightPtr> Planner::least_flights_cheapest_route(
    int start_city, int end_city, long long t1, long long t2) {

    struct Node {
        int flights;
        long long cost;
        FlightPtr f;
        bool operator>(Node const &o) const {
            if (flights != o.flights) return flights > o.flights;
            return cost > o.cost;
        }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    std::map<int, std::pair<int, long long>> bestForFlight; // flight_no -> (flights, cost)
    std::map<int, FlightPtr> parents; // child flight_no -> parent flight
    FlightPtr best = nullptr;
    std::pair<int,long long> bestDestState = {INT_MAX, std::numeric_limits<long long>::max()};

    // seed with first-leg flights from start_city
    for (const auto &f : adj[start_city]) {
        if (f->departure_time >= t1 && f->arrival_time <= t2) {
            pq.push({1, f->fare, f});
            bestForFlight[f->flight_no] = {1, f->fare};
            parents[f->flight_no] = nullptr;
        }
    }

    while (!pq.empty()) {
        Node cur = pq.top(); pq.pop();

        // stale entry: if we have a strictly better recorded state for this flight number, skip
        if (bestForFlight.count(cur.f->flight_no)) {
            auto rec = bestForFlight[cur.f->flight_no];
            if (cur.flights != rec.first || cur.cost != rec.second) continue;
        }

        // If current flight reaches destination, check and update best dest state
        if (cur.f->end_city == end_city) {
            // Is this better than bestDestState?
            if (cur.flights < bestDestState.first ||
               (cur.flights == bestDestState.first && cur.cost < bestDestState.second)) {
                bestDestState = {cur.flights, cur.cost};
                best = cur.f;
            }
            // do not break — there might be another path with same flights and lower cost
            continue;
        }

        // If current state's flights already exceed bestDestState.flights, no need to expand
        if (bestDestState.first != INT_MAX && cur.flights > bestDestState.first) continue;

        // expand
        for (const auto &nxt : adj[cur.f->end_city]) {
            if (nxt->departure_time >= (cur.f->arrival_time + 20) &&
                nxt->arrival_time <= t2) {

                int nf = cur.flights + 1;
                long long nc = cur.cost + nxt->fare;

                bool better = false;
                if (!bestForFlight.count(nxt->flight_no)) better = true;
                else {
                    auto rec = bestForFlight[nxt->flight_no];
                    if (nf < rec.first) better = true;
                    else if (nf == rec.first && nc < rec.second) better = true;
                }

                if (better) {
                    bestForFlight[nxt->flight_no] = {nf, nc};
                    parents[nxt->flight_no] = cur.f;
                    pq.push({nf, nc, nxt});
                }
            }
        }
    }

    if (!best) return {};
    return reconstructPath(parents, best);
}
