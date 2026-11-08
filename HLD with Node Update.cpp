#include <bits/stdc++.h>
using namespace std;

struct state {
    int val;
    state() {}
    state(int n) : val(n) {}
};
state make_identity() {
    return state{0};
}
state combine(const state &a, const state &b){
    state c;
    c.val = (a.val + b.val);
    return c;
}
struct SegmentTree {
    vector<state> sgt;
    vector<state> a;
    int n;
    void init(int N) {
         this->n = N;
        a.assign(N + 1, make_identity());
        sgt.assign(N * 4 + 4, make_identity());
    }
    void build (int node, int l, int r) {
        if (l == r) {
            sgt[node] = a[l];
            return;
        }
        int mid = l + (r - l) / 2;
        build(node * 2, l, mid);
        build(node * 2 + 1, mid + 1, r);
        sgt[node] = combine(sgt[2 * node], sgt[2 * node + 1]);
    }
    void update(int node, int l, int r, int idx, state &value) {
        if (l == r) {
            sgt[node] = value;
            return;
        }
        int mid = l + (r - l) / 2;
        if(idx <= mid) {
            update(node * 2, l, mid, idx, value);
        } else {
            update(node * 2 + 1, mid + 1, r, idx, value);
        }
        sgt[node] = combine(sgt[2 * node], sgt[2 * node + 1]);
    }
    state query(int node, int l, int r, int ql, int qr) {
        if (l > qr or r < ql) {
            return make_identity();
        }
        if (l >= ql and r <= qr) {
            return sgt[node];
        }
        int mid = l + (r - l) / 2;
        state left = query(node * 2, l, mid, ql, qr);
        state right = query(node * 2 + 1, mid + 1, r, ql, qr);
        return combine(left, right);
    }
    void build(vector<state> &a) {
        this->a = a;
        build(1, 1, n);
    }
    void update(int idx, state val) { update(1, 1, n, idx, val); }
    state query(int l, int r) { return query(1, 1, n, l, r); }
};
struct HLD {
    int n;
    vector<vector<int>> g;
    vector<int> val;
    vector<int> parent, depth, heavy, sz, head, pos;
    int cur_pos;

    SegmentTree seg;
    HLD(int n = 0){ init(n); }
    void init(int n_) {
        n = n_;
        g.assign(n+1, {});
        val.assign(n+1, 0);
        parent.assign(n+1, -1);
        depth.assign(n+1, 0);
        heavy.assign(n+1, -1);
        sz.assign(n+1, 0);
        head.assign(n+1, 0);
        pos.assign(n+1, 0);
        cur_pos = 1;
        seg.init(n);
    }
    void addEdge(int u, int v) { g[u].push_back(v), g[v].push_back(u); }
    void setValue(int u, int x) { val[u] = x; }
    int dfs_sz(int u, int p){
        parent[u] = p;
        depth[u] = (p == -1 ? 0 : depth[p] + 1);
        sz[u] = 1;
        heavy[u] = -1;
        int best = 0;
        for (int v : g[u]) {
            if (v != p) {
                int sub = dfs_sz(v, u);
                sz[u] += sub;
                if (sub > best) {
                    best = sub;
                    heavy[u] = v;
                }
            }
        }
        return sz[u];
    }
    void dfs_hld(int u, int h){
        head[u] = h;
        pos[u] = cur_pos++;
        if (heavy[u] != -1) {
            dfs_hld(heavy[u], h);
        }
        for (int v : g[u]) {
            if (v != parent[u] && v != heavy[u]) {
                dfs_hld(v, v);
            }
        }
    }
    void build(int root = 1){
        dfs_sz(root, -1);
        dfs_hld(root, root);
        vector<state> base(n + 2, 0);
        for (int u = 1; u <= n; ++u) {
            base[pos[u]] = state(val[u]);
        }
        seg.build(base);
    }
    void pointSet(int u, int x) {
        val[u] = x;
        seg.update(pos[u], x);
    }
    state queryPath(int u, int v) {
        state res = make_identity();
        while(head[u] != head[v]){
            if (depth[head[u]] < depth[head[v]]) {
                swap(u, v);
            }
            state cur = seg.query(pos[head[u]], pos[u]);
            res = combine(res, cur);
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) {
            swap(u, v);
        }
        state last = seg.query(pos[u], pos[v]);
        res = combine(res, last);
        return res;
    }
    state SegmentQuery(int l, int r) { return seg.query(l, r); }
    state SubtreeQuery(int u) { return seg.query(pos[u], pos[u] + sz[u] - 1); }
    state PathQuery(int u, int v) { return queryPath(u, v); }
};
void solve() {
    int n;
    cin >> n;

    HLD hld(n);
    for (int u = 1; u <= n; u++) {
        int x; cin >> x;
        hld.setValue(u, x);
    }

    for (int i = 1; i < n; i++) {
        int u, v;
        cin >> u >> v;
        u++, v++;
        hld.addEdge(u, v);
    }
    hld.build();

    int q;
    cin >> q;
    while (q--) {
        int t;
        cin >> t;

        if (t == 0) {
            int u, v;
            cin >> u >> v;
            u++, v++;

            state ans = hld.PathQuery(u, v);
            cout << ans.val << endl;
        } else {
            int u, x;
            cin >> u >> x;
            u++;
            hld.pointSet(u, x);
        }
    }
}
signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL), cout.tie(NULL);
    int n;
    cin >> n;

    for (int i = 1; i <= n; i++) {
        cout << "Case " << i << ":\n";
        solve();
    }
    return 0;
}

