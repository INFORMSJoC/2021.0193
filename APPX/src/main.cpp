//
// Created by DY on 2021/12/7.
//
#include <cstring>
//#include "miniSpanTree.h"
#include "main.h"
#include <iostream>
#include <math.h>
#include <iomanip>
#include <vector>
#include <ctime>
#include <algorithm>
#include <climits>
#include "Matching.h"
#include "Graph.h"
#include <map>
#define INFINITE 0xFFFFFFFF
# define M 1000
#define type 2 //type of depot: 2/4/6
#define data_type 0 //type of instance: SD=0, p/pr=1

//upper bound for combinations to avoid error
#define transfer_idx_limit 20000  //upper bound for permulation and combination
#define transfer_limit 2000   //upper bound for edge exchange times except single edge exchange
#define transfer_limit_all 100000   // upper bound for edge exchange times
// number of feasible solutions by Algorithm 2
#define add_limit 100000

using namespace std;
#define vetex_number 10 // num of vetex( = depots num + customer num )
string depots = "SD1.txt"; //instance file name
string path_0_depots = "/home/li/CLionProjects/attach/data/SD set/k=2/"+depots; //instance file path
string path_1_depots = "/home/li/CLionProjects/attach/data/P set/"+depots;

struct Info{
    int customer_num;
    int capacity;
    int depots_num;
    int vehicle_num;
    vector<int> vehicles;
    vector<int> demands;
    vector<vector<int>> loc;
};

typedef struct Tree
{
    bool tree[vetex_number][vetex_number];
    int cost;
} TREE;

typedef struct TreeDepots
{
    bool tree[vetex_number-1][vetex_number-1];
    int cost;
} TREE_DEPOTS;

Info readTxt(){
    Info info;
    ifstream infile;
    if(data_type == 1){
        infile.open(path_1_depots, ios::in);
        if(!infile.is_open()){
            cout<<"can not open this file"<<endl;
            return info;
        }
        string s;
        vector<vector<int>> res;
        while (getline(infile, s)){
            stringstream input(s);
            float result;
            vector<int> in;
            while(input>>result){
                in.push_back(result);
            }
            res.push_back(in);
        }
        info.customer_num = res[0][2]; //customer number
        info.capacity = res[2][1]; //capacity(assume it is same for each vehicle)
        info.depots_num = res[0][3]; //depot number
        info.vehicle_num = info.depots_num * res[0][1];
        for(int i = 0; i<info.depots_num;i++){
            info.vehicles.push_back(res[0][1]);
        }

        int begin_line = info.depots_num+1;
        for(int i =begin_line; i<info.customer_num+begin_line; i++){
            info.demands.push_back(res[i][4]);
        }
        int demand = 0;
        for(int i =0;i<info.demands.size(); i++){
            demand += info.demands[i];
        }
        // location infos
        int len = info.depots_num + info.customer_num;
        for(int i = len+1; i<len+1+info.depots_num; i++){
            vector<int> xy;
            xy.push_back(res[i][1]);
            xy.push_back(res[i][2]);
            info.loc.push_back(xy);
        }
        for(int i=begin_line; i<begin_line + info.customer_num; i++){
            vector<int> xy;
            xy.push_back(res[i][1]);
            xy.push_back(res[i][2]);
            info.loc.push_back(xy);
        }
    }else if(data_type == 0){
        infile.open(path_0_depots, ios::in);
        if(!infile.is_open()){
            cout<<"can not open this file"<<endl;
            return info;
        }
        string s;
        vector<vector<int>> res;
        while (getline(infile, s)){
            stringstream input(s);
            int result;
            vector<int> in;
            while(input>>result){
                in.push_back(result);
            }
            res.push_back(in);
        }
        info.customer_num = res[0][0]; // customer number
        info.capacity = res[0][1]; // capacity
        info.depots_num = res[0].size()-2; // depot number
        info.vehicle_num = 0;
        for(int i = 2;i<res[0].size();i++){
            info.vehicles.push_back(res[0][i]);
            info.vehicle_num = info.vehicle_num+res[0][i];
        } // vehiclle number of corresponding depots
        for(int i =0; i<info.customer_num; i++){
            info.demands.push_back(res[1][i]);
        } // customer demand
        int len = info.depots_num + info.customer_num;
        for(int i=2; i<len+2; i++){
            vector<int> xy;
            for(int j=0; j<2;j++){
                xy.push_back(res[i][j]);
            }
            info.loc.push_back(xy);
        } // location infos
    }
    infile.close();
    return info;
}



Info info = readTxt();
int depot_number = info.depots_num;
int customer_number = info.customer_num;
int vehicle_number = info.vehicle_num;
int capacity = info.capacity;

int demand_all(){
    int demand = 0;
    for(int i =0;i<info.demands.size(); i++){
        demand += info.demands[i];
    }
    return demand;
}
int demand = demand_all();
// prevent the change of demand
vector<int> demand_copy(){
    vector<int> demand_cp;
    demand_cp.assign(info.demands.begin(), info.demands.end());
    return demand_cp;
}
vector<int> demand_cp = demand_copy();

vector<CITIES> city(Info info){
    vector<CITIES> v;
    int size = info.loc.size();
    CITIES city;
    for(int i = 0;i<size;i++){
        city.x =info.loc[i][0];
        city.y =info.loc[i][1];
        if(i<info.depots_num){
            city.d =0;
        }else{
            city.d =info.demands[i-info.depots_num];
        }
        v.push_back(city);
    }
    return v;
}


unsigned int adjMatrix[vetex_number][vetex_number] = { 0 };
//vector<vector<int>> adjMatrix;

//calculate distance
int distance(CITIES c1, CITIES c2)
{
    int distance=0;
    distance=sqrt((double)((c1.x-c2.x)*(c1.x-c2.x)+(c1.y-c2.y)*(c1.y-c2.y)));
    return distance;
}

//initial adjacent matrix
void Matrix(vector<CITIES> cities)
{
    for (int i = 0; i < vetex_number; i++){
        for (int j = 0; j < vetex_number; j++)
        {
            CITIES c1 = cities[i];
            CITIES c2 = cities[j];
            adjMatrix[i][j] = distance(c1, c2);
        }
    }
}

vector<int> idx;
vector<int> minDepots(int depots_num){
    vector<int> tmp;
    for(int i = 0; i<vetex_number; i++){
        int mini = adjMatrix[i][0];
        int index = 0;
        for (int j = 1; j < depots_num; j++){
            if(adjMatrix[i][j] < mini){
                mini = adjMatrix[i][j];
                index = j;
            }
        }
        tmp.push_back(mini);
        idx.push_back(index);
    }
    return tmp;
}

struct node{
    VertexData data;
    unsigned int lowestcost;
}pNode[vetex_number];


// minimize edge cost
int Mini(struct node *node){
    unsigned int min = INFINITE;
    int index = -1;
    for (int i = 0; i < vetex_number; i++){
        if(node[i].lowestcost < min && node[i].lowestcost != 0){
            min = node[i].lowestcost;
            index = i;
        }
    }
    return index;
}

