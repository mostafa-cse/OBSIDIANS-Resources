struct state {
    int val;
    state() : val(0) {}
    state(int n) : val(n) {}
};
state make_identity() {
    return state{0};
}
state combine(const state &a, const state &b) {
    state c;
    c.val = (a.val + b.val);
    return c;
}
struct SegmentTree2D {
    vector<state> sgt;
    int n, m, stride;

    inline int idx(int vx, int vy) const {
        return vx * stride + vy;
    }

    void init(int N, int M) {
        this->n = N;
        this->m = M;
        this->stride = 4 * M + 4;
        sgt.assign((4 * N + 4) * stride, make_identity());
    }

    void buildY(int vx, int lx, int rx, int vy, int ly, int ry, const vector<vector<int>> &a) {
        if (ly == ry) {
            if (lx == rx) {
                sgt[idx(vx, vy)] = state(a[lx - 1][ly - 1]);
            } else {
                sgt[idx(vx, vy)] = combine(sgt[idx(vx * 2, vy)], sgt[idx(vx * 2 + 1, vy)]);
            }
            return;
        }
        int mid = ly + (ry - ly) / 2;
        buildY(vx, lx, rx, vy * 2, ly, mid, a);
        buildY(vx, lx, rx, vy * 2 + 1, mid + 1, ry, a);
        sgt[idx(vx, vy)] = combine(sgt[idx(vx, vy * 2)], sgt[idx(vx, vy * 2 + 1)]);
    }

    void buildX(int vx, int lx, int rx, const vector<vector<int>> &a) {
        if (lx != rx) {
            int mid = lx + (rx - lx) / 2;
            buildX(vx * 2, lx, mid, a);
            buildX(vx * 2 + 1, mid + 1, rx, a);
        }
        buildY(vx, lx, rx, 1, 1, m, a);
    }

    void updateY(int vx, int lx, int rx, int vy, int ly, int ry, int y, const state &value) {
        if (ly == ry) {
            if (lx == rx) {
                sgt[idx(vx, vy)] = value;
            } else {
                sgt[idx(vx, vy)] = combine(sgt[idx(vx * 2, vy)], sgt[idx(vx * 2 + 1, vy)]);
            }
            return;
        }
        int mid = ly + (ry - ly) / 2;
        if (y <= mid) {
            updateY(vx, lx, rx, vy * 2, ly, mid, y, value);
        } else {
            updateY(vx, lx, rx, vy * 2 + 1, mid + 1, ry, y, value);
        }
        sgt[idx(vx, vy)] = combine(sgt[idx(vx, vy * 2)], sgt[idx(vx, vy * 2 + 1)]);
    }

    void updateX(int vx, int lx, int rx, int x, int y, const state &value) {
        if (lx != rx) {
            int mid = lx + (rx - lx) / 2;
            if (x <= mid) {
                updateX(vx * 2, lx, mid, x, y, value);
            } else {
                updateX(vx * 2 + 1, mid + 1, rx, x, y, value);
            }
        }
        updateY(vx, lx, rx, 1, 1, m, y, value);
    }

    state queryY(int vx, int vy, int ly, int ry, int qly, int qry) {
        if (ly > qry or ry < qly) {
            return make_identity();
        }
        if (ly >= qly and ry <= qry) {
            return sgt[idx(vx, vy)];
        }
        int mid = ly + (ry - ly) / 2;
        state left = queryY(vx, vy * 2, ly, mid, qly, qry);
        state right = queryY(vx, vy * 2 + 1, mid + 1, ry, qly, qry);
        return combine(left, right);
    }

    state queryX(int vx, int lx, int rx, int qlx, int qrx, int qly, int qry) {
        if (lx > qrx or rx < qlx) {
            return make_identity();
        }
        if (lx >= qlx and rx <= qrx) {
            return queryY(vx, 1, 1, m, qly, qry);
        }
        int mid = lx + (rx - lx) / 2;
        state left = queryX(vx * 2, lx, mid, qlx, qrx, qly, qry);
        state right = queryX(vx * 2 + 1, mid + 1, rx, qlx, qrx, qly, qry);
        return combine(left, right);
    }
    void build(const vector<vector<int>> &arr) {
        buildX(1, 1, n, arr);
    }
    void update(int x, int y, state val) {
        updateX(1, 1, n, x, y, val);
    }
    state query(int x1, int y1, int x2, int y2) {
        return queryX(1, 1, n, x1, x2, y1, y2);
    }
} st;
