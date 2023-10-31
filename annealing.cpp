#include <bits/stdc++.h>

#include <iostream>
using namespace std;
// #include <ext/pb_ds/tree_policy.hpp>
// #include <ext/pb_ds/assoc_container.hpp>
// template<typename T>
// using ordered_set = __gnu_pbds::tree<T, __gnu_pbds::null_type, less<T>, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;
// template<typename T, typename S>
// using ordered_map = __gnu_pbds::tree<T, S, less<T>, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;

template <typename T, typename S>
ostream& operator<<(ostream& o, pair<T, S>& p) {
    return o << "(" << p.first << ", " << p.second << ")";
}
template <typename Container, typename S = typename enable_if<!is_same<Container, string>::value, typename Container::value_type>::type>
ostream& operator<<(ostream& __out, Container& __ct) {
    __out << "{";
    bool first = true;
    for (auto&& __el : __ct) {
        if (!first) __out << ", ";
        __out << __el;
        first = false;
    }
    return __out << "}";
}
template <typename T1, typename T2, typename T3, typename T4>
ostream& operator<<(ostream& o, tuple<T1, T2, T3, T4>& p) {
    return o << "(" << get<0>(p) << ", " << get<1>(p) << ", " << get<2>(p) << ", " << get<3>(p) << ")";
}

template <typename Tail>
void _dbg(Tail T) {
    cerr << T << endl;
}
template <typename Head, typename... Tail>
void _dbg(Head H, Tail... T) {
    cerr << H << ", ";
    _dbg(T...);
}
#ifdef DEBUG
#define dbg(...)                              \
    do {                                      \
        cerr << "[" << #__VA_ARGS__ << "]: "; \
        _dbg(__VA_ARGS__);                    \
    } while (0)
#else
#define dbg(...) 42
#endif

// ANNEALING FOR TSP -> IS IT POSSIBLE TO TRANSLATE FOR THE 15 PUZZLE?
/* const double BOLTZMAN = 0.01;
const double INIT_TEMP = 1;

// const double COOLIGN_FRACTION = 0.97; // up-down together
// const ull COOLIGN_STEP = 500; // up-down together
// const ull STEP_PER_TEMP = 1000; // up-down together 
const double COOLIGN_FRACTION = 0.999;  // up-down together
const ull COOLIGN_STEP = 5000;          // up-down together
const ull STEP_PER_TEMP = 10000;        // up-down together

int anneal(vi& v, vector<vector<int>>& distances) {
    int distance = 0;
    for (int i = 0; i < v.size(); i++) distance += distances[v[i]][v[(i + 1) % v.size()]];
    double temperature = INIT_TEMP;
    for (int i = 0; i < COOLIGN_STEP; i++) {
        int start_value = distance;
        double intial_temperature = temperature;
        temperature *= COOLIGN_FRACTION;
        for (int j = 0; j < STEP_PER_TEMP; j++) {
            int k = rand() % v.size();
            int l = rand() % v.size();
            int cost = cost_change(v, distances, k, l);
            if (cost < 0) {
                swap(v[k], v[l]);
                distance = distance + cost;
                continue;
            }
            double e = -cost / (distance * BOLTZMAN * temperature);
            double merit = exp(e);
            float flip = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            if (merit > flip) {
                swap(v[k], v[l]);
                distance = distance + cost;
            }
        }
        if (distance - start_value < 0) temperature = intial_temperature;
    }
    return distance;
} */