void ReadArc(unsigned int adjMat[][vetex_number], vector<Arc> &vertexArc){
    Arc *tmp = NULL;
    for (unsigned int i = 0; i < vetex_number; i++){
        for (unsigned int j = i+1; j < vetex_number; j++){
            if(adjMat[i][j] != INFINITE){
                tmp = new Arc;
                tmp->u = i;
                tmp->v = j;
                tmp->cost = adjMat[i][j];
                vertexArc.push_back(*tmp);
            }
        }
    }
}

void ReadArcDepots(unsigned int adjMat_depots[][vetex_number], vector<Arc> &vertexArc){
    Arc *tmp = NULL;
    int num = info.customer_num+1;
    for (unsigned int i = 0; i < num; i++){
        for (unsigned int j = i+1; j < num; j++){
            if(adjMat_depots[i][j] != INFINITE){
                tmp = new Arc;
                tmp->u = i;
                tmp->v = j;
                tmp->cost = adjMat_depots[i][j];
                vertexArc.push_back(*tmp);
            }
        }
    }
}

bool compare(Arc A, Arc B){
    return A.cost < B.cost;
}

//Judge whether vertexes are in the same tree, if not, combine and return true; else return false.
bool FindTree(VertexData u,VertexData v, vector<vector<VertexData>> &TreeS){
    unsigned int index_u = INFINITE;
    unsigned int index_v = INFINITE;
    for(unsigned int i = 0;i < TreeS.size(); i++){
        if (find(TreeS[i].begin(), TreeS[i].end(), u) != TreeS[i].end())
            index_u = i;
        if (find(TreeS[i].begin(), TreeS[i].end(), v) != TreeS[i].end())
            index_v = i;
    }
    if (index_u != index_v){
        for (unsigned int i = 0; i < TreeS[index_v].size(); i++){
            TreeS[index_u].push_back(TreeS[index_v][i]);
        }
        TreeS[index_v].clear();
        return true;
    }
    return false;
}

unsigned int adjMat[vetex_number][vetex_number];

void printTree0(Tree t){
    cout<<endl;
    for (int i = 0; i<vetex_number; i++){
        for (int j = 0; j<vetex_number; j++){
            cout << setw(3) << t.tree[i][j];
        }
        cout<<endl;
    }
}


Tree Kruskal(unsigned int adjMat[][vetex_number]){
    vector<Arc> vertexArc;
    Tree vertexTree;
    // initial path infos
    for (unsigned int i = 0; i < vetex_number; i++){
        for (unsigned int j = 0; j < vetex_number; j++){
            vertexTree.tree[i][j] = 0;
            vertexTree.cost = 0;
        }
    }
    ReadArc(adjMat, vertexArc);
    sort(vertexArc.begin(), vertexArc.end(), compare);//ascending order
    vector<vector<VertexData>> TreeS(vetex_number);
    for (unsigned int i = 0; i < vetex_number; i++){
        TreeS[i].push_back(i);
    }
    for (unsigned int i = 0; i < vertexArc.size(); i++){
        VertexData u = vertexArc[i].u;
        VertexData v = vertexArc[i].v;
        if (FindTree(u, v, TreeS)){
            cout << u << "---" << v << endl;
            vertexTree.tree[u][v] = 1;
            vertexTree.cost = vertexArc[i].cost;
        }
    }
    return vertexTree;
}

// minimize spanning tree
TreeDepots Kruskal_depots(unsigned int matrixWithDepots[][vetex_number]){
    vector<Arc> vertexArc;
    TreeDepots vertexTree;
    int num = info.customer_num+1;
    for (unsigned int i = 0; i < num; i++){
        for (unsigned int j = 0; j < num; j++){
            vertexTree.tree[i][j] = 0;
            vertexTree.cost = 0;
        }
    }
    ReadArcDepots(matrixWithDepots, vertexArc);
    sort(vertexArc.begin(), vertexArc.end(), compare);
    vector<vector<VertexData>> TreeS(num);
    for (unsigned int i = 0; i < num; i++){
        TreeS[i].push_back(i);
    }
    for (unsigned int i = 0; i < vertexArc.size(); i++){
        VertexData u = vertexArc[i].u;
        VertexData v = vertexArc[i].v;
        if (FindTree(u, v, TreeS)){
            vertexTree.tree[u][v] = 1;
            vertexTree.cost = vertexArc[i].cost;
        }
    }
    return vertexTree;
}


unsigned int matrixWithDepots[][vetex_number-1] = { 0 };

unsigned int matrixWithMultiDepots[][vetex_number] = { 0 };

void MultiDepotsToMatrix(){
    vector<int> tmp;
    tmp = minDepots(info.depots_num);
    int num = vetex_number-info.depots_num;
    for(int i = 0; i < num; i++){
        for(int j = 0; j < num; j++){
            matrixWithMultiDepots[i][j] = adjMatrix[i+info.depots_num][j+info.depots_num];
        }
    }

    for(int i=0; i<num; i++){
        matrixWithMultiDepots[i][num] = tmp[i+info.depots_num];
        matrixWithMultiDepots[num][i] = tmp[i+info.depots_num];
    }
}

// combine two depots
void DepotsToMatrix(int c1 , int c2){
    vector<int> tmp;
    tmp = minDepots(info.depots_num);
    for(int i = 0; i < vetex_number; i++){
        for(int j = 0; j < vetex_number; j++){
            if (i<c1 && j<c1){
                matrixWithDepots[i][j] = adjMatrix[i][j];
            }
            if (i<c1 && c1<j && j<c2){
                matrixWithDepots[i][j-1] = adjMatrix[i][j];
            }
            if (i<c1 && c2<j){
                matrixWithDepots[i][j-2] = adjMatrix[i][j];
            }
            if (c1<i && i<c2 && j<c1){
                matrixWithDepots[i-1][j] = adjMatrix[i][j];
            }
            if (c1<i && i<c2 && c1<j && j<c2){
                matrixWithDepots[i-1][j-1] = adjMatrix[i][j];
            }
            if (c1<i && i<c2 && c2<j){
                matrixWithDepots[i-1][j-2] = adjMatrix[i][j];
            }
            if (c2<i && j<c1){
                matrixWithDepots[i-2][j] = adjMatrix[i][j];
            }
            if (c2<i && c1<j && j<c2){
                matrixWithDepots[i-2][j-1] = adjMatrix[i][j];
            }
            if (c2<i && c2<j){
                matrixWithDepots[i-2][j-2] = adjMatrix[i][j];
            }
        }
    }

    int c = 0;
    for (vector<int>::iterator it = tmp.begin(); it != tmp.end(); it++){
        if ( c == c1){
            it = tmp.erase(it);
        }else if(c == c2){
            it = tmp.erase(it-1);
        }
        c = c+1;
    }
    for(int i=0; i<vetex_number-2; i++){
        matrixWithDepots[i][vetex_number-2] = tmp[i];
        matrixWithDepots[vetex_number-2][i] = tmp[i];
    }
}

void printTreeDepots(TreeDepots t){
    for (int i = 0; i<info.customer_num+1; i++){
        for (int j = 0; j<info.customer_num+1; j++){
        }
    }
}

