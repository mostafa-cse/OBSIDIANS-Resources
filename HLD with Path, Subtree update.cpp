// https://vjudge.net/problem/LightOJ-1348
#include <bits/stdc++.h>
#define int long long
using namespace std;
#define all(x) x.begin(), x.end()

struct state {
    int val;
};
struct lazy_state {
    int add, set;
    bool pending;
};
state combine(state a, state b) {
    return {a.val + b.val};
}
state identity_state() {
    return {0};
}
lazy_state identity_lazy() {
    return {0, 0, false};
}
void propagate(state &x, lazy_state &g, const lazy_state &f, int len) {
    if (f.pending) {
        x.val = f.set * len;
        g.set = f.set;
        g.pending = true;
        g.add = 0;
    }
    if (f.add != 0) {
        x.val += f.add * len;
        g.add += f.add;
    }
}
struct segment_tree {
    int n;
    vector<state> st;
    vector<lazy_state> lz;

    void init(int _n) {
        n = _n;
        st.assign(4 * n + 4, identity_state());
        lz.assign(4 * n + 4, identity_lazy());
    }
    void apply(int p, int l, int r, const lazy_state &f) {
        propagate(st[p], lz[p], f, r - l + 1);
    }
    void push(int p, int l, int r) {
        apply(p << 1, l, r, lz[p]);
        apply(p << 1 | 1, l, r, lz[p]);
        lz[p] = identity_lazy();
    }
    void build(vector<state> &base, int p, int l, int r) {
        if (l == r) {
            st[p] = base[l];
            return;
        }
        int mid = (l + r) >> 1;
        build(base, p << 1, l, mid);
        build(base, p << 1 | 1, mid + 1, r);
        st[p] = combine(st[p << 1], st[p << 1 | 1]);
    }
    void update(int p, int l, int r, int ql, int qr, const lazy_state &f) {
        if (qr < l or r < ql) return;
        if (ql <= l and r <= qr) {
            apply(p, l, r, f);
            return;
        }
        push(p, l, r);
        int m = (l + r) >> 1;
        update(p << 1, l, m, ql, qr, f);
        update(p << 1 | 1, m + 1, r, ql, qr, f);
        st[p] = combine(st[p << 1], st[p << 1 | 1]);
    }
    state query(int p, int l, int r, int ql, int qr) {
        if (qr < l or r < ql) return identity_state();
        if (ql <= l and r <= qr) return st[p];
        push(p, l, r);
        int mid = (l + r) >> 1;
        state left = query(p << 1, l, mid, ql, qr);
        state right = query(p << 1 | 1, mid + 1, r, ql, qr);
        return combine(left, right);
    }
    void build(vector<state> &base) { build(base, 1, 1, n); }
    void update(int l, int r, const lazy_state &f) { update(1, 1, n, l, r, f); }
    state query(int l, int r) { return query(1, 1, n, l, r); }
};

struct HLD {
    int n;
    vector<vector<int>> g;
    vector<int> val, parent, depth, heavy, sz, head, pos, jump;
    int cur_pos;

    segment_tree seg;

    HLD(int _n) { init(_n); }

    void init(int _n) {
        n = _n;
        g.assign(n + 1, {});
        val.assign(n + 1, 0);
        parent.assign(n + 1, 0);
        depth.assign(n + 1, 0);
        heavy.assign(n + 1, -1);
        sz.assign(n + 1, 0);
        head.assign(n + 1, 0);
        pos.assign(n + 1, 0);
        jump.assign(n + 1, 0);
        cur_pos = 1;
        seg.init(n);
    }

    void addEdge(int u, int v) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    void setValue(int u, int x) {
        val[u] = x;
    }

    int dfs_sz(int u, int p) {
        parent[u] = (p == -1 ? 0 : p);
        depth[u] = (p == -1 ? 1 : depth[p] + 1);

        int par1 = parent[u];
        int par2 = jump[par1];
        if (par2 != 0 and jump[par2] != 0 and
            depth[par1] - depth[par2] == depth[par2] - depth[jump[par2]]) {
            jump[u] = jump[par2];
        } else {
            jump[u] = par1;
        }

        sz[u] = 1;
        heavy[u] = -1;
        int mx_sz = 0;
        for (int v : g[u]) {
            if (v != p) {
                int sub = dfs_sz(v, u);
                sz[u] += sub;
                if (sub > mx_sz) {
                    mx_sz = sub;
                    heavy[u] = v;
                }
            }
        }
        return sz[u];
    }
    void dfs_hld(int u, int h) {
        head[u] = h;
        pos[u] = cur_pos++;
        if (heavy[u] != -1) {
            dfs_hld(heavy[u], h);
        }
        for (int v : g[u]) {
            if (v != parent[u] and v != heavy[u]) {
                dfs_hld(v, v);
            }
        }
    }
    void build(int root = 1) {
        cur_pos = 1;
        seg.init(n);
        dfs_sz(root, -1);
        dfs_hld(root, root);
        vector<state> base(n + 1, identity_state());

        for (int u = 1; u <= n; u++) {
            base[pos[u]] = {val[u]};
        }
        seg.build(base);
    }
    int kth_ancestor(int u, int k) {
        int target = depth[u] - k;
        if (target < 1) return 0;
        while (depth[u] > target) {
            if (jump[u] != 0 and depth[jump[u]] >= target) {
                u = jump[u];
            } else {
                u = parent[u];
            }
        }
        return u;
    }
    int getLCA(int u, int v) {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) {
                swap(u, v);
            }
            u = parent[head[u]];
        }
        return (depth[u] < depth[v] ? u : v);
    }
    void update_subtree(int u, const lazy_state &f) {
        seg.update(pos[u], pos[u] + sz[u] - 1, f);
    }
    void update_path(int u, int v, const lazy_state &f) {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) {
                swap(u, v);
            }
            seg.update(pos[head[u]], pos[u], f);
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) {
            swap(u, v);
        }
        seg.update(pos[u], pos[v], f);
    }
    state query_subtree(int u) {
        return seg.query(pos[u], pos[u] + sz[u] - 1);
    }
    state query_path(int u, int v) {
        state ans_u = identity_state(), ans_v = identity_state();
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) {
                ans_u = combine(seg.query(pos[head[u]], pos[u]), ans_u);
                u = parent[head[u]];
            } else {
                ans_v = combine(seg.query(pos[head[v]], pos[v]), ans_v);
                v = parent[head[v]];
            }
        }
        if (depth[u] > depth[v]) {
            ans_u = combine(seg.query(pos[v], pos[u]), ans_u);
        } else {
            ans_v = combine(seg.query(pos[u], pos[v]), ans_v);
        }
        return combine(ans_u, ans_v);
    }
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

            state ans = hld.query_path(u, v);
            cout << ans.val << endl;
        } else {
            int u, x;
            cin >> u >> x;
            u++;
            lazy_state cng = {0, x, true};
            hld.update_path(u, u, cng);
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

