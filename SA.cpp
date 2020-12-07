//
// Created by Administrator on 2020/7/16.
//
#include <iostream>
#include "SA.h"
#include <cstring>
#include <vector>

SA::SA(const string &filename, int (*dist)(int, int, double (*)[2])):infile(filename),dist(dist) {}

void SA::run() {
    clock_t start,end;
    double duration;
    start=clock();
    init();
    srand(time(0));
    double t=T;
    while(true){
        for(int i=0;i<ILOOP;i++)
        {
            getNext();
            double dE=new_len-cur_len;
            if(dE<0){
                for(int j=0;j<N;j++)
                    cur_path[j]=new_path[j];
                cur_len=new_len;
            }
            else
            {
                double p=rand() / (RAND_MAX+1.0);
                if(exp(dE/t)>p && exp(dE/t) < 1) {
                    for (int j = 0; j < N; j++)
                        cur_path[j] = new_path[j];
                    cur_len=new_len;
                }
            }
        }
        if(cur_len<best_length){
            for (int i = 0; i < N; i++)
                best_path[i] = cur_path[i];
            best_length=cur_len;
        }
        if(t<EPS)
            break;
        t *= DELTA;
    }
    end=clock();
    duration=(double )(end-start)/CLOCKS_PER_SEC;
    cout<<best_length<<endl;
    for(int k=0;k<N;k++)
        cout<<best_path[k]<<"->";
    cout<<best_path[0]<<endl;
    ofstream fout("./data.txt");
    fout<<best_length<<endl;
    fout<<duration<<endl;
    for(int i=0;i<N;i++)
        fout<<city[best_path[i]][0]<<' '<<city[best_path[i]][1]<<endl;
    fout<<city[best_path[0]][0]<<' '<<city[best_path[0]][1]<<endl;
//    double tmp=0;
//    for(int k=1;k<N;k++){
//        tmp+=distance[best_path[k-1]][best_path[k]];
//    }
//    tmp+=distance[best_path[N-1]][best_path[0]];
//    cout<<tmp<<endl;

}

SA::~SA() {
    delete[] city;
    delete[] best_path;
    delete[] cur_path;
    delete[] new_path;
    for(int i=0;i<N;i++){
        delete[] distance[i];
    }
    delete[] distance;
}

void SA::init() {
    string tmp;
    while (infile >> tmp)if (tmp == "DIMENSION:"||tmp=="DIMENSION")break;
    if(tmp=="DIMENSION:"){
        infile >> N;
    }else{
        infile>>tmp;
        infile >> N;
    }
    while (infile >> tmp)if (tmp == "NODE_COORD_SECTION")break;
    int n;
    double x,y;
    best_path = new int[N];
    cur_path = new int[N];
    new_path = new int[N];
    best_length = 0;
    city = new double[N][2];
    for(int i=0;i<N;i++){
        infile>>n>>x>>y;
        city[n-1][0]=x;
        city[n-1][1]=y;
        //cout<<x<<' '<<y<<endl;
    }
    distance = new int*[N];
    for(int i=0;i<N;i++){
        distance[i]=new int[N];
        for(int j=0;j<N;j++){
            if(i == j)
                distance[i][j] = INF;
            else
                distance[i][j] = dist(i,j,city);
            //cout<<distance[i][j]<<' ';
        }
    }
#if GREEDY==1
    bool* visited=new bool[N];
    memset(visited,0,sizeof(bool)*N);
    best_path[0]=cur_path[0]=new_path[0]=0;
    int cur=0;
    visited[0]=true;
    for(int i=1;i<N;i++){
        int index=0;
        for(int k=0;k<N;k++){
            if(!visited[k]){
                if(index==0) index=k;
                if(distance[cur][index]>distance[cur][k])
                    index=k;
            }
        }
        best_length+=distance[cur][index];
        visited[index]=true;
        best_path[i]=cur_path[i]=new_path[i]=index;
        cur=index;
    }
    best_length+=distance[cur][0];
    delete[] visited;
#else
    for(int i=0;i<N;i++){
        best_path[i]=cur_path[i]=new_path[i]=i;
        if(i!=N-1)
            best_length+=distance[i][i+1];
        else
            best_length+=distance[N-1][0];
    }
#endif
    cur_len=new_len=best_length;
    //cout<<best_length<<endl;
}

void SA::getNext() {
    for (int i = 0; i < N; i++) {
        new_path[i] = cur_path[i];
        //cout<<new_path[i]<<' ';
    }
    //cout<<endl;
    if(rand()/(RAND_MAX+1.0)<0.5) {
        int x = rand() % N;
        int y = rand() % N;
        while (x == y) {
            x = rand() % N;
            y = rand() % N;
        }

        if (x > y) swap(x, y);
        //cout<<x<<' '<<y<<endl;
        for (int i = x, j = y; i < j; ++i, --j)
            swap(new_path[i], new_path[j]);
    }
    else{
        int loc1=rand() % N,loc2=rand() % N,loc3=rand() % N;
        while(loc1==loc2||loc2==loc3||loc3==loc1){
            loc1=rand() % N;
            loc2=rand() % N;
            loc3=rand() % N;
        }
        if(loc1>loc2) swap(loc1,loc2);
        if(loc2>loc3) swap(loc2,loc3);
        if(loc1>loc2) swap(loc1,loc2);
        //cout<<loc1<<' '<<loc2<<' '<<loc3<<endl;
        vector<int> v;
        for(int i=loc1;i<loc2;i++)
            v.push_back(new_path[i]);
        for(int i=loc2,j=loc1;i<=loc3;i++,j++){
            new_path[j]=new_path[i];
        }
        for(int i=loc3-loc2+1+loc1,j=0;i<=loc3;i++,j++){
            new_path[i]=v[j];
        }
    }
//    for (int i = 0; i < N; i++) {
//        cout<<new_path[i]<<' ';
//    }
//    cout<<endl<<endl;
    new_len=0;
    for(int i=1;i<N;i++)
        new_len+=distance[new_path[i-1]][new_path[i]];
    new_len+=distance[new_path[N-1]][new_path[0]];

//    double prob=0;
//    int insert=0;
//    double p=rand()/(RAND_MAX+1.0);
//    for(;insert<N;insert++){
//        prob+=distance[cur_path[(insert-1+N)%N]][cur_path[insert]]/cur_len;
//        if(p<prob)
//            break;
//    }
//    double max=0;
//    double sum=0;
//    for(int i=0;i<N;i++){
//        if(i!=insert){
//            if(distance[insert][i]>max)
//                max=distance[insert][i];
//            sum+=distance[insert][i];
//        }
//    }
//    p=rand()/(RAND_MAX+1.0);
//    prob=0;
//    int j=0;
//    for(;j<N;j++){
//        if(j!=insert){
//            prob+=(max-distance[cur_path[insert]][cur_path[j]])/((N-1)*max-sum);
//            if(p<prob)
//                break;
//        }
//    }
//    if(j>insert) {
//        int tmp=new_path[j];
//        for (int k = insert; k < j; k++) {
//            new_path[k+1]=new_path[k];
//        }
//        new_path[insert]=tmp;
//    }
//    else{
//        int tmp=new_path[j];
//        for(int k=0;k<j;k++){
//            new_path[k+1]=new_path[k];
//        }
//        new_path[0]=new_path[N-1];
//        for(int k=insert;k<N-1;k++){
//            new_path[k+1]=new_path[k];
//        }
//        new_path[insert]=tmp;
//    }
//    new_len=0;
//    for(int i=1;i<N;i++)
//        new_len+=distance[new_path[i-1]][new_path[i]];
//    new_len+=distance[N-1][0];
}