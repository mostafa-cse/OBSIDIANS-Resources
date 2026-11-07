template<typename T>
struct SparseTable2D {
  int n, m, log_n, log_m;
  vector<T> mx;

  int id(int k1, int k2, int i, int j) const {
    return (((k1 * log_m + k2) * n) + i) * m + j;
  }

  SparseTable2D(const vector<vector<T>>& grid) {
    if (grid.empty() || grid[0].empty()) return;
    n = grid.size(); m = grid[0].size();
    log_n = __lg(n) + 1; log_m = __lg(m) + 1;
    int sz = log_n * log_m * n * m;
     mx.resize(sz);

    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++)
        mx[id(0, 0, i, j)] = grid[i][j];

    for (int k1 = 0; k1 < log_n; k1++) {
      for (int k2 = 0; k2 < log_m; k2++) {
        if (!k1 && !k2) continue;
        for (int i = 0; i + (1 << k1) <= n; i++) {
          for (int j = 0; j + (1 << k2) <= m; j++) {
            int cur = id(k1, k2, i, j);
            int p1 = k1 ? id(k1 - 1, k2, i, j) : id(k1, k2 - 1, i, j);
            int p2 = k1 ? id(k1 - 1, k2, i + (1 << (k1 - 1)), j)
                        : id(k1, k2 - 1, i, j + (1 << (k2 - 1)));
            mx[cur] = max(mx[p1], mx[p2]);
          }
        }
      }
    }
  }

    int query(int r1, int c1, int r2, int c2) const {
    int k1 = __lg(r2 - r1 + 1), k2 = __lg(c2 - c1 + 1);
    int i1 = id(k1, k2, r1, c1);
    int i2 = id(k1, k2, r1, c2 - (1 << k2) + 1);
    int i3 = id(k1, k2, r2 - (1 << k1) + 1, c1);
    int i4 = id(k1, k2, r2 - (1 << k1) + 1, c2 - (1 << k2) + 1);
    return max({mx[i1], mx[i2], mx[i3], mx[i4]});
  }
};
