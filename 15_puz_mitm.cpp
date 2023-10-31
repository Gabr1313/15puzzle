// MEET IN THE MIDDLE GETS WRONG RESULTS!

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
using vu8 = vector<u8>;
using pii = pair<int, int>;

#define SZ 16
#define ROW 4
#define COL 4

// mat, dist, pos_of_0_prec, processed
using mapp = unordered_map<ull, tuple<u8, u8, bool>>;
using state = tuple<u8, u8, u8, ull>;
using pqq = priority_queue<state, vector<state>, std::greater<state>>;
mapp moves1;
mapp moves2;

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

vector<ull> solution(ull mid) {
    vector<ull> stak;
    stak.push_back(mid);
    ull mat = mid;
    u8 idx, tmp;
    for (idx = 0; idx < SZ; idx++)
        if ((mid >> (idx << 2) & 15) == 0) break;
    tmp = (idx <<= 2);

    while (true) {
        auto idx_next = get<1>(moves1[mat]);
        if (idx_next == 0xff) break;
        ull el = (mat >> idx_next) & 15;
        mat &= ~(15ull << idx_next);
        mat |= (el << idx);
        stak.push_back(mat);
        idx = idx_next;
    }
    reverse(stak.begin(), stak.end());
    idx = tmp, mat = mid;
    while (true) {
        auto idx_next = get<1>(moves2[mat]);
        if (idx_next == 0xff) break;
        ull el = (mat >> idx_next) & 15;
        mat &= ~(15ull << idx_next);
        mat |= (el << idx);
        stak.push_back(mat);
        idx = idx_next;
    }
    return stak;
}

inline void try_insert(pqq& q, const state& node, u8 k, u8 k2, int diff, u8 node_value, mapp& moves) {
    ull new_mat = get<3>(node);
    new_mat -= (ull)node_value << k2;
    new_mat += (ull)node_value << k;
    int heu = get<1>(node) + diff;
    int dist = get<0>(node) + diff + 1;
    auto search = moves.find(new_mat);
    if (search == moves.end() || dist < get<0>(search->second)) {
        moves[new_mat] = {dist, k, false};
        q.push({dist, heu, k2, new_mat});
    }
}

inline void process(ull& col, ull& row, pqq& q, state& node, mapp& moves) {
    u8 k = get<2>(node), i = k >> 4, j = (k >> 2) & 3;
    if (i > 0) {
        u8 k2 = k - (COL << 2);
        u8 node_value = (get<3>(node) >> k2) & 15;
        u8 r = (row >> (node_value << 2)) & 15;
        int diff = abs(i - r) - abs(i - 1 - r);
        try_insert(q, node, k, k2, diff, node_value, moves);
    }
    if (i < ROW - 1) {
        u8 k2 = k + (COL << 2);
        u8 node_value = (get<3>(node) >> k2) & 15;
        u8 r = (row >> (node_value << 2)) & 15;
        int diff = abs(i - r) - abs(i + 1 - r);
        try_insert(q, node, k, k2, diff, node_value, moves);
    }
    if (j > 0) {
        u8 k2 = k - (1 << 2);
        u8 node_value = (get<3>(node) >> k2) & 15;
        u8 c = (col >> (node_value << 2)) & 15;
        int diff = abs(j - c) - abs(j - 1 - c);
        try_insert(q, node, k, k2, diff, node_value, moves);
    }
    if (j < COL - 1) {
        u8 k2 = k + (1 << 2);
        u8 node_value = (get<3>(node) >> k2) & 15;
        u8 c = (col >> (node_value << 2)) & 15;
        int diff = abs(j - c) - abs(j + 1 - c);
        try_insert(q, node, k, k2, diff, node_value, moves);
    }
}

pair<ull, vector<ull>> a_star(ull start_mat, ull sol) {
    // swap(solution, start_mat);
    ull col1 = 0, row1 = 0, col2 = 0, row2 = 0;
    for (ull i = 0; i < SZ; i++) {
        u8 val = (sol >> (i << 2)) & 15;
        col1 |= ((i & 3) << (val << 2));
        row1 |= ((i >> 2) << (val << 2));
    }
    for (ull i = 0; i < SZ; i++) {
        u8 val = (start_mat >> (i << 2)) & 15;
        col2 |= ((i & 3) << (val << 2));
        row2 |= ((i >> 2) << (val << 2));
    }

    pqq q1, q2;
    u8 heu1 = heuristic(start_mat, col1, row1), heu2 = heuristic(sol, col2, row2);
    u8 idx_zero1, idx_zero2;

    for (idx_zero1 = 0; idx_zero1 < SZ; idx_zero1++)
        if ((start_mat >> (idx_zero1 << 2) & 15) == 0) break;
    q1.push({heu1, heu1, idx_zero1 << 2, start_mat});
    moves1[start_mat] = {0, 0xff, false};

    for (idx_zero2 = 0; idx_zero2 < SZ; idx_zero2++)
        if ((sol >> (idx_zero2 << 2) & 15) == 0) break;
    q2.push({heu2, heu2, idx_zero2 << 2, sol});
    moves2[sol] = {0, 0xff, false};

    ull cnt = 0, mid = 0;
    while (!q1.empty() && !q2.empty()) {
        state node = q1.top();
        q1.pop();

        auto search = moves1.find(get<3>(node));
        if (!get<2>(search->second)) {
            get<2>(search->second) = true;
            if (auto check = moves2.find(get<3>(node)); check != moves2.end() && get<2>(check->second)) {
                mid = get<3>(node);
                break;
            }
            // if (get<1>(node) == 0) return cnt;
            cnt++;
            process(col1, row1, q1, node, moves1);
        }

        node = q2.top();
        q2.pop();

        search = moves2.find(get<3>(node));
        if (!get<2>(search->second)) {
            get<2>(search->second) = true;
            if (auto check = moves1.find(get<3>(node)); check != moves1.end() && get<2>(check->second)) {
                mid = get<3>(node);
                break;
            }
            // if (get<1>(node) == 0) return cnt;
            cnt++;
            process(col1, row1, q2, node, moves2);
        }
    }
    assert(mid);

    auto step = solution(mid);
    return {cnt, step};
    // return {cnt, {}};
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

    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    ull sol = 0x0fedcba987654321;
    auto [processed, step] = a_star(mat, sol);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << step.size() - 1 << " moves found in " << duration.count() << "ms (" << processed << " different position processed)" << endl;

    for (ull t = 0; t < step.size(); t++) {
        for (int i = 0; i < SZ; i++) {
            u8 el = (step[t] >> (i << 2)) & 15;
            if (el) cout << setw(3) << (int)el << " ";
            else cout << "    ";
            if (i % 4 == 3) cout << endl;
        }
        {
            // cout << endl;
            if (t < step.size() - 1)
                for (int i = 0; i < n; i++) cout << "\x1b[A";
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    } 

    return 0;
}
