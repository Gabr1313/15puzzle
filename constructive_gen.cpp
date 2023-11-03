#include <bits/stdc++.h>
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

int main() {
    int n, m, p, t;
    cin >> n >> m, p = n * m, t = p * p /* * p */;
    assert(p > 0 && p <= 255);
    vector<int> mat(p);
    for (int i = 1; i < p; i++) mat[i - 1] = i;
    srand(time(0));
    int i = n - 1, j = m - 1;
    for (int tt = 0; tt < t; tt++) {
        int r = rand() % 4;
        switch (r) {
            case 0:
                if (i > 0) swap(mat[i * m + j], mat[(i - 1) * m + j]), i--;
                else tt--;
                break;
            case 1:
                if (i < n - 1) swap(mat[i * m + j], mat[(i + 1) * m + j]), i++;
                else tt--;
                break;
            case 2:
                if (j > 0) swap(mat[i * m + j], mat[i * m + j - 1]), j--;
                else tt--;
                break;
            case 3:
                if (j < m - 1) swap(mat[i * m + j], mat[i * m + j + 1]), j++;
                else tt--;
                break;
        }
    }
    cout << n << " " << m ;
    for (int i = 0; i < p; i++) {
        if (i % m == 0) cout << endl;
        cout << setw(3) << mat[i] << " ";
    }
    cout << endl;
    return 0;
}
