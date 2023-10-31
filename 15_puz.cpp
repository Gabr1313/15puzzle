#include <bits/stdc++.h>
using namespace std;

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
using ull = unsigned long long;

#define SZ 16
#define ROW 4
#define COL 4

struct memo {
    u8 dist, pos_of_0_prec;
    bool processed;
};

struct state {
    u8 dist, heu, istant_0_idx;
    ull mat;
    bool operator>(const state& other) const {
        if (dist == other.dist) return heu > other.heu;
        return dist > other.dist;
    }
};

inline u8 heuristic(ull mat, ull col, ull row) {
    int sum = 0;
    for (u8 k = 0; k < SZ; k++) {
        int el = (mat >> (k << 2)) & 15;
        if (el == 0) continue;
        u8 r1 = (row >> (el << 2)) & 15;
        u8 c1 = (col >> (el << 2)) & 15;
        sum += abs((k >> 2) - r1) + abs((k & 3) - c1);
    }
    return sum;
}

inline void try_insert(priority_queue<state, vector<state>, std::greater<state>>& q, const state& node, u8 k, u8 k2, int diff, u8 node_value,
                       unordered_map<ull, memo>& moves) {
    ull new_mat = node.mat;
    new_mat -= (ull)node_value << k2;
    new_mat += (ull)node_value << k;
    u8 heu = node.heu + diff;
    u8 dist = node.dist + diff + 1;
    auto search = moves.find(new_mat);
    if (search == moves.end() || dist < (search->second).dist) {
        moves[new_mat] = {dist, k, false};
        q.push({dist, heu, k2, new_mat});
    }
}

pair<u8, ull> a_star(ull start_mat, unordered_map<ull, memo>& moves) {
    ull sol = 0x0fedcba987654321;
    ull col = 0, row = 0;
    for (ull i = 0; i < SZ; i++) {
        u8 val = (sol >> (i << 2)) & 15;
        col |= ((i & 3) << (val << 2));
        row |= ((i >> 2) << (val << 2));
    }

    priority_queue<state, vector<state>, std::greater<state>> q;
    u8 heu = heuristic(start_mat, col, row);
    u8 idx_zero;
    for (idx_zero = 0; idx_zero < SZ; idx_zero++)
        if ((start_mat >> (idx_zero << 2) & 15) == 0) break;
    q.push({heu, heu, (u8)(idx_zero << 2), start_mat});
    moves[start_mat] = {0, 0xff, false};

    ull cnt = 0;
    while (!q.empty()) {
        auto node = q.top();
        q.pop();
        auto search = moves.find(node.mat);
        if ((search->second).processed) continue;
        else (search->second).processed = true;
        cnt++;
        if (node.heu == 0) return {node.dist, cnt};
        u8 k = node.istant_0_idx, i = k >> 4, j = (k >> 2) & 3;
        if (i > 0) {
            u8 k2 = k - (COL << 2);
            u8 node_value = (node.mat >> k2) & 15;
            u8 r = (row >> (node_value << 2)) & 15;
            int diff = abs(i - r) - abs(i - 1 - r);
            try_insert(q, node, k, k2, diff, node_value, moves);
        }
        if (i < ROW - 1) {
            u8 k2 = k + (COL << 2);
            u8 node_value = (node.mat >> k2) & 15;
            u8 r = (row >> (node_value << 2)) & 15;
            int diff = abs(i - r) - abs(i + 1 - r);
            try_insert(q, node, k, k2, diff, node_value, moves);
        }
        if (j > 0) {
            u8 k2 = k - (1 << 2);
            u8 node_value = (node.mat >> k2) & 15;
            u8 c = (col >> (node_value << 2)) & 15;
            int diff = abs(j - c) - abs(j - 1 - c);
            try_insert(q, node, k, k2, diff, node_value, moves);
        }
        if (j < COL - 1) {
            u8 k2 = k + (1 << 2);
            u8 node_value = (node.mat >> k2) & 15;
            u8 c = (col >> (node_value << 2)) & 15;
            int diff = abs(j - c) - abs(j + 1 - c);
            try_insert(q, node, k, k2, diff, node_value, moves);
        }
    }
    return {0xff, cnt};
}

vector<ull> solution(u8 step, unordered_map<ull, memo>& moves) {
    vector<ull> stak;
    ull mat = 0;
    for (ull i = 1; i < SZ; i++) mat += (i << ((i - 1) << 2));
    stak.push_back(mat);
    for (u8 k = 60; step > 0; step--) {
        auto k0 = (moves[mat]).pos_of_0_prec;
        ull el = (mat >> k0) & 15;
        mat &= ~(15ull << k0);
        mat |= (el << k);
        stak.push_back(mat);
        k = k0;
    }
    reverse(stak.begin(), stak.end());
    return stak;
}

bool is_solvable(ull mat) {
    int inv_count = 0;
    for (u8 i = 0; i < SZ - 1; i++) {
        u8 val1 = (mat >> (i << 2)) & 15;
        if (!val1) continue;
        for (u8 j = i + 1; j < SZ; j++) {
            u8 val2 = (mat >> (j << 2)) & 15;
            inv_count += (val2 && val1 > val2);
        }
    }

    u8 idx_zero;
    for (idx_zero = 0; idx_zero < SZ; idx_zero++)
        if ((mat >> (idx_zero << 2) & 15) == 0) break;
    u8 row = idx_zero >> 2;

    if (row & 1) return !(inv_count & 1);
    else return inv_count & 1;
}

int main() {
    int n, m;
    cin >> n >> m;
    assert(n == ROW && m == COL);
    ull mat = 0;
    vector<bool> verif(SZ, false);
    for (int i = 0; i < SZ; i++) {
        ull x;
        cin >> x;
        assert(x < SZ && !verif[x]);
        verif[x] = true;
        mat += (x << (i << 2));
    }
    assert(is_solvable(mat));

    unordered_map<ull, memo> moves;
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    auto [step, processed] = a_star(mat, moves);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << (step == 0xff ? -1 : int(step)) << " moves found in " << duration.count() << "ms (" << processed << " different position processed)"
         << endl;

    if (step != 0xff) {
        auto sol = solution(step, moves);
        for (ull t = 0; t < sol.size(); t++) {
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
