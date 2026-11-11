struct state {
    int val;
};
struct lazy_state {
    int add, assignment;
    bool pending;
};
state combine(state a, state b) {
    return {a.val + b.val};      // sum
    return {min(a.val , b.val)}; // min
}
state identity_state() {
    return {0};                  // Edit here
}
lazy_state identity_lazy() {
    return {0, 0, false};        // edit here
}
void propagate(state &x, lazy_state &g, const lazy_state &f, int l, int r) {
    long long len = r - l + 1;   // For min/max, len multiplier should be 1
    if (f.pending) {
        x.val = f.assignment * len;
        g.assignment = f.assignment;
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
        propagate(st[p], lz[p], f, l, r);
    }
    void push(int p, int l, int r) {
        if (!lz[p].pending && lz[p].add == 0) return; 
        int m = (l + r) >> 1;
        apply(p << 1, l, m, lz[p]);
        // u can modify lazy_state f for right and pass from here
        apply(p << 1 | 1, m + 1, r, lz[p]);
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
