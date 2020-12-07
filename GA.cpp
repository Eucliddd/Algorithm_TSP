//
// Created by Lenovo on 2020/7/15.
//
#include <bits/stdc++.h>
#include "GA.h"
//#define SAFE//安全性检测
//#define TEST//测试结果
//#define TMPANS//中间结果
//#define EXGA //迭代次数为结点的平方
#define GREEDYEX
using namespace std;

namespace GAdata {
    int nodeNum;//种群个体数
    int MAXTIME = 10000;
    bool youhuaFlag=false;
    int *youhuaPath;
    int youhuaStart;
    random_device randomDevice;
    default_random_engine randomEngine;//随机数生成引擎
    static int dis(int a, int b);
    //O(N)
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

    struct life {
        //起点0:=1,终点N:=1
        vector<int> route;
        int length;//路径长度
        double fitness;//适应度
        //计算个体信息
        void init() {
            length = 0;
            for (int i = 1; i < route.size(); i++) {
                length += dis(route[i - 1], route[i]);
            }
            fitness = 1.0 / length;
        }

        bool operator<(const life &b) {
            return this->fitness > b.fitness;
        }
    };

    struct population {//种群
        int size;//种群大小
        double crossCount;//交叉概率
        double mutationCount;//变异概率
        vector<life> nowLifes;//当代种群,生成完成后按照适应度从大到小排序
        vector<double> preSumFitness;//适应度的前缀和,生成完成后按照适应度从大到小排序
        vector<life> nextLifes;//次代种群
        bool choice = 1;

        //计算适应度的前缀和O(N)
        void getPreSumFitness() {
            preSumFitness[0] = nowLifes[0].fitness;
            for (int i = 1; i < size; i++) {
                preSumFitness[i] = 0;
                preSumFitness[i] = preSumFitness[i - 1] + nowLifes[i].fitness;
            }
        }

        //蒙特卡洛法？生成初始种群,加入贪心路径进行优化O(k*Size^2)
        void initPopulation() {
            int k = 10;//初始生成的倍数
            life tmp;
            for (int i = 0; i < k * size; i++) {
                tmp.route.clear();
                for (int j = 1; j <= nodeNum; j++) {
                    tmp.route.push_back(j);
                }
                randomArray(tmp.route);
                tmp.route.push_back(tmp.route[0]);
                tmp.init();
                nowLifes.push_back(tmp);
            }
#ifdef GREEDYEX
            //加入优化路径
            if(youhuaFlag){
                tmp.route.clear();
                for(int i=0;i<nodeNum;i++){
                    tmp.route.push_back(youhuaPath[i]+youhuaStart);
                }
                tmp.route.push_back(youhuaPath[0]+youhuaStart);
                tmp.init();
                youhuaFlag= false;
                nowLifes.push_back(tmp);
            }
            //加入贪心路径
            auto greedyAns=Greedy(nodeNum,dis);
            tmp.route=greedyAns.second;
            tmp.init();
            nowLifes.push_back(tmp);
#endif
            sort(nowLifes.begin(), nowLifes.end());
#ifdef TEST
            for(auto it=nowLifes.begin();it!=nowLifes.end();it++){
                cout<<it->fitness<<' ';
            }
            cout<<endl;
#endif
            while (nowLifes.size() > size)nowLifes.pop_back();//保留最优
            preSumFitness.resize(size);
            getPreSumFitness();
#ifdef TEST
            for(auto it=nowLifes.begin();it!=nowLifes.end();it++){
                cout<<it->fitness<<' ';
            }
            cout<<endl;
#endif
        }

        //按照赌轮盘的方法随机获取个体
        life getOne(uniform_real_distribution<double> randRear) {
            double index = randRear(randomEngine);
            auto it = lower_bound(preSumFitness.begin(), preSumFitness.end(), index);
            int posion = it - preSumFitness.begin();
            return nowLifes[posion];
        }

