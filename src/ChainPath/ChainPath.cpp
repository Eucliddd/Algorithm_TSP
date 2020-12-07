#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <string>
#include <ctime>
#include<cstring>

#include "ChainPath.h"

using namespace std;

ChainPath::ChainPath()
{
	vCount = 0;
}

ChainPath::ChainPath(const string &filename, int (*dist)(int, int, double [][2]))//:infile(filename),dist(dist) {}
{
	vCount = 0;
	infile.open(filename.c_str());
	this->dist=dist;
}

int ChainPath::Find(int x)
{
    int root,parent;
    root = x;
    while (point_set[root] != root)
        root = point_set[root];
    parent = point_set[x]; //路径压缩
    while (root != parent)
    {
        point_set[x] = root;
        x = parent;
        parent = point_set[x];
	}
    return root;
}

int ChainPath::Union(int a, int b)
{
    int root_a = Find(a);
    int root_b = Find(b);
    point_set[root_b] = root_a;
    return root_a;
}

bool com_point(point a, point b)
{
	return a.value < b.value;
}

int ChainPath::TSP_Short(int dis[][N], int n, point point[])
{
	int temp = (n * (n - 1)) / 2;   // 边的个数
	int i;
	int count = 0;
	int k = 0;
	for (i = 0; i < temp; i++)
		if (point[i].value != 0)
		{
			if ((Find(point[i].from) != Find(point[i].end))&&radio[point[i].from] != 2 && radio[point[i].end] != 2)
			{
				count += point[i].value;
				radio[point[i].from]++;
				radio[point[i].end]++;
				result[k].value = point[i].value;
				result[k].from = point[i].from;
				result[k].end = point[i].end;
				Union(point[i].from,point[i].end);
				k++;
				if (k == n - 1)
					break;

			}

		}
	return count;
}

void ChainPath::DFS(int matrix[300][300], int v, int *visited, int n)
{
	vList[vCount++] = v;
    visited[v] = 1;
    for (int i = 1; i <= n; i++)
        if (!visited[i] && matrix[v][i] == 1)
          DFS(matrix, i, visited, n);
}

void ChainPath::run()
{
    string tmp;
    int n, i, j;

	clock_t start_clock, end_clock;
	start_clock = clock();

	vCount = 0;
	ofstream ofile("./data.txt");

    while (infile >> tmp)
		if (tmp == "DIMENSION:" || tmp == "DIMENSION") break;
    if (tmp == "DIMENSION:")
	{
        infile >> n;
    }
    else
	{
        infile >> tmp;
        infile >> n;
    }

    while (infile >> tmp)
		if (tmp == "NODE_COORD_SECTION") break;

	//double x[n + 1], y[n + 1];
	double *x = new double[n + 1];
	double *y = new double[n + 1];

	for(i = 1; i <= n; i++)
    {
        infile >> x[i] >> x[i] >> y[i];
    }
    infile.close();

	for (i = 1; i <= n; i++)
    {
        for (j = 1; j <= i; j++)
        {
            if (i == j)
                dis[i][j] = INF;
            else
			{
				double xy[2][2];
				xy[0][0]=x[i];
				xy[0][1]=y[i];
				xy[1][0]=x[j];
				xy[1][1]=y[j];
                dis[i][j] = dis[j][i] = dist(0, 1, xy);
			}
        }
    }

	point point[N];
	int cost  = 0;
	int t = 0;
	for (i = 1; i <= n; i++)
	{
		for (j = 1; j <= i - 1; j++)
		{
			point[t].value = dis[i][j];
			if (i < j)
			{
		 		point[t].from = i;
				point[t].end = j;

			}
			else
			{
		 		point[t].from = j;
				point[t].end = i;

			}
			t++;
		}
	}

	sort(point, point + t, com_point);
	for (i = 1; i <= n; i++)
		point_set[i] = i;
	memset(radio, 0, sizeof(int)*N);
	cost = TSP_Short(dis,n,point);
	int end[2];
	int m = 0;
	for (i = 1; i <= n; i++)
	{
		if (radio[i] == 1)
		end[m++] = i;
	}

	end_clock = clock();
	double duration = (double)(end_clock - start_clock) / CLOCKS_PER_SEC;
	best_length = cost + dis[end[0]][end[1]];
	ofile << best_length << endl;
	ofile << duration << endl;

	int matrix[300][300] = {0}, visited[300] = {0};
	matrix[end[0]][end[1]] = matrix[end[1]][end[0]] = 1;
	for (i = 0; i < n - 1; i++)
        matrix[result[i].from][result[i].end] = matrix[result[i].end][result[i].from] = 1;
    DFS(matrix, 1, visited, n);

	ofile << x[1] << " " << y[1] << endl;
	for (i = vCount - 1; i >= 0; i--)
	{
		ofile << x[vList[i]] << " " << y[vList[i]] << endl;
	}
	ofile.close();
	delete []x;
	delete []y;
}
