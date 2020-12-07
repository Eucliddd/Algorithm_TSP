//
// Created by Administrator on 2020/7/15.
//

#ifndef AS_TSP_ANTCOLONY_H
#define AS_TSP_ANTCOLONY_H
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <ctime>
using namespace std;
#define OPTIMIZE 1
class AntColony{
public:
    AntColony()=default;
    AntColony(const string& filename,int (*dist)(int,int,double [][2]));    //构造函数，初始化tsp文件和距离函数
    ~AntColony();
    void run();                                                               //对外接口
    int best_length;                                                       //最短路径长度
    int* best_path;                                                           //最短路径
    vector<int> length_per_ic;
private:
#define Alpha 1
#define Beta 7
#define  Rho 0.15
#define I_MAX 200
#define Q 100
#define eps 100000
    int N;                                                                    //城市数
    int M;
    double (*city)[2];                                                        //城市坐标
    int** distance;                                                        //距离矩阵
    int** Tabu;
    double** Tau;
    double** Eta;
    bool** allow;
    double* prob;
    double* cumsum;
    ifstream infile;
    int (*dist)(int, int,double [][2]);                                    //距离函数
    void initDist();
    void initTabu();
    void visitCity();
    double update();
};
#endif //AS_TSP_ANTCOLONY_H
