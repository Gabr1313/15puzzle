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

#define dbg_mat(mat) \
    for (u8 k = 0; k < SZ;) dbg((mat >> (k++ << 2)) & 15, (mat >> (k++ << 2)) & 15, (mat >> (k++ << 2)) & 15, (mat >> (k++ << 2)) & 15);
#else
#define dbg(...) 42
#define dbg_mat(...) 42
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

struct table_memo {
    u8 dist, blank;
    u64 state;
};

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

inline u8 walking_distance(u64 mat, u64 map_r, u64 map_c, table& tab_row, table& tab_col) {
    for (u8 i = 0; i < 4; i++) fast_sort(map_r, i), fast_sort(map_c, i);
    return tab_row[map_r] + tab_col[map_c];
}

table tab_row, tab_col;
unordered_set<u64> visited, snapshot;
u64 finish, vis, max_step;
u8 max_dist;

struct state {
    u64 mat, map_r, map_c;
    u8 blank, heu, dist;
};

inline state do_things(state st, u8 new_blank) {
    u8 conv_transpose[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};
    u64 node_value = (st.mat >> (new_blank << 2)) & 15;
    u64 new_mat = st.mat;
    new_mat -= node_value << (new_blank << 2);
    new_mat += node_value << (st.blank << 2);

    u64 node_value_r = (st.map_r >> (new_blank << 2)) & 15;
    u64 new_map_r = st.map_r;
    new_map_r -= node_value_r << (new_blank << 2);
    new_map_r += node_value_r << (st.blank << 2);

    u64 node_value_c = (st.map_c >> (conv_transpose[new_blank] << 2)) & 15;
    u64 new_map_c = st.map_c;
    new_map_c -= node_value_c << (conv_transpose[new_blank] << 2);
    new_map_c += node_value_c << (conv_transpose[st.blank] << 2);

    u8 new_heu = walking_distance(new_mat, new_map_r, new_map_c, tab_row, tab_col);
    u8 new_dist = st.dist - st.heu + new_heu + 1;
    return {new_mat, new_map_r, new_map_c, new_blank, new_heu, new_dist};
}

bool recur(state& st) {
    if (st.dist >= max_dist || visited.count(st.mat)) return false;
    if (st.mat == finish) {
        if (st.dist < max_dist) {
            max_dist = st.dist;
            snapshot = visited;
        }
        return true;
    }
    visited.insert(st.mat);
    if (++vis > max_step) return false;
    state states[4];
    bool todo[4] = {false, false, false, false};
    u8 i = st.blank >> 2, j = st.blank & 3;
    if (i > 0) {
        u8 new_blank = st.blank - COL;
        auto new_state = do_things(st, new_blank);
        // recur(new_state);
        states[0] = new_state;
        todo[0] = true;
    }
    if (i < ROW - 1) {
        u8 new_blank = st.blank + COL;
        auto new_state = do_things(st, new_blank);
        // recur(new_state);
        states[1] = new_state;
        todo[1] = true;
    }
    if (j > 0) {
        u8 new_blank = st.blank - 1;
        auto new_state = do_things(st, new_blank);
        // recur(new_state);
        states[2] = new_state;
        todo[2] = true;
    }
    if (j < COL - 1) {
        u8 new_blank = st.blank + 1;
        auto new_state = do_things(st, new_blank);
        // recur(new_state);
        states[3] = new_state;
        todo[3] = true;
    }

    for (u8 k = 0; k < 4; k++)
        if (todo[k] && states[k].dist == st.dist)
            if (recur(states[k])) return true;
    for (u8 k = 0; k < 4; k++)
        if (todo[k] && states[k].dist != st.dist)
            if (recur(states[k])) return true;
    visited.erase(st.mat);
    return false;
}

u64 ida_star_bsearch(u64 start, u64 fin, u64 max_step_per_search) {
    finish = fin, max_step = max_step_per_search;
    auto map_r = map_row(start), map_c = map_col(start);
    tab_row = generate_table(map_row(finish));
    tab_col = generate_table(map_col(finish));
    auto heu = walking_distance(start, map_r, map_c, tab_row, tab_col);
    u8 blank;
    for (blank = 0; blank < SZ; blank++)
        if ((start >> (blank << 2) & 15) == 0) break;
    state st = {start, map_r, map_c, blank, heu, heu};

    u64 total_cnt = 0;

    max_dist = 1;
    while (!recur(st)) {
        max_dist *= 2;
        vis = 0;
        visited.clear();
    }

    u8 lo = max_dist >> 1, hi = max_dist;
    while (lo != hi) {
        u8 mid = (lo + hi) >> 1;
        max_dist = mid;
        vis = 0;
        visited.clear();
        if (recur(st)) hi = mid;
        else lo = mid + 1;
        total_cnt += vis;
    }

    return total_cnt;
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

vector<u64> construct_solution(u64 start, u64 finish) {
    snapshot.insert(finish);
    vector<u64> poss, sol;
    poss.push_back(start);
    while (snapshot.size()) {
        u8 k;
        for (k = 0; k < poss.size() - 1; k++)
            if (snapshot.count(poss[k])) break;

        u64 mat = poss[k];
        poss.clear();
        sol.push_back(mat);
        snapshot.erase(mat);
        u8 blank;
        for (blank = 0; blank < SZ; blank++)
            if ((mat >> (blank << 2) & 15) == 0) break;
        u8 i = blank >> 2, j = blank & 3;
        if (i > 0) {
            u8 new_blank = blank - COL;
            u64 node_value = (mat >> (new_blank << 2)) & 15;
            u64 new_mat = mat;
            new_mat -= node_value << (new_blank << 2);
            new_mat += node_value << (blank << 2);
            poss.push_back(new_mat);
        }
        if (i < ROW - 1) {
            u8 new_blank = blank + COL;
            u64 node_value = (mat >> (new_blank << 2)) & 15;
            u64 new_mat = mat;
            new_mat -= node_value << (new_blank << 2);
            new_mat += node_value << (blank << 2);
            poss.push_back(new_mat);
        }
        if (j > 0) {
            u8 new_blank = blank - 1;
            u64 node_value = (mat >> (new_blank << 2)) & 15;
            u64 new_mat = mat;
            new_mat -= node_value << (new_blank << 2);
            new_mat += node_value << (blank << 2);
            poss.push_back(new_mat);
        }
        if (j < COL - 1) {
            u8 new_blank = blank + 1;
            u64 node_value = (mat >> (new_blank << 2)) & 15;
            u64 new_mat = mat;
            new_mat -= node_value << (new_blank << 2);
            new_mat += node_value << (blank << 2);
            poss.push_back(new_mat);
        }
    }
    return sol;
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

    using namespace std::chrono;
    auto start_time = high_resolution_clock::now();
    auto processed = ida_star_bsearch(mat, finish, 10e6);
    auto stop_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop_time - start_time);
    cout << snapshot.size() << " moves found in " << duration.count() << "ms (" << processed << " position processed)" << endl;

    auto sol = construct_solution(mat, finish);
    print_solution(sol);
    return 0;
}