//Split the tree to forest
Tree TreetoForest(TreeDepots d){
    Tree vertexForest;
    for (unsigned int i = 0; i < vetex_number; i++){
        for (unsigned int j = 0; j < vetex_number; j++){
            vertexForest.tree[i][j] = 0;
            vertexForest.cost = 0;
        }
    }
    vector<Arc> arc;
    int c = 0;
    Arc *tmp = NULL;
    int n = customer_number;
    for (unsigned int i = 0; i < n; i++){
        if(d.tree[i][n] == 1){
            tmp = new Arc;
            tmp->u = i+info.depots_num;
            tmp->v = idx[i+info.depots_num];
            tmp->cost = d.cost;
            arc.push_back(*tmp);
        }
    }
    for (unsigned int i = 0; i < arc.size(); i++){
        VertexData u = arc[i].u;
        VertexData v = arc[i].v;
        if (u<v){
            vertexForest.tree[u][v] = 1;
            vertexForest.cost = arc[i].cost;
        } else{
            vertexForest.tree[v][u] = 1;
            vertexForest.cost = arc[i].cost;
        }
    }
    for (unsigned int i = 0; i < info.customer_num; i++){
        for (unsigned int j = i; j < info.customer_num; j++){
            vertexForest.tree[i+info.depots_num][j+info.depots_num] = d.tree[i][j];
            vertexForest.cost = d.cost;
        }
    }
    return vertexForest;
}

// Minimize spanning tree
Tree Spanning_forest(vector<CITIES> vertices)
{
    Matrix(vertices);
    MultiDepotsToMatrix();
    TreeDepots spanning_tree;
    spanning_tree =  Kruskal_depots(matrixWithMultiDepots);
    printTreeDepots(spanning_tree);
    Tree spanning_forest;
    spanning_forest = TreetoForest(spanning_tree);
    return spanning_forest;
}

// Function of finding edge
vector<vector<Arc>> Subedge(Tree tree)
{
    vector<vector<Arc>> E;
    vector<Arc> E_1;
    vector<Arc> E_2;
    Arc *tmp=NULL;
    for (int i = 0; i<vetex_number; i++){
        for(int j = i+1; j<vetex_number; j++){
            tmp = new Arc;
            tmp->u = i;
            tmp->v = j;
            tmp->cost = tree.cost;
            if(tree.tree[i][j] == 1){
                E_1.push_back(*tmp);
            }else{
                E_2.push_back(*tmp);
            }
        }
    }
    E.push_back(E_1);
    E.push_back(E_2);
    return E;
}

// permulation and combination
vector<vector<int>> combine(int size, int num, int limit){
    vector<vector<int>> idx_combine;
    if (num == 2){
        for(int i=0; i<size-1; i++){
            for(int j=i+1; j<size; j++){
                vector<int> idx;
                idx.push_back(i);
                idx.push_back(j);
                idx_combine.push_back(idx);
            }
            //limit=0 is without limit
            if(limit !=0 && limit <= idx_combine.size())
                break;
        }
    } else if(num == 3){
        for(int i=0; i<size-2; i++){
            for(int j=i+1; j<size-1; j++){
                for(int m=j+1; m<size; m++){
                    vector<int> idx;
                    idx.push_back(i);
                    idx.push_back(j);
                    idx.push_back(m);
                    idx_combine.push_back(idx);
                }
            }
            if(limit !=0 && limit <= idx_combine.size())
                break;
        }
    }else if(num == 4){
        for(int i=0; i<size-3; i++){
            for(int j=i+1; j<size-2; j++){
                for(int m=j+1; m<size-1; m++){
                    for(int n=m+1; n<size; n++){
                        vector<int> idx;
                        idx.push_back(i);
                        idx.push_back(j);
                        idx.push_back(m);
                        idx.push_back(n);
                        idx_combine.push_back(idx);
                    }
                    if(limit !=0 && limit <= idx_combine.size())
                        break;
                }

                if(limit !=0 && limit <= idx_combine.size())
                    break;
            }
            if(limit !=0 && limit <= idx_combine.size())
                break;
        }
    }else if(num == 5){
        for(int i=0; i<size-4; i++){
            for(int j=i+1; j<size-3; j++){
                for(int m=j+1; m<size-2; m++){
                    for(int n=m+1; n<size-1; n++){
                        for(int a=n+1; a<size; a++){
                            vector<int> idx;
                            idx.push_back(i);
                            idx.push_back(j);
                            idx.push_back(m);
                            idx.push_back(n);
                            idx.push_back(a);
                            idx_combine.push_back(idx);
                        }
                        if(limit !=0 && limit <= idx_combine.size())
                            break;
                    }
                    if(limit !=0 && limit <= idx_combine.size())
                        break;
                }
                if(limit !=0 && limit <= idx_combine.size())
                    break;
            }

            if(limit !=0 && limit <= idx_combine.size())
                break;
        }
    }
    return idx_combine;
}


