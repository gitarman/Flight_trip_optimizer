#pragma once
#include <bits/stdc++.h>
using namespace std;

struct Flight {
    int id;             // flight number (0-based index)
    int start_city;
    int dep;            // departure time
    int end_city;
    int arr;            // arrival time
    int fare;

    Flight() = default;
    Flight(int _id, int s, int d, int e, int a, int f)
        : id(_id), start_city(s), dep(d), end_city(e), arr(a), fare(f) {}
};

