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
using u64 = unsigned long long;

#define SZ 16
#define ROW 4
#define COL 4

struct memo {
    u8 dist, pos_of_0_prec;
    bool processed;
};

struct state {
    u8 dist, heu, istant_0_idx;
    u64 mat;
    bool operator>(const state& other) const {
        if (dist == other.dist) return heu > other.heu;
        return dist > other.dist;
    }
};

using pq = priority_queue<state, vector<state>, std::greater<state>>;
using hm = unordered_map<u64, memo>;

inline u8 manhattan(u64 mat, u64 col, u64 row) {
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

inline void try_insert(pq& q, const state& node, u8 k, u8 k2, hm& moves, u64 col, u64 row) {
    u8 node_value = (node.mat >> k2) & 15;
    u64 new_mat = node.mat;
    new_mat -= (u64)node_value << k2;
    new_mat += (u64)node_value << k;
    u8 heu = manhattan(new_mat, col, row);
    u8 dist = node.dist - node.heu + heu + 1;
    auto search = moves.find(new_mat);
    if (search == moves.end() || dist < (search->second).dist) {
        moves[new_mat] = {dist, k, false};
        q.push({dist, heu, k2, new_mat});
    }
}

u64 a_star(u64 start, u64 finish, hm& moves) {
    u64 col = 0, row = 0;
    for (u64 i = 0; i < SZ; i++) {
        u8 val = (finish >> (i << 2)) & 15;
        col |= ((i & 3) << (val << 2));
        row |= ((i >> 2) << (val << 2));
    }

    pq q;
    u8 heu = manhattan(start, col, row);
    u8 idx_zero;
    for (idx_zero = 0; idx_zero < SZ; idx_zero++)
        if ((start >> (idx_zero << 2) & 15) == 0) break;
    q.push({heu, heu, (u8)(idx_zero << 2), start});
    moves[start] = {0, 0xff, false};

    u64 cnt = 0;
    while (!q.empty()) {
        auto node = q.top();
        q.pop();
        auto search = moves.find(node.mat);
        if ((search->second).processed) continue;
        else (search->second).processed = true;
        cnt++;
        if (node.mat == finish) return cnt;
        u8 k = node.istant_0_idx, i = k >> 4, j = (k >> 2) & 3;
        if (i > 0) {
            u8 k2 = k - (COL << 2);
            try_insert(q, node, k, k2, moves, col, row);
        }
        if (i < ROW - 1) {
            u8 k2 = k + (COL << 2);
            try_insert(q, node, k, k2, moves, col, row);
        }
        if (j > 0) {
            u8 k2 = k - (1 << 2);
            try_insert(q, node, k, k2, moves, col, row);
        }
        if (j < COL - 1) {
            u8 k2 = k + (1 << 2);
            try_insert(q, node, k, k2, moves, col, row);
        }
    }
    assert(false);
}

vector<u64> get_solution(u64 start, u64 end, hm& moves) {
    vector<u64> stak;
    stak.push_back(end);
    u64 mat = end;
    u8 idx, tmp;
    for (idx = 0; idx < SZ; idx++)
        if ((end >> (idx << 2) & 15) == 0) break;
    tmp = (idx <<= 2);

    while (mat != start) {
        auto idx_next = moves[mat].pos_of_0_prec;
        u64 el = (mat >> idx_next) & 15;
        mat &= ~(15ull << idx_next);
        mat |= (el << idx);
        stak.push_back(mat);
        idx = idx_next;
    }
    reverse(stak.begin(), stak.end());
    return stak;
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
    u64 finish = 0x0fedcba987654321;

    hm moves;
    using namespace std::chrono;
    auto start_time = high_resolution_clock::now();
    auto processed = a_star(mat, finish, moves);
    auto stop_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop_time - start_time);
    auto sol = get_solution(mat, finish, moves);
    cout << sol.size() - 1 << " moves found in " << duration.count() << "ms (" << processed << " different positions processed)" << endl;

    print_solution(sol);
    return 0;
}