// Edge exchange
vector<vector<Arc>> Add(vector<vector<Arc>> edges)
{
    vector<vector<Arc>> edges_transfer;
    edges_transfer.push_back(edges[0]);
    int k = depot_number-1;
    while(k > 0) {
        //change one edge
        if (k == 1) {
            int transfer_size_1 = edges_transfer.size();
            for (int i = 0; i < edges[0].size(); i++) {
                for (int j = 0; j < edges[1].size(); j++) {
                    vector<Arc> e;
                    for (int n = 0; n < edges[0].size(); n++) {
                        if (n != i) {
                            e.push_back(edges[0][n]);
                        } else {
                            e.push_back(edges[1][j]);
                        }
                    }
                    edges_transfer.push_back(e);
                    if (edges_transfer.size() >= transfer_limit_all)
                        break;
                }
                if (edges_transfer.size() >= transfer_limit_all)
                    break;
            }
            k--;
        } else if (k == 2) {
            vector<vector<int>> idx_0;
            vector<vector<int>> idx_1;
            int transfer_size_2 = edges_transfer.size();
            idx_0 = combine(edges[0].size(), k, transfer_idx_limit);
            idx_1 = combine(edges[1].size(), k, transfer_idx_limit);
            for (int i = 0; i < idx_0.size(); i++) {
                for (int j = 0; j < idx_1.size(); j++) {
                    vector<Arc> e;
                    for (int n = 0; n < edges[0].size(); n++) {
                        if ((n != idx_0[i][0]) && (n != idx_0[i][1])) {
                            e.push_back(edges[0][n]);
                        } else if (n == idx_0[i][0]) {
                            e.push_back(edges[1][idx_1[j][0]]);
                        } else if (n == idx_0[i][1]) {
                            e.push_back(edges[1][idx_1[j][1]]);
                        }
                    }
                    edges_transfer.push_back(e);
                    if ((edges_transfer.size() - transfer_size_2) >= transfer_limit)
                        break;
                }
                if ((edges_transfer.size() - transfer_size_2) >= transfer_limit)
                    break;
            }
            k--;
        } else if (k == 3) {
            vector<vector<int>> idx_0;
            vector<vector<int>> idx_1;
            int transfer_size_3 = edges_transfer.size();
            idx_0 = combine(edges[0].size(), k, transfer_idx_limit);
            idx_1 = combine(edges[1].size(), k, transfer_idx_limit);
            for (int i = 0; i < idx_0.size(); i++) {
                for (int j = 0; j < idx_1.size(); j++) {
                    vector<Arc> e;
                    for (int n = 0; n < edges[0].size(); n++) {
                        if ((n != idx_0[i][0]) && (n != idx_0[i][1]) && (n != idx_0[i][2])) {
                            e.push_back(edges[0][n]);
                        } else if (n == idx_0[i][0]) {
                            e.push_back(edges[1][idx_1[j][0]]);
                        } else if (n == idx_0[i][1]) {
                            e.push_back(edges[1][idx_1[j][1]]);
                        } else if (n == idx_0[i][2]) {
                            e.push_back(edges[1][idx_1[j][2]]);
                        }
                    }
                    edges_transfer.push_back(e);
                    if ((edges_transfer.size() - transfer_size_3) >= transfer_limit)
                        break;
                }
                if ((edges_transfer.size() - transfer_size_3) >= transfer_limit)
                    break;
            }
            k--;
        }
        else if (k == 4) {
            vector<vector<int>> idx_0;
            vector<vector<int>> idx_1;
            int transfer_size_4 = edges_transfer.size();
            idx_0 = combine(edges[0].size(), k, transfer_idx_limit);
            idx_1 = combine(edges[1].size(), k, transfer_idx_limit);
            for (int i = 0; i < idx_0.size(); i++) {
                for (int j = 0; j < idx_1.size(); j++) {
                    vector<Arc> e;
                    for (int n = 0; n < edges[0].size(); n++) {
                        if ((n != idx_0[i][0]) && (n != idx_0[i][1]) && (n != idx_0[i][2]) && (n != idx_0[i][3])) {
                            e.push_back(edges[0][n]);
                        } else if (n == idx_0[i][0]) {
                            e.push_back(edges[1][idx_1[j][0]]);
                        } else if (n == idx_0[i][1]) {
                            e.push_back(edges[1][idx_1[j][1]]);
                        } else if (n == idx_0[i][2]) {
                            e.push_back(edges[1][idx_1[j][2]]);
                        } else if (n == idx_0[i][3]) {
                            e.push_back(edges[1][idx_1[j][3]]);
                        }
                    }
                    edges_transfer.push_back(e);
                    if ((edges_transfer.size() - transfer_size_4) >= transfer_limit)
                        break;
                }
                if ((edges_transfer.size() - transfer_size_4) >= transfer_limit)
                    break;
            }
            k--;
        } else if (k == 5) {
            vector<vector<int>> idx_0;
            vector<vector<int>> idx_1;
            int transfer_size_5 = edges_transfer.size();
            idx_0 = combine(edges[0].size(), k, transfer_idx_limit);
            idx_1 = combine(edges[1].size(), k, transfer_idx_limit);
            for (int i = 0; i < idx_0.size(); i++) {
                for (int j = 0; j < idx_1.size(); j++) {
                    vector<Arc> e;
                    for (int n = 0; n < edges[0].size(); n++) {
                        if ((n != idx_0[i][0]) && (n != idx_0[i][1]) && (n != idx_0[i][2]) && (n != idx_0[i][3]) &&
                        (n != idx_0[i][4])){
                            e.push_back(edges[0][n]);
                        } else if (n == idx_0[i][0]) {
                            e.push_back(edges[1][idx_1[j][0]]);
                        } else if (n == idx_0[i][1]) {
                            e.push_back(edges[1][idx_1[j][1]]);
                        } else if (n == idx_0[i][2]) {
                            e.push_back(edges[1][idx_1[j][2]]);
                        } else if (n == idx_0[i][3]) {
                            e.push_back(edges[1][idx_1[j][3]]);
                        } else if (n == idx_0[i][4]) {
                            e.push_back(edges[1][idx_1[j][4]]);
                        }
                    }
                    edges_transfer.push_back(e);
                    if ((edges_transfer.size() - transfer_size_5) >= transfer_limit)
                        break;
                }
                if ((edges_transfer.size() - transfer_size_5) >= transfer_limit)
                    break;
            }
            k--;
        }
    }
    return edges_transfer;
}


// Verify whether tree is CSF
bool VerifyCSF(vector<Arc> arc, int k){
    vector<vector<VertexData>> TreeS(vetex_number);
    for (unsigned int i = 0; i < vetex_number; i++){
        TreeS[i].push_back(i);
    }
    for (unsigned int i = 0; i < arc.size(); i++){
        VertexData u = arc[i].u;
        VertexData v = arc[i].v;
        unsigned int index_u = INFINITE;
        unsigned int index_v = INFINITE;
        for(int i = 0;i < TreeS.size(); i++){
            if (find(TreeS[i].begin(), TreeS[i].end(), u) != TreeS[i].end())//u是否在Tree[i]上
                index_u = i;
            if (find(TreeS[i].begin(), TreeS[i].end(), v) != TreeS[i].end())
                index_v = i;
        }
        if (index_u != index_v){
            for (unsigned int i = 0; i < TreeS[index_v].size(); i++){
                TreeS[index_u].push_back(TreeS[index_v][i]);
            }
            TreeS[index_v].clear();
        }
    }
    int n = 0;
    for (int i = 0;i < TreeS.size(); i++){
        if (TreeS[i].size() == 1){
            return false;
        }
        if(TreeS[i].size()>0){
            n = n+1;
        }
    }
    if (n==k){
        return true;
    }else{
        return false;
    }
}

//Verify whether tree is forest
bool VerifyForest(vector<Arc> arc) {
    vector<vector<VertexData>> TreeS(vetex_number);
    for (unsigned int i = 0; i < vetex_number; i++) {
        TreeS[i].push_back(i);
    }
    for (unsigned int i = 0; i < arc.size(); i++) {
        VertexData u = arc[i].u;
        VertexData v = arc[i].v;
        unsigned int index_u = INFINITE;
        unsigned int index_v = INFINITE;
        for (int i = 0; i < TreeS.size(); i++) {
            if (find(TreeS[i].begin(), TreeS[i].end(), u) != TreeS[i].end())
                index_u = i;
            if (find(TreeS[i].begin(), TreeS[i].end(), v) != TreeS[i].end())
                index_v = i;
        }
        if (index_u != index_v) {
            for (unsigned int i = 0; i < TreeS[index_v].size(); i++) {
                TreeS[index_u].push_back(TreeS[index_v][i]);
            }
            TreeS[index_v].clear();
        }
    }
    int n = 0;
    for (int i = 0; i < TreeS.size(); i++) {
        if (TreeS[i].size() == 1) {
            return false;
        }
    }
    return true;
}

int compute_cost(vector<Arc> arc){
    int cost = 0;
    int m;
    for (int i=0; i<arc.size(); i++){
        m = adjMatrix[arc[i].u][arc[i].v];
        cost = cost+m;
    }
    return cost;
}


