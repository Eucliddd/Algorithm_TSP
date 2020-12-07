#ifndef ChinaPath_H
#define ChinaPath_H

#include <iostream>
#include <fstream>
using namespace std;

#define N 30000
#define INF 100000

typedef struct point
{
	int value;
	int from;
	int end;
}point;

class ChainPath
{
public:
	ChainPath();
	ChainPath(const string& filename, int(*dist)(int,int,double[][2]));
	int Find(int x);
	int Union(int a, int b);
	int TSP_Short(int dis[][N], int n, point point[]);

	int vList[300], vCount;
	void DFS(int matrix[300][300], int v, int *visited, int n);
	void run();

	int best_length;

private:
	ifstream infile;
	int(*dist)(int,int,double [][2]);
	int dis[N][N];
	int point_set[N];
	int radio[N];
	point result[N];
};
#endif // ChinaPath