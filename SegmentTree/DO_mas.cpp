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

    void upd(int l, int r, T val) {
        upd(l, r, val, 0, 0, sz);
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

    void upd(int l, int r, T val, int v, int tl, int tr) {
        tree[v].push(tl, tr, &tree[2*v+1], &tree[2*v+2]);
        if (tl >= r || tr <= l) return;
        if (tl >= l && tr <= r) {
            tree[v].p = val.p;
            tree[v].push(tl, tr, &tree[2*v+1], &tree[2*v+2]);
            return;
        }
        int m = (tl + tr) / 2;
        upd(l, r, val, 2*v + 1, tl, m);
        upd(l, r, val, 2*v + 2, m, tr);
        tree[v] = tree[2*v + 1] + tree[2*v + 2];
    }

    T get(int l, int r, int v, int tl, int tr) {
        tree[v].push(tl, tr, &tree[2*v+1], &tree[2*v+2]);
        if (tl >= r || tr <= l) return T();
        if (tl >= l && tr <= r) return tree[v];
        int m = (tl + tr) / 2;
        return get(l, r, 2*v + 1, tl, m) + get(l, r, 2*v + 2, m, tr);
    }
};

struct NodeSum {
    ll s = 0;
    ll p = -1;

    NodeSum() = default;
    NodeSum(ll p): p(p) {};
    NodeSum(ll s, ll p): s(s), p(p) {};

    NodeSum operator+ (NodeSum other) const {
        NodeSum tmp;
        tmp.s = s + other.s;
        return tmp;
    }

    void push(int tl, int tr, NodeSum *l, NodeSum *r) {
        if (p == -1) return;
        s = p * (tr - tl);
        if (tl + 1 != tr) {
            l->p = p;
            r->p = p;
        }
        p = -1;
    }
};

void solve() {
    int n, q;
    cin >> n >> q;

    segTree<NodeSum> t(n);

    while (q--) {
        char c; cin >> c;
        if (c == 'A') {
            int l, r, v;
            cin >> l >> r >> v;
            t.upd(l-1, r, {v});
        } else {
            int l, r;
            cin >> l >> r;
            cout << t.get(l-1, r).s << '\n';
        }
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);

    solve();

    return 0;
}