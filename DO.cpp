#include <iostream>
#include <vector>
using namespace std;

typedef long long ll;
typedef long double ld;

template <typename T>
struct segTree {
public:
    segTree(int sz): sz(sz) {
        tree.resize(4*sz);
    };
    segTree(vector<T> &a) {
        sz = (int)a.size();
        tree.resize(4*sz);
        build(a, 0, 0, sz);
    }

    void upd(int id, T val) {
        upd(id, val, 0, 0, sz);
    }

    T get(int l, int r) {
        return get(l, r, 0, 0, sz);
    }

private:
    int sz;
    vector<T> tree;

    void build(vector<T> &a, int v, int tl, int tr) {
        if (tl + 1 == tr) {
            tree[v] = a[tl];
            return;
        }
        int m = (tl + tr) / 2;
        build(a, 2*v + 1, tl, m);
        build(a, 2*v + 2, m, tr);
        tree[v] = tree[2*v + 1] + tree[2*v + 2];
    }

    void upd(int id, T val, int v, int tl, int tr) {
        if (tl + 1 == tr) {
            tree[v] = val;
            return;
        }
        int m = (tl + tr) / 2;
        if (id < m) upd(id, val, 2*v + 1, tl, m);
        else upd(id, val, 2*v + 2, m, tr);
        tree[v] = tree[2*v + 1] + tree[2*v + 2];
    }

    T get(int l, int r, int v, int tl, int tr) {
        if (tl >= r || tr <= l) return T();
        if (tl >= l && tr <= r) return tree[v];
        int m = (tl + tr) / 2;
        return get(l, r, 2*v + 1, tl, m) + get(l, r, 2*v + 2, m, tr);
    }
};

struct NodeSum {
    ll s = 0;

    NodeSum() = default;
    NodeSum(ll s): s(s) {};

    NodeSum operator+ (NodeSum other) const {
        NodeSum tmp;
        tmp.s = s + other.s;
        return tmp;
    }
};

struct NodeMax {
    int id = -1;
    ll mx = -(1e18);

    NodeMax() = default;
    NodeMax(int id, ll mx): id(id), mx(mx) {};

    NodeMax operator+ (NodeMax other) const {
        if (mx >= other.mx) return *this;
        return other;
    }
};

void solve() {
    int n; cin >> n;
    vector<NodeMax> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i].mx;
        a[i].id = i+1;
    }

    segTree<NodeMax> t(a);

    int q; cin >> q;
    while (q--) {
        char c; cin >> c;
        if (c == 'q') {
            int l, r;
            cin >> l >> r;
            cout << t.get(l-1, r).id << '\n';
        } else {
            int id, v;
            cin >> id >> v;
            t.upd(id-1, {id, v});
        }
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);

    solve();

    return 0;
}