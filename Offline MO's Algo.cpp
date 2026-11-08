struct Query {
    int l, r, idx, block;
    inline bool operator<(const Query& o) const {
        if (block != o.block) return block < o.block;
        return (block & 1) ? (r < o.r) : (r > o.r);
    }
};
template <typename Derived, typename AnsType = int>
struct MoBase {
protected:
    int n, block_size;
    const int* __restrict__ a;
    vector<Query> qs;

public:
    MoBase(const vector<int>& arr, int expected_queries)
        : n((int)arr.size()),
          block_size(max(1, (int)(n / sqrt(max(1.0, expected_queries * 2.0 / 3.0))))),
          a(arr.data()) {
        qs.reserve(expected_queries);
    }

    inline void addQuery(int l, int r, int idx) {
        qs.push_back({l, r, idx, l / block_size});
    }

    vector<AnsType> process() {
        int qcount = (int)qs.size();
        sort(qs.begin(), qs.end());

        vector<AnsType> res(qcount);
        Derived& self = static_cast<Derived&>(*this);

        int curL = 0, curR = -1;
        for (const auto& Q : qs) {
            while (curL > Q.l) self.add(--curL);
            while (curR < Q.r) self.add(++curR);
            while (curL < Q.l) self.remove(curL++);
            while (curR > Q.r) self.remove(curR--);
            res[Q.idx] = self.getAnswer();
        }
        return res;
    }
};

int cnt[1000005];

struct MOS : MoBase<MOS, int> {
    int ans = 0;

    MOS(const vector<int>& arr, int qcount) : MoBase<MOS, int>(arr, qcount) {}

    [[gnu::always_inline]] inline void add(int pos) {

    }

    [[gnu::always_inline]] inline void remove(int pos) {

    }

    [[gnu::always_inline]] inline int getAnswer() const {
        return ans;
    }
};
