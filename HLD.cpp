struct state {
    state() {}
    state(int x) {
        
    }
};
state make_identity() {
    return state{};
}
state combine(const state &a, const state &b) {
    state c;
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
    void build(int node, int l, int r) {
        if (l == r) {
            sgt[node] = a[l];
            return;
        }
        int mid = l + (r - l) / 2;
        build(node * 2, l, mid);
        build(node * 2 + 1, mid + 1, r);
        sgt[node] = combine(sgt[2 * node], sgt[2 * node + 1]);
    }
    void update(int node, int l, int r, int idx, const state &value) {
        if (l == r) {
            sgt[node] = value;
            return;
        }
        int mid = l + (r - l) / 2;
        if (idx <= mid) {
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
    void build(const vector<state> &a) {
        this->a = a;
        build(1, 1, n);
    }
    void update(int idx, state val) { update(1, 1, n, idx, val); }
    state query(int l, int r) { 
        if (l > r) return make_identity();
        return query(1, 1, n, l, r); 
    }
};

struct HLD {
    int n;
    vector<vector<int>> g;
    vector<int> val;
    vector<int> parent, depth, heavy, sz, head, pos;
    int cur_pos;
    bool is_edge;

    SegmentTree seg;
    HLD(int n = 0) { init(n); }
    void init(int n_) {
        n = n_;
        g.assign(n + 1, {});
        val.assign(n + 1, 0);
        parent.assign(n + 1, -1);
        depth.assign(n + 1, 0);
        heavy.assign(n + 1, -1);
        sz.assign(n + 1, 0);
        head.assign(n + 1, 0);
        pos.assign(n + 1, 0);
        cur_pos = 1;
        is_edge = false;
        seg.init(n);
    }
    void addEdge(int u, int v) { g[u].push_back(v), g[v].push_back(u); }
    void setValue(int u, int x) { val[u] = x; }

    int dfs_sz(int u, int p) {
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
    void dfs_hld(int u, int h) {
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

    // Call this for Node-based HLD
    void build_node(int root = 1) {
        is_edge = false;
        dfs_sz(root, -1);
        dfs_hld(root, root);
        vector<state> base(n + 2, make_identity());
        for (int u = 1; u <= n; ++u) {
            base[pos[u]] = state(val[u]);
        }
        seg.build(base);
    }

    // Call this for Edge-based HLD (edges = {u, v, wt})
    void build_edge(int root, const vector<array<int, 3>> &edges) {
        is_edge = true;
        dfs_sz(root, -1);
        dfs_hld(root, root);
        for (auto &e : edges) {
            int child = (depth[e[0]] > depth[e[1]]) ? e[0] : e[1];
            setValue(child, e[2]);
        }
        vector<state> base(n + 2, make_identity());
        for (int u = 1; u <= n; ++u) {
            if (u != root) {
                base[pos[u]] = state(val[u]);
            }
        }
        seg.build(base);
    }
    // Lowest Common Ancestor in O(log N)
    int lca(int u, int v) {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) {
                swap(u, v);
            }
            u = parent[head[u]];
        }
        return (depth[u] < depth[v]) ? u : v;
    }
    // Number of edges between u and v in O(log N)
    int dist(int u, int v) {
        return depth[u] + depth[v] - 2 * depth[lca(u, v)];
    }
    // k-th ancestor of u in O(log N) (0-th is u; returns -1 if out of bounds)
    int kth_ancestor(int u, int k) {
        if (depth[u] < k) return -1;
        while (u != -1) {
            int d = depth[u] - depth[head[u]];
            if (k <= d) return inv[pos[u] - k];
            k -= d + 1;
            u = parent[head[u]];
        }
        return -1;
    }
    // k-th node on simple path from u to v (0-indexed: 0-th is u; returns -1 if k > dist(u, v))
    int kth_on_path(int u, int v, int k) {
        int w = lca(u, v);
        int d_uw = depth[u] - depth[w];
        int d_vw = depth[v] - depth[w];
        if (k > d_uw + d_vw) return -1;
        if (k <= d_uw) return kth_ancestor(u, k);
        return kth_ancestor(v, d_uw + d_vw - k);
    }


    // Node value update: u-th node = x
    void pointSet(int u, int x) {
        val[u] = x;
        seg.update(pos[u], state(x));
    }

    // Edge weight update: edge (u, v) = x
    void updateEdge(int u, int v, int x) {
        int child = (depth[u] > depth[v]) ? u : v;
        pointSet(child, x);
    }

    // Works automatically for both Node and Edge queries
    state queryPath(int u, int v) {
        state res = make_identity();
        while (head[u] != head[v]) {
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
        if (!is_edge || u != v) {
            state last = seg.query(pos[u] + is_edge, pos[v]);
            res = combine(res, last);
        }
        return res;
    }

    state SegmentQuery(int l, int r) { return seg.query(l, r); }
    state SubtreeQuery(int u) { return seg.query(pos[u] + is_edge, pos[u] + sz[u] - 1); }
    state PathQuery(int u, int v) { return queryPath(u, v); }
};
