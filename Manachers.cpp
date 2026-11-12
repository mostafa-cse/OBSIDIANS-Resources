struct Manacher {
    string t;
    vector<int> p;
    int n;
    int m;

    Manacher(const string &s) {
        n = s.size();
        if (n == 0) { m = 0; return; }

        t.reserve(2 * n + 1);
        for (char c : s) { t.push_back('#'); t.push_back(c); }
        t.push_back('#');
        m = (int)t.size();
        p.assign(m, 0);

        for (int i = 0, l = 0, r = -1; i < m; ++i) {
            int k = (i > r) ? 1 : min(p[l + r - i], r - i + 1);
            while (i - k >= 0 && i + k < m && t[i - k] == t[i + k]) ++k;
            p[i] = k; --k;                    
            if (i + k > r) { l = i - k; r = i + k; }
        }
    }

    bool isPalindrome(int l, int r) const {
        if (l > r) return true;
        if (n == 0) return true;
        int center = l + r + 1;
        int needed = r - l + 1;
        if (center < 0 || center >= m) return false;
        return p[center] >= needed;
    }
};