        //顺序交叉
        life son1, son2;
        bool oxflag[300];//出现标记
        void oxCross(life &p1, life &p2,uniform_int_distribution<int> randPosion) {
            //确定交叉起点和终点
            int index1, index2;
            index1 = randPosion(randomEngine);
            index2 = randPosion(randomEngine);
            //while(index1==index2)index2=randPosion(randomEngine);
            if (index1 > index2)swap(index1, index2);
            //获取孩子1
            memset(oxflag, 0, sizeof(oxflag));
            son1.route.resize(nodeNum + 1);
            for (int i = index1; i <= index2; i++) {
                son1.route[i] = p1.route[i];
                oxflag[p1.route[i]] = true;//标记为存在
            }
            for (int i = 0, j = 0; i <= nodeNum; i++) {
                if (j == index1)j = index2 + 1;
                if (oxflag[p2.route[i]])continue;
                son1.route[j] = p2.route[i];
                j++;
            }
            son1.init();
            //获取孩子2
            memset(oxflag, 0, sizeof(oxflag));
            son2.route.resize(nodeNum + 1);
            for (int i = index1; i <= index2; i++) {
                son2.route[i] = p2.route[i];
                oxflag[p2.route[i]] = true;//标记为存在
            }
            for (int i = 0, j = 0; i <= nodeNum; i++) {
                if (j == index1)j = index2 + 1;
                if (oxflag[p1.route[i]])continue;
                son2.route[j] = p1.route[i];
                j++;
            }
            son2.init();
            p1 = son1;
            p2 = son2;

        }

        //部分映射交叉
        void pmxCross(life &p1, life &p2,uniform_int_distribution<int> randPosion) {
            int index1, index2;
            //uniform_int_distribution<int> randPosion(1, nodeNum - 1);//生成1~nodeNum-1的随机数，防止改变起点0和终点N
            index1 = randPosion(randomEngine);
            index2 = randPosion(randomEngine);
            //while(index1==index2)index2=randPosion(randomEngine);
            if (index1 > index2)swap(index1, index2);
            int shine[nodeNum + 1];//字符-编码映射表
            int shineR[nodeNum + 1];//编码-字符映射表
            for (int i = 1; i <= nodeNum; i++) {//初始化
                shine[i] = i;
            }
            for (int i = index1; i <= index2; i++) {
                swap(shine[p1.route[index1]], shine[p2.route[index1]]);
            }
            for (int i = 1; i <= nodeNum; i++) {
                shineR[shine[i]] = i;
            }
            for (int i = 1; i < nodeNum; i++) {
                p1.route[i] = shineR[p1.route[i]];
                p2.route[i] = shineR[p2.route[i]];
            }
#ifdef SAFE
            map<int,bool>record;
            for(int i=1;i<nodeNum;i++){
                if(record[p1.route[i]]==1){
                    cout<<"ERRRRRRRRRRR"<<endl;
                    exit(1);
                }
                record[p1.route[i]]=1;
            }
#endif
            p1.init();
            p2.init();
        }
        //两点变异
        void spMutation(life &p) {
            uniform_int_distribution<int> randPosion(1, nodeNum - 1);//生成1~nodeNum-1的随机数，防止改变起点0和终点N
            int index1 = randPosion(randomEngine);
            int index2 = randPosion(randomEngine);
            swap(p.route[index1], p.route[index2]);
            p.init();//重新计算长度/适应度
            //可以优化
        }

        //逆序变异
        void roMutation(life &p,uniform_int_distribution<int> randPosion) {
            int index1 = randPosion(randomEngine);
            int index2 = randPosion(randomEngine);
            if (index1 > index2)swap(index1, index2);
            //优化计算距离、适应度计算方法
            p.length -= dis(p.route[index1 - 1], p.route[index1]);
            p.length -= dis(p.route[index2], p.route[index2 + 1]);
            int indexR1 = index1;
            int indexR2 = index2;
            while (index1 < index2) {
                swap(p.route[index1], p.route[index2]);
                index1++;
                index2--;
            }
            p.length += dis(p.route[indexR1 - 1], p.route[indexR1]);
            p.length += dis(p.route[indexR2], p.route[indexR2 + 1]);
            p.fitness = 1.0 / p.length;
        }

