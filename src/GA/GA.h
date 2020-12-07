//
// Created by Lenovo on 2020/7/15.
//
#ifndef TSP_GA_H
#define TSP_GA_H

#include "../Greedy/Greedy.h"
//n顶点数
//distance距离函数
struct GA{
    int best_length;
    int *best_path;
    double times;
    GA(const string&filename,int(*dist)(int,int,double[][2]));
    void run();
    int nodeNum;
    void output(const string&fileName);
    void youhua(int start,int *path);//起始下标相对于0的增量
};

#endif //TSP_GA_H
