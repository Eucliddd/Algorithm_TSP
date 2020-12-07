//
// Created by Administrator on 2020/7/16.
//

#ifndef SA_TSP_SA_H
#define SA_TSP_SA_H
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#define GREEDY 1
using namespace std;
class SA{
public:
    SA()=default;
    SA(const string& filename,int(*dist)(int,int,double[][2]));
    ~SA();
    void run();
    int best_length;
    int* best_path;

private:
#define T     3000    //初始温度
#define EPS   1e-3    //终止温度
#define DELTA 0.98    //温度衰减率
#define LIMIT 10000   //概率选择上限
#define OLOOP 20000    //外循环次数
#define ILOOP 100   //内循环次数
#define INF 100000
    int** distance;
    int cur_len;
    int* cur_path;
    int new_len;
    int* new_path;
    ifstream infile;
    int (*dist)(int, int,double [][2]);
    int N;
    double (*city)[2];
    void init();
    void getNext();

};
#endif //SA_TSP_SA_H
