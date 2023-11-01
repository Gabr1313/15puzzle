#include <bits/stdc++.h>

#include <cassert>
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
using vi = vector<int>;

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
        return heu > other.heu;
        /* if (dist == other.dist) return heu > other.heu;
        return dist > other.dist; */
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
    if ((search == moves.end() || dist < (search->second).dist) && !moves[new_mat].processed) {
        moves[new_mat] = {dist, k, false};
        q.push({dist, heu, k2, new_mat});
    }
}

// write a better one
// first 1 2 3 4
// than 5 9 13
// than 6 7 8
// than 10 11 12
// than 14 15
ull get_a_solution(ull start_mat, ull sol, unordered_map<ull, memo>& moves) {
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
        if (node.heu == 0) return cnt;
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
    return cnt;
}

vector<ull> solution(ull start, ull end, unordered_map<ull, memo>& moves) {
    vector<ull> stak;
    stak.push_back(end);
    ull mat = end;
    u8 idx, tmp;
    for (idx = 0; idx < SZ; idx++)
        if ((end >> (idx << 2) & 15) == 0) break;
    tmp = (idx <<= 2);

    while (mat != start) {
        auto idx_next = moves[mat].pos_of_0_prec;
        ull el = (mat >> idx_next) & 15;
        mat &= ~(15ull << idx_next);
        mat |= (el << idx);
        stak.push_back(mat);
        idx = idx_next;
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

const double BOLTZMAN = 0.01;
const double INIT_TEMP = 1;

const double COOLIGN_FRACTION = 0.97;  // up-down together
const ull COOLIGN_STEP = 50 * 2;            // up-down together
const ull STEP_PER_TEMP = 100 * 2;          // up-down together
/* const double COOLIGN_FRACTION = 0.97;  // up-down together
const ull COOLIGN_STEP = 500;          // up-down together
const ull STEP_PER_TEMP = 1000;        // up-down together */
// const double COOLIGN_FRACTION = 0.999;  // up-down together
// const ull COOLIGN_STEP = 5000;          // up-down together
// const ull STEP_PER_TEMP = 10000;        // up-down together

pair<ull, vector<ull>> anneal_tsp(ull start_mat) {
    unordered_map<ull, memo> moves;
    ull sol = 0x0fedcba987654321;
    auto processed = get_a_solution(start_mat, sol, moves);
    auto v = solution(start_mat, sol, moves);
    srand(time(NULL));
    double temperature = INIT_TEMP;
    for (int i = 0; i < COOLIGN_STEP; i++) {
        int start_value = v.size();
        double intial_temperature = temperature;
        temperature *= COOLIGN_FRACTION;
        for (int j = 0; j < STEP_PER_TEMP; j++) {
            int k = rand() % v.size();
            moves.clear();
            auto new_processed = get_a_solution(v[k], sol, moves);
            processed += new_processed;
            auto new_tail = solution(v[k], sol, moves);
            int cost = new_tail.size() + k - v.size();
            if (cost < 0) {
                for (int i = k, tmp = v.size(); i < tmp; i++) v.pop_back();
                v.insert(v.end(), new_tail.begin(), new_tail.end());
                continue;
            }
            double e = -cost / (v.size() * BOLTZMAN * temperature);
            double merit = exp(e);
            float flip = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            if (merit > flip) {
                for (int i = k, tmp = v.size(); i < tmp; i++) v.pop_back();
                v.insert(v.end(), new_tail.begin(), new_tail.end());
            }
        }
        if (v.size() - start_value < 0) temperature = intial_temperature;
    }
    return {processed, v};
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

    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    auto [processed, sol] = anneal_tsp(mat);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << sol.size() - 1 << " moves found in " << duration.count() << "ms (" << processed << " position processed)" << endl;

    /* for (ull t = 0; t < sol.size(); t++) {
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
    } */

    return 0;
}
