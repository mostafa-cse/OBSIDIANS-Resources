struct Node {
    int val;
    Node(int _val = 0) {
        val = _val;
    }
    Node& operator+=(const Node &other) {
        this->val += other.val;
        return *this;
    }
};

struct FenwickTree { // 1-based query & update
private:
    vector<Node> bit;
    vector<int> a;
    int n;

public:
    FenwickTree(int _n) {
        this->n = _n;
        bit.assign(n + 1, Node(0));
    }
    // 0-based input vector 'a' (size = n)
    FenwickTree(const vector<int> &_a) {
        this->a = _a;
        this->n = a.size();
        bit.assign(n + 1, Node(0));
        build();
    }

    void update(int i, int val) {
        if (i <= 0) return;
        while (i <= n) {
            bit[i] += Node(val);
            i += (i & (-i));
        }
    }

    void build() {
        for (int i = 1; i <= n; i++) {
            update(i, a[i - 1]);
        }
    }
    // 1 থেকে idx পর্যন্ত যোগফল (1-based)
    Node query(int idx) {
        Node ans(0);
        while (idx > 0) {
            ans += bit[idx];
            idx -= (idx & (-idx));
        }
        return ans;
    }
};