vector<vector<VertexData>> edgeToTree(vector<Arc> arc){
    vector<vector<VertexData>> TreeS(vetex_number);
    for (unsigned int i = 0; i < vetex_number; i++){
        TreeS[i].push_back(i);
    }
    for (unsigned int i = 0; i < arc.size(); i++){
        VertexData u = arc[i].u;
        VertexData v = arc[i].v;
        unsigned int index_u = INFINITE;
        unsigned int index_v = INFINITE;
        for(int i = 0;i < TreeS.size(); i++){
            if (find(TreeS[i].begin(), TreeS[i].end(), u) != TreeS[i].end())//u是否在Tree[i]上
                index_u = i;
            if (find(TreeS[i].begin(), TreeS[i].end(), v) != TreeS[i].end())
                index_v = i;
        }
        if (index_u != index_v){
            for (unsigned int i = 0; i < TreeS[index_v].size(); i++){
                TreeS[index_u].push_back(TreeS[index_v][i]);
            }
            TreeS[index_v].clear();
        }
    }
    vector<vector<VertexData>> vtree;
    for (int i = 0;i < TreeS.size(); i++){
        if(TreeS[i].size()>1){
            vtree.push_back(TreeS[i]);
        }
    }
    return vtree;
}

//Add edges between two trees
vector<Arc> addArc(vector<vector<VertexData>> tree){
    vector<Arc> arclist;
    Arc *tmp=NULL;
    int treeNum = tree.size();
    for(int i = 0; i<treeNum-1; i++){
        for(int j = i+1; j< treeNum; j++){
            for(int m = 0; m<tree[i].size();m++){
                for(int n = 0; n<tree[j].size();n++){
                    tmp = new Arc;
                    tmp->u = tree[i][m];
                    tmp->v = tree[j][n];
                    tmp->cost = adjMatrix[tmp->u][tmp->v];
                    arclist.push_back(*tmp);
                }
            }
        }
    }
    return arclist;
}


//Add edges between multiple trees and select edge with minimum cost
vector<Arc> addArcMultiDepots(vector<vector<VertexData>> tree){
    vector<Arc> miniarclist;
    Arc *tmp = NULL;
    int treeNum = tree.size();
    unsigned int minCost = INFINITE;
    for(int i = 0; i<treeNum-1; i++){
        int j = i+1;
        vector<Arc> arclist;
        for(int m = 0; m<tree[i].size();m++){
            for(int n = 0; n<tree[j].size();n++){
                tmp = new Arc;
                tmp->u = tree[i][m];
                tmp->v = tree[j][n];
                tmp->cost = adjMatrix[tmp->u][tmp->v];
                if(minCost>tmp->cost){
                    minCost = tmp->cost;
                }
                arclist.push_back(*tmp);
            }
        }
        Arc mini;
        for(auto mini:arclist){
            if(mini.cost == minCost){
                miniarclist.push_back(mini);
                break;
            }
        }
    }
    return miniarclist;
}


bool feasibility (vector<vector<VertexData>> tree, vector<CITIES> vertices)
{
    int vehicle_demand = 0;
    for (int i=0; i<tree.size(); i++){
        int demands = 0;
        for(int j = 0; j<tree[i].size(); j++){
            demands=demands+vertices[tree[i][j]].d;
        }
        int p=ceil(double (demands)/capacity);
        vehicle_demand += p;
    }
    if (vehicle_demand > vehicle_number){
        return false;
    }else
        return true;
}


// Verify the degree
vector<int> Odd(vector<Arc> forest)
{
    int degree[vetex_number] = {0};
    for(int i=0; i<forest.size(); i++){
        int u = forest[i].u;
        int v = forest[i].v;
        degree[u] ++;
        degree[v] ++;
    }
    vector<int> oddvertex;
    for(int j=0; j<vetex_number; j++){
        if(degree[j]%2 == 1){
            oddvertex.push_back(j);
        }
    }
    return oddvertex;
}

#define inf 0x3f3f3f3f
int nx,ny;
int link[M],lx[M],ly[M],slack[M];
int visx[M],visy[M];
int w[M][M];

int DFS(int x)
{
    visx[x] = 1;
    for (int y = 1;y <= ny;y ++)
    {
        if (visy[y])
            continue;
        int t = lx[x] + ly[y] - w[x][y];
        if (t == 0)       //
        {
            visy[y] = 1;
            if (link[y] == -1||DFS(link[y]))
            {
                link[y] = x;
                return 1;
            }
        }
        else if (slack[y] > t)
            slack[y] = t;
    }
    return 0;
}

vector<int> KM()
{
    int i,j;
    memset (link,-1,sizeof(link));
    memset (ly,0,sizeof(ly));
    for (i = 1;i <= nx;i ++)
        for (j = 1,lx[i] = -inf;j <= ny;j ++)
            if (w[i][j] > lx[i])
                lx[i] = w[i][j];
    for (int x = 1;x <= nx;x ++)
    {
        for (i = 1;i <= ny;i ++)
            slack[i] = inf;
        while (1)
        {
            memset (visx,0,sizeof(visx));
            memset (visy,0,sizeof(visy));
            if (DFS(x))
                break;
            int d = inf;
            for (i = 1;i <= ny;i ++)
                if (!visy[i]&&d > slack[i])
                    d = slack[i];
            for (i = 1;i <= nx;i ++)
                if (visx[i])
                    lx[i] -= d;
            for (i = 1;i <= ny;i ++)
                if (visy[i])
                    ly[i] += d;
                else
                    slack[i] -= d;
        }
    }
    int res = 0;
    vector<int> v;
    for (i = 1;i <= ny;i ++)
        if (link[i] > -1){
            res += w[link[i]][i];
            v.push_back(link[i]);
        }
    return v;
}

//Minimum matching
vector<int> MatchingKM(vector<int> Odds)
{
    int num = Odds.size();
    unsigned int adjM[num][num];
    for (int a = 0; a < num; a++) {
        for (int b = 0; b < num; b++) {
            int p;
            int q;
            p = Odds[a];
            q = Odds[b];
            adjM[a][b] = adjMatrix[p][q];
        }
    }
    for(int i=1;i<=num;i++){
        for (int j=1;j<=num;j++){
            if(i==j){
                w[i][j] = -inf;
            }else{
                w[i][j] = 0-adjM[i-1][j-1];
            }
        }
    }
    nx = ny = num;
    vector<int> link;
    link = KM();
    return link;
}

pair< Graph, vector<double> > GetWeightedGraph(vector<int> Odds){
    int u, v;
    int n = Odds.size();
    Graph G(n);
    vector<double> cost;
    for(int i = 0; i<n; i++){
        for(int j = i+1; j<n; j++){
            u = Odds[i];
            v = Odds[j];
            G.AddEdge(i, j);
            cost.push_back(adjMatrix[u][v]);
        }
    }
    return make_pair(G, cost);
}


