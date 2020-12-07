//
// Created by Lenovo on 2020/7/16.
//
#define GREEDYINIT
//#define ALLNEIBORHOOD //搜索全部邻域
#define NEIS 0.5    //局部搜索的大小
#define MAXTIMES 5000 //最大迭代次数
#include "Tabu.h"
#include "../Greedy/Greedy.h"

//#define TMPANS
namespace TABU {
    int *youhuaPath;
    int youhuaStart;
    bool youhuaFlag=false;
    random_device randomDevice;
    default_random_engine randomEngine;//随机数生成引擎
    bernoulli_distribution randomBool(0.5);//控制领域的大小
    int dis(int, int);
    void randomArray(vector<int> &Array) {
        int LENGTH = Array.size();
        // 随机打乱
        for (int i = LENGTH; i > 0; i--) {
            // 选中的随机下标
            int index = randomDevice() % i;
            swap(Array[index], Array[i - 1]);
        }
        //确保一号结点位于开头
        for (int i = 0; i < LENGTH; i++) {
            if (Array[i] == 1) {
                swap(Array[0], Array[i]);
                break;
            }
        }
    }
    struct changeNode {//邻域结点
        int ca, cb;//交换结点
        int add;//路径长度变化
        bool operator<(changeNode b) {//add小的优先
            return this->add < b.add;
        }
    };

///参数
    int MAXGEN;//最大迭代次数
    int tabuTableLength;//禁忌表长度
    int nodeNum;//结点数量
    vector<int> route;//当前路径
    vector<int> bestRoute;//最好路径

    int nowEvaluation;//当前路径长度
    int bestEvaluation;//最好路径长度
    list<pair<int, int> > tabuTable;//禁忌表
    vector<vector<list<pair<int, int> >::iterator> > tabuRecord;//禁忌记录,禁忌的是两个位置的互换
    vector<changeNode> neighborRoutes;//邻域
//参数设置
    void set(int n) {
        nodeNum=n;
        MAXGEN=min(150*n,MAXTIMES);
        tabuTableLength=n*n/8;
        randomEngine.seed(randomDevice());
    }

//计算路线长度
    int evaluate(vector<int> &path) {
        int ans = 0;
        for (int i = 1; i < path.size(); i++) {
            ans += dis(path[i], path[i - 1]);
        }
        return ans;
    }

//初始化
    void generateInitGroup() {
        if(!youhuaFlag) {
            route.clear();
            //生成初始路径，计算长度，设置为最优路径
#ifndef GREEDYINIT
            for (int i = 1; i <= nodeNum; i++) {
                route.push_back(i);
            }
            randomArray(route);
            route.push_back(1);
#endif
#ifdef GREEDYINIT
            route=Greedy(nodeNum,dis).second;
#endif
        }else{
            route.clear();
            for(int i=0;i<nodeNum;i++){
                route.push_back(youhuaPath[i]+youhuaStart);
            }
            route.push_back(youhuaPath[0]+youhuaStart);
            youhuaFlag=false;
        }
        bestRoute=route;
        nowEvaluation=bestEvaluation = evaluate(route);
        //清空禁忌表,将禁忌记录全部设置为禁忌表的end迭代器
        tabuTable.clear();
        tabuRecord.resize(nodeNum + 1);
        for (int i = 0; i <=nodeNum; i++) {
            tabuRecord[i].clear();
            for(int j=0;j<=nodeNum;j++){
                tabuRecord[i].push_back(tabuTable.end());
            }
        }
        //清空领域
        neighborRoutes.clear();

    }

//获取部分邻域,百分之50的领域

