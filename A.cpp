#include <iostream>
#include <vector>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <utility>
#include <queue>
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
        int HN,
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
        int HN,
        int C,
        vector<int> WX,
        vector<int> WY,
        vector<int> HX,
        vector<int> HY,
        function<int(int x, int y, int P)> excavate
    ) {
        bool R[N][N] = {};

        for (int i=0; i<HN; i++)
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

class SolverMain: public Solver
{
public:
    virtual void solve(
        int WN,
        int HN,
        int C,
        vector<int> WX,
        vector<int> WY,
        vector<int> HX,
        vector<int> HY,
        function<int(int x, int y, int P)> excavate
    ) {
        // 掘削済み。
        bool excavated[N][N] = {};

        int S[N][N];
        for (int y=0; y<N; y++)
            for (int x=0; x<N; x++)
                S[y][x] = 5000;

        // 試掘。
        int CS = 10;
        for (int cy=0; cy<N; cy+=CS)
            for (int cx=0; cx<N; cx+=CS)
            {
                int x = cx+CS/2;
                int y = cy+CS/2;
                for (int i=0; i<5; i++)
                    if (excavate(x, y, 100)==1)
                    {
                        excavated[y][x] = true;
                        S[y][x] = 100*(i+1);
                        break;
                    }
            }

        // 最近傍
        //for (int cy=0; cy<N; cy+=CS)
        //    for (int cx=0; cx<N; cx+=CS)
        //        for (int dx=0; dx<CS; dx++)
        //            for (int dy=0; dy<CS; dy++)
        //                S[cy+dy][cx+dx] = S[cy+CS/2][cx+CS/2];

        // とりあえず線型補間。
        for (int cy=CS/2; cy<N; cy+=CS)
        {
            for (int x=0; x<CS/2; x++)
                S[cy][x] = S[cy][CS/2];
            for (int cx=CS/2; cx+CS<N; cx+=CS)
                for (int dx=1; dx<CS; dx++)
                    S[cy][cx+dx] = (S[cy][cx]*(CS-dx)+dx*S[cy][cx+CS]+CS/2)/CS;
            for (int x=N-CS+CS/2+1; x<N; x++)
                S[cy][x] = S[cy][N-CS+CS/2];
        }
        for (int x=0; x<N; x++)
        {
            for (int y=0; y<CS/2; y++)
                S[y][x] = S[CS/2][x];
            for (int cy=CS/2; cy+CS<N; cy+=CS)
                for (int dy=1; dy<CS; dy++)
                    S[cy+dy][x] = (S[cy][x]*(CS-dy)+dy*S[cy+CS][x]+CS/2)/CS;
            for (int y=N-CS+CS/2+1; y<N; y++)
                S[y][x] = S[N-CS+CS/2][x];
        }
        //for (int y=0; y<N; y++)
        //{
        //    for (int x=0; x<N; x++)
        //        cout<<" "<<S[y][x];
        //    cout<<endl;
        //}

        auto breakRock = [&](int x, int y)
        {
            if (excavated[y][x])
                return;
            excavated[y][x] = true;
            while (excavate(x, y, 100)==0);
        };

        // 家を掘削。
        for (int h=0; h<HN; h++)
            breakRock(HX[h], HY[h]);

        // 家に水を通す。
        for (int h=0; h<HN; h++)
        {
            int S2[N][N];
            for (int y=0; y<N; y++)
                for (int x=0; x<N; x++)
                {
                    S2[y][x] = S[y][x];
                    if (excavated[y][x])
                        S2[y][x] = 0;
                }

            int oo = 99999999;
            int D[N][N];
            for (int y=0; y<N; y++)
                for (int x=0; x<N; x++)
                    D[y][x] = oo;
            int PX[N][N];
            int PY[N][N];
            priority_queue<pair<int, pair<int, int>>> Q;
            for (int w=0; w<WN; w++)
            {
                int x = WX[w];
                int y = WY[w];
                D[y][x] = S2[y][x];
                PX[y][x] = -1;
                Q.push({-D[y][x], {x, y}});
            }

            int DX[] = {1, -1, 0, 0};
            int DY[] = {0, 0, 1, -1};

            int hx = HX[h];
            int hy = HY[h];
            while (D[hy][hx]==oo)
            {
                int d = -Q.top().first;
                int x = Q.top().second.first;
                int y = Q.top().second.second;
                Q.pop();
                if (d>D[y][x])
                    continue;

                for (int d=0; d<4; d++)
                {
                    int tx = x+DX[d];
                    int ty = y+DY[d];
                    if (0<=tx && tx<N && 0<=ty && ty<N &&
                        D[y][x]+S2[ty][tx]<D[ty][tx])
                    {
                        D[ty][tx] = D[y][x]+S2[ty][tx];
                        PX[ty][tx] = x;
                        PY[ty][tx] = y;
                        Q.push({-D[ty][tx], {tx, ty}});
                    }
                }
            }

            int x = PX[hy][hx];
            int y = PY[hy][hx];
            while (x>=0)
            {
                breakRock(x, y);
                int nx = PX[y][x];
                int ny = PY[y][x];
                x = nx;
                y = ny;
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
            cerr<<"error"<<endl;
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
    //SolverSample solver;
    SolverMain solver;

#ifdef TOPCODER_LOCAL
    solveLocal(&solver);
#else
    solveJudge(&solver);
#endif
}