vector<Arc> MinimumCostPerfectMatching(vector<int> Odds)
{
    Graph G;
    vector<double> cost;
    //Read the graph
    pair< Graph, vector<double> > p = GetWeightedGraph(Odds);
    //pair< Graph, vector<double> > p = CreateRandomGraph();
    G = p.first;
    cost = p.second;
    //Create a Matching instance passing the graph
    //Pass the costs to solve the problem
    //The algorithm for finding a perfect match in a general graph is the standard code from the Chinese Software Developer Network(CSDN). It cannot be shared due to copyright reasons. The link is as follow:https://download.csdn.net/download/weixin_42105169/18558404?username=u013888261
    Matching MM(G);
    pair< list<int>, double > solution = MM.SolveMinimumCostPerfectMatching(cost);
    list<int> matching = solution.first;
    double obj = solution.second;
    vector<Arc> match_arc;
    Arc arc;
    for(list<int>::iterator it = matching.begin(); it != matching.end(); it++)
    {
        pair<int, int> e = G.GetEdge( *it );
        arc.u = Odds[e.first];
        arc.v = Odds[e.second];
        arc.cost = adjMatrix[arc.u][arc.v];
        match_arc.push_back(arc);
    }
    return match_arc;
}


vector<vector<int>> Matching_vd(vector<vector<int>> s)
{
    int num = vehicle_number;
    vector<vector<int>> d_s;
    vector<int> xy;
    vector<int> min_index;
    for(int m=0; m<depot_number; m++){
        vector<int> d_s_min;
        for(int i = 0;i<s.size();i++){
            int min = adjMatrix[m][s[i][0]];
            int index = 0;
            int tmp = 0;
            for(int j=1; j<s[i].size(); j++){
                tmp = adjMatrix[m][s[i][j]];
                if(min > tmp){
                    min = tmp;
                    index = j;
                }
            }
            min_index.push_back(index);
            d_s_min.push_back(min);
        }
        d_s.push_back(d_s_min);
    }
    vector<int> ind_vd;

    for(int i=1; i<=num; i++){
        int index=0, v= 0;
        for(int n = 0; n<info.vehicles.size(); n++){
            v = v+info.vehicles[n];
            if(i<=v){
                index = n;
                break;
            }
        }
        ind_vd.push_back(index);
        for (int j=1;j<=s.size();j++){
            w[i][j] = 0-d_s[index][j-1];
        }
        for(int m = vehicle_number; m>s.size(); m--){
            w[i][m] = 0;
        }
    }
    nx = ny = num;
    vector<int> link;
    link = KM();
    vector<vector<int>> all_path;
    for(int i=0; i<s.size(); i++){
        vector<int> path;
        if(find(s[i].begin(), s[i].end(),ind_vd[link[i]-1]) != s[i].end()){
            vector<int>::iterator it = find(s[i].begin(), s[i].end(),ind_vd[link[i]-1]);
            path.assign(it, s[i].end());
            path.insert(path.end(), s[i].begin(), it);
        } else{
            path.push_back(ind_vd[link[i]-1]);
            path.push_back(s[i][min_index[i]]);
            for(int j=min_index[i]-1; j>=0; j--){
                path.push_back(s[i][j]);
            }
            for(int j=min_index[i]+1; j<s[i].size();j++){
                path.push_back(s[i][j]);
            }
        }
        path.push_back(ind_vd[link[i]-1]);
        all_path.push_back(path);
    }
    return all_path;
}

bool checkMatching(vector<Arc> arcs){
    int degree[vetex_number] = { 0 };
    for(vector<Arc>::iterator it = arcs.begin(); it != arcs.end(); it++){
        degree[it->u]++;
        degree[it->v]++;
    }
    for(int i=0; i<vetex_number;i++){
        if(degree[i]%2 != 0){
            return false;
        }
    }
    return true;
}

vector<vector<int>> shortcut(vector<Arc> arcs){
    vector<vector<int>> path_all;
    while (arcs.size() != 0){
        int degree[vetex_number] = { 0 };
        //The degree of each node is counted
        for(vector<Arc>::iterator it = arcs.begin(); it != arcs.end(); it++){
            degree[it->u]++;
            degree[it->v]++;
        }
        vector<int> path;
        // The first edge is added directly to path
        path.push_back(arcs[0].u);
        path.push_back(arcs[0].v);
        int p = arcs[0].v;
        degree[arcs[0].u]--;
        degree[arcs[0].v]--;
        arcs.erase(arcs.begin());
        // Find edge
        while(degree[p] != 0){
            vector<Arc> arc_cp;
            int p_add_n = 0;
            for(vector<Arc>::iterator it = arcs.begin(); it != arcs.end(); it++){
                if(p == it->u){
                    p = it->v;
                    path.push_back(p);
                    degree[it->u]--;
                    degree[it->v]--;
                    p_add_n++;
                }else if(p == it->v){
                    p = it->u;

                    path.push_back(p);
                    p_add_n++;
                    degree[it->u]--;
                    degree[it->v]--;
                }else{
                    arc_cp.push_back(*it);
                }
            }
            arcs.clear();
            arcs.assign(arc_cp.begin(), arc_cp.end());
        }
        path_all.push_back(path);
    }
    return path_all;
}

vector<int> path_cut(vector<int> path){
    vector<int> path_after;
    path_after.push_back(path[0]);
    for(int i=1; i<path.size();i++){
        if(find(path_after.begin(), path_after.end(), path[i]) == path_after.end()){
            path_after.push_back(path[i]);
        }
    }
    return path_after;
}

vector<int> findPath(vector<vector<int>> path_all){
    vector<int> path = path_cut(path_all[0]);
    for (int i = 0; i<path_all.size()-1; i++){
        int start = path_all[i+1][0];
        if(find(path.begin(), path.end(), start) != path.end()){
            vector<int> path_cp;
            path.insert(find(path.begin(), path.end(), start)+1, path_all[i+1].begin()+1, path_all[i+1].end());
            path = path_cut(path);
        }else{
            vector<int> path_cp;
            for(int j = 1; j<path_all[i+1].size(); j++){
                start = path_all[i+1][j];
                if(find(path.begin(), path.end(), start) != path.end()){
                    vector<int>::iterator it = find(path_all[i+1].begin(), path_all[i+1].end(), start);
                    path_cp.insert(path_cp.begin(), it, path_all[i+1].end());
                    path_cp.insert(path_cp.end(), path_all[i+1].begin(), it);
                    path.insert(find(path.begin(), path.end(), start)+1, path_cp.begin()+1, path_cp.end());
                    path = path_cut(path);
                    break;
                }else if(j == path_all[i+1].size()-1 && find(path.begin(), path.end(), start) == path.end()){
                    path.insert(path.end(), path_all[i+1].begin(), path_all[i+1].end());
                    path = path_cut(path);
                    break;
                }
            }

        }
    }
    if(path.size() == vetex_number){
        return path;
    }
}


