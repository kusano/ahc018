#include <iostream>
#include <vector>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <chrono>
using namespace std;

class UnionFind
{
public:
    vector<int> parent;
    vector<int> sz;

    UnionFind(int n): parent(n), sz(n)
    {
        for(int i=0;i<n;i++)
        {
            parent[i] = i;
            sz[i] = 1;
        }
    }

    int root(int x)
    {
        return parent[x]==x ? x : parent[x] = root(parent[x]);
    }

    bool same(int x, int y)
    {
        return root(x)==root(y);
    }

    void unite(int x, int y)
    {
        x = root(x);
        y = root(y);
        if (x != y)
        {
            if (sz[x] < sz[y])
            {
                parent[x] = y;
                sz[y] += sz[x];
            }
            else
            {
                parent[y] = x;
                sz[x] += sz[y];
            }
        }
    }
};

const int N = 200;

class Solver
{
public:
    virtual void solve(
        int WN,
        int KN,
        int C,
        vector<int> WX,
        vector<int> WY,
        vector<int> HX,
        vector<int> HY,
        function<int(int x, int y, int P)> excavate
    ) = 0;
};

// サンプルと同じ。
class SolverSample: public Solver
{
public:
    virtual void solve(
        int WN,
        int KN,
        int C,
        vector<int> WX,
        vector<int> WY,
        vector<int> HX,
        vector<int> HY,
        function<int(int x, int y, int P)> excavate
    ) {
        bool R[N][N] = {};

        for (int i=0; i<KN; i++)
        {
            int x = HX[i];
            int y = HY[i];
            while (x!=WX[0])
            {
                if (!R[y][x])
                {
                    R[y][x] = true;
                    while (excavate(x, y, 100)==0);
                }
                if (x>WX[0])
                    x--;
                else
                    x++;
            }
            while (y!=WY[0])
            {
                if (!R[y][x])
                {
                    R[y][x] = true;
                    while (excavate(x, y, 100)==0);
                }
                if (y>WY[0])
                    y--;
                else
                    y++;
            }
            if (!R[y][x])
            {
                R[y][x] = true;
                while (excavate(x, y, 100)==0);
            }
        }
    };
};

// ジャッジプログラムと通信。
void solveJudge(Solver *solver)
{
    int N, W, K, C;
    cin>>N>>W>>K>>C;
    vector<int> a(W), b(W);
    for (int i=0; i<W; i++)
        cin>>a[i]>>b[i];
    vector<int> c(K), d(K);
    for (int i=0; i<K; i++)
        cin>>c[i]>>d[i];

    solver->solve(W, K, C, b, a, d, c, [&](int x, int y, int P) -> int
        {
            cout<<y<<" "<<x<<" "<<P<<endl;
            int r;
            cin>>r;
            return r;
        }
    );
}

// ジャッジサーバー無し。
void solveLocal(Solver *solver)
{
    int N, W, K, C;
    cin>>N>>W>>K>>C;
    vector<vector<int>> S(N, vector<int>(N));
    for (int y=0; y<N; y++)
        for (int x=0; x<N; x++)
            cin>>S[y][x];
    vector<int> a(W), b(W);
    for (int i=0; i<W; i++)
        cin>>a[i]>>b[i];
    vector<int> c(K), d(K);
    for (int i=0; i<K; i++)
        cin>>c[i]>>d[i];

    UnionFind UF(N*N);

    int n = 0;
    long long score = 0;

    auto excavate = [&](int x, int y, int P) -> int
    {
        cout<<y<<" "<<x<<" "<<P<<endl;

        if (S[y][x]<=0)
        {
            cout<<"error"<<endl;
            exit(1);
        }

        n++;
        score += C+P;

        S[y][x] -= P;
        if (S[y][x]>0)
            return 0;

        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};
        for (int d=0; d<4; d++)
        {
            int tx = x+dx[d];
            int ty = y+dy[d];
            if (0<=tx && tx<N && 0<=ty && ty<N && S[ty][tx]<=0)
                UF.unite(y*N+x, ty*N+tx);
        }

        for (int k=0; k<K; k++)
        {
            bool ok = false;
            for (int w=0; w<W && !ok; w++)
                if (UF.same(c[k]*N+d[k], a[w]*N+b[w]))
                    ok = true;
            if (!ok)
                return 1;
        }
        return 2;
    };

    chrono::system_clock::time_point start = chrono::system_clock::now();

    solver->solve(W, K, C, b, a, d, c, excavate);

    double time = chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now()-start).count()*1e-9;

    fprintf(stderr, "%1d %2d %3d %.3f %6d %8lld\n", W, K, C, time, n, score);
}

int main()
{
    SolverSample solver;
    //solveJudge(&solver);
    solveLocal(&solver);
}
