
struct state {
    int val;
    state() : val(0) {}
    state(int n) : val(n) {}
};

state make_identity() {
    return state{0};
}
state combine(const state &a, const state &b) {
    return state{a.val + b.val};
}
state exclude(const state &a, const state &b) {
    return state{a.val - b.val};
}
struct FenwickTree2D {
    vector<state> bit;
    vector<state> a;
    int n, m, stride;

    inline int idx(int x, int y) const {
        return x * stride + y;
    }
    void init(int N, int M) {
        this->n = N;
        this->m = M;
        this->stride = M + 1;
        bit.assign((N + 1) * stride, make_identity());
        a.assign((N + 1) * stride, make_identity());
    }

    void build(const vector<vector<int>> &arr) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                state v = state(arr[i - 1][j - 1]);
                a[idx(i, j)] = v;
                bit[idx(i, j)] = v;
            }
        }
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                int nj = j + (j & -j);
                if (nj <= m) {
                    bit[idx(i, nj)] = combine(bit[idx(i, nj)], bit[idx(i, j)]);
                }
            }
        }
        for (int i = 1; i <= n; i++) {
            int ni = i + (i & -i);
            if (ni <= n) {
                for (int j = 1; j <= m; j++) {
                    bit[idx(ni, j)] = combine(bit[idx(ni, j)], bit[idx(i, j)]);
                }
            }
        }
    }

    // a[x][y] += delta
    void add(int x, int y, const state &delta) {
        a[idx(x, y)] = combine(a[idx(x, y)], delta);
        for (int i = x; i <= n; i += i & -i) {
            for (int j = y; j <= m; j += j & -j) {
                bit[idx(i, j)] = combine(bit[idx(i, j)], delta);
            }
        }
    }

    // a[x][y] = value
    void update(int x, int y, const state &value) {
        state delta = exclude(value, a[idx(x, y)]);
        a[idx(x, y)] = value;
        for (int i = x; i <= n; i += i & -i) {
            for (int j = y; j <= m; j += j & -j) {
                bit[idx(i, j)] = combine(bit[idx(i, j)], delta);
            }
        }
    }

    // (1, 1) থেকে (x, y) পর্যন্ত প্রিফিক্স কুয়েরি
    state queryPrefix(int x, int y) {
        state res = make_identity();
        for (int i = x; i > 0; i -= i & -i) {
            for (int j = y; j > 0; j -= j & -j) {
                res = combine(res, bit[idx(i, j)]);
            }
        }
        return res;
    }

    // (x1, y1) থেকে (x2, y2) রেঞ্জ কুয়েরি (1-indexed)
    state query(int x1, int y1, int x2, int y2) {
        state total = queryPrefix(x2, y2);
        state top = queryPrefix(x1 - 1, y2);
        state left = queryPrefix(x2, y1 - 1);
        state corner = queryPrefix(x1 - 1, y1 - 1);
        return combine(exclude(exclude(total, top), left), corner);
    }
} ft;
