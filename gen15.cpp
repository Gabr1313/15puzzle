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

bool is_solvable(vector<u8>& v) {
    int inv_count = 0;
    for (u8 i = 0; i < 15; i++) {
        if (!v[i]) continue;
        for (u8 j = i + 1; j < 16; j++) {
            inv_count += (v[j] && v[i] > v[j]);
        }
    }

    u8 idx_zero;
    for (idx_zero = 0; idx_zero < 16; idx_zero++)
        if (!v[idx_zero]) break;
    u8 row = idx_zero >> 2;

    if (row & 1) return !(inv_count & 1);
    else return inv_count & 1;
}

int main() {
    vector<u8> v(16);
    u8 cnt = 0;
    for (auto& x : v) x = cnt++;
    do {
        shuffle(v.begin(), v.end(), default_random_engine(time(0)));
    } while (!is_solvable(v));
    cout << "4 4" << endl;
    for (int i = 0; i < 16; i++) {
        cout << setw(3) << (int)v[i] << " ";
        if (i % 4 == 3) cout << endl;
    }
}
