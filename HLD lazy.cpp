#include <bits/stdc++.h>
#define int long long
using namespace std;

const int INF = 1e18;

struct state {
    int sum, mn, mx;
    state() : sum(0), mn(INF), mx(-INF) {}
    state(int x) : sum(x), mn(x), mx(x) {}
    state(int sum, int mn, int mx) : sum(sum), mn(mn), mx(mx) {}
};

struct lazy_state {
    int add, set;
    bool pending;
};

state identity_state() {
    return state(0, INF, -INF);
}

lazy_state identity_lazy() {
    return {0, 0, false};
}

state combine(const state &a, const state &b) {
    return state(
        a.sum + b.sum,
        min(a.mn, b.mn),
        max(a.mx, b.mx)
    );
}

void propagate(state &x, lazy_state &g, const lazy_state &f, int len) {
    if (f.pending) {
        x.sum = f.set * len;
        x.mn = f.set;
        x.mx = f.set;
        g.set = f.set;
        g.pending = true;
        g.add = 0;
    }
    if (f.add != 0) {
        x.sum += f.add * len;
        x.mn += f.add;
        x.mx += f.add;
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
        if (!lz[p].pending and lz[p].add == 0) return;
        int mid = (l + r) >> 1;
        apply(p << 1, l, mid, lz[p]);
        apply(p << 1 | 1, mid + 1, r, lz[p]);
        lz[p] = identity_lazy();
    }
    void build(const vector<state> &base, int p, int l, int r) {
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
    void build(const vector<state> &base) { build(base, 1, 1, n); }
    void update(int l, int r, const lazy_state &f) {
        if (l > r) return;
        update(1, 1, n, l, r, f);
    }
    state query(int l, int r) {
        if (l > r) return identity_state();
        return query(1, 1, n, l, r);
    }
};

struct HLD {
    int n;
    vector<vector<int>> g;
    vector<int> val, parent, depth, heavy, sz, head, pos, jump;
    int cur_pos;
    bool is_edge;

    segment_tree seg;

    HLD(int _n = 0) { init(_n); }

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
        is_edge = false;
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

    // 1. Build for Node values
    void build_node(int root = 1) {
        is_edge = false;
        cur_pos = 1;
        seg.init(n);
        dfs_sz(root, -1);
        dfs_hld(root, root);
        vector<state> base(n + 1, identity_state());
        for (int u = 1; u <= n; u++) {
            base[pos[u]] = state(val[u]);
        }
        seg.build(base);
    }

    void build(int root = 1) { build_node(root); }

    // 2. Build for Edge values: edges = {{u, v, weight}, ...}
    void build_edge(int root, const vector<array<int, 3>> &edges) {
        is_edge = true;
        cur_pos = 1;
        seg.init(n);
        dfs_sz(root, -1);
        dfs_hld(root, root);
        for (auto &e : edges) {
            int child = (depth[e[0]] > depth[e[1]]) ? e[0] : e[1];
            setValue(child, e[2]);
        }
        vector<state> base(n + 1, identity_state());
        for (int u = 1; u <= n; u++) {
            if (u != root) {
                base[pos[u]] = state(val[u]);
            }
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

    // ================= CORE UPDATE & QUERY =================
    void update_node(int u, const lazy_state &f) {
        seg.update(pos[u], pos[u], f);
    }

    void update_edge(int u, int v, const lazy_state &f) {
        int child = (depth[u] > depth[v]) ? u : v;
        seg.update(pos[child], pos[child], f);
    }

    void update_subtree(int u, const lazy_state &f) {
        seg.update(pos[u] + is_edge, pos[u] + sz[u] - 1, f);
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
        seg.update(pos[u] + is_edge, pos[v], f);
    }

    state query_subtree(int u) {
        return seg.query(pos[u] + is_edge, pos[u] + sz[u] - 1);
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
            ans_u = combine(seg.query(pos[v] + is_edge, pos[u]), ans_u);
        } else {
            ans_v = combine(seg.query(pos[u] + is_edge, pos[v]), ans_v);
        }
        return combine(ans_u, ans_v);
    }

    // ================= ADD UPDATES (+x) =================
    void add_node(int u, int x)         { update_node(u, {x, 0, false}); }
    void add_edge(int u, int v, int x)  { update_edge(u, v, {x, 0, false}); }
    void add_path(int u, int v, int x)  { update_path(u, v, {x, 0, false}); }
    void add_subtree(int u, int x)      { update_subtree(u, {x, 0, false}); }

    // ============= ASSIGNMENT UPDATES (=x) ==============
    void set_node(int u, int x)         { update_node(u, {0, x, true}); }
    void set_edge(int u, int v, int x)  { update_edge(u, v, {0, x, true}); }
    void set_path(int u, int v, int x)  { update_path(u, v, {0, x, true}); }
    void set_subtree(int u, int x)      { update_subtree(u, {0, x, true}); }

    // ============= DIRECT PATH QUERIES ==================
    int query_path_sum(int u, int v)    { return query_path(u, v).sum; }
    int query_path_min(int u, int v)    { return query_path(u, v).mn; }
    int query_path_max(int u, int v)    { return query_path(u, v).mx; }

    // ============ DIRECT SUBTREE QUERIES ================
    int query_subtree_sum(int u)        { return query_subtree(u).sum; }
    int query_subtree_min(int u)        { return query_subtree(u).mn; }
    int query_subtree_max(int u)        { return query_subtree(u).mx; }
};
