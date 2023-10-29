#include <bits/stdc++.h>

#include <cassert>
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

using ll = unsigned long long;
using u8 = unsigned char;
using vu8 = vector<u8>;
using pii = pair<int, int>;

#define sz 16
#define row 4
#define col 4

inline ll hasher(const vu8& mat) {
    ll hash = 0, pow = 1;
    for (int i = 0; i < sz; i++, pow *= sz) hash += mat[i] * (1ull << (4 * i));
    return hash;
}

unordered_map<ll, pair<int, u8>> moves;

inline int heuristic(vu8& mat) {
    int sum = 0;
    for (u8 i = 0, k = 0; i < row; i++) {
        for (u8 j = 0; j < col; j++, k++) {
            if (mat[k] == 0) continue;
            int el = mat[k] - 1;
            sum += abs(i - (el >> 2)) + abs(j - (el & 3));
        }
    }
    return sum;
}

// dist, heu, pos_of_0, mat
using state = tuple<int, int, u8, ll>;
struct myComp {
    constexpr bool operator()(state const& a, state const& b) const noexcept { return get<0>(a) > get<0>(b); }
};

inline void try_insert(priority_queue<state, vector<state>, myComp>& q, const state& node, u8 k, u8 k2, int diff, int node_value) {
    ll new_hash = get<3>(node);
    new_hash -= node_value * (1ull << (4 * k2));
    new_hash += node_value * (1ull << (4 * k));
    int heu = get<1>(node) + diff;
    int dist = get<0>(node) + diff + 1;
    auto search = moves.find(new_hash);
    if (search == moves.end() || dist < search->second.first) {
        moves[new_hash] = {dist, k};
        q.push({dist, heu, k2, new_hash});
    }
}

int a_star(vu8& start) {
    priority_queue<state, vector<state>, myComp> q;
    {
        int heu = heuristic(start);
        int idx_zero;
        for (idx_zero = 0; idx_zero < sz; idx_zero++)
            if (start[idx_zero] == 0) break;
        ll hash = hasher(start);
        q.push({heu, heu, idx_zero, hash});
        moves[hash] = {0, idx_zero};
    }

    while (!q.empty()) {
        auto node = q.top();
        q.pop();
        if (get<1>(node) == 0) return get<0>(node);
        u8 k = get<2>(node), i = k >> 2, j = k & 3;
        if (i > 0) {
            // dbg("up");
            u8 k2 = k - col;
            int node_value = (get<3>(node) >> (4 * k2)) & 15;
            int diff = abs(i - ((node_value - 1) >> 2)) - abs(i - 1 - ((node_value - 1) >> 2));
            try_insert(q, node, k, k2, diff, node_value);
        }
        if (i < row - 1) {
            // dbg("down");
            u8 k2 = k + col;
            int node_value = (get<3>(node) >> (4 * k2)) & 15;
            int diff = abs(i - ((node_value - 1) >> 2)) - abs(i + 1 - ((node_value - 1) >> 2));
            try_insert(q, node, k, k2, diff, node_value);
        }
        if (j > 0) {
            // dbg("left");
            u8 k2 = k - 1;
            int node_value = (get<3>(node) >> (4 * k2)) & 15;
            int diff = abs(j - ((node_value - 1) & 3)) - abs(j - 1 - ((node_value - 1) & 3));
            try_insert(q, node, k, k2, diff, node_value);
        }
        if (j < col - 1) {
            // dbg("right");
            u8 k2 = k + 1;
            int node_value = (get<3>(node) >> (4 * k2)) & 15;
            int diff = abs(j - ((node_value - 1) & 3)) - abs(j + 1 - ((node_value - 1) & 3));
            try_insert(q, node, k, k2, diff, node_value);
        }
    }
    return -1;
}

// a bit too slow, but nevermind
vector<vector<u8>> solution(int step) {
    u8 k = sz;
    vector<u8> mat(k);
    vector<vector<u8>> stak;
    for (int i = 1; i < k; i++) mat[i - 1] = i;
    stak.push_back(mat);
    for (k--; step > 0; step--) {
        auto [_, k0] = moves[hasher(mat)];
        swap(mat[k], mat[k0]);
        stak.push_back(mat);
        k = k0;
    }
    reverse(stak.begin(), stak.end());
    return stak;
}

int main() {
    int n, m;
    cin >> n >> m;
    assert(n == row && m == col);
    vector mat(sz, (u8)0);
    vector<bool> verif(sz, false);
    for (auto& el : mat) {
        int x;
        cin >> x;
        assert(x < sz && !verif[x]);
        el = (u8)x;
        verif[x] = true;
    }
    int step = a_star(mat);
    cout << step << endl;
    if (step != -1) {
        auto sol = solution(step);
        for (int t = 0; t < sol.size(); t++) {
            for (int i = 0, k = 0; i < row; i++) {
                for (int j = 0; j < col; j++, k++)
                    if (sol[t][k]) cout << setw(3) << (int)sol[t][k] << " ";
                    else cout << "    ";
                cout << endl;
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
