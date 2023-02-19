#include <iostream>
#include <vector>
#include <functional>
using namespace std;

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

int main()
{
    SolverSample solver;
    solveJudge(&solver);
}