pair<vector<vector<int>>, vector<map<int,int>>> cycle_split(vector<int> path){
    vector<vector<int>> S;
    vector<map<int,int>> demand_list;
    //Calculate n_c
    int demands = 0;
    for(int j = 0; j<info.demands.size(); j++){
        demands=demands+info.demands[j];
    }
    int n_c = ceil((double)demands/capacity);
    // Copy to demand_cp (demands need to be changed later)
    int demand_cp[vetex_number] = {0};
    for(int a=0; a<vetex_number; a++){
        if(a >= depot_number){
            demand_cp[a] = info.demands[a-depot_number];
        }else{
            demand_cp[a] = 0;
        }
    }
    int p = 0;
    for(int m=0; m<n_c;m++){
        int q = 0;
        vector<int> s_p;
        int d = 0;
        map<int, int> x;
        for(int n=p;n<path.size();n++){
            if(path[n] >= depot_number){
                q = q + demand_cp[path[n]];
            }
            s_p.push_back(path[n]);
            p = n;
            if(path[n] >= depot_number){
                if(q >= capacity){
                    d += demand_cp[path[n]]-(q-capacity);
                    x[path[n]] = demand_cp[path[n]]-(q-capacity);
                    demand_cp[path[n]] = q-capacity;
                    break;
                }else{
                    d += demand_cp[path[n]];
                    x[path[n]] = demand_cp[path[n]];

                }
            }

        }
        demand_list.push_back(x);
        S.push_back(s_p);
    }
    return pair<vector<vector<int>>, vector<map<int,int>>> (S, demand_list);
}

void printRoute(vector<vector<int>> s){
    cout<<"-------path---------"<<endl;
    for (int i = 0; i<s.size(); i++){
        int depot = s[i][0];
        cout<<depot<<":";
        for(int j=0; j<s[i].size(); j++){
            cout<<setw(8)<<s[i][j];
        }
        cout<<endl;
    }
}


int routing(vector<vector<int>> s){
    int all_cost = 0;
    for(int i = 0; i<s.size(); i++){
        int p, q, cost = 0;
        for(int j=0; j<s[i].size()-1; j++){
            p = s[i][j];
            q = s[i][j+1];
            cost = cost+adjMatrix[p][q];
        }
        all_cost = all_cost + cost;
    }
    return all_cost;
}

vector<vector<Arc>> forest_add_all(vector<vector<Arc>> forest_all, vector<CITIES> vertices){
    vector<vector<Arc>> forest_add;
    int limit_num = add_limit;
    int forestCount = 0;
    int feasibleCount = 0;
    int add_num_5 = 0;
    int add_num_4 = 0;
    int add_num_3 = 0;
    int add_num_2 = 0;
    int add_num_1 = 0;
    for (int i=0; i<forest_all.size(); i++){
        vector<vector<VertexData>> tree = edgeToTree(forest_all[i]);
        vector<Arc> arc_list;
        if (type == 2){
            arc_list = addArc(tree);
        }else if(type == 4 or type == 6){
            arc_list = addArcMultiDepots(tree);
        }
        int k = depot_number-1;
        while(k>0){
            if (k == 5){
                vector<vector<int>> idx = combine(arc_list.size(), k, transfer_idx_limit);
                for(int j = 0; j<idx.size(); j++){
                    vector<Arc> forest;
                    forest.assign(forest_all[i].begin(), forest_all[i].end());
                    forest.push_back(arc_list[idx[j][0]]);
                    forest.push_back(arc_list[idx[j][1]]);
                    forest.push_back(arc_list[idx[j][2]]);
                    forest.push_back(arc_list[idx[j][3]]);
                    forest.push_back(arc_list[idx[j][4]]);
                    add_num_5 += 1;
                    if(VerifyForest(forest)){
                        forestCount ++;
                        vector<vector<VertexData>> isforest = edgeToTree(forest);
                        if(feasibility(isforest, vertices)){
                            forest_add.push_back(forest);
                            feasibleCount++;
                        }
                    }
                    if(forest_add.size()>=limit_num){
                        break;
                    }
                }
                k--;
            }else if (k == 4){ //
                vector<vector<int>> idx = combine(arc_list.size(), k, transfer_idx_limit);
                for(int j = 0; j<idx.size(); j++){
                    vector<Arc> forest;
                    forest.assign(forest_all[i].begin(), forest_all[i].end());
                    forest.push_back(arc_list[idx[j][0]]);
                    forest.push_back(arc_list[idx[j][1]]);
                    forest.push_back(arc_list[idx[j][2]]);
                    forest.push_back(arc_list[idx[j][3]]);
                    add_num_4 += 1;
                    if(VerifyForest(forest)){
                        forestCount ++;
                        vector<vector<VertexData>> isforest = edgeToTree(forest);
                        if(feasibility(isforest, vertices)){
                            forest_add.push_back(forest);
                            feasibleCount++;
                        }
                    }
                    if(forest_add.size() >= limit_num){
                        break;
                    }
                }
                k--;
            }else if (k == 3){
                vector<vector<int>> idx = combine(arc_list.size(), k, transfer_idx_limit);
                for(int j = 0; j<idx.size(); j++){
                    vector<Arc> forest;
                    forest.assign(forest_all[i].begin(), forest_all[i].end());
                    forest.push_back(arc_list[idx[j][0]]);
                    forest.push_back(arc_list[idx[j][1]]);
                    forest.push_back(arc_list[idx[j][2]]);
                    add_num_3 += 1;
                    if(VerifyForest(forest)){
                        forestCount ++;
                        vector<vector<VertexData>> isforest = edgeToTree(forest);
                        if(feasibility(isforest, vertices)){
                            forest_add.push_back(forest);
                            feasibleCount++;
                        }
                    }
                    if(forest_add.size()>=limit_num){
                        break;
                    }
                }
                k--;
            }else if(k == 2){
                for(int j = 0; j<arc_list.size()-1; j++){
                    for(int m = j+1; m<arc_list.size(); m++){
                        vector<Arc> forest;
                        forest.assign(forest_all[i].begin(), forest_all[i].end());
                        forest.push_back(arc_list[j]);
                        forest.push_back(arc_list[m]);
                        add_num_2 += 1;
                        if(VerifyForest(forest)){
                            forestCount += 1;
                            vector<vector<VertexData>> isforest = edgeToTree(forest);
                            if(feasibility(isforest, vertices)){
                                feasibleCount+=1;
                                forest_add.push_back(forest);
                            }
                        }
                    }
                }
                if(forest_add.size()>=limit_num){
                    break;
                }
                k--;
            }else if(k==1){
                for(int j = 0; j<arc_list.size(); j++){
                    vector<Arc> forest;
                    forest.assign(forest_all[i].begin(), forest_all[i].end());
                    forest.push_back(arc_list[j]);
                    add_num_1 += 1;
                    if(VerifyForest(forest)){
                        forestCount += 1;
                        vector<vector<VertexData>> isforest = edgeToTree(forest);
                        if(feasibility(isforest, vertices)){
                            feasibleCount+=1;
                            forest_add.push_back(forest);
                        }
                    }
                    if(forest_add.size() >= limit_num){
                        break;
                    }
                }
                if(VerifyForest(forest_all[i])){
                    forestCount += 1;
                    vector<vector<VertexData>> isforest = edgeToTree(forest_all[i]);
                    if(feasibility(isforest, vertices)){
                        forest_add.push_back(forest_all[i]);
                    }
                }
                k--;
            }
        }
    }
    return forest_add;
}

vector<vector<int>> transfer_route(vector<vector<int>> path, vector<int> idx){
    for(int i = 0; i<path.size(); i++){
        for(int j = 0; j<path[i].size(); j++){
            if(path[i][j] == idx[0]){
                path[i][j] = idx[1];
            }else if (path[i][j] == idx[1]){
                path[i][j] = idx[0];
            }
        }
    }
    return path;
}