    void getPartNeighborhood(vector<int> &path) {
        neighborRoutes.clear();
        changeNode tmp{};
        bool flag;
        for (int i = 1; i < nodeNum; i++) {
            flag=randomBool(randomEngine);
            if(flag<=NEIS)continue;
            for (int j = i + 1; j < nodeNum; j++) {
                if(j==i+1){
                    tmp.add=(dis(path[j],path[i-1])+dis(path[j+1],path[i]))
                            -(dis(path[i-1],path[i])+dis(path[j],path[j+1]));
                }
                else {
                    tmp.add = (dis(path[j], path[i - 1]) + dis(path[i + 1], path[j]) + dis(path[i], path[j - 1]) +
                               dis(path[j + 1], path[i]))
                              - (dis(path[i], path[i - 1]) + dis(path[i + 1], path[i]) + dis(path[j], path[j - 1]) +
                                 dis(path[j + 1], path[j]));
                }
                tmp.ca = i;
                tmp.cb = j;
                neighborRoutes.push_back(tmp);
            }
        }
        //按add从小到大排序
        sort(neighborRoutes.begin(), neighborRoutes.end());
    }
    void getAllNeighborhood(vector<int> &path) {
        neighborRoutes.clear();
        changeNode tmp{};
        for (int i = 1; i < nodeNum; i++) {
            for (int j = i + 1; j < nodeNum; j++) {
                if(j==i+1){
                    tmp.add=(dis(path[j],path[i-1])+dis(path[j+1],path[i]))
                            -(dis(path[i-1],path[i])+dis(path[j],path[j+1]));
                }
                else {
                    tmp.add = (dis(path[j], path[i - 1]) + dis(path[i + 1], path[j]) + dis(path[i], path[j - 1]) +
                               dis(path[j + 1], path[i]))
                              - (dis(path[i], path[i - 1]) + dis(path[i + 1], path[i]) + dis(path[j], path[j - 1]) +
                                 dis(path[j + 1], path[j]));
                }
                tmp.ca = i;
                tmp.cb = j;
                neighborRoutes.push_back(tmp);
            }
        }
        //按add从小到大排序
        sort(neighborRoutes.begin(), neighborRoutes.end());
    }
//对路径path执行c交换
    void change(vector<int> &path, changeNode c) {
        swap(path[c.ca], path[c.cb]);
    }

//迭代
    void generate() {
        //获取邻域
#ifndef ALLNEIBORHOOD
        getPartNeighborhood(route);
#else
        getAllNeighborhood(route);
#endif
        //破禁
        if (nowEvaluation + neighborRoutes.front().add < bestEvaluation) {
            //更改now
            swap(route[neighborRoutes.front().ca],route[neighborRoutes.front().cb]);
            //change(route, neighborRoutes.front());
            nowEvaluation += neighborRoutes.front().add;
            //更新best
            bestRoute = route;
            bestEvaluation = nowEvaluation;
            //禁忌
            int a = neighborRoutes.front().ca;
            int b = neighborRoutes.front().cb;
            if (tabuRecord[a][b] != tabuTable.end()) {//在禁忌表中,重新加入
                tabuTable.erase(tabuRecord[a][b]);
                tabuTable.push_front(make_pair(a, b));
                tabuRecord[a][b] = tabuTable.begin();
            } else {//不在禁忌表中，新加入
                tabuTable.push_front(make_pair(a, b));
                tabuRecord[a][b] = tabuTable.begin();
                //维持紧急表的长度
                if (tabuTable.size() > tabuTableLength) {
                    a=tabuTable.back().first;
                    b=tabuTable.back().second;
                    tabuTable.pop_back();
                    tabuRecord[a][b]=tabuTable.end();
                }
            }
        } else {
            int a;
            int b;
            for (auto it = neighborRoutes.begin(); it != neighborRoutes.end(); it++) {
                a = it->ca;
                b = it->cb;
                if (tabuRecord[a][b] == tabuTable.end()) {//不在禁忌表中
                    tabuTable.push_front(make_pair(a,b));
                    tabuRecord[a][b]=tabuTable.begin();
                    //更新路径
                    nowEvaluation += it->add;
                    //swap(route[a],route[b]);
                    change(route,*it);
                    break;
                }
            }
            if (tabuTable.size() > tabuTableLength) {
                a=tabuTable.back().first;
                b=tabuTable.back().second;
                tabuTable.pop_back();
                tabuRecord[a][b]=tabuTable.end();
            }
        }
    }

    //地图数据存储以及接口转接
    vector<pair<double, double>> coordinate;

    int getData(const string &fileName) {
        coordinate.clear();
        int num = 0;
        ifstream fin(fileName);
        string tmp;
        //获取顶点的个数
        while (fin >> tmp)if (tmp == "DIMENSION:"||tmp=="DIMENSION")break;
        if(tmp=="DIMENSION:"){
            fin >> num;
        }else{
            fin>>tmp;
            fin >> num;
        }
        //获取顶点坐标
        while (fin >> tmp)if (tmp == "NODE_COORD_SECTION")break;
        int index;
        double x = 0.0;
        double y = 0.0;
        coordinate.emplace_back(x, y);
        for (int i = 1; i <= num; i++) {
            fin >> index >> x >> y;
            coordinate.emplace_back(x, y);
        }
        fin.close();
        return num;
    }

    int (*dist)(int, int, double [][2]);

    int dis(int a, int b) {
        double xxyy[2][2];
        xxyy[0][0] = coordinate[a].first;
        xxyy[0][1] = coordinate[a].second;
        xxyy[1][0] = coordinate[b].first;
        xxyy[1][1] = coordinate[b].second;
        return dist(0, 1, xxyy);
    }
}
using namespace TABU;
pair<int, vector<int>> tabu(int n) {
    TABU::set(n);//参数设置
    generateInitGroup();
    int index = 1;
    for (; index <= MAXGEN; index++) {
        generate();
#ifdef TMPANS
        cout<<bestEvaluation<<endl;
#endif
    }
    return make_pair(bestEvaluation, bestRoute);
}

Tabu::Tabu(const string &filename, int (*nowdist)(int, int, double [][2])){
    int ans=getData(filename);
    this->nodeNum=ans;
    dist=nowdist;
}

void Tabu::run() {
    clock_t start,end;
    start = clock();

    auto ans=tabu(this->nodeNum);

    end=clock();
    //保存结果
    this->times=(double)(end-start)/CLOCKS_PER_SEC;
    this->best_length=ans.first;
    this->best_path=new int [this->nodeNum+1];
    for(int i=0;i<=this->nodeNum;i++){
        this->best_path[i]=ans.second[i];
    }
    output("./data.txt");//输出结果
}

void Tabu::output(const string &fileName) {
    ofstream fout(fileName,ios:: trunc);
    if(fout.is_open()){
        fout<<this->best_length<<endl;
        fout<<this->times<<endl;
        for(int i=0;i<=nodeNum;i++){
            fout<<coordinate[this->best_path[i]].first<<' '<<coordinate[this->best_path[i]].second<<endl;
        }
    }
    fout.close();
}

void Tabu::youhua(int start,int *path) {
    youhuaFlag=true;
    youhuaPath=path;
    youhuaStart=start;
}