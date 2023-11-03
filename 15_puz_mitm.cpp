// I DON'T KNOW IF IT'S BETTER THAN DEFAULT A* (it's faster on some cases, slower on others)
// DON'T KNOW IF IT ALWAYS GETS OPTIMAL RESULTS

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

using u64 = uint64_t;
using u8 = uint8_t;

#define SZ 16
#define ROW 4
#define COL 4

struct memo {
    u8 dist, heu, pos_of_0_prec;
    bool processed;
};

struct state {
    u8 dist, heu, istant_0_idx, type;
    u64 mat;
};

using pqq = priority_queue<state, vector<state>, std::greater<state>>;
using mapp = unordered_map<u64, memo>;

inline u8 heuristic(u64 mat, u64 col, u64 row) {
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

vector<u64> solution(u64 mid, mapp& moves1, mapp& moves2) {
    vector<u64> stak;
    stak.push_back(mid);
    u64 mat = mid;
    u8 idx, tmp;
    for (idx = 0; idx < SZ; idx++)
        if ((mid >> (idx << 2) & 15) == 0) break;
    tmp = (idx <<= 2);

    while (true) {
        auto idx_next = moves1[mat].pos_of_0_prec;
        if (idx_next == 0xff) break;
        u64 el = (mat >> idx_next) & 15;
        mat &= ~(15ull << idx_next);
        mat |= (el << idx);
        stak.push_back(mat);
        idx = idx_next;
    }
    reverse(stak.begin(), stak.end());
    idx = tmp, mat = mid;
    while (true) {
        auto idx_next = moves2[mat].pos_of_0_prec;
        if (idx_next == 0xff) break;
        u64 el = (mat >> idx_next) & 15;
        mat &= ~(15ull << idx_next);
        mat |= (el << idx);
        stak.push_back(mat);
        idx = idx_next;
    }
    return stak;
}

inline void try_insert(vector<state>& q, const state& node, u8 k, u8 k2, int diff, u8 node_value, u8 type, mapp& moves) {
    u64 new_mat = node.mat;
    new_mat -= (u64)node_value << k2;
    new_mat += (u64)node_value << k;
    u8 heu = node.heu + diff;
    u8 dist = node.dist + diff + 1;
    auto search = moves.find(new_mat);
    if (search == moves.end() || dist < (search->second).dist) {
        moves[new_mat] = {dist, heu, k, false};
        q.push_back({dist, heu, k2, type, new_mat});
    }
}

inline void process(u64& col, u64& row, vector<state>& q1, vector<state>& q2, state& node, mapp& moves) {
    u8 k = node.istant_0_idx, i = k >> 4, j = (k >> 2) & 3;
    if (i > 0) {
        u8 k2 = k - (COL << 2);
        u8 node_value = (node.mat >> k2) & 15;
        u8 r = (row >> (node_value << 2)) & 15;
        int diff = abs(i - r) - abs(i - 1 - r);
        try_insert((diff < 0 ? q1 : q2), node, k, k2, diff, node_value, node.type, moves);
    }
    if (i < ROW - 1) {
        u8 k2 = k + (COL << 2);
        u8 node_value = (node.mat >> k2) & 15;
        u8 r = (row >> (node_value << 2)) & 15;
        int diff = abs(i - r) - abs(i + 1 - r);
        try_insert((diff < 0 ? q1 : q2), node, k, k2, diff, node_value, node.type, moves);
    }
    if (j > 0) {
        u8 k2 = k - (1 << 2);
        u8 node_value = (node.mat >> k2) & 15;
        u8 c = (col >> (node_value << 2)) & 15;
        int diff = abs(j - c) - abs(j - 1 - c);
        try_insert((diff < 0 ? q1 : q2), node, k, k2, diff, node_value, node.type, moves);
    }
    if (j < COL - 1) {
        u8 k2 = k + (1 << 2);
        u8 node_value = (node.mat >> k2) & 15;
        u8 c = (col >> (node_value << 2)) & 15;
        int diff = abs(j - c) - abs(j + 1 - c);
        try_insert((diff < 0 ? q1 : q2), node, k, k2, diff, node_value, node.type, moves);
    }
}

pair<u64, vector<u64>> a_star(u64 start_mat, u64 end_mat) {
    u64 col1 = 0, row1 = 0, col2 = 0, row2 = 0;
    for (u64 i = 0; i < SZ; i++) {
        u8 val = (end_mat >> (i << 2)) & 15;
        col1 |= ((i & 3) << (val << 2));
        row1 |= ((i >> 2) << (val << 2));
    }
    for (u64 i = 0; i < SZ; i++) {
        u8 val = (start_mat >> (i << 2)) & 15;
        col2 |= ((i & 3) << (val << 2));
        row2 |= ((i >> 2) << (val << 2));
    }

    vector<state> q1, q2;
    u8 heu = heuristic(start_mat, col1, row1);
    u8 idx_zero1, idx_zero2;
    mapp moves1, moves2;

    for (idx_zero1 = 0; idx_zero1 < SZ; idx_zero1++)
        if ((start_mat >> (idx_zero1 << 2) & 15) == 0) break;
    q1.push_back({heu, heu, (u8)(idx_zero1 << 2), 1, start_mat});
    moves1[start_mat] = {heu, heu, 0xff, false};

    for (idx_zero2 = 0; idx_zero2 < SZ; idx_zero2++)
        if ((end_mat >> (idx_zero2 << 2) & 15) == 0) break;
    q1.push_back({heu, heu, (u8)(idx_zero2 << 2), 2, end_mat});
    moves2[end_mat] = {heu, heu, 0xff, false};

    u8 max_depth = 0xff, min_val = 0xff;
    u64 cnt = 0, mid = 0;
    while (true) {
        if (q1.empty()) swap(q1, q2);
        if (q1.empty()) break;
        state node = q1.back();
        if (node.dist > max_depth) break;
        q1.pop_back();
        auto type = node.type;
        auto search = (type == 1) ? moves1.find(node.mat) : moves2.find(node.mat);
        if ((search->second).processed) continue;
        (search->second).processed = true;
        cnt++;
        if (type == 1) {
            if (auto check = moves2.find(node.mat); check != moves2.end() && (check->second).processed) {
                if (max_depth == 0xff) max_depth = node.dist + 2;
                u8 tmp = (search->second).dist + (check->second).dist - (search->second).heu - (check->second).heu;
                if (tmp < min_val) {
                    min_val = tmp;
                    mid = node.mat;
                }
            }
            process(col1, row1, q1, q2, node, moves1);
        } else {
            if (auto check = moves1.find(node.mat); check != moves1.end() && (check->second).processed) {
                if (max_depth == 0xff) max_depth = node.dist + 2;
                u8 tmp = (search->second).dist + (check->second).dist - (search->second).heu - (check->second).heu;
                if (tmp < min_val) {
                    min_val = tmp;
                    mid = node.mat;
                }
            }
            process(col2, row2, q1, q2, node, moves2);
        }
    }
    assert(mid);
    auto step = solution(mid, moves1, moves2);
    return {cnt, step};
}

bool is_solvable(u64 mat) {
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

void print_solution(vector<u64>& sol, bool erase = true) {
    for (u64 t = 0; t < sol.size(); t++) {
        for (int i = 0; i < SZ; i++) {
            u8 el = (sol[t] >> (i << 2)) & 15;
            if (el) cout << setw(3) << (int)el << " ";
            else cout << "    ";
            if (i % COL == COL - 1) cout << endl;
        }
        if (erase) {
            if (t < sol.size() - 1)
                for (int i = 0; i < ROW; i++) cout << "\x1b[A";
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        } else cout << endl;
    }
}

int main() {
    int n, m;
    cin >> n >> m;
    assert(n == ROW && m == COL);
    u64 mat = 0;
    vector<bool> verif(SZ, false);
    for (int i = 0; i < SZ; i++) {
        u64 x;
        cin >> x;
        assert(x < SZ && !verif[x]);
        verif[x] = true;
        mat += (x << (i << 2));
    }
    assert(is_solvable(mat));

    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    u64 sol = 0x0fedcba987654321;
    auto [processed, step] = a_star(mat, sol);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << step.size() - 1 << " moves found in " << duration.count() << "ms (" << processed << " different position processed)" << endl;

    // print_solution(step);
    return 0;
}
