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

// using u8 = int;
// TODO
using u8 = unsigned char;
using vu8 = vector<u8>;
using pii = pair<int, int>;

struct MatrixHasher {
    int operator()(const vu8& mat) const {
        unsigned int hash = mat.size();
        for (auto i : mat) hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

unordered_map<vu8, pair<int, u8>, MatrixHasher> moves;

int heuristic(vu8& mat, u8 row, u8 col) {
    int sum = 0;
    for (u8 i = 0, k = 0; i < row; i++) {
        for (u8 j = 0; j < col; j++, k++) {
            if (mat[k] == 0) continue;
            int el = mat[k] - 1;
            sum += abs(i - el / col) + abs(j - el % col);
        }
    }
    return sum;
}

// dist, heu, pos_of_0, mat
using state = tuple<int, int, u8, vu8>;
struct myComp {
    constexpr bool operator()(state const& a, state const& b) const noexcept { return get<0>(a) > get<0>(b); }
};

void try_insert(priority_queue<state, vector<state>, myComp>& q, const state& node, u8 k, u8 k2, int diff) {
    auto new_mat = get<3>(node);
    swap(new_mat[k], new_mat[k2]);
    int heu = get<1>(node) + diff;
    int dist = get<0>(node) + diff + 1;
    auto search = moves.find(new_mat);
    if (search == moves.end() || dist < search->second.first) {
        moves[new_mat] = {dist, k};
        q.push({dist, heu, k2, new_mat});
    }
}

int a_star(vu8& start, u8 row, u8 col) {
    priority_queue<state, vector<state>, myComp> q;
    {
        int heu = heuristic(start, row, col);
        int k;
        for (k = 0; k < start.size(); k++)
            if (start[k] == 0) break;
        q.push({heu, heu, k, start});
        moves[start] = {0, k};
    }

    while (!q.empty()) {
        auto node = q.top();
        q.pop();
        if (get<1>(node) == 0) return get<0>(node);

        u8 k = get<2>(node), i = k / col, j = k % col;
        if (i > 0) {
            // dbg("up");
            u8 k2 = k - col;
            int diff = abs(i - (get<3>(node)[k2] - 1) / col) - abs(i - 1 - (get<3>(node)[k2] - 1) / col);
            try_insert(q, node, k, k2, diff);
        }
        if (i < row - 1) {
            // dbg("down");
            u8 k2 = k + col;
            int diff = abs(i - (get<3>(node)[k2] - 1) / col) - abs(i + 1 - (get<3>(node)[k2] - 1) / col);
            try_insert(q, node, k, k2, diff);
        }
        if (j > 0) {
            // dbg("left");
            u8 k2 = k - 1;
            int diff = abs(j - (get<3>(node)[k2] - 1) % col) - abs(j - 1 - (get<3>(node)[k2] - 1) % col);
            try_insert(q, node, k, k2, diff);
        }
        if (j < col - 1) {
            // dbg("right");
            u8 k2 = k + 1;
            int diff = abs(j - (get<3>(node)[k2] - 1) % col) - abs(j + 1 - (get<3>(node)[k2] - 1) % col);
            try_insert(q, node, k, k2, diff);
        }
    }
    return -1;
}

vector<vector<u8>> solution(int row, int col, int step) {
    int k = row * col;
    vector<u8> mat(k);
    vector<vector<u8>> stak;
    for (int i = 1; i < k; i++) mat[i - 1] = i;
    stak.push_back(mat);
    for (k--; step > 0; step--) {
        auto [_, k0] = moves[mat];
        swap(mat[k], mat[k0]);
        stak.push_back(mat);
        k = k0;
    }
    reverse(stak.begin(), stak.end());
    return stak;
}

int main() {
    int n, m, k;
    cin >> n >> m;
    k = n * m;
    assert(k > 0 && k <= 255);
    vector mat(k, (u8)0);
    vector<bool> verif(n * m, false);
    for (auto& el : mat) {
        int x;
        cin >> x;
        assert(x < k && !verif[x]);
        el = (u8)x;
        verif[x] = true;
    }

    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    int step = a_star(mat, n, m);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << step << " moves found in " << duration.count() << "ms" << endl;

    cout << step << endl;
    if (step != -1) {
        auto sol = solution(n, m, step);
        for (int t = 0; t < sol.size(); t++) {
            for (int i = 0, k = 0; i < n; i++) {
                for (int j = 0; j < m; j++, k++)
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
