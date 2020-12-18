#include <bits/stdc++.h>
using namespace std;
#define int long long
struct edge {
    int a, b;
};
struct graph {
    vector<edge> E;
    vector<int> erased;
    vector<list<int> > adj;
    vector<bool> V_1;
    int size;
    graph(){};
    graph(vector<edge> E, int max) {
        this->E = E;
        erased.resize(max);
        adj.resize(max);
        V_1.resize(max);
        for (int i = 0; i < E.size(); i++) {
            adj[E[i].a].push_back(i);
            adj[E[i].b].push_back(i);
        }
        size = max;
        buildFirstV();
    }
    void dfs(int pos, int is) {
        erased[pos] = true;
        V_1[pos] = is;
        for (int i : adj[pos]) {
            edge at = E[i];
            if (pos == at.a) {
                if (!erased[at.b]) {
                    dfs(at.b, !is);
                }
            } else if (pos == at.b) {
                if (!erased[at.a]) {
                    dfs(at.a, !is);
                }
            }
        }
    }
    void buildFirstV() {
        for (int i = 0; i < size; i++) {
            if (!erased[i]) {
                dfs(i, 1);
            }
        }
        for (int i = 0; i < size; i++) {
            erased[i] = 0;
        }
    }
};
// euler partition
vector<list<edge> > eulerPartition(graph G) {
    vector<list<edge> > P;
    queue<int> S;
    for (int i = 0; i < G.size; i++) {
        if (G.adj[i].size() % 2 == 1) {
            S.push(i);
        }
    }
    for (int i = 0; i < G.size; i++) {
        if (G.adj[i].size() % 2 == 0 and G.adj[i].size() != 0) {
            S.push(i);
        }
    }
    while (!S.empty()) {
        int s = S.front();
        S.pop();
        while (G.adj[s].size() != 0 and G.erased[G.adj[s].front()])
            G.adj[s].pop_front();
        if (G.adj[s].size() != 0) {
            list<edge> p;
            int v = s;
            while (G.adj[v].size() != 0) {
                int e = G.adj[v].front();
                G.erased[e] = true;
                G.adj[v].pop_front();
                p.push_back(G.E[e]);
                if (G.E[e].a == v)
                    v = G.E[e].b;
                else
                    v = G.E[e].a;
                while (G.adj[v].size() != 0 and G.erased[G.adj[v].front()])
                    G.adj[v].pop_front();
            }
            P.push_back(p);
            if (G.adj[s].size() != 0) S.push(s);
        }
    }
    return P;
}

pair<graph, graph> make_graphs(graph G) {
    vector<list<edge> > aux = eulerPartition(G);
    vector<edge> g1, g2;
    bool um = 0;
    for (int i = 0; i < aux.size(); i++) {
        for (auto e : aux[i]) {
            if (um)
                g1.push_back(e);
            else
                g2.push_back(e);
            um = !um;
        }
    }
    return make_pair(graph(g1, G.size), graph(g2, G.size));
}
graph unite(graph g1, graph g2) {
    for (int i = 0; i < g2.size; i++) {
        for (int e : g2.adj[i]) {
            g1.adj[i].push_back(e);
        }
    }
    return g1;
}

