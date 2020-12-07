//
// Created by Lenovo on 2020/7/16.
//

#ifndef TSP_TABU_H
#define TSP_TABU_H

#include <bits/stdc++.h>
using namespace std;
struct Tabu{
    int best_length;
    int *best_path;
    double times;
    int nodeNum;
    void run();
    void output(const string &fileName);
    void youhua(int start,int *path);//start起始下标相对0的增量
    Tabu(const string&filename,int(*dist)(int,int,double[][2]));
};


#endif //TSP_TABU_H
