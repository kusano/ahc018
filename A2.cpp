#include <iostream>
#include <vector>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <utility>
#include <queue>
#include <cmath>
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
    int PREDICT_R = 20;

    int DX[4] = {1, -1, 0, 0};
    int DY[4] = {0, 0, 1, -1};

    int WN;
    int HN;
    int C;
    vector<int> WX;
    vector<int> WY;
    vector<int> HX;
    vector<int> HY;
    function<int(int x, int y, int P)> excavateFunc;

    int Smin[N][N];
    int Smax[N][N];
    bool excavated[N][N];
    int SPnum[N][N];
    // TODO: 精度を上げた方が良い？
    int SPden[N][N];
    int Emin[N][N];

    virtual void solve(
        int WN,
        int HN,
        int C,
        vector<int> WX,
        vector<int> WY,
        vector<int> HX,
        vector<int> HY,
        function<int(int x, int y, int P)> excavateFunc
    ) {
        this->WN = WN;
        this->HN = HN;
        this->C = C;
        this->WX = WX;
        this->WY = WY;
        this->HX = HX;
        this->HY = HY;
        this->excavateFunc = excavateFunc;

        int num = 0;
        int den = 0;
        for (int y=-PREDICT_R; y<=PREDICT_R; y++)
            for (int x=-PREDICT_R; x<=PREDICT_R; x++)
            {
                int d = hypot(x, y);
                if (d<PREDICT_R)
                {
                    num += 1000;
                    den += 1;
                }
            }

        for (int y=0; y<N; y++)
            for (int x=0; x<N; x++)
            {
                Smin[y][x] = 10;
                Smax[y][x] = 5000;
                excavated[y][x] = false;
                SPnum[y][x] = num;
                SPden[y][x] = den;
                Emin[y][x] = 9999;
            }

        bool R[N][N];
        while (true)
        {
            dijkstra(R);

            int mx = -1;
            int my;
            for (int y=0; y<N; y++)
                for (int x=0; x<N; x++)
                    if (R[y][x] &&
                        !excavated[y][x] &&
                        (mx==-1 || Emin[y][x]>Emin[my][mx]))
                    {
                        mx = x;
                        my = y;
                    }

            if (mx==-1)
                break;

            excavate(mx, my, 100);
        }
    };

    void excavate(int x, int y, int P)
    {
        if (excavated[y][x])
            return;

        if (Smin[y][x]>10)
            for (int ty=y-PREDICT_R; ty<=y+PREDICT_R; ty++)
                for (int tx=x-PREDICT_R; tx<=x+PREDICT_R; tx++)
                    if (0<=tx && tx<N && 0<=ty && ty<N)
                    {
                        int d = hypot(tx-x, ty-y);
                        if (d<PREDICT_R)
                        {
                            SPnum[ty][tx] -= (Smax[y][x]+Smin[y][x])/2*(PREDICT_R-d);
                            SPden[ty][tx] -= PREDICT_R-d;
                        }
                    }

        if (excavateFunc(x, y, P)==0)
            Smin[y][x] += P+(Smin[y][x]==10?-10:0);
        else
        {
            Smax[y][x] = Smin[y][x]+P+(Smin[y][x]==10?-10:0);
            excavated[y][x] = true;
        }

        for (int ty=y-PREDICT_R; ty<=y+PREDICT_R; ty++)
            for (int tx=x-PREDICT_R; tx<=x+PREDICT_R; tx++)
                if (0<=tx && tx<N && 0<=ty && ty<N)
                {
                    int d = hypot(tx-x, ty-y);
                    if (d<PREDICT_R)
                    {
                        SPnum[ty][tx] += (Smax[y][x]+Smin[y][x])/2*(PREDICT_R-d);
                        SPden[ty][tx] += PREDICT_R-d;
                        Emin[ty][tx] = min(Emin[ty][tx], d);
                    }
                }
    };

    int hypot(int x, int y)
    {
        static int R2[80001] = {-1};
        if (R2[0]==-1)
            for (int i=0; i<=80000; i++)
                R2[i] = int(sqrt(i)+.5);
        return R2[x*x+y*y];
    }

    void dijkstra(bool R[N][N])
    {
        for (int y=0; y<N; y++)
            for (int x=0; x<N; x++)
                R[y][x] = excavated[y][x];

        // 添え字の家に水路が繋がったか
        vector<bool> H(HN);

        for (int h=0; h<HN; h++)
        {
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
                // TODO: C
                D[y][x] = (SPnum[y][x]+SPden[y][x]/2)/SPden[y][x];
                PX[y][x] = -1;
                Q.push({-D[y][x], {x, y}});
            }

            int hx;
            int hy;
            while (true)
            {
                int d = -Q.top().first;
                int x = Q.top().second.first;
                int y = Q.top().second.second;
                Q.pop();
                if (d>D[y][x])
                    continue;

                bool ok = false;
                for (int h=0; h<HN; h++)
                    if (!H[h] && HX[h]==x && HY[h]==y)
                    {
                        H[h] = true;
                        hx = x;
                        hy = y;
                        ok = true;
                        break;
                    }
                if (ok)
                    break;

                for (int d=0; d<4; d++)
                {
                    int tx = x+DX[d];
                    int ty = y+DY[d];
                    if (0<=tx && tx<N && 0<=ty && ty<N)
                    {
                        int td = D[y][x];
                        if (!R[y][x])
                            // TODO: C
                            td += (SPnum[ty][tx]+SPden[ty][tx]/2)/SPden[y][x];
                        if (td<D[ty][tx])
                        {
                            D[ty][tx] = td;
                            PX[ty][tx] = x;
                            PY[ty][tx] = y;
                            Q.push({-D[ty][tx], {tx, ty}});
                        }
                    }
                }
            }

            int x = hx;
            int y = hy;
            while (x>=0)
            {
                R[y][x] = true;
                int nx = PX[y][x];
                int ny = PY[y][x];
                x = nx;
                y = ny;
            }
        }
    }
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

int main(int argc, char **argv)
{
    //SolverSample solver;
    SolverMain solver;

#ifdef TOPCODER_LOCAL
    solveLocal(&solver);
#else
    solveJudge(&solver);
#endif
}
