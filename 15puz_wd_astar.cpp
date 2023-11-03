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

using u8 = uint8_t;
using u16 = uint16_t;
using u64 = uint64_t;

#define SZ 16
#define ROW 4
#define COL 4

struct memo {
    u8 dist, blank_prec;
    bool processed;
};

struct state {
    u8 dist, heu, blank;
    u64 mat;
    bool operator>(const state& other) const {
        if (dist == other.dist) return heu > other.heu;
        return dist > other.dist;
    }
};

struct table_memo {
    u8 dist, blank;
    u64 state;
};

using pq = priority_queue<state, vector<state>, std::greater<state>>;
using hm = unordered_map<u64, memo>;
using table = unordered_map<u64, u8>;

unordered_map<u16, u16> sort_table;
inline void fast_sort(u64& v, u8 i) {
    u16 row = (v >> (i << 4)) & 0xffff;
    v &= ~(u64(0xffff) << (i << 4));
    if (auto ptr = sort_table.find(row); ptr != sort_table.end()) {
        v += (u64(ptr->second) << (i << 4));
        return;
    }
    auto old_row = row;
    for (u8 k = 0; k < 4; k++) {
        for (u8 j = 0; j < 3 - k; j++) {
            u64 el1 = (row >> (j << 2)) & 15;
            u64 el2 = (row >> ((j + 1) << 2)) & 15;
            if (el1 > el2) {
                row &= ~(15 << (j << 2));
                row &= ~(15 << ((j + 1) << 2));
                row += el1 << ((j + 1) << 2);
                row += el2 << (j << 2);
            }
        }
    }
    sort_table[old_row] = row;
    v += (u64(row) << (i << 4));
}

u64 transpose(u64 mat) {
    for (u8 i = 1; i < ROW; i += 1) {
        for (u8 j = 0; j < i; j++) {
            u64 el1 = (mat >> ((i * 4 + j) << 2)) & 15;
            u64 el2 = (mat >> ((j * 4 + i) << 2)) & 15;
            mat -= el1 << ((i * 4 + j) << 2);
            mat -= el2 << ((j * 4 + i) << 2);
            mat += el1 << ((j * 4 + i) << 2);
            mat += el2 << ((i * 4 + j) << 2);
        }
    }
    return mat;
}

u64 map_row(u64 mat) {
    for (u8 i = 0; i < SZ; i++) {
        auto el = (mat >> (i << 2)) & 15;
        if (el != 0) mat += (-el << (i << 2)) + (((el + 3) >> 2) << (i << 2));
    }
    return mat;
}

u64 map_col(u64 mat) {
    mat = transpose(mat);
    for (u8 i = 0; i < SZ; i++) {
        auto el = (mat >> (i << 2)) & 15;
        auto diff = (!(el & 3) && el != 0) ? 4 : 0;
        mat += (-el << (i << 2)) + (((el & 3) + diff) << (i << 2));
    }
    return mat;
}

table generate_table(u64 start) {
    u8 blank_row;
    for (u8 i = 0; i < COL; i++) {
        fast_sort(start, i);
        if ((start >> (i << 4) & 15) == 0) blank_row = i;
    }

    deque<table_memo> q;
    table tab;
    q.push_back({0, blank_row, start});
    tab[start] = 0;
    while (!q.empty()) {
        auto [dist, blank_row, v] = q.front();
        q.pop_front();
        if (blank_row > 0) {
            for (u8 k = ((blank_row - 1) << 2); k < (blank_row << 2); k++) {
                u64 el = (v >> (k << 2)) & 15;
                u64 new_v = v;
                new_v -= el << (k << 2);
                new_v += el << (blank_row << 4);
                fast_sort(new_v, blank_row);
                fast_sort(new_v, blank_row - 1);
                if (tab.find(new_v) == tab.end()) {
                    tab[new_v] = dist + 1;
                    q.push_back({u8(dist + 1), u8(blank_row - 1), new_v});
                }
            }
        }
        if (blank_row < ROW - 1) {
            for (u8 k = ((blank_row + 1) << 2); k < ((blank_row + 2) << 2); k++) {
                u64 el = (v >> (k << 2)) & 15;
                u64 new_v = v;
                new_v -= el << (k << 2);
                new_v += el << (blank_row << 4);
                fast_sort(new_v, blank_row);
                fast_sort(new_v, blank_row + 1);
                if (tab.find(new_v) == tab.end()) {
                    tab[new_v] = dist + 1;
                    q.push_back({u8(dist + 1), u8(blank_row + 1), new_v});
                }
            }
        }
    }
    return tab;
}

inline u8 walking_distance(u64 mat, table& tab_row, table& tab_col) {
    u64 tmp1 = map_row(mat), tmp2 = map_col(mat);
    for (u8 i = 0; i < COL; i++) fast_sort(tmp1, i), fast_sort(tmp2, i);
    return tab_row[tmp1] + tab_col[tmp2];
}

inline void try_insert(pq& q, const state& node, u8 k, u8 k2, hm& moves, table& tab_row, table& tab_col) {
    u8 node_value = (node.mat >> k2) & 15;
    u64 new_mat = node.mat;
    new_mat -= (u64)node_value << k2;
    new_mat += (u64)node_value << k;
    u8 heu = walking_distance(new_mat, tab_row, tab_col);
    u8 dist = node.dist - node.heu + heu + 1;
    auto search = moves.find(new_mat);
    if (search == moves.end() || dist < (search->second).dist) {
        moves[new_mat] = {dist, k, false};
        q.push({dist, heu, k2, new_mat});
    }
}

u64 a_star(u64 start, u64 finish, hm& moves, table& tab_row, table& tab_col) {
    pq q;
    u8 heu = walking_distance(start, tab_row, tab_col);
    u8 blank_start;
    for (blank_start = 0; blank_start < SZ; blank_start++)
        if ((start >> (blank_start << 2) & 15) == 0) break;
    blank_start <<= 2;
    q.push({heu, heu, blank_start, start});
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
        u8 k = node.blank, i = k >> 4, j = (k >> 2) & 3;
        if (i > 0) {
            u8 k2 = k - (COL << 2);
            try_insert(q, node, k, k2, moves, tab_row, tab_col);
        }
        if (i < ROW - 1) {
            u8 k2 = k + (COL << 2);
            try_insert(q, node, k, k2, moves, tab_row, tab_col);
        }
        if (j > 0) {
            u8 k2 = k - (1 << 2);
            try_insert(q, node, k, k2, moves, tab_row, tab_col);
        }
        if (j < COL - 1) {
            u8 k2 = k + (1 << 2);
            try_insert(q, node, k, k2, moves, tab_row, tab_col);
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
        auto idx_next = moves[mat].blank_prec;
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
    table tab_row = generate_table(map_row(finish));
    table tab_col = generate_table(map_col(finish));
    hm moves;

    using namespace std::chrono;
    auto start_time = high_resolution_clock::now();
    auto processed = a_star(mat, finish, moves, tab_row, tab_col);
    auto stop_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop_time - start_time);
    auto sol = get_solution(mat, finish, moves);
    cout << sol.size() - 1 << " moves found in " << duration.count() << "ms (" << processed << " different positions processed)" << endl;

    print_solution(sol);
    return 0;
}
