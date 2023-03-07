#include <iostream>


#include <string>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <climits>
#include <numeric>
#include <map>
using namespace std;

#define INFINITE 0xFFFFFFFF //4294967295
#define type 2 //type of depot: 2/4/6
#define data_type 0 //type of instance: SD=0，p/pr=1

#define vetex_number 10 // num of vetex( = depots num + customer num )
string depot_0 = "SD1.txt"; //instance file name
string path_0_depots = "/home/li/CLionProjects/attach/data/SD set/k=2/" + depot_0; //instance file path
string path_1_depots = "/home/li/CLionProjects/attach/data/P set/" + depot_0;


typedef struct candidate
{
    int x;  //横坐标
    int y;  //纵坐标
    int d;  //需求
} CITIES;

struct Info{
    int customer_num;
    int capacity;
    int depots_num;
    int vehicle_num;
    vector<int> vehicles;
    vector<int> demands;
    vector<vector<int>> loc;
};

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
        info.customer_num = res[0][2];
        info.capacity = res[2][1];
        info.depots_num = res[0][3];
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
        info.customer_num = res[0][0];
        info.capacity = res[0][1];
        info.depots_num = res[0].size()-2;
        info.vehicle_num = 0;
        for(int i = 2;i<res[0].size();i++){
            info.vehicles.push_back(res[0][i]);
            info.vehicle_num = info.vehicle_num+res[0][i];
        }
        for(int i =0; i<info.customer_num; i++){
            info.demands.push_back(res[1][i]);
        }
        int len = info.depots_num + info.customer_num;

        for(int i=2; i<len+2; i++){
            vector<int> xy;
            for(int j=0; j<2;j++){
                xy.push_back(res[i][j]);
            }
            info.loc.push_back(xy);
        }
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
    for(int i =0; i<info.demands.size(); i++){
        demand += info.demands[i];
    }
    return demand;
}
int demand = demand_all();

vector<int> demand_copy(){
    vector<int> demand_cp;
    demand_cp.assign(info.demands.begin(), info.demands.end());
    return demand_cp;
}
vector<int> demand_cp = demand_copy();

vector<vector<int>> demand_service;

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

int distance(CITIES c1, CITIES c2)
{
    int distance=0;
    distance=sqrt((double)((c1.x-c2.x)*(c1.x-c2.x)+(c1.y-c2.y)*(c1.y-c2.y)));
    return distance;
}

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

int Alpha(int i, int k, int j){
    int alpha = adjMatrix[i][k]+adjMatrix[k][j]-adjMatrix[i][j];
    return alpha;
}

int Beta(pair<int, vector<int>> G, int k){
    int alpha = G.first;
    vector<int> path = G.second;
    //path[0]为depots
    int beta = adjMatrix[path[0]][k] - alpha;
    return beta;
}

void printPath(vector<int> path){
    cout<<"path:";
    for(int i=0; i<path.size(); i++){
        cout<<setw(3)<<path[i];
    }
    cout<<endl;
}

//返回<index信息（插入的位置，插入的route_idx(对应车辆）），<对应的最小alpha值，path>>
pair<vector<int>, pair<int, vector<int>>> minDepots(vector<vector<int>> route, int customer){
    vector<int> path;
    int flag_insert_in = 0;
    int flag_insert = 0; //标记插入点
    int flag_alpha = 0; //标记最小的路径idx
    unsigned int min_path = INFINITE; //最小alpha值
    for(int i=0; i<route.size(); i++){
        unsigned int min= INFINITE;
        //使alpha最小的位置
        for(int j=0; j<route[i].size()-1; j++){
            int alpha = Alpha(route[i][j], customer, route[i][j+1]);
            if(min > alpha){
                min = alpha;
                flag_insert_in = j+1;
            }
        }
        //route中最小的alpha
        if(min_path > min){
            min_path = min;
            flag_insert = flag_insert_in;
            path = route[i];
            flag_alpha = i;
        }
    }
    vector<int> index;
    index.push_back(flag_insert);
    index.push_back(flag_alpha);
    return make_pair(index, make_pair(min_path, path));
}

pair<bool, int> isfeasibility(vector<int> path, int customer){
    int demand = 0;
    // 去除首尾depots，求path需求
    for(int i = 1; i<path.size()-1; i++){
        demand += info.demands[path[i]-depot_number];
    }
    int cut = demand - capacity;
    int x;
    map<int, int> y;
    if(cut<0){
        x = info.demands[customer-depot_number];
        return pair<bool, int> (true,x);
    }else{
        x = info.demands[customer-depot_number]-cut;
        info.demands[customer-depot_number] = cut;
        return pair<bool, int> (false,x);
    }
}

void printInfo(vector<int> demand){
    cout<<"demand:";
    for(int i=0 ; i<demand.size(); i++){
        cout<<demand[i]<<setw(3);
    }
    cout<<endl;
}

