#include <iostream>
#include "AntColony.h"
#include <cstring>

AntColony::AntColony(const string &filename, int (*dist)(int, int,double [][2])):infile(filename),dist(dist){

}

AntColony::~AntColony() {
    delete[] city;
    delete[] best_path;
    delete[] prob;
    delete[] cumsum;
    for(int i=0;i<N;i++){
        delete[] distance[i];
        delete[] Eta[i];
        delete[] Tau[i];
    }
    delete[] distance;
    delete[] Eta;
    delete[] Tau;
    for(int i=0;i<M;i++){
        delete[] Tabu[i];
        delete[] allow[i];
    }
    delete[] Tabu;
    delete[] allow;

}

void AntColony::run() {
    srand(time(0));
    clock_t start,end;
    double duration;
    start=clock();
    initDist();
    for(int ic=0;ic<I_MAX;ic++){
        initTabu();
        visitCity();
        length_per_ic.push_back(update());
    }
    end=clock();
    duration=(double )(end-start)/CLOCKS_PER_SEC;
    cout<<duration<<"sec"<<endl;
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
//    int tmp=0;
//    for(int k=1;k<N;k++){
//        tmp+=distance[best_path[k-1]][best_path[k]];
//    }
//    tmp+=distance[best_path[N-1]][best_path[0]];
//    cout<<tmp<<endl;
}

void AntColony::initDist() {
    string tmp;
    while (infile >> tmp)if (tmp == "DIMENSION:"||tmp=="DIMENSION")break;
    if(tmp=="DIMENSION:"){
        infile >> N;
    }else{
        infile>>tmp;
        infile >> N;
    }                   //读取城市数
    M = (int)(1.5*N);
    while (infile >> tmp)if (tmp == "NODE_COORD_SECTION")break;
    int n;
    double x,y;
    best_length = eps*N;
    best_path = new int[N];
    city = new double[N][2];
    for(int i=0;i<N;i++){
        infile>>n>>x>>y;                               //读取城市坐标
        city[n-1][0]=x;
        city[n-1][1]=y;
        //cout<<x<<' '<<y<<endl;
    }
    distance = new int*[N];
    Eta = new double*[N];
    Tau = new double*[N];
    int min=eps;
    for(int i=0;i<N;i++){
        distance[i]=new int[N];
        Eta[i]=new double[N];
        Tau[i]=new double[N];
        for(int j=0;j<N;j++){
            if(i == j)
                distance[i][j] = eps;
            else
                distance[i][j] = dist(i,j,city);       //计算城市之间的距离
            //cout<<distance[i][j]<<' ';
            Eta[i][j]=1.0/distance[i][j];
            if(distance[i][j]<min)
                min=distance[i][j];
        }
    }

    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            Tau[i][j] = 1/(Rho*min*N);

    Tabu = new int*[M];
    allow = new bool*[M];
    prob=new double[N];
    cumsum=new double[N];
    for(int i=0;i<M;i++){
        Tabu[i] = new int[N];
        allow[i] = new bool[N];
    }
    infile.close();
}

void AntColony::initTabu() {
    for(int i=0;i<M;i++){
        for(int j=0;j<N;j++){
            Tabu[i][j]=-1;
            allow[i][j]= true;
        }
        //double p=rand() % N;
        Tabu[i][0] = rand() % N;
        allow[i][Tabu[i][0]] = false;
    }
}

void AntColony::visitCity() {
    for(int i=1;i<N;i++){
        double sum=0;
        int city_now=-1;
        for(int j=0;j<M;j++){
            sum = 0;
            city_now = Tabu[j][i-1];
            vector<int> nvisit;
            for(int k=0;k<N;k++){
                if(allow[j][k])
                    nvisit.push_back(k);
            }
            for(int k=0;k<nvisit.size();++k){
                prob[k] = pow(Tau[city_now][nvisit[k]],Alpha)*pow(Eta[city_now][nvisit[k]],Beta);
                sum += prob[k];
            }
            for(int k=0;k<nvisit.size();++k)
                prob[k] = prob[k]/sum;
            cumsum[0] = prob[0];
            for(int k=1;k<nvisit.size();k++){
                cumsum[k]=cumsum[k-1]+prob[k];
            }
            double p=rand()/(double)(RAND_MAX+1.0);
            //cout<<p<<' '<<cumsum[N-1]<<endl;
            for(int k=0;k<nvisit.size();k++){
                if(p<=cumsum[k]){
                    Tabu[j][i] = nvisit[k];
                    allow[j][nvisit[k]] = false;
                    break;
                }
            }
            if(Tabu[j][i]==-1){
                //double tmp=cumsum[nvisit.size()-1];
                Tabu[j][i]=nvisit.back();

            }
            memset(prob,0,sizeof(double)*N);
            memset(cumsum,0,sizeof(double)*N);
        }
    }
}

