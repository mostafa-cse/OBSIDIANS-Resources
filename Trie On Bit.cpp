// Usage Quick Reference:
// Initialize:        Trie T;
// Insert Value:      T.add(val, 1);
// Delete Value:      T.add(val, -1);
// Count values < K:  T.count_xor_less(0, K);
// Rank of val:       T.count_xor_less(0, val) + 1;
// Get Median:        T.get_kth((T.root->cnt + 1) / 2);
// Min XOR with x:    int v = T.get_xor_kth(x, 1);            // Result is v ^ x
// Max XOR with x:    int v = T.get_xor_kth(x, T.root->cnt);  // Result is v ^ x
// Special:           T.count_special(X);

struct Trie {
  static const int B = 31;
  struct Node {
    Node *c[2];
    int cnt;
    Node() { c[0] = c[1] = nullptr, cnt = 0; }
  } *root;

  Trie() { root = new Node(); }

  // v = 1 to Insert, v = -1 to Erase
  void add(int x, int v) {
    Node *u = root;
    u->cnt += v;
    for (int i = B - 1; ~i; --i) {
      int d = (x >> i) & 1;
      if (!u->c[d]) u->c[d] = new Node();
      u = u->c[d], u->cnt += v;
    }
  }

  // Count values where (val ^ x) < k
  int count_xor_less(int x, int k) {
    Node *u = root;
    int ans = 0;
    for (int i = B - 1; ~i; --i) {
      if (!u) break;
      int d = (x >> i) & 1;
      if ((k >> i) & 1) ans += (u->c[d] ? u->c[d]->cnt : 0), u = u->c[!d];
      else u = u->c[d];
    }
    return ans;
  }

  int count_xor_greater(int x, int k) {
    return root->cnt - count_xor_less(x, k + 1);
  }

  // Returns the K-th smallest number in the Trie (1-based)
  int get_kth(int k) {
    Node *u = root;
    if (u->cnt < k) return -1;
    int ans = 0;
    for (int i = B - 1; ~i; --i) {
      int left = u->c[0] ? u->c[0]->cnt : 0;
      if (k <= left) u = u->c[0];
      else k -= left, u = u->c[1], ans |= (1 << i);
    }
    return ans;
  }

  // Check if x exists in the Trie
  int count(int x) {
    Node *u = root;
    for (int i = B - 1; ~i; --i) {
      int d = (x >> i) & 1;
      if (!u->c[d] || u->c[d]->cnt == 0) return 0;
      u = u->c[d];
    }
    return u->cnt;
  }

  // Returns value 'v' where (v ^ x) is the K-th smallest XOR sum
  // k = 1 -> Min XOR | k = Total -> Max XOR
  int get_xor_kth(int x, int k) {
    Node *u = root;
    if (u->cnt < k) return -1;
    int ans = 0;
    for (int i = B - 1; ~i; --i) {
      int d = (x >> i) & 1;
      int count_desired = u->c[d] ? u->c[d]->cnt : 0;
      if (k <= count_desired) u = u->c[d], ans |= (d << i);
      else k -= count_desired, u = u->c[!d], ans |= ((!d) << i);
    }
    return ans;
  }

  // Special: Count values satisfying val <= (val ^ x) <= x
  int64_t go(Node *u, int b, int x, int t1, int t2) {
    if (!u || !t1 || t2 == 2) return 0;
    if ((t1 == 2 && !t2) || b < 0) return u->cnt;
    int d = (x >> b) & 1;
    int64_t res = 0;
    if (d) {
      res += go(u->c[1], b - 1, x, t1 == 1 ? 0 : t1, t2 == 1 ? 0 : t2);
      res += go(u->c[0], b - 1, x, t1 == 1 ? 2 : t1, t2);
    } else {
      res += go(u->c[1], b - 1, x, t1, t2 == 1 ? 2 : t2);
      res += go(u->c[0], b - 1, x, t1, t2);
    }
    return res;
  }

  int64_t count_special(int x) { return go(root, B - 1, x, 1, 1); }
};
