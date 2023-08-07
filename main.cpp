// testing jaledit - just a little editor
#include <bits/stdc++.h>

constexpr int maxN = 1e5 + 5;
constexpr long long inf = 1e18 + 5;

int n, nEdge;
std::vector<std::pair<long long, int>> g[maxN];

std::priority_queue<std::pair<long long, int>> q;
long long d[maxN];
int pre[maxN];

void Dijkstra() {
    std::fill_n(d + 2, n - 1, inf);
    q.emplace(0, 1);
    pre[1] = -1;

    while (!q.empty()) {
        auto [du, u] = q.top(); q.pop();
        if (-du != d[u]) continue;

        for (const auto& [v, c] : g[u]) {
            if (d[v] > d[u] + c) {
                d[v] = d[u] + c;
                q.emplace(-d[v], v);
                pre[v] = u;
            }
        }
    }
}

void trace(int u) {
    if (u == -1) {
        return;
    }

    trace(pre[u]);
    std::cout << u << ' ';
}

int main() {
    std::cin.tie(nullptr)->sync_with_stdio(false);

    std::cin >> n >> nEdge;

    while (nEdge--) {
        int u, v, c; std::cin >> u >> v >> c;
        g[u].emplace_back(v, c);
        g[v].emplace_back(u, c);
    }

    Dijkstra();

    if (pre[n] == 0) {
        std::cout << -1 << '\n';
        return 0;
    }

    trace(n); 
}
