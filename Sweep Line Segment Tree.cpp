// https://vjudge.net/problem/LightOJ-1204
#include <bits/stdc++.h>
#define int long long
using namespace std;
#define all(x) x.begin(), x.end()
vector<int> Y;
int k;
struct segment_tree{
    vector<int> a;
    vector<vector<int>> sum;
    vector<int> lazy;
    int n;
    segment_tree(int N) {
        this->n = N;
        a.assign(N + 1, 0);
        sum.assign(N * 4, vector<int>(11, 0));
        lazy.assign(N * 4, 0);
    }
    void push(int node, int start, int end) {
        int length = Y[end + 1] - Y[start];
        int cover = lazy[node];

        for (int i = 0; i <= k; i++) {
            if (cover >= i) {
                sum[node][i] = length;
            } else {
                if (start == end) {
                    sum[node][i] = 0;
                } else {
                    int need = i - cover;
                    sum[node][i] = sum[node * 2][need] + sum[node * 2 + 1][need];
                }
            }
        }

    }

    void update(int node, int start, int end, int l, int r, int value){
        if (Y[start] >= r or Y[end + 1] <= l) {
            return;
        }

        if (Y[start] >= l and Y[end + 1] <= r) {
            lazy[node] += value;
            push(node, start, end);
            return;
        }
        int mid = start + (end - start) / 2;
        update(node * 2, start, mid, l, r, value);
        update(node * 2 + 1, mid + 1, end, l, r, value);
        push(node, start, end);
    }
    void update(int l, int r, int val) { update(1, 0, n - 1, l, r, val); }
};

void solve() {
    int n;
    cin >> n >> k;

    struct info {
        int x, y1, y2;
        int type;
    };
    vector<info> a;
    vector<int> y;
    for (int i = 0; i < n; i++) {
        int x1, y1, x2, y2;
        cin >> x1 >> y1 >> x2 >> y2;
        x2++, y2++;

        y.push_back(y1);
        y.push_back(y2);

        a.push_back({x1, y1, y2, +1});
        a.push_back({x2, y1, y2, -1});
    }

    sort(all(y));
    y.erase(unique(all(y)), y.end());
    dout(y);
    int m = y.size();
    Y.assign(m, 0);
    for (int i = 0; i < m; i++) {
        Y[i] = y[i];
    }

    sort(all(a), [&](info &x, info &y){
        if (x.x != y.x) return x.x < y.x;
        return x.type > y.type;
    });

    segment_tree st(y.size() - 1);
    int ans = 0;
    for (int i = 0; i < 2 * n; i++) {
        if (i >= 1) {
            int dx = a[i].x - a[i - 1].x;
            int dy = st.sum[1][k];
            ans += dy * dx;
        }
        st.update(a[i].y1, a[i].y2, a[i].type);
    }
    cout << ans << endl;
}
signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL), cout.tie(NULL);
    int n;
    cin >> n;

    for (int i = 1; i <= n; i++) {
        cout << "Case " << i << ": ";
        solve();
    }
    return 0;
}

