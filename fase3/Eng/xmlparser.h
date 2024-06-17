#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "tinyxml2.h"
#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

extern int eixos;
extern int width;
extern int height;
extern double lookx;
extern double looky;
extern double lookz;
extern double upx;
extern double upy;
extern double upz;
extern double fov;
extern double near;
extern double far;
extern double camx;
extern double camy;
extern double camz;
extern double alpha; // horizontal
extern double beta;  // vertical
extern double dist;
extern std::vector<std::vector<float>> numbersArrays;        
extern std::vector<std::pair<std::string,int>> ficheiros;
extern std::vector<std::pair<std::string,int>> info;
extern std::vector<std::pair<std::vector<std::string>,int>> transf;
extern std::vector<std::pair<std::vector<float>,int>> final;
extern std::vector<int> pais;
extern int contador;


void get_attributes(tinyxml2::XMLElement*);

void traverse_elements(tinyxml2::XMLElement*);

void get_transformacoes(tinyxml2::XMLElement*, std::string="Null", int=-1);

#endif

