#include <iostream>
#include "SA.h"
#include "GA.h"
#include "Nearest.h"
#include "OPT-2.h"
#include "AntColony.h"
#include "Tabu.h"
#include "ChainPath.h"
#include <ctime>

#define GUI 1

typedef int(*Dist_Func)(int,int,double[][2])  ;

int EUC_2D(int x,int y,double _city[][2]){
    double dx=_city[x][0]-_city[y][0],dy=_city[x][1]-_city[y][1];
    return round(sqrt(dx*dx+dy*dy));
}

int GEO(int x,int y,double _city[][2]){
    const double PI=3.141592;
    int deg = round(_city[x][0]);
    double min = _city[x][0] - deg;
    double latitude_x = PI * (deg + 5.0 * min / 3.0) / 180.0;
    deg = round(_city[x][1]);
    min = _city[x][1] - deg;
    double longtitude_x = PI * (deg + 5.0 * min / 3.0) / 180.0;
    deg = round(_city[y][0]);
    min = _city[y][0] - deg;
    double latitude_y = PI * (deg + 5.0 * min / 3.0) / 180.0;
    deg = round(_city[y][1]);
    min = _city[y][1] - deg;
    double longtitude_y = PI * (deg + 5.0 * min / 3.0) / 180.0;

    const double RRR = 6378.388;
    double q1 = cos(longtitude_x-longtitude_y);
    double q2 = cos(latitude_x-latitude_y);
    double q3 = cos(latitude_x+latitude_y);
    return (int)(RRR * acos(0.5 *((1.0+q1)*q2-(1.0-q1)*q3))+1.0);
}

map<string,Dist_Func> funMap;

void test(){
    const string head="../";
    //分隔符
    const char spe=',';
    //outputAddress
    const string allName=head+"testAns/allAns.txt";

    vector< string >Names;
    Names.push_back(head+"testAns/optAns.txt");
    Names.push_back(head+"testAns/NearestAns.txt");
    Names.push_back(head+"testAns/ASAns.txt");
    Names.push_back(head+"testAns/SAAns.txt");
    Names.push_back(head+"testAns/GAAns.txt");
    Names.push_back(head+"testAns/TabuAns.txt");
    Names.push_back(head+"testAns/ChainPath.txt");
    vector<ofstream> sonFout(Names.size());
    for(int i=0;i<Names.size();i++){
        sonFout[i].open(Names[i],ios::trunc);
        sonFout[i]<<"TSPLIB.Instance"<<spe<<"Tour_Length"<<spe<<"Running_Time"<<endl;
    }
    int start,end;
    double times;
    //inputName
    ifstream fin(head+"nameList.txt");
    //dataAddress
    string str1=head+"exdata/";
    string dataName;
    ofstream fout;

    //output
    fout.open(allName,ios::trunc);
    //table heaed
    fout<<"TSPLIB.Instance"<<spe
        <<"opt.Tour_Length"<<spe<<"opt.Running_Time"<<spe
        <<"Nearest.Tour_Length"<<spe<<"Nearest.Running_Time"<<spe
        <<"AS.Tour_Length"<<spe<<"AS.Running_Time"<<spe
        <<"SA.Tour_Length"<<spe<<"SA.Running_Time"<<spe
        <<"GA.Tour_Length"<<spe<<"GA.Running_Time"<<spe
        <<"Tabu.Tour_Length"<<spe<<"Tabu.Running_Time"<<spe
        <<"Chain.Tour_Length"<<spe<<"Chain.Running_Time"<<endl;

    if(fin.is_open()){
        int index=0;
        while(fin>>dataName){
            fout<<dataName<<spe;
            for(int i=0;i<sonFout.size();i++){
                sonFout[i]<<dataName<<spe;
            }
            Dist_Func f=funMap[dataName];
            dataName=str1+dataName;
            //opt
            OPT2 opt(dataName,f);
            start=clock();
            opt.run();
            end=clock();
            times=(double)(end-start)/CLOCKS_PER_SEC;
            fout<<opt.best_length<<spe<<times<<spe;
            sonFout[index++]<<opt.best_length<<spe<<times<<endl;

            //Nearest
            start=clock();
            Nearest ne(dataName,f);
            ne.run();
            end=clock();
            times=(double)(end-start)/CLOCKS_PER_SEC;
            fout<<ne.best_length<<spe<<times<<spe;
            sonFout[index++]<<ne.best_length<<spe<<times<<endl;

            //AS
            start=clock();
            AntColony ac(dataName,f);
            ac.run();
            end=clock();
            times=(double)(end-start)/CLOCKS_PER_SEC;
            fout<<ac.best_length<<spe<<times<<spe;
            sonFout[index++]<<ac.best_length<<spe<<times<<endl;

            //SA
            start=clock();
            SA sa(dataName,f);
            sa.run();
            end=clock();
            times=(double)(end-start)/CLOCKS_PER_SEC;
            fout<<sa.best_length<<spe<<times<<spe;
            sonFout[index++]<<sa.best_length<<spe<<times<<endl;

            //GA
            start=clock();
            GA ga(dataName,f);
            ga.run();
            end=clock();
            times=(double)(end-start)/CLOCKS_PER_SEC;
            fout<<ga.best_length<<spe<<times<<spe;
            sonFout[index++]<<ga.best_length<<spe<<times<<endl;

            //TABU
            start=clock();
            Tabu tabu(dataName,f);
            tabu.youhua(1,sa.best_path);
            tabu.run();
            end=clock();
            times=(double)(end-start)/CLOCKS_PER_SEC;
            fout<<tabu.best_length<<spe<<times<<spe;
            sonFout[index++]<<tabu.best_length<<spe<<times<<endl;


            //Chain
            start=clock();
            ChainPath* ch=new ChainPath(dataName,f);
            ch->run();
            end=clock();
            times=(double)(end-start)/CLOCKS_PER_SEC;
            fout<<ch->best_length<<spe<<times<<spe;
            fout<<endl;
            sonFout[index++]<<ch->best_length<<spe<<times<<endl;
            index=0;
            delete ch;
        }

    }
    fout.close();
    fin.close();
    for(int i=0;i<6;i++)sonFout[i].close();
}


