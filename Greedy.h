//
// Created by Lenovo on 2020/7/15.
//

#ifndef TSP_GREEDY_H
#define TSP_GREEDY_H

#include <bits/stdc++.h>
using namespace std;
//n顶点数
//distance距离函数
pair<int,vector<int>> Greedy(int n,int(*distance)(int,int));
#endif //TSP_GREEDY_H
