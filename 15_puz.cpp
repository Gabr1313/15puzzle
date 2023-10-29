#include <bits/stdc++.h>
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

using u8 = unsigned char;
using ll = unsigned long long;
using vu8 = vector<u8>;
using pii = pair<int, int>;

#define SZ 16
#define ROW 4
#define COL 4

inline ll hasher(const vu8& mat) {
    ll hash = 0, pow = 1;
    for (int i = 0; i < SZ; i++, pow *= SZ) hash += mat[i] * (1ull << (i << 2));
    return hash;
}

// hash, dist, pos_of_0_prec
unordered_map<ll, pair<u8, u8>> moves;

inline u8 heuristic(ll mat) {
    int sum = 0;
    for (u8 i = 0, k = 0; i < ROW; i++) {
        for (u8 j = 0; j < COL; j++, k++) {
            int el = (mat >> (4 * k)) & 15;
            if (el == 0) continue;
            sum += abs(i - ((el - 1) >> 2)) + abs(j - ((el - 1) & 3));
        }
    }
    return sum;
}

// dist, heu, pos_of_0, mat
using state = tuple<u8, u8, u8, ll>;
struct myComp {
    constexpr bool operator()(state const& a, state const& b) const noexcept { return get<0>(a) > get<0>(b); }
};

inline void try_insert(priority_queue<state, vector<state>, myComp>& q, const state& node, u8 k, u8 k2, int diff, int node_value) {
    ll new_hash = get<3>(node);
    new_hash -= node_value * (1ull << k2);
    new_hash += node_value * (1ull << k);
    int heu = get<1>(node) + diff;
    int dist = get<0>(node) + diff + 1;
    auto search = moves.find(new_hash);
    if (search == moves.end() || dist < search->second.first) {
        moves[new_hash] = {dist, k};
        q.push({dist, heu, k2, new_hash});
    }
}

u8 a_star(ll start_hash) {
    priority_queue<state, vector<state>, myComp> q;
    u8 heu = heuristic(start_hash);
    u8 idx_zero;
    for (idx_zero = 0; idx_zero < SZ; idx_zero++)
        if ((start_hash >> (idx_zero << 2) & 15) == 0) break;
    q.push({heu, heu, idx_zero << 2, start_hash});
    moves[start_hash] = {0, idx_zero << 2};

    while (!q.empty()) {
        auto node = q.top();
        q.pop();
        if (get<1>(node) == 0) return get<0>(node);
        u8 k = get<2>(node), i = k >> 4, j = (k >> 2) & 3;
        if (i > 0) {
            u8 k2 = k - (COL << 2);
            int node_value = (get<3>(node) >> k2) & 15;
            int diff = abs(i - ((node_value - 1) >> 2)) - abs(i - 1 - ((node_value - 1) >> 2));
            try_insert(q, node, k, k2, diff, node_value);
        }
        if (i < ROW - 1) {
            u8 k2 = k + (COL << 2);
            int node_value = (get<3>(node) >> k2) & 15;
            int diff = abs(i - ((node_value - 1) >> 2)) - abs(i + 1 - ((node_value - 1) >> 2));
            try_insert(q, node, k, k2, diff, node_value);
        }
        if (j > 0) {
            u8 k2 = k - (1 << 2);
            int node_value = (get<3>(node) >> k2) & 15;
            int diff = abs(j - ((node_value - 1) & 3)) - abs(j - 1 - ((node_value - 1) & 3));
            try_insert(q, node, k, k2, diff, node_value);
        }
        if (j < COL - 1) {
            u8 k2 = k + (1 << 2);
            int node_value = (get<3>(node) >> k2) & 15;
            int diff = abs(j - ((node_value - 1) & 3)) - abs(j + 1 - ((node_value - 1) & 3));
            try_insert(q, node, k, k2, diff, node_value);
        }
    }
    return 0xff;
}

vector<ll> solution(u8 step) {
    vector<ll> stak;
    ll mat_hash = 0;
    for (int i = 1; i < SZ; i++) mat_hash += i * (1ull << ((i - 1) << 2));
    stak.push_back(mat_hash);
    for (u8 k = 60; step > 0; step--) {
        auto [_, k0] = moves[mat_hash];
        ll el = (mat_hash >> k0) & 15;
        mat_hash &= ~(15ull << k0);
        mat_hash |= (el << k);
        stak.push_back(mat_hash);
        k = k0;
    }
    reverse(stak.begin(), stak.end());
    return stak;
}

int main() {
    int n, m;
    cin >> n >> m;
    assert(n == ROW && m == COL);
    ll mat_hash = 0;
    vector<bool> verif(SZ, false);
    for (int i = 0; i < SZ; i++) {
        int x;
        cin >> x;
        assert(x < SZ && !verif[x]);
        verif[x] = true;
        mat_hash += x * (1ull << (i << 2));
    }

    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    u8 step = a_star(mat_hash);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << (int)step << " moves found in " << duration.count() << "ms" << endl;

    if (step != 0xff) {
        auto sol = solution(step);
        dbg(solution(step));
        for (ll t = 0; t < sol.size(); t++) {
            for (int i = 0; i < SZ; i++) {
                u8 el = (sol[t] >> (i << 2)) & 15;
                if (el) cout << setw(3) << (int)el << " ";
                else cout << "    ";
                if (i % 4 == 3) cout << endl;
            }
            {
                // cout << endl;
                if (t < sol.size() - 1)
                    for (int i = 0; i < n; i++) cout << "\x1b[A";
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
        }
    }

    return 0;
}