void initMap(){
    funMap["d198.tsp"]=EUC_2D;
    funMap["bier127.tsp"]=EUC_2D;
    funMap["berlin52.tsp"]=EUC_2D;
    funMap["ch130.tsp"]=EUC_2D;
    funMap["ch150.tsp"]=EUC_2D;
    funMap["eil101.tsp"]=EUC_2D;
    funMap["eil76.tsp"]=EUC_2D;
    funMap["kroA100.tsp"]=EUC_2D;
    funMap["kroA150.tsp"]=EUC_2D;
    funMap["kroA200.tsp"]=EUC_2D;
    funMap["kroB100.tsp"]=EUC_2D;
    funMap["kroB150.tsp"]=EUC_2D;
    funMap["kroB200.tsp"]=EUC_2D;
    funMap["kroC100.tsp"]=EUC_2D;
    funMap["kroD100.tsp"]=EUC_2D;
    funMap["kroE100.tsp"]=EUC_2D;
    funMap["lin105.tsp"]=EUC_2D;
    funMap["pr107.tsp"]=EUC_2D;
    funMap["pr124.tsp"]=EUC_2D;
    funMap["pr136.tsp"]=EUC_2D;
    funMap["pr144.tsp"]=EUC_2D;
    funMap["pr152.tsp"]=EUC_2D;
    funMap["pr76.tsp"]=EUC_2D;
    funMap["rat195.tsp"]=EUC_2D;
    funMap["rat99.tsp"]=EUC_2D;
    funMap["rd100.tsp"]=EUC_2D;
    funMap["st70.tsp"]=EUC_2D;
    funMap["ts225.tsp"]=EUC_2D;
    funMap["tsp225.tsp"]=EUC_2D;
    funMap["u159.tsp"]=EUC_2D;
    funMap["gr229.tsp"]=GEO;
}


int main(int argc,char** argv) {
    initMap();
    test();
    return 0;


//    if(argc <= 1) cerr<<"please input!"<<endl;
//    else {
//        string str1 = "./exdata/", str2 = argv[2];
//        str1 += argv[1];
//        Dist_Func f=funMap[argv[1]];
//        if (str2 == "2-opt") {
//            OPT2 opt(str1, f);
//            opt.run();
//        } else if (str2 == "Nearest") {
//            Nearest ne(str1, f);
//            ne.run();
//        } else if (str2 == "AS") {
//            AntColony ac(str1, f);
//            ac.run();
//        } else if (str2 == "SA") {
//            SA sa(str1, f);
//            sa.run();
//        } else if (str2 == "GA") {
//            GA ga(str1, f);
//            ga.run();
//        } else if (str2 == "Tabu") {
//            SA sa(str1,f);
//            sa.run();
//            Tabu tabu(str1, f);
//            tabu.youhua(1,sa.best_path);
//            tabu.run();
//        } else if(str2 == "Chain"){
//            ChainPath* ch=new ChainPath(str1,f);
//            ch->run();
//            delete ch;
//        }
//    }
//    return 0;

}