        //种群迭代
        void next() {//生成下一代
            uniform_real_distribution<double> randRear(0, 1);//随机数分布
            uniform_real_distribution<double> randGetOne(0, preSumFitness.back());
            uniform_int_distribution<int> randOxCross(1, nodeNum - 1);
            while (nextLifes.size() < size) {
                life parent1 = getOne(randGetOne);//获得双亲1
                life parent2 = getOne(randGetOne);//获得双亲2
                double p1 = randRear(randomEngine);//交叉概率
                if (p1 < crossCount) {
                    if (choice)
                        pmxCross(parent1, parent2,randOxCross);
                    else
                        oxCross(parent1, parent2,randOxCross);
                }
                double p2_1 = randRear(randomEngine);//变异概率
                if (p2_1 < mutationCount) {
                    roMutation(parent1,randOxCross);
                    //spMutation(parent1);
                }
                double p2_2 = randRear(randomEngine);//变异概率
                if (p2_2 < mutationCount) {
                    roMutation(parent2,randOxCross);
                    //spMutation(parent2);
                }
                nextLifes.push_back(parent1);//加入新种群
                nextLifes.push_back(parent2);
            }

            nextLifes.push_back(best());//保留最优个体
            sort(nextLifes.begin(), nextLifes.end());//排序
            while (nextLifes.size() > size)nextLifes.pop_back();//保证种群大小不变
            nowLifes = nextLifes;
            nextLifes.clear();//种群迭代
            getPreSumFitness();//计算新种群的适应度
        }

        //种群最优个体
        life best() {//获取最优个体
            return nowLifes.front();
        }

        //设置参数
        void set(int num, double cross, double mutation) {//参数设定
            randomEngine.seed(randomDevice());//随机数引擎重新播种
            size = num;
            crossCount = cross;
            mutationCount = mutation;
        }

        population() {}
    };

    pair<int, vector<int>> myGA(int n) {
        nodeNum = n;
        population tsp;
        tsp.set(n, 0.95, 0.1);
        tsp.initPopulation();

        int preBest = 0;//前一个最好值
        int repetCount = 0;//重复次数
        while (MAXTIME--) {
            tsp.next();
#ifdef TMPANS
            cout << tsp.best().length << endl;
#endif
            if (preBest == tsp.best().length) {
                repetCount++;
            } else {
                preBest = tsp.best().length;
                repetCount = 1;
            }
            if (repetCount >= MAXTIME / 200) {
                tsp.mutationCount += 0.05;
                tsp.choice ^= 1;
            } else {
                tsp.mutationCount = 0.05;
            }
            if (repetCount >= MAXTIME / 9) {
                break;
            }
        }
        life king = tsp.best();
#ifdef TMPANS
        for (auto it = king.route.begin(); it != king.route.end(); it++) {
            cout << *it << ' ';
        }
        cout << endl;
        cout << MAXTIME << endl;
#endif
        return make_pair(king.length, king.route);
    }

//点的坐标
    vector<pair<double, double>> coordinate;

    int getData(const string &fileName) {
        coordinate.clear();
        int nodeNum = 0;
        ifstream fin(fileName);
        string tmp;
        //获取顶点的个数
        while (fin >> tmp)if (tmp == "DIMENSION:"||tmp=="DIMENSION")break;
        if(tmp=="DIMENSION:"){
            fin >> nodeNum;
        }else{
            fin>>tmp;
            fin >> nodeNum;
        }
        //获取顶点坐标
        while (fin >> tmp)if (tmp == "NODE_COORD_SECTION")break;
        int index;
        double x = 0.0;
        double y = 0.0;
        coordinate.emplace_back(x, y);
        for (int i = 1; i <= nodeNum; i++) {
            fin >> index >> x >> y;
            coordinate.emplace_back(x, y);
        }
        fin.close();
        return nodeNum;
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
using namespace GAdata;

GA::GA(const string &filename, int (*mydis)(int, int, double [][2])){
    int ans=getData(filename);
    this->nodeNum=ans;
    dist=mydis;
#ifndef EXGA
    MAXTIME=min(10000,this->nodeNum*this->nodeNum);
#else
    MAXTIME=max(10000,this->nodeNum*this->nodeNum);
#endif

}
void GA::run() {
    clock_t start,end;
    start = clock();
    auto ans=myGA(this->nodeNum);
    end=clock();
    this->times=(double)(end-start)/CLOCKS_PER_SEC;
    this->best_length=ans.first;
    //copy path
    //1~.....~1
    this->best_path=new int [this->nodeNum+1];
    for(int i=0;i<=this->nodeNum;i++){
        this->best_path[i]=ans.second[i];
    }
    output("./data.txt");
}

void GA::output(const string &fileName) {
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

void GA::youhua(int start,int *path) {
    youhuaPath=path;
    youhuaFlag=true;
    youhuaStart=start;
}
