#include"OPT-2.h"
#include<iostream>
#include<cstring>
#include<vector>

OPT2::OPT2(const string& filename,int(*dist)(int,int,double (*)[2])):infile(filename),dist(dist){}

void OPT2::run(){
    clock_t start,end;
    double duration;
    start=clock();
    initDist();
    int count=0;
    int pay;
    best_path[0]=0;
    for(int i=1;i<N;i++)
    {
        best_path[i]=i;
        best_length+=distance[i][i-1];

    }
    best_path[N]=0;
    best_length+=distance[N-1][0];
    while(count<MAXCOUNT)
    {   int k=rand()%(N-1)+1;//[1,n-1]
        int m=rand()%(N-1)+1;
        if(m!=k)
        {
            if(k>m) swap(k,m);
            pay=(distance[best_path[k-1]][best_path[k]]+distance[best_path[m]][best_path[m+1]])-(distance[best_path[k-1]][best_path[m]]+distance[best_path[k]][best_path[m+1]]);
            if(pay>0)
            {
                while(k<m)
                {
                    swap(best_path[k],best_path[m]);
                    k++;
                    m--;
                }
                count=0;
                best_length-=pay;
            }
            count++;
        }
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
}


OPT2::~OPT2() {
    delete[] city;
    delete[] best_path;

    for(int i=0;i<N;i++){
        delete[] distance[i];
    }
    delete[] distance;
}

void OPT2::initDist(){
    string tmp;
    while (infile >> tmp)if (tmp == "DIMENSION:"||tmp=="DIMENSION")break;
    if(tmp=="DIMENSION:"){
        infile >> N;
    }else{
        infile>>tmp;
        infile >> N;
    }
    while (infile >> tmp)if (tmp == "NODE_COORD_SECTION")break;
    best_length=0;
    int n;
    double x,y;
    best_path=new int[N+1];
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
            distance[i][j]==O_INF;
        else
            distance[i][j]=dist(i,j,city);

        }
    }
}

