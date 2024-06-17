#include "xmlparser.h"

double camx;
double camy;
double camz;

int width;
int height;

double lookx;
double looky;
double lookz;

double upx;
double upy;
double upz;

double fov;
double near;
double far;

double dist;

double alpha; // horizontal
double beta;  // vertical

int eixos=1;

std::vector<std::vector<float>> numbersArrays;        
std::vector<std::pair<std::string,int>> ficheiros;
std::vector<std::pair<std::string,int>> info;
std::vector<std::pair<std::vector<std::string>,int>> transf;
std::vector<std::pair<std::vector<float>,int>> final;
std::vector<int> pais;
int contador = 0;


//função que recebe um elemento xml, e percorre os seus atributos, recolhendo informaçôes referentes à câmara e à janela, 
void get_attributes(tinyxml2::XMLElement* element) { 
    for (const tinyxml2::XMLAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()) {

        if(std::string(element->Name())=="eixos"){
            if(std::string(attr->Value())=="true" ||std::string(attr->Value())=="True"  ){
                eixos=1;
            }
            else if(std::string(attr->Value())=="false" || std::string(attr->Value())=="False" ){
                eixos=0;
            }

        }

        else if(std::string(element->Name())=="window"){

            if(std::string(attr->Name())=="width"){
                width=std::stoi(attr->Value());
            }

            else if(std::string(attr->Name())=="height"){
                height=std::stoi(attr->Value());
            }

        }

        else if(std::string(element->Name())=="lookAt"){

            if(std::string(attr->Name())=="x"){
                lookx=std::stod(attr->Value());
            }

            else if(std::string(attr->Name())=="y"){
                looky=std::stod(attr->Value());
            }

            else if(std::string(attr->Name())=="z"){
                lookz=std::stod(attr->Value());
            }
        }

        else if(std::string(element->Name())=="up"){

            if(std::string(attr->Name())=="x"){
                upx=std::stod(attr->Value());
            }

            else if(std::string(attr->Name())=="y"){
                upy=std::stod(attr->Value());
            }

            else if(std::string(attr->Name())=="z"){
                upz=std::stod(attr->Value());
            }
        }

        else if(std::string(element->Name())=="projection"){

            if(std::string(attr->Name())=="fov"){
                fov=std::stod(attr->Value());
            }

            else if(std::string(attr->Name())=="near"){
                near=std::stod(attr->Value());
            }

            else if(std::string(attr->Name())=="far"){
                far=std::stod(attr->Value());
            }

        }

        else if(std::string(element->Name())=="position"){

            if(std::string(attr->Name())=="x"){
                camx=std::stod(attr->Value());
            }

            else if(std::string(attr->Name())=="y"){
                camy=std::stod(attr->Value());
            }

            else if(std::string(attr->Name())=="z"){
                camz=std::stod(attr->Value());
            }
        }

    }

    dist=sqrt(camx*camx + camy*camy + camz*camz);
    beta = asin(camy / dist);
    alpha = asin(camx / (dist * cos(beta)));
}


//percorre todo o ficheiro elemento do ficheiro xml, chamando recursivamente a funçao, para cada elemento, chama a função get_attributes
void traverse_elements(tinyxml2::XMLElement* element) {
    get_attributes(element);

    for (tinyxml2::XMLElement* child = element->FirstChildElement(); child; child = child->NextSiblingElement()) {
        traverse_elements(child);
    }
}


//Funçaõ recursiva, percorre o ficheiro xml, recolhendo informação sobre o elemento, e o pai do elemento
void get_transformacoes(tinyxml2::XMLElement* element, std::string pai, int painum) {

    //recebe como paremetro o identificador do seu pai, que chamou a função recursivamente, e guarda-o no vetor pais
    std::string elementName = element->Name(); 
    int num=contador;
    pais.push_back(painum);

    //caso seja uma transformação, percorre os atributos, e guarda os valores de angle,x,y,z em tranfs. guarda o nome do elemento no vetor info
    if(elementName=="translate" or elementName=="rotate" or elementName=="scale"){ 
        const tinyxml2::XMLAttribute* attr = element->FirstAttribute();

        if(elementName=="translate" && std::string(attr->Name())=="time"){ //caso seja uma transformaçao com tempo (catmull-rom)

            info.push_back(std::make_pair("translatet", contador));         //guardamos o nome da translaçao temporal
            std::vector<std::string> valores;
            int draw=1;
            for (const tinyxml2::XMLAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){    //percorremos os seus atributos
                                                                                    
                if(std::string(attr->Name())=="time"){                          //guardamos o valor do tempo*1000
                    std::string time=std::string(attr->Value());
                    float realtime=std::stod(time)*1000;
                    valores.push_back(std::to_string(realtime));                //primeiros 3 valores do vetor nao sao pontos
                }
                else if(std::string(attr->Name())=="align"){                   //se esta alinhado ou nao com a curva
                    valores.push_back(std::string(attr->Value()));
                }
                else if(std::string(attr->Name())=="draw"){                     //caso nao seja necessario renderizar a curva mudamos o valor de draw para 0
                    if(std::string(attr->Value())=="false" || std::string(attr->Value())=="False"){
                        draw=0;
                    }
                }

            }
            for (tinyxml2::XMLElement* child = element->FirstChildElement(); child; child = child->NextSiblingElement()) { //agora percorremos os pontos
                for (const tinyxml2::XMLAttribute* attr2 = child->FirstAttribute(); attr2; attr2 = attr2->Next()){        //e guardamos tudo
                    valores.push_back(std::string(attr2->Value()));
                }
            }
            valores.push_back("0");// variavel current time
            valores.push_back("0");// variavel t
                                                                    //temos que ultimos 6 valores do vetor nao sao pontos 
            valores.push_back("0");// derivada_y.x
            valores.push_back("1");// derivada_y.y
            valores.push_back("0");// derivada_y.z

            valores.push_back(std::to_string(draw));// flag que indica se a curva de catmull roll sera desenhada
            transf.push_back(std::make_pair(valores, contador));

        }


        else if(elementName=="rotate" && std::string(attr->Name())=="time"){    //rotacao temporal
            info.push_back(std::make_pair("rotatet", contador));
            std::vector<std::string> valores;                                       
            for (const tinyxml2::XMLAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){
                valores.push_back(std::string(attr->Value()));
            }
            transf.push_back(std::make_pair(valores, contador));


        }
        else{
            info.push_back(std::make_pair(elementName, contador));
            std::vector<std::string> valores;
            for (const tinyxml2::XMLAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()){ //rotacao, escala, translacao
                valores.push_back(std::string(attr->Value()));
            }
            transf.push_back(std::make_pair(valores, contador));

        }
        

    }

    //caso seja um ficheiro, guarda o seu path para o vetor ficheiros, e para o vetor info
    else if(elementName=="model"){ 
        const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
        //"../tests/"+std::string(attr->Value())
        info.push_back(std::make_pair("tests/"+std::string(attr->Value()), contador));
        ficheiros.push_back(std::make_pair("tests/"+std::string(attr->Value()), contador));
    }


    //caso contrario, apenas guarda o nome do elemento para o vetor info
    else{
        info.push_back(std::make_pair(std::string(element->Name()), contador));
    }


    contador+=1;
 
    //percorre todos os filhos do elemento e chama recursivamente a função para eles, dando o seu identificador como atributo
    for (tinyxml2::XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        
        get_transformacoes(child, elementName,num); 
    }
}


