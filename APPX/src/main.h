//
// Created by DY on 2021/12/7.
//


#ifndef MDSDVRP_TEST_H
#define MDSDVRP_TEST_H
#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>
#define VertexData unsigned int
#ifndef MDSDVRP_MINISPANTREE_H
#endif //MDSDVRP_MINISPANTREE_H
class main {

};

typedef struct candidate
{
    int x;  //X-coordinate
    int y;  //Y-coordinateX
    int d;  //demand
} CITIES;



//Edge Info
typedef struct{
    VertexData u;
    VertexData v;
    unsigned int cost;
}Arc;

typedef struct Edge
{
    int start;
    int end;
}  EDGE;

#endif //MDSDVRP_MAIN_H
