#ifndef OPT2_H
#define OPT2_H
#include<string>
#include<fstream>
#include<ctime>
#include<cmath>
#include<cstdlib>
#include<vector>
using namespace std;
class OPT2{
public:
    OPT2()=default;
    OPT2(const string& filename,int(*dist)(int,int,double[][2]));
    ~OPT2();
    void run();
    int best_length;
    int*best_path;

private:
    #define MAXCOUNT 1000
    #define  O_INF 100000
    int N;
    double (*city)[2];
    int**distance;
    ifstream infile;
    int(*dist)(int,int,double [][2]);
    void initDist();
};
#endif // OPT2_H
