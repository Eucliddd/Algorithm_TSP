//
// Created by Lenovo on 2020/7/15.
//
#include <bits/stdc++.h>
#include "Greedy.h"
using namespace std;
int(*dis)(int,int);
pair<int,vector<int>> Greedy(int n,int(*distance)(int,int)){
    dis=distance;
    bool visted[n+1];
    memset(visted,0,sizeof(visted));
    vector<int>route;
    route.push_back(1);//固定起点为1
    visted[route.front()]=1;

    int length=0;
    for(int i=1;i<n;i++){
        int next = 0;
        int nextLen=-1;
        for(int j=1;j<=n;j++){
            if(!visted[j]){
                if(nextLen==-1||nextLen>dis(route.back(),j)){
                    next=j;
                    nextLen=dis(route.back(),j);
                }
            }
        }
        visted[next]=true;
        length+=nextLen;
        route.push_back(next);
    }
    length+=dis(route.back(),route.front());
    route.push_back(route.front());
    return make_pair(length,route);
}