double AntColony::update() {

    int lenP=0;
    int min=eps*N;
    int index=0;
    vector<pair<int,int>> tmp;
    for(int i=0;i<M;i++) {
        int* Ri=Tabu[i];
        lenP=0;
        for (int j = 1; j < N; j++)
            lenP+= distance[Ri[j-1]][Ri[j]];
        lenP+=distance[Ri[N-1]][Ri[0]];
        tmp.push_back(make_pair(lenP,i));
        if(lenP<min){
            min=lenP;
            index=i;
        }
    }
#if OPTIMIZE==1
    void GetKNumbers(vector<pair<int,int>>& res,int k);
    GetKNumbers(tmp,M/2);
    void two_opt(int N,int** D,int* path,int& cost);
    for(int i=0;i<M/2;i++){
        int k=tmp[i].second;
        two_opt(N,distance,Tabu[k],tmp[i].first);
        if(tmp[i].first<min){
            min=tmp[i].first;
            index=k;
        }
    }
#endif
    int* R_best=Tabu[index];
    if(min<best_length) {
        best_length = min;
        for(int i=0;i<N;i++)
            best_path[i]=R_best[i];
    }

    double TauMax=1/(Rho*best_length);
    double pbest=0.05;
    pbest=pow(pbest,1/N);
    double TauMin=TauMax*(1-pbest)/((N/2.0-1)*pbest);

    double** DeltaT=new double*[N];
    for(int i=0;i<N;i++){
        DeltaT[i]=new double[N];
        memset(DeltaT[i],0,sizeof(double)*N);
    }
    for(int i=1;i<N;i++)
        DeltaT[R_best[i-1]][R_best[i]]+=1.0/min;
    DeltaT[R_best[N-1]][R_best[0]]+=1.0/min;
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++) {
            Tau[i][j] = (1 - Rho) * Tau[i][j] + DeltaT[i][j];
            if(Tau[i][j]>TauMax) Tau[i][j]=TauMax;
            else if(Tau[i][j]<TauMin) Tau[i][j]=TauMin;
        }

    for(int i=0;i<N;i++)
        delete[] DeltaT[i];
    delete[] DeltaT;

    return min;
}

int Partition(vector<pair<int,int>> &res,int s,int e)
{
    auto tp=res[s];
    while(s<e)
    {
        while(s<e&&res[e].first>=tp.first)e--;
        res[s]=res[e];
        while(s<e&&res[s].first<=tp.first)s++;
        res[e]=res[s];
    }
    res[s]=tp;
    return s;
}

void GetKNumbers(vector<pair<int,int>>& res,int k)
{
    int s=0,e=res.size()-1;
    int index=Partition(res,s,e);
    while(index!=k-1)
    {
        if(index>k-1)e=index-1;
        else s=index+1;
        index=Partition(res,s,e);
    }
}

void two_opt(int N,int** D,int* path,int& cost){
#define MAXCOUNT 100
    int count=0;
    while(count<MAXCOUNT){
        int k,m;
        int pay=0;
        do{
            k=rand()%N;
            m=rand()%N;
        }while(k==m);
        if(k>m) swap(k,m);
        pay=(D[path[(k-1+N)%N]][path[k]]+D[path[m]][path[(m+1)%N]])-(D[path[(k-1+N)%N]][path[m]]+D[path[k]][path[(m+1)%N]]);
        if(pay>0)
        {
            while(k<m)
            {
                swap(path[k],path[m]);
                k++;
                m--;
            }
            cost-=pay;
            count=0;
        }
        count++;
    }
}


//
// Created by Administrator on 2020/7/15.
//

