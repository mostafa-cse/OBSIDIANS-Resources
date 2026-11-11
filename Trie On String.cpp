template<int ALPHABET = 26, char BASE = 'a'>
struct Trie {
    struct Node {
        int next[ALPHABET];
        int end_cnt = 0;  // Count of this exact string
        int pref_cnt = 0; // Count of strings passing through this prefix
        
        Node() {
            memset(next, -1, sizeof(next));
        }
    };

    vector<Node> nodes;

    Trie() {
        nodes.emplace_back();
    }

    void insert(const string& s, int k = 1) {
        int u = 0;
        nodes[u].pref_cnt += k;
        for (char c : s) {
            int v = c - BASE;
            if (nodes[u].next[v] == -1) {
                nodes[u].next[v] = nodes.size();
                nodes.emplace_back();
            }
            u = nodes[u].next[v];
            nodes[u].pref_cnt += k;
        }
        nodes[u].end_cnt += k;
    }

    // Returns the exact number of times this string was inserted and currently exists
    int count(const string& s) {
        int u = 0;
        for (char c : s) {
            int v = c - BASE;
            if (nodes[u].next[v] == -1) return 0;
            u = nodes[u].next[v];
        }
        return nodes[u].end_cnt;
    }

    // Removes exactly ONE occurrence of the string
    void erase_one(const string& s) {
        if (count(s) == 0) return; // Safeguard against invalid erasures
        
        int u = 0;
        nodes[u].pref_cnt--;
        for (char c : s) {
            int v = c - BASE;
            u = nodes[u].next[v];
            nodes[u].pref_cnt--;
        }
        nodes[u].end_cnt--;
    }

    // Removes ALL occurrences of the string
    void erase_all(const string& s) {
        int occurrences = count(s);
        if (occurrences == 0) return; // Safeguard
        
        int u = 0;
        nodes[u].pref_cnt -= occurrences;
        for (char c : s) {
            int v = c - BASE;
            u = nodes[u].next[v];
            nodes[u].pref_cnt -= occurrences;
        }
        nodes[u].end_cnt -= occurrences;
    }
};
