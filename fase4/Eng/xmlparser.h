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
#include <tuple>






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
extern std::vector<std::vector<float>> normalsArrays;  
extern std::vector<std::vector<float>> texturesArrays;
extern std::vector<std::string> textures;   

extern std::vector<std::vector<float>> luzglobal;
extern std::vector<std::vector<std::vector<float>>> luzObj;

extern int contador;
extern int groupcounter;

extern std::vector<std::vector<int>> childs;
extern std::vector<std::vector<std::pair<int, std::string>>> files;
extern std::vector<std::vector<std::vector<float>>> ts;

extern int fileCounter;

extern bool hasNorm;
extern bool hasLight;
extern bool hasColour;
extern bool hasText; 


void get_attributes(tinyxml2::XMLElement*);

void traverse_elements(tinyxml2::XMLElement*);

void get_lights_textures(tinyxml2::XMLElement*);

void processGroup(tinyxml2::XMLElement*, int, int&); 

#endif