vector<vector<int>> combine(int size){
    vector<vector<int>> idx_combine;
    for(int i=0; i<size-2; i++) {
        for (int j = i + 1; j < size - 1; j++) {
            for (int m = j + 1; m < size; m++) {
                vector<int> idx;
                idx.push_back(i);
                idx.push_back(j);
                idx.push_back(m);
                idx_combine.push_back(idx);
            }
        }
    }
    return idx_combine;
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

int cost(vector<int> path){
    int path_cost = 0;
    for(int i=0;i<path.size()-1; i++){
        path_cost += adjMatrix[path[i]][path[i+1]];
    }
    return path_cost;
}


pair<int, vector<int>> transfer(vector<int> path){
    vector<vector<int>> cmb = combine(path.size()-2);//选出三元组
    int min_cost = cost(path);
    vector<int> min_path;
    int path_cost;
    for(int i=0; i<cmb.size(); i++){
        vector<int> path_cp;
        path_cp.assign(path.begin(), path.end());
        min_path.assign(path_cp.begin(), path_cp.end());
        //交换5次
        swap(path_cp[cmb[i][0]+1], path_cp[cmb[i][1]+1]);
        path_cost = cost(path_cp);
        if(path_cost<min_cost){
            min_cost = path_cost;
            min_path.clear();
            min_path.assign(path_cp.begin(), path_cp.end());
        }

        swap(path_cp[cmb[i][1]+1], path_cp[cmb[i][2]+1]);
        path_cost = cost(path_cp);
        if(path_cost<min_cost){
            min_cost = path_cost;
            min_path.clear();
            min_path.assign(path_cp.begin(), path_cp.end());
        }
        swap(path_cp[cmb[i][0]+1], path_cp[cmb[i][1]+1]);
        path_cost = cost(path_cp);

        if(path_cost<min_cost){
            min_cost = path_cost;
            min_path.clear();
            min_path.assign(path_cp.begin(), path_cp.end());
        }
        swap(path_cp[cmb[i][1]+1], path_cp[cmb[i][2]+1]);
        path_cost = cost(path_cp);

        if(path_cost<min_cost){
            min_cost = path_cost;
            min_path.clear();
            min_path.assign(path_cp.begin(), path_cp.end());
        }
        swap(path_cp[cmb[i][0]+1], path_cp[cmb[i][1]+1]);
        path_cost = cost(path_cp);
        if(path_cost<min_cost){
            min_cost = path_cost;
            min_path.clear();
            min_path.assign(path_cp.begin(), path_cp.end());
        }
    }
    return make_pair(min_cost, min_path);
}

//vector<int> demand_sep;
pair<vector<vector<int>>, vector<map<int, int>>> path(){
    //初始化route
    vector<vector<int>> route;
    vector<map<int, int>> route_demand;
    for(int i=0; i < depot_number; i++){
        for(int j=0; j < info.vehicles[i]; j++){
            vector<int> path;
            path.push_back(i);
            path.push_back(i);
            route.push_back(path);
        }
    }
    vector<vector<int>> cus_demand;
    for(int i=0; i<info.customer_num+info.depots_num; i++){
        vector<int> d;
        d.push_back(0);
        cus_demand.push_back(d);
    }
    vector<int> path;
    //初始化customer
    vector<int> customer;
    for(int i=depot_number; i<customer_number+depot_number; i++){
        customer.push_back(i);
    }
    vector<vector<int>> final_route;

    while (customer.size()>0 && !route.empty()){
        int flag_c = 0;
        int maxBeta = INT_MIN;
        int insert_loc;
        int route_idx;
        vector<int> path_update;

        for(int i=0; i < customer.size(); i++){
            pair<vector<int>, pair<int, vector<int>>> find_path = minDepots(route, customer[i]);
            vector<int> index = find_path.first;
            pair<int, vector<int>> minAlpha = find_path.second;
            int beta = Beta(minAlpha, customer[i]);
            if(maxBeta < beta){
                maxBeta = beta;
                path_update = minAlpha.second;
                flag_c = customer[i];
                insert_loc = index[0];
                route_idx = index[1];
            }
        }
        path_update.insert(path_update.begin()+insert_loc, flag_c);
        pair<bool, int> service = isfeasibility(path_update, flag_c);

        if(service.first){
            route[route_idx].clear();
            route[route_idx].assign(path_update.begin(), path_update.end()); // 更新route
            customer.erase(find(customer.begin(), customer.end(), flag_c));
            cout<<flag_c<<"_"<<service.second<<" ";
            cus_demand[flag_c].push_back(service.second);
        }else{
            final_route.push_back(path_update);
            route.erase(route.begin()+route_idx); // 去掉更新的path
            cout<<flag_c<<"_"<<service.second<<endl;
            cus_demand[flag_c].push_back(service.second);
        }
    }
    cout<<endl;
    for(int i = 0 ; i<route.size(); i++){
        if(route[i].size() > 2){
            final_route.push_back(route[i]);
        }
    }
    vector<map<int,int>> route_map;
    for(int i = 0; i<final_route.size();i++){
        map<int,int> m;
        for(int j = 1; j<final_route[i].size()-1; j++){
            m[final_route[i][j]] = cus_demand[final_route[i][j]][1];
            cus_demand[final_route[i][j]].erase(cus_demand[final_route[i][j]].begin()+1);
        }
        route_map.push_back(m);
    }
    return pair<vector<vector<int>>, vector<map<int, int>>>(final_route, route_map);
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

void printRoute(vector<vector<int>> path){
    for(int i=0; i<path.size(); i++){
        for(int j=0; j<path[i].size(); j++){
            cout<<setw(5)<<path[i][j];
        }
        cout<<endl;
    }
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
    vector<int> D(n, INT_MAX);//标号，D：当前最大可行流量
    vector<bool> visited(n);
    L[s] = 0;
    while(1) {
        //顺序查找第一个已标号且未搜索过的顶点
        int i = 0;
        while (i<n && (visited[i] || L[i]==INT_MAX)){
            i++;
        }
        if (i==n){  //若没有已标号未搜索过的顶点
            return;
        }
        visited[i] = true;
        for (int j=0; j<n; j++){
            if (L[j]==INT_MAX){  //未标号且有路径
                if (F[i][j]<C[i][j]){  //前向边，F[i][j]>=0，所以隐含了C[i][j]>0
                    D[j] = min(D[i], C[i][j]-F[i][j]);
                    L[j] = i;
                }
                else if(F[j][i]>0){  //后向边
                    D[j] = min(D[i], F[j][i]);
                    L[j] = -i;
                }
                //找到增广路径，从t回溯，修改流量，结束BFS
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
    //利用最大流算法，判断是否可行
    //构造矩阵
    int n, m, s, t;
    n = vetex_number-type+2;
    s = 0;
    t = n-1;
    m = type-1;
    vector<vector<int>> C(n, vector<int>(n));//邻接矩阵，值为容量
    //起始点为0，n
    for(int i = 0; i<route.size(); i++){
        C[s][route[i][1]-m] += capacity;
        for(int j=2; j<route[i].size()-1; j++){
            C[route[i][1]-m][route[i][j]-m] += capacity;
        }
    }
    for(int i = 1; i<vetex_number-m; i++){
        C[i][t] = demand_cp[i-1];
    }
    vector<vector<int>> F(n, vector<int>(n));//流
    vector<int> L(n, INT_MAX);//标号，L：流量来源
    do{
        fill(L.begin(), L.end(), INT_MAX);
        BFS(s, t, C, F, L);
    }while(L[t]!=INT_MAX);
//    cout << "Maximum flow:" << endl;
    int totalFlow = 0;
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++){
            if (C[i][j]>0){  //i到j
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

int main() {
    clock_t start, mid;
    start = clock();
    vector<CITIES> vertices;
    vertices = city(info);
    //初始化邻接矩阵
    Matrix(vertices);
    pair<vector<vector<int>>, vector<map<int, int>>> p = path();
    vector<vector<int>> route = p.first;
    vector<map<int, int>> cus_demand = p.second;
    printRoute(route);
    int minCost = calculateCost(route);
    cout << "Minimum cost (without opt2):" << minCost << endl;
    cout<<"--------------------"<<endl;
    mid = clock();
    double time_cost = double (mid-start)/CLOCKS_PER_SEC;
    cout << "The run time (without opt-2) is: "<< time_cost  << " (s) "<< endl;
    ofstream outfile;
    outfile.open("/home/li/CLionProjects/MJ/RESULTS/k"+ to_string(type)+"/"+ depot_0);
    outfile<<"The minimum cost is:"<<minCost<<endl;
    outfile<<"The run time is:"<<time_cost<<"(s)"<<endl;
    outfile<<"The optimal route and corresponding demands serviced:"<<endl;
    int idx=0;
    for(int i=0; i<route.size(); i++){
        int d = 0;
        for(int j=0; j<route[i].size(); j++){
            if(j==0){
                outfile<<route[i][j];
            }else if (j==(route[i].size()-1)){
                outfile<<setw(5)<<route[i][j];
                outfile<<"    total service:"<<d;
                idx = idx+j-1;
            }
            else{
                outfile<<setw(5)<<route[i][j]<<"_"<<cus_demand[i][route[i][j]];
                d += cus_demand[i][route[i][j]];
                }
        }
        outfile<<endl;
    }
    outfile.close();
};