vector<vector<int>> transfer_cmb(){
    vector<vector<int>> idx_combine;
    int b = type;
    int e = vetex_number;
    for(int i=b; i<e-1; i++) {
        for (int j=i+1; j<e; j++) {
            vector<int> idx;
            idx.push_back(i);
            idx.push_back(j);
            idx_combine.push_back(idx);
        }
    }
    return idx_combine;
}

int calculateCost(vector<vector<int>> s) {
    int all_cost = 0;
    for (int i = 0; i < s.size(); i++) {
        int p, q, cost = 0;
        for (int j = 0; j < s[i].size() - 1; j++) {
            p = s[i][j];
            q = s[i][j+1];
            cost = cost + adjMatrix[p][q];
        }
        all_cost = all_cost + cost;
    }
    return all_cost;
}


void BFS(int s, int t, vector<vector<int> > &C, vector<vector<int> > &F, vector<int> &L){
    int n = L.size();
    vector<int> D(n, INT_MAX);
    vector<bool> visited(n);
    L[s] = 0;
    while(1) {
        int i = 0;
        while (i<n && (visited[i] || L[i]==INT_MAX)){
            i++;
        }
        if (i==n){
            return;
        }
        visited[i] = true;
        for (int j=0; j<n; j++){
            if (L[j]==INT_MAX){
                if (F[i][j]<C[i][j]){
                    D[j] = min(D[i], C[i][j]-F[i][j]);
                    L[j] = i;
                }
                else if(F[j][i]>0){
                    D[j] = min(D[i], F[j][i]);
                    L[j] = -i;
                }
                if (L[t]!=INT_MAX){
                    do {
                        if (L[j]>=0){
                            F[L[j]][j] += D[t];
                            j = L[j];
                        }
                        else{
                            F[j][-L[j]] -= D[t];
                            j = -L[j];
                        }
                    }while(j!=s);
                    return;
                }
            }
        }
    }
}


bool is_feasible(vector<vector<int>> route){
    int n, m, s, t;
    n = vetex_number-type+2;
    s = 0;
    t = n-1;
    m = type-1;
    vector<vector<int>> C(n, vector<int>(n));//Adjacency matrix with the value of capacity
    //Start point is 0, n
    for(int i = 0; i<route.size(); i++){
        C[s][route[i][1]-m] += capacity;
        for(int j=2; j<route[i].size()-1; j++){
            C[route[i][1]-m][route[i][j]-m] += capacity;
        }
    }
    for(int i = 1; i<vetex_number-m; i++){
        C[i][t] = demand_cp[i-1];
    }
    vector<vector<int>> F(n, vector<int>(n));
    vector<int> L(n, INT_MAX);
    do{
        fill(L.begin(), L.end(), INT_MAX);
        BFS(s, t, C, F, L);
    }while(L[t]!=INT_MAX);
    int totalFlow = 0;
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++){
            if (C[i][j]>0){
                if (i==0){
                    totalFlow += F[i][j];
                }
            }
        }
    }
    if(totalFlow == demand){
        return true;
    } else
        return false;
}

int main(){
    clock_t start, mid, end;
    start = clock();
    ofstream outfile;
    string result_path = "/home/li/CLionProjects/APP/RESULTS/k"+to_string(type)+"/"+ depots;
    cout<<result_path<<endl;
    outfile.open(result_path);
    vector<CITIES> vertices;
    vertices = city(info);
    Tree spanning_forest = Spanning_forest(vertices);
    vector<vector<Arc>> e = Subedge(spanning_forest);
    int min_forest_cost = compute_cost(e[0]);
    // Exchange edge
    vector<vector<Arc>> t = Add(e);
    vector<vector<Arc>> forest_all;
    int n = depot_number;
    for (int i=0; i<t.size(); i++){
        bool isCSF = VerifyCSF(t[i], n);
        if(isCSF){
            forest_all.push_back(t[i]);
        }
    }
    vector<vector<Arc>> forest_add;
    forest_add = forest_add_all(forest_all, vertices);
    vector<vector<int>> Odds_list;
    vector<vector<Arc>> forest_matching;
    vector<vector<Arc>> forest_matching_less;
    for(int m = 0; m<forest_add.size(); m++) {
        vector<int> Odds = Odd(forest_add[m]);
        Odds_list.push_back(Odds);
        vector<Arc> match_arc;
        match_arc = MinimumCostPerfectMatching(Odds);
        vector<Arc> combine;
        combine.insert(combine.end(), forest_add[m].begin(), forest_add[m].end());
        combine.insert(combine.end(), match_arc.begin(), match_arc.end());
        if(checkMatching(combine)){
            forest_matching.push_back(combine);
        }else{
            forest_matching_less.push_back(combine);
        }
    }

    int min_cost = inf;
    vector<vector<int>> min_route;
    vector<map<int,int>> best_demand;
    for(int k=0; k<forest_matching.size();k++){
        vector<vector<int>> path_all;
        path_all = shortcut(forest_matching[k]);
        vector<int> path = findPath(path_all);
//        vector<vector<int>> s;
        pair<vector<vector<int>>, vector<map<int,int>>> section = cycle_split(path);
        vector<vector<int>> s = section.first;
        vector<map<int,int>> s_demand = section.second;

        vector<vector<int>> depots = Matching_vd(s);
        vector<vector<int>> depots_cp;
        for(int i = 0 ; i<depots.size(); i++){
            vector<int> d;
            d.push_back(depots[i][0]);
            for(int j = 1; j< depots[i].size()-1; j++){
                if(depots[i][j] >= type){
                    d.push_back(depots[i][j]);
                }
            }
            d.push_back(depots[i][depots[i].size()-1]);
            depots_cp.push_back(d);
        }

        int cost = routing(depots_cp);

        if(min_cost>cost){
            min_cost = cost;
            min_route = depots_cp;
            best_demand = s_demand;
        }
    }

    cout<<endl;
    cout<<"======================================"<<endl;
    cout<<"The minimum cost is（without opt-2）:"<<min_cost<<endl;
    outfile<<"The minimum cost is:"<<min_cost<<endl;
    cout<<"--------------------"<<endl;
    mid = clock();
    cout << "The run time （without opt-2）is: "<<double (mid -start)/CLOCKS_PER_SEC  << " (s) "<< endl;
    outfile << "The run time is: "<<double (mid -start)/CLOCKS_PER_SEC  << " (s) "<< endl;
    outfile<<"The optimal route and corresponding demands serviced:"<<endl;
    printRoute(min_route);
    for (int i = 0; i<min_route.size(); i++){
        int d = 0;
        for(int j=0; j<min_route[i].size(); j++) {
            if (j == 0) {
                outfile << min_route[i][j];
            } else if (j == (min_route[i].size() - 1)) {
                outfile << setw(5) << min_route[i][j];
                outfile << "    total service:" << d;
            } else {
                int x = best_demand[i][min_route[i][j]];
                outfile << setw(5) << min_route[i][j] << "_" << x;
                d += x;
            }
        }
        outfile<<endl;
    }
    outfile.close();
    return 0;
}

