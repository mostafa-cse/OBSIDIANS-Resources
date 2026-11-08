// https://vjudge.net/problem/LightOJ-1424
#include <bits/stdc++.h>
#define int long long
using namespace std;
#define all(x) x.begin(), x.end()
#ifndef ONLINE_JUDGE
#define dout(...) cerr << "["; _print(__VA_ARGS__)
#else
#define dout(...)
#endif

void solve() {
    int n, m;
    cin >> n >> m;

    vector<vector<char>> a(n + 1, vector<char>(m + 1));
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            cin >> a[i][j];
        }
    }
    dout(a);

    vector<int> pfx(m + 1, 0);
    int ans = 0;
    for (int i = 1; i <= n; i++) {
        vector<int> curPfx(m + 1, 0);
        for (int j = 1; j <= m; j++) {
            if (a[i][j] != '1') {
                curPfx[j] = pfx[j] + 1;
            }
        }

        vector<int> nxt(m + 1, m + 1);
        stack<int> st;
        for (int j = 1; j <= m; j++) {
            while (!st.empty() and curPfx[j] < curPfx[st.top()]) {
                nxt[st.top()] = j;
                st.pop();
            }
            st.push(j);
        }
        pfx = curPfx;

        vector<int> pv(m + 1, 0);
        st = stack<int>();
        for (int j = m; j >= 1; j--) {
            while (!st.empty() and curPfx[j] < curPfx[st.top()]) {
                pv[st.top()] = j;
                st.pop();
            }
            st.push(j);
        }

        for (int j = 1; j <= m; j++) {
            int width = nxt[j] - pv[j] - 1;
            ans = max(ans, curPfx[j] * width);
        }
    }
    cout << ans << '\n';
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL), cout.tie(NULL);
    int t;
    cin >> t;

    for (int i = 1; i <= t; i++) {
        cout << "Case " << i << ": ";
        solve();
    }
    return 0;
}
