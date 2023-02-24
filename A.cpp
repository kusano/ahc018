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
    int LEVEL_START = 0;
    int LEVEL_END = 0;
    int PROSPECT_P = 0;
    int PROSPECT_N = 0;
    int BREAK_DECAY = 0;
    int BREAK_ADD = 0;

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
        if (LEVEL_START==0)
            switch (C)
            {
            case 1:
                LEVEL_START = 14;
                LEVEL_END = 8;
                PROSPECT_P = 21;
                PROSPECT_N = 15;
                BREAK_DECAY = 58;
                BREAK_ADD = 3;
                break;
            case 2:
                LEVEL_START = 9;
                LEVEL_END = 7;
                PROSPECT_P = 25;
                PROSPECT_N = 8;
                BREAK_DECAY = 76;
                BREAK_ADD = 3;
                break;
            case 4:
                LEVEL_START = 15;
                LEVEL_END = 8;
                PROSPECT_P = 24;
                PROSPECT_N = 14;
                BREAK_DECAY = 74;
                BREAK_ADD = 6;
                break;
            case 8:
                LEVEL_START = 14;
                LEVEL_END = 7;
                PROSPECT_P = 41;
                PROSPECT_N = 6;
                BREAK_DECAY = 78;
                BREAK_ADD = 8;
                break;
            case 16:
                LEVEL_START = 12;
                LEVEL_END = 8;
                PROSPECT_P = 49;
                PROSPECT_N = 5;
                BREAK_DECAY = 81;
                BREAK_ADD = 14;
                break;
            case 32:
                LEVEL_START = 16;
                LEVEL_END = 8;
                PROSPECT_P = 92;
                PROSPECT_N = 4;
                BREAK_DECAY = 82;
                BREAK_ADD = 18;
                break;
            case 64:
                LEVEL_START = 9;
                LEVEL_END = 8;
                PROSPECT_P = 59;
                PROSPECT_N = 4;
                BREAK_DECAY = 85;
                BREAK_ADD = 21;
                break;
            case 128:
                LEVEL_START = 15;
                LEVEL_END = 8;
                PROSPECT_P = 137;
                PROSPECT_N = 2;
                BREAK_DECAY = 86;
                BREAK_ADD = 35;
                break;
            }

        // 掘削済み。
        bool excavated[N][N] = {};

        int Smin[N][N];
        int Smax[N][N];
        for (int y=0; y<N; y++)
            for (int x=0; x<N; x++)
            {
                Smin[y][x] = 0;
                Smax[y][x] = 5000;
            }

        // 試掘。
        auto prospect = [&](int x, int y)
        {
            int P = PROSPECT_P;
            for (int i=0; i<PROSPECT_N; i++)
                if (excavate(x, y, P)==1)
                {
                    excavated[y][x] = true;
                    Smax[y][x] = Smin[y][x]+P;
                    break;
                }
                else
                {
                    Smin[y][x] += P;
                }
        };

        // 補間。
        auto interpolate = [&](int level, int S[N][N], int S2[N][N])
        {
            for (int y=0; y<N; y++)
                for (int x=0; x<N; x++)
                {
                    if (level%2==0)
                    {
                        int d = 1<<(level/2);
                        int rx = ((x-N/2)%d+d)%d;
                        int ry = ((y-N/2)%d+d)%d;
                        // s0 s1
                        // s2 s3
                        int s0 = 0<=x-rx  && 0<=y-ry  ? S[y-ry][x-rx]     : 0<=x-rx  ? S[y-ry+d][x-rx]   : 0<=y-ry  ? S[y-ry][x-rx+d]   : S[y-ry+d][x-rx+d];
                        int s1 = x-rx+d<N && 0<=y-ry  ? S[y-ry][x-rx+d]   : x-rx+d<N ? S[y-ry+d][x-rx+d] : 0<=y-ry  ? S[y-ry][x-rx]     : S[y-ry+d][x-rx];
                        int s2 = 0<=x-rx  && y-ry+d<N ? S[y-ry+d][x-rx]   : 0<=x-rx  ? S[y-ry][x-rx]     : y-ry+d<N ? S[y-ry+d][x-rx+d] : S[y-ry][x-rx+d];
                        int s3 = x-rx+d<N && y-ry+d<N ? S[y-ry+d][x-rx+d] : x-rx+d<N ? S[y-ry][x-rx+d]   : y-ry+d<N ? S[y-ry+d][x-rx]   : S[y-ry][x-rx];
                        S2[y][x] = (s0*(d-rx)*(d-ry)+s1*rx*(d-ry)+s2*(d-rx)*ry+s3*rx*ry+d*d/2)/(d*d);
                    }
                    else
                    {
                        int d = 1<<(level/2);
                        int rx = (((x-N/2)+d)%(d*2)+d*2)%(d*2)-d;
                        int ry = (((y-N/2)  )%(d*2)+d*2)%(d*2)-d;
                        if (-rx-ry>d)
                        {
                            rx += d;
                            ry += d;
                        }
                        else if (rx-ry>d)
                        {
                            rx -= d;
                            ry += d;
                        }
                        else if (-rx+ry>d)
                        {
                            rx += d;
                            ry -= d;
                        }
                        else if (rx+ry>d)
                        {
                            rx -= d;
                            ry -= d;
                        }
                        //   s0
                        // s1  s2
                        //   s3
                        int s0p = 0<=x-rx   && x-rx  <N && 0<=y-ry-d && y-ry-d<N ? S[y-ry-d][x-rx  ] : -1;
                        int s1p = 0<=x-rx-d && x-rx-d<N && 0<=y-ry   && y-ry  <N ? S[y-ry  ][x-rx-d] : -1;
                        int s2p = 0<=x-rx+d && x-rx+d<N && 0<=y-ry   && y-ry  <N ? S[y-ry  ][x-rx+d] : -1;
                        int s3p = 0<=x-rx   && x-rx  <N && 0<=y-ry+d && y-ry+d<N ? S[y-ry+d][x-rx  ] : -1;

                        int s0 = s0p;
                        if (s0==-1)
                        {
                            if (s1p==-1 && s2p==-1)
                                s0 = s3p;
                            else if (s1p==-1 && s3p==-1)
                                s0 = s2p;
                            else if (s2p==-1 && s3p==-1)
                                s0 = s1p;
                            else if (s1p==-1)
                                s0 = s2p;
                            else if (s2p==-1)
                                s0 = s1p;
                            else
                                s0 = (s1p+s2p)/2;
                        }
                        int s1 = s1p;
                        if (s1==-1)
                        {
                            if (s0p==-1 && s2p==-1)
                                s1 = s3p;
                            else if (s0p==-1 && s3p==-1)
                                s1 = s2p;
                            else if (s2p==-1 && s3p==-1)
                                s1 = s0p;
                            else if (s0p==-1)
                                s1 = s3p;
                            else if (s3p==-1)
                                s1 = s0p;
                            else
                                s1 = (s0p+s3p)/2;
                        }
                        int s2 = s2p;
                        if (s2==-1)
                        {
                            if (s0p==-1 && s1p==-1)
                                s2 = s3p;
                            else if (s0p==-1 && s3p==-1)
                                s2 = s1p;
                            else if (s1p==-1 && s3p==-1)
                                s2 = s0p;
                            else if (s0p==-1)
                                s2 = s3p;
                            else if (s3p==-1)
                                s2 = s0p;
                            else
                                s2 = (s0p+s3p)/2;
                        }
                        int s3 = s3p;
                        if (s3==-1)
                        {
                            if (s0p==-1 && s1p==-1)
                                s3 = s2p;
                            else if (s0p==-1 && s2p==-1)
                                s3 = s1p;
                            else if (s1p==-1 && s2p==-1)
                                s3 = s0p;
                            else if (s1p==-1)
                                s3 = s2p;
                            else if (s2p==-1)
                                s3 = s1p;
                            else
                                s3 = (s1p+s2p)/2;
                        }
                        if (s0>=0)
                            S2[y][x] = ((d-(rx+ry))*(d-(ry-rx))*s0+(d-(rx+ry))*(d+(ry-rx))*s1+(d+(rx+ry))*(d-(ry-rx))*s2+(d+(rx+ry))*(d+(ry-rx))*s3+2*d*d)/(4*d*d);
                        else
                            S2[y][x] = 5000;
                    }
                }
        };

        // 頑丈さがSと仮定し、掘削するべき場所を返す。
        auto dijkstra = [&](int S[N][N], bool R[N][N])
        {
            for (int y=0; y<N; y++)
                for (int x=0; x<N; x++)
                    R[y][x] = false;

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
                    D[y][x] = S[y][x]+C;
                    PX[y][x] = -1;
                    Q.push({-D[y][x], {x, y}});
                }

                int DX[] = {1, -1, 0, 0};
                int DY[] = {0, 0, 1, -1};

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
                        if (0<=tx && tx<N && 0<=ty && ty<N &&
                            D[y][x]+S[ty][tx]+C<D[ty][tx])
                        {
                            D[ty][tx] = D[y][x]+S[ty][tx]+C;
                            PX[ty][tx] = x;
                            PY[ty][tx] = y;
                            Q.push({-D[ty][tx], {tx, ty}});
                        }
                    }
                }

                int x = hx;
                int y = hy;
                while (x>=0)
                {
                    R[y][x] = true;
                    S[y][x] = 0;

                    int nx = PX[y][x];
                    int ny = PY[y][x];
                    x = nx;
                    y = ny;
                }
            }
        };

        int level = LEVEL_START;

        for (int y=0; y<N; y++)
            for (int x=0; x<N; x++)
                if (level%2==0)
                {
                    int d = 1<<(level/2);
                    if ((x-N/2)%d==0 && (y-N/2)%d==0)
                        prospect(x, y);
                }
                else
                {
                    int d = 1<<(level/2);
                    if ((x-N/2)%d==0 && (y-N/2)%d==0 && (((x-N/2)/d^(y-N/2)/d)&1)==0)
                        prospect(x, y);
                }

        for (; level>LEVEL_END; level--)
        {
            int S2[N][N];
            interpolate(level, Smax, S2);
            for (int y=0; y<N; y++)
                for (int x=0; x<N; x++)
                    if (excavated[y][x])
                        S2[y][x] = 0;
            for (int h=0; h<HN; h++)
                S2[HY[h]][HX[h]] = 0;

            bool R[N][N];
            dijkstra(S2, R);

            // 掘削する場所に近い試掘場所をマーク。
            bool M[N][N] = {};

            for (int y=0; y<N; y++)
                for (int x=0; x<N; x++)
                    if (R[y][x])
                    {
                        int tx, ty;
                        if (level%2==0)
                        {
                            int d = 1<<(level/2);
                            int rx = ((x-N/2)%d+d)%d;
                            int ry = ((y-N/2)%d+d)%d;
                            if (rx<d/2)
                                tx = x-rx;
                            else
                                tx = x-rx+d;
                            if (ry<d/2)
                                ty = y-ry;
                            else
                                ty = y-ry+d;
                        }
                        else
                        {
                            int d = 1<<(level/2);
                            int rx = (((x-N/2)+d)%(d*2)+d*2)%(d*2)-d;
                            int ry = (((y-N/2)  )%(d*2)+d*2)%(d*2)-d;
                            tx = x-rx;
                            ty = y-ry;
                            if (ry<=0 && abs(rx)<=abs(ry))
                                ty -= d;
                            else if (abs(rx)<abs(ry))
                                ty += d;
                            else if (rx<0)
                                tx -= d;
                            else
                                tx += d;
                        }
                        if (0<=tx && tx<N && 0<=ty && ty<N)
                            M[ty][tx] = true;
                    }

            // 次のレベルの試掘。
            for (int y=0; y<N; y++)
                for (int x=0; x<N; x++)
                {
                    int d1 = 1<<(level/2);
                    int d2 = 1<<((level-1)/2);
                    if (level%2==0)
                    {
                        if (!((x-N/2)%d1==0 && (y-N/2)%d1==0) &&
                            (x-N/2)%d2==0 && (y-N/2)%d2==0 && (((x-N/2)/d2^(y-N/2)/d2)&1)==0)
                        {
                            int dx[] = {1, -1, 1, -1};
                            int dy[] = {1, 1, -1, -1};
                            for (int d=0; d<4; d++)
                            {
                                int tx = x+dx[d]*d2;
                                int ty = y+dy[d]*d2;
                                if (0<=tx && tx<N && 0<=ty && ty<N &&
                                    M[ty][tx])
                                {
                                    prospect(x, y);
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (!((x-N/2)%d1==0 && (y-N/2)%d1==0 && (((x-N/2)/d1^(y-N/2)/d1)&1)==0) &&
                            (x-N/2)%d2==0 && (y-N/2)%d2==0)
                        {
                            int dx[] = {1, -1, 0, 0};
                            int dy[] = {0, 0, 1, -1};
                            for (int d=0; d<4; d++)
                            {
                                int tx = x+dx[d]*d2;
                                int ty = y+dy[d]*d2;
                                if (0<=tx && tx<N && 0<=ty && ty<N &&
                                    M[ty][tx])
                                {
                                    prospect(x, y);
                                    break;
                                }
                            }
                        }
                    }
                }
        }

        // 掘削
        int S2[N][N];
        interpolate(level, Smax, S2);
        for (int y=0; y<N; y++)
            for (int x=0; x<N; x++)
                if (excavated[y][x])
                    S2[y][x] = 0;
        for (int h=0; h<HN; h++)
            S2[HY[h]][HX[h]] = 0;

        bool R[N][N];
        dijkstra(S2, R);

        bool U[N][N] ={};
        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};

        function<void(int, int)> breakRock = [&](int x, int y)
        {
            if (U[y][x])
                return;
            U[y][x] = true;

            if (!excavated[y][x])
            {
                excavated[y][x] = true;
                // 周囲の破壊に必要だったパワーの平均を求め、それを少し減らしたパワーで掘削する。
                // ここでの掘削は基本的に一撃なので、Sminは見ない。
                int ps = 0;
                int n = 0;
                for (int d=0; d<4; d++)
                {
                    int tx = x+dx[d];
                    int ty = y+dy[d];
                    if (0<=tx && tx<N && 0<=ty && ty<N && excavated[ty][tx])
                    {
                        ps += Smax[ty][tx];
                        n ++;
                    }
                }
                int p = 100;
                if (n>0)
                    p = (ps+n/2)/n;

                if (excavate(x, y, max(10, p*BREAK_DECAY/100))!=0)
                {
                    Smax[y][x] = Smin[y][x]+max(10, p*BREAK_DECAY/100);
                }
                else
                {
                    Smin[y][x] += max(10, p*BREAK_DECAY/100);
                    while (true)
                        if (excavate(x, y, max(10, p*BREAK_ADD/100))==0)
                            Smin[y][x] += max(10, p*BREAK_ADD/100);
                        else
                        {
                            Smax[y][x] = Smin[y][x]+max(10, p*BREAK_ADD/100);
                            break;
                        }
                }
            }

            for (int d=0; d<4; d++)
            {
                int tx = x+dx[d];
                int ty = y+dy[d];
                if (0<=tx && tx<N && 0<=ty && ty<N && R[ty][tx])
                    breakRock(tx, ty);
            }
        };
        for (int y=0; y<N; y++)
            for (int x=0; x<N; x++)
                if (R[y][x] && !U[y][x])
                    breakRock(x, y);
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

int main(int argc, char **argv)
{
    //SolverSample solver;
    SolverMain solver;

    if (argc>1)
    {
        solver.LEVEL_START = atoi(argv[1]);
        solver.LEVEL_END = atoi(argv[2]);
        solver.PROSPECT_P = atoi(argv[3]);
        solver.PROSPECT_N = atoi(argv[4]);
        solver.BREAK_DECAY = atoi(argv[5]);
        solver.BREAK_ADD = atoi(argv[6]);
    }

#ifdef TOPCODER_LOCAL
    solveLocal(&solver);
#else
    solveJudge(&solver);
#endif
}
