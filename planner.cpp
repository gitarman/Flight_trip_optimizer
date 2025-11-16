// planner.cpp
#include "planner.hpp"
#define ll long long
// Constructor: build adjacency list
Planner::Planner(const vector<Flight>& flights_data) {
    flights = flights_data;

    int max_city = 0;
    for (auto &f : flights) {
        max_city = max(max_city, max(f.start_city, f.end_city));
    }

    adj.assign(max_city + 1, {});
    for (int i = 0; i < (int)flights.size(); ++i) {
        adj[flights[i].start_city].push_back(i);
    }
}

// Helper: reconstruct full path using parent[]
vector<int> Planner::reconstructPath(const vector<int>& parent, int end_index) {
    vector<int> path;
    int cur = end_index;
    while (cur != -1) {
        path.push_back(cur);
        cur = parent[cur];
    }
    reverse(path.begin(), path.end());
    return path;
}

// ------------------ TASK 1 ------------------
// Fewest flights + earliest arrival
vector<int> Planner::least_flights_earliest_route(int start_city, int end_city, ll t1, ll t2) {
    int m = flights.size();
    const int INF = 1e9;

    vector<int> dist(m, INF);
    vector<int> parent(m, -1);
    queue<int> q;

    // Start BFS with flights from start_city
    for (int i : adj[start_city]) {
        const Flight &f = flights[i];
        if (f.dep >= t1 && f.arr <= t2) {
            dist[i] = 1;
            parent[i] = -1;
            q.push(i);
        }
    }

    int best_idx = -1, best_flights = INF, best_arr = INF;

    while (!q.empty()) {
        int cur = q.front();
        q.pop();

        int cnt = dist[cur];
        const Flight &fc = flights[cur];

        if (cnt > best_flights) continue;

        if (fc.end_city == end_city) {
            if (cnt < best_flights || (cnt == best_flights && fc.arr < best_arr)) {
                best_flights = cnt;
                best_arr = fc.arr;
                best_idx = cur;
            }
            continue;
        }

        for (int nxt : adj[fc.end_city]) {
            const Flight &fn = flights[nxt];
            if (fn.dep >= fc.arr + 20 && fn.arr <= t2 && dist[nxt] == INF) {
                dist[nxt] = cnt + 1;
                parent[nxt] = cur;
                q.push(nxt);
            }
        }
    }

    if (best_idx == -1) return {};
    return reconstructPath(parent, best_idx);
}

// ------------------ TASK 2 ------------------
// Cheapest (Dijkstra)
vector<int> Planner::cheapest_route(int start_city, int end_city, ll t1, ll t2) {
    int m = flights.size();
    const ll INFLL = 1e18;

    vector<ll> bestCost(m, INFLL);
    vector<int> parent(m, -1);

    priority_queue<pair<ll,int>, vector<pair<ll,int>>, greater<pair<ll,int>>> pq;

    for (int i : adj[start_city]) {
        const Flight &f = flights[i];
        if (f.dep >= t1 && f.arr <= t2) {
            bestCost[i] = f.fare;
            parent[i] = -1;
            pq.push({bestCost[i], i});
        }
    }

    int best_idx = -1;
    ll best_total = INFLL;

    while (!pq.empty()) {
        auto item = pq.top();
        pq.pop();
        ll cost = item.first;
        int cur = item.second;

        if (cost != bestCost[cur]) continue; // stale entry

        const Flight &fc = flights[cur];

        if (fc.end_city == end_city) {
            if (cost < best_total) {
                best_total = cost;
                best_idx = cur;
            }
            continue;
        }

        for (int nxt : adj[fc.end_city]) {
            const Flight &fn = flights[nxt];

            if (fn.dep >= fc.arr + 20 && fn.arr <= t2) {
                ll nc = cost + fn.fare;
                if (nc < bestCost[nxt]) {
                    bestCost[nxt] = nc;
                    parent[nxt] = cur;
                    pq.push({nc, nxt});
                }
            }
        }
    }

    if (best_idx == -1) return {};
    return reconstructPath(parent, best_idx);
}

// ------------------ TASK 3 ------------------
// Fewest flights first + cheapest (multi-criteria Dijkstra)
vector<int> Planner::least_flights_cheapest_route(int start_city, int end_city, ll t1, ll t2) {
    int m = flights.size();
    const ll INFLL = 1e18;
    const int INF = 1e9;

    vector<pair<int,ll>> bestState(m, {INF, INFLL});
    vector<int> parent(m, -1);

    struct Node { int flights; ll cost; int idx; };
    struct Cmp {
        bool operator()(const Node &a, const Node &b) const {
            if (a.flights != b.flights) return a.flights > b.flights;
            return a.cost > b.cost;
        }
    };

    priority_queue<Node, vector<Node>, Cmp> pq;

    for (int i : adj[start_city]) {
        const Flight &f = flights[i];
        if (f.dep >= t1 && f.arr <= t2) {
            bestState[i] = {1, f.fare};
            parent[i] = -1;
            pq.push({1, f.fare, i});
        }
    }

    pair<int,ll> bestDest = {INF, INFLL};
    int best_idx = -1;

    while (!pq.empty()) {
        Node curNode = pq.top();
        pq.pop();

        int cur = curNode.idx;
        if (bestState[cur].first != curNode.flights || bestState[cur].second != curNode.cost) continue;

        const Flight &fc = flights[cur];

        if (fc.end_city == end_city) {
            if (curNode.flights < bestDest.first || (curNode.flights == bestDest.first && curNode.cost < bestDest.second)) {
                bestDest = {curNode.flights, curNode.cost};
                best_idx = cur;
            }
            continue;
        }

        if (bestDest.first != INF && curNode.flights > bestDest.first) continue;

        for (int nxt : adj[fc.end_city]) {
            const Flight &fn = flights[nxt];

            if (fn.dep >= fc.arr + 20 && fn.arr <= t2) {
                int nf = curNode.flights + 1;
                ll nc = curNode.cost + fn.fare;

                auto rec = bestState[nxt];
                bool better = false;
                if (nf < rec.first) better = true;
                else if (nf == rec.first && nc < rec.second) better = true;

                if (better) {
                    bestState[nxt] = {nf, nc};
                    parent[nxt] = cur;
                    pq.push({nf, nc, nxt});
                }
            }
        }
    }

    if (best_idx == -1) return {};
    return reconstructPath(parent, best_idx);
}

