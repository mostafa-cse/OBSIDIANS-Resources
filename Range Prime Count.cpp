using ll = long long;
/*
    COUNTS NUMBER OF PRIMES BETWEEN a and b for 1 <=a <= b <= 1e13
    Meissel-Lehmer template
*/
namespace PrimeCount {
    // Uncomment ONE preset based on max(b):
    // constexpr int N = 10000005, PN = 1, PM = 1;       // b <= 10^7  (O(1) Sieve)
    constexpr int N = 2000005, PN = 20000, PM = 100;     // b <= 2^31  (~12 MB)
    // constexpr int N = 2000000, PN = 40000, PM = 120;  // b <= 10^11 (~28 MB)
    // constexpr int N = 5000000, PN = 100000, PM = 200; // b <= 10^12 (~95 MB)
    // constexpr int N = 20000000, PN = 200000, PM = 250;// b <= 10^13 (~275 MB)

    int pi[N], dp[PN][PM];
    vector<int> pr;
    bitset<N> isp;

    void init() {
        isp.set(); isp[0] = isp[1] = 0;
        for (int i = 2; i < N; i++) {
            if (isp[i]) {
                pr.push_back(i);
                for (ll j = 1LL * i * i; j < N; j += i) isp[j] = 0;
            }
            pi[i] = pi[i - 1] + isp[i];
        }
        if (PN > 1 && PM > 1) {
            for (int n = 0; n < PN; n++) dp[n][0] = n;
            for (int m = 1; m < min(PM, (int)pr.size() + 1); m++)
                for (int n = 0; n < PN; n++)
                    dp[n][m] = dp[n][m - 1] - dp[n / pr[m - 1]][m - 1];
        }
    }

    ll isqrt(ll x) { ll r = sqrtl(x); while ((r + 1) * (r + 1) <= x) r++; while (r * r > x) r--; return r; }
    ll icbrt(ll x) { ll r = cbrtl(x); while ((r + 1) * (r + 1) * (r + 1) <= x) r++; while (r * r * r > x) r--; return r; }

    ll phi(ll x, int a) {
        if (!a) return x;
        if (x < PN && a < PM) return dp[x][a];
        if (x < pr[a - 1]) return 1;
        if (x < N && 1LL * pr[a - 1] * pr[a - 1] >= x) return pi[x] - a + 1;
        return phi(x, a - 1) - phi(x / pr[a - 1], a - 1);
    }

    ll count(ll x) {
        if (x <= 1) return 0;
        if (x < N) return pi[x];
        int a = count(isqrt(isqrt(x))), b = count(isqrt(x)), c = count(icbrt(x));
        ll sum = phi(x, a) + 1LL * (b + a - 2) * (b - a + 1) / 2;
        for (int i = a + 1; i <= b; i++) {
            ll w = x / pr[i - 1];
            sum -= count(w);
            if (i <= c) {
                int lim = count(isqrt(w));
                for (int j = i; j <= lim; j++) sum -= count(w / pr[j - 1]) - (j - 1);
            }
        }
        return sum;
    }

    ll range(ll a, ll b) {
        if (a > b) swap(a, b);
        if (b < 2) return 0;
        a = max(2LL, a);
        if (b < N) return pi[b] - pi[a - 1];
        if (b - a <= 200000 && 1LL * pr.back() * pr.back() >= b) {
            vector<bool> seg(b - a + 1, true);
            for (int p : pr) {
                if (1LL * p * p > b) break;
                for (ll j = max(1LL * p * p, ((a + p - 1) / p) * p); j <= b; j += p) seg[j - a] = false;
            }
            return std::count(seg.begin(), seg.end(), true);
        }
        return count(b) - count(a - 1);
    }
}
