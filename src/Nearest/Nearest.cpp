#include"Nearest.h"
#include<iostream>
#include<cstring>
#include<vector>

Nearest::Nearest(const string& filename,int(*dist)(int,int,double (*)[2])):infile(filename),dist(dist){}

void Nearest::run(){
    clock_t start,end;
    double duration;
    start=clock();
    initDist();
    bool* visited=new bool[N];
    memset(visited,0,sizeof(bool)*N);
    best_path[0]=0;
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
        best_path[i]=index;
        cur=index;
    }
    best_length+=distance[cur][0];
    delete[] visited;
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
}

Nearest::~Nearest() {
    delete[] city;
    delete[] best_path;

    for(int i=0;i<N;i++){
        delete[] distance[i];
    }
    delete[] distance;
}
void Nearest::initDist(){
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
best_path=new int[N];
best_length=0;
city=new double[N][2];
for(int i=0;i<N;i++)
{
    infile>>n>>x>>y;
    city[n-1][0]=x;
    city[n-1][1]=y;
}
distance=new int*[N];
for(int i=0;i<N;i++)
{
    distance[i]=new int[N];
    for(int j=0;j<N;j++)
    {
        if(i==j)
            distance[i][j]==N_INF;
        else
            distance[i][j]=dist(i,j,city);

    }
}
}

