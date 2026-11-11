static const int MAX_NODES = 2000005;
static const int B = 63;
class BitwiseTrie {
private:
    int trie[MAX_NODES][2];
    int cnt[MAX_NODES];
    int nodes_cnt = 1;

public:
    BitwiseTrie() {
        reset();
    }

    void reset() {
        for (int i = 0; i < nodes_cnt; ++i) {
            trie[i][0] = trie[i][1] = 0;
            cnt[i] = 0;
        }
        nodes_cnt = 1;
    }

    int count(int num) {
        int cur = 0;
        for (int b = B; b >= 0; --b) {
            int bit = (num >> b) & 1;
            if (!trie[cur][bit]) return 0;
            cur = trie[cur][bit];
        }
        return cnt[cur];
    }

    void insert(int num) {
        int cur = 0;
        cnt[cur]++;
        for (int b = B; b >= 0; --b) {
            int bit = (num >> b) & 1;
            if (!trie[cur][bit]) {
                trie[cur][bit] = nodes_cnt++;
            }
            cur = trie[cur][bit];
            cnt[cur]++;
        }
    }

    void remove(int num) {
        if (count(num) == 0) return;

        int cur = 0;
        cnt[cur]--;
        for (int b = B; b >= 0; --b) {
            int bit = (num >> b) & 1;
            cur = trie[cur][bit];
            cnt[cur]--;
        }
    }

    int queryMaxXor(int num) {
        if (cnt[0] == 0) return 0;
        int cur = 0, res = 0;
        for (int b = B; b >= 0; --b) {
            int bit = (num >> b) & 1;
            int want = bit ^ 1;
            if (trie[cur][want] && cnt[trie[cur][want]] > 0) {
                res |= (1LL << b);
                cur = trie[cur][want];
            } else {
                cur = trie[cur][bit];
            }
        }
        return res;
    }

    int queryMinXor(int num) {
        if (cnt[0] == 0) return 0;
        int cur = 0, res = 0;
        for (int b = B; b >= 0; --b) {
            int bit = (num >> b) & 1;
            if (trie[cur][bit] && cnt[trie[cur][bit]] > 0) {
                cur = trie[cur][bit];
            } else {
                res |= (1LL << b);
                cur = trie[cur][bit ^ 1];
            }
        }
        return res;
    }
} btr; // declare this globally for ignoring stack memory and reset before use
