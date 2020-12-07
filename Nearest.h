#ifndef Nearest_H
#define Nearest_H
#include<string>
#include<fstream>
#include<ctime>
#include<cmath>
#include<cstdlib>
#include<vector>
using namespace std;
class Nearest{
public:
    Nearest()=default;
    Nearest(const string& filename,int(*dist)(int,int,double[][2]));
    ~Nearest();
    void run();
    int best_length;
    int*best_path;

private:
#define  N_INF 100000
    int**distance;
    ifstream infile;
    int(*dist)(int,int,double [][2]);
    void initDist();
    int N;
    double (*city)[2];
};
#endif // Nearest_H