pair<graph, graph> partition(graph G) {
    vector<bool> M(G.size);
    int mai = 0;
    for (int i = 0; i < G.size; i++) {
        mai = max(mai, (int)G.adj[i].size());
    }
    for (int i = 0; i < G.size; i++) {
        if (G.adj[i].size() == mai) {
            M[i] = true;
        }
    }
    int k, r;
    if (mai % 2 != 0) {
        if (mai % 4 == 3) {
            k = mai / 4 + 1, r = -1;
        } else if (mai % 4 == 1) {
            k = mai / 4, r = 1;
        }
    }
    vector<bool> M_1(G.size), M_2(G.size);
    graph H1, H2;
    pair<graph, graph> aux = make_graphs(G);
    H1 = aux.first;
    H2 = aux.second;
    if (mai % 2 == 0) {
        return {H1, H2};
    }
    int qtm2 = 0;
    for (int i = 0; i < G.size; i++) {
        if (M[i] and (H1.adj[i].size() == (2 * k) + r)) {
            M_1[i] = true;
        } else if (M[i]) {
            M_2[i] = true;
            qtm2++;
        }
    }
    while (qtm2 != 0) {
        vector<bool> M_21(G.size), M_22(G.size);
        graph H21, H22;
        pair<graph, graph> aux = make_graphs(H2);
        H21 = aux.first;
        H22 = aux.second;
        for (int i = 0; i < G.size; i++) {
            if (M_2[i] and H21.adj[i].size() == k + r) {
                M_21[i] = true;
            } else if (M_2[i]) {
                M_22[i] = true;
            }
        }
        if (k % 2 == 0) {
            H1 = unite(H1, H21);
            H2 = H22;
        } else {
            H1 = H22;
            H2 = unite(H1, H21);
        }
        for (int i = 0; i < G.size; i++) {
            M_1[i] = M_1[i] or M_21[i];
            M_2[i] = M_22[i];
        }
    }
    return {H1, H2};
}
bool cmp(pair<int, int> a, pair<int, int> b) { return a.second < b.second; }
graph refactorGraph(graph &G, int D) {
    vector<pair<int, int> > vertexToMergeV1;
    vector<pair<int, int> > vertexToMergeV2;
    for (int i = 0; i < G.adj.size(); i++) {
        if (G.adj[i].size() <= D / 2 and G.V_1[i]) {
            vertexToMergeV1.push_back({i, G.adj[i].size()});
        }
        if (G.adj[i].size() <= D / 2 and !G.V_1[i]) {
            vertexToMergeV2.push_back({i, G.adj[i].size()});
        }
    }
    sort(vertexToMergeV1.begin(), vertexToMergeV1.end(), cmp);
    sort(vertexToMergeV2.begin(), vertexToMergeV2.end(), cmp);
    int esq = 0, dir = vertexToMergeV1.size() - 1;
    int qt = vertexToMergeV1[dir].second;
    while (esq < dir) {
        if (qt + vertexToMergeV1[esq].second <= D / 2) {
            for (int &i : G.adj[vertexToMergeV1[esq].first]) {
                G.adj[vertexToMergeV1[dir].first].push_back(i);
            }
            qt += vertexToMergeV1[esq].second;
            G.adj[vertexToMergeV1[esq].first].clear();
            esq++;
        } else {
            dir--;
            qt = vertexToMergeV1[dir].second;
        }
    }
    esq = 0, dir = vertexToMergeV2.size() - 1;
    qt = vertexToMergeV2[dir].second;
    while (esq < dir) {
        if (qt + vertexToMergeV2[esq].second <= D / 2) {
            for (int &i : G.adj[vertexToMergeV2[esq].first]) {
                G.adj[vertexToMergeV2[dir].first].push_back(i);
            }
            qt += vertexToMergeV2[esq].second;
            G.adj[vertexToMergeV2[esq].first].clear();
            esq++;
        } else {
            dir--;
            qt = vertexToMergeV2[dir].second;
        }
    }
}
signed main() {
    int n, m;
    cin >> n >> m;
    graph G;
    vector<edge> edges(m);
    for (int i = 0; i < m; i++) {
        cin >> edges[i].a >> edges[i].b, edges[i].a--, edges[i].b--;
    }
    G = graph(edges, n);
    pair<graph, graph> ans = partition(G);
    cout << "first now:" << endl;
    for (int i = 0; i < ans.first.E.size(); i++) {
        cout << ans.first.E[i].a + 1 << " " << ans.first.E[i].b + 1 << endl;
    }

    cout << "second now:" << endl;
    for (int i = 0; i < ans.second.E.size(); i++) {
        cout << ans.second.E[i].a + 1 << " " << ans.second.E[i].b + 1 << endl;
    }
}
