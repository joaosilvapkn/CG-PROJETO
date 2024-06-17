#include "xmlparser.h"

double camx;
double camy;
double camz;

int eixos=1;
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

std::vector<std::vector<float>> numbersArrays; 
std::vector<std::vector<float>> normalsArrays;  
std::vector<std::vector<float>> texturesArrays;  

std::vector<std::string> textures;   


std::vector<std::vector<float>> luzglobal; 
std::vector<std::vector<std::vector<float>>> luzObj;

int groupcounter=0;

std::vector<std::vector<int>> childs;
std::vector<std::vector<std::pair<int, std::string>>> files;
std::vector<std::vector<std::vector<float>>> ts;
int fileCounter=0;


bool hasNorm=false;
bool hasLight=false;
bool hasColour=false;
bool hasText=false; 




//função que recebe um elemento xml, e percorre os seus atributos, recolhendo informaçôes referentes à câmara e à janela, 
void get_attributes(tinyxml2::XMLElement* element) { 



    if(std::string(element->Name())=="light"){

        std::vector<float> luz;

        for (const tinyxml2::XMLAttribute* attr = element->FirstAttribute(); attr; attr = attr->Next()) {
            if(std::string(attr->Value())=="directional"){
                luz.push_back(0.0f);
            }

            else if(std::string(attr->Value())=="point"){
                luz.push_back(1.0f);
            }

            else if(std::string(attr->Value())=="spot"){
                luz.push_back(2.0f);
            }

            else{
                luz.push_back(std::stod(attr->Value()));
            }
        }

        luzglobal.push_back(luz);
    }

    else{
    
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
    }

  //  if(luz.size()>0){
    //    luzglobal.push_back(luz);
    //}
    

    dist=sqrt(camx*camx + camy*camy + camz*camz);
    beta = asin(camy / dist);
    alpha = atan2(camx, camz);

}


//percorre todo o ficheiro elemento do ficheiro xml, chamando recursivamente a funçao, para cada elemento, chama a função get_attributes
void traverse_elements(tinyxml2::XMLElement* element) {


    if(std::string(element->Name())=="group"){
        get_lights_textures(element);
        processGroup(element,-1,groupcounter);
        return;
    }




    get_attributes(element);




    for (tinyxml2::XMLElement* child = element->FirstChildElement(); child; child = child->NextSiblingElement()) {
        traverse_elements(child);
    }

}


//Funçaõ recursiva, percorre o ficheiro xml, recolhendo informação sobre o elemento, e o pai do elemento
void get_lights_textures(tinyxml2::XMLElement* element) {

    //recebe como paremetro o identificador do seu pai, que chamou a função recursivamente, e guarda-o no vetor pais
    std::string elementName = element->Name(); 

    //caso seja uma transformação, percorre os atributos, e guarda os valores de angle,x,y,z em tranfs. guarda o nome do elemento no vetor info
    if(elementName=="translate" or elementName=="rotate" or elementName=="scale"){ 
        const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
    }

        

    //caso seja um ficheiro, guarda o seu path para o vetor ficheiros, e para o vetor info
    if(elementName=="model"){ 

        tinyxml2::XMLElement* childtext = element->FirstChildElement();
        if(childtext){
            if(std::string(childtext->Name())=="texture"){
                const tinyxml2::XMLAttribute* text = childtext->FirstAttribute();  
                textures.push_back("textures/"+std::string(text->Value()));
            }
            
        }

    }


 
    //percorre todos os filhos do elemento e chama recursivamente a função para eles, dando o seu identificador como atributo
    for (tinyxml2::XMLElement* child = element->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        

        if(std::string(child->Name())=="color"){   

            std::vector<std::vector<float>> Obj;

            for (tinyxml2::XMLElement* child2 = child->FirstChildElement(); child2 != nullptr; child2 = child2->NextSiblingElement()){

                
                std::vector<float> tipoluz;

                if(std::string(child2->Name())=="diffuse"){
                    tipoluz.push_back(0);
                }

                else if(std::string(child2->Name())=="ambient"){
                    tipoluz.push_back(1);
                }

                else if(std::string(child2->Name())=="specular"){
                    tipoluz.push_back(2);   
                }


                else if(std::string(child2->Name())=="emissive"){
                    tipoluz.push_back(3);
                }  
                

                else if(std::string(child2->Name())=="shininess"){
                    tipoluz.push_back(4);    
                }                                                              



                
                std::vector<float> luz;

                for (const tinyxml2::XMLAttribute* attr2 = child2->FirstAttribute(); attr2; attr2 = attr2->Next()){ 

                    tipoluz.push_back(std::stod(attr2->Value()));
                }


                Obj.push_back(tipoluz);
            }

            luzObj.push_back(Obj);
        }


        else if(std::string(child->Name())!="texture"){
            get_lights_textures(child); 
        }
    }
}


void processGroup(tinyxml2::XMLElement* group, int parentGroupIndex, int& groupCounter) {
    int currentIndex = groupCounter++;
    if (childs.size() <= currentIndex) {
        childs.resize(currentIndex + 1);
    }
    if (parentGroupIndex != -1) {
        if (childs.size() <= parentGroupIndex) {
            childs.resize(parentGroupIndex + 1);
        }
        childs[parentGroupIndex].push_back(currentIndex);
    }

    std::vector<std::pair<int, std::string>> groupFiles;
    std::vector<std::vector<float>> groupTransf;

    // Search for transform element
    tinyxml2::XMLElement* transform = group->FirstChildElement("transform");
    if (transform) {
        // Process each child element inside the transform
        for (tinyxml2::XMLElement* transformChild = transform->FirstChildElement(); transformChild; transformChild = transformChild->NextSiblingElement()) {

            std::string elementName = transformChild->Name(); 
            const tinyxml2::XMLAttribute* attr = transformChild->FirstAttribute();

            if(elementName=="translate" && std::string(attr->Name())=="time"){ 

                std::vector<float> valores;
                valores.push_back(4);
                float draw=1;
             
                for (const tinyxml2::XMLAttribute* attr = transformChild->FirstAttribute(); attr; attr = attr->Next()){    //percorremos os seus atributos
                                                                                        
                    if(std::string(attr->Name())=="time"){                          //guardamos o valor do tempo*1000
                        float time=std::stod(attr->Value());
                        float realtime=time*1000;
                        valores.push_back(realtime);                //primeiros 3 valores do vetor nao sao pontos
                    }
                    else if(std::string(attr->Name())=="align"){                   //se esta alinhado ou nao com a curva
                        if(std::string(attr->Value())=="true" || std::string(attr->Value())=="True"){
                            valores.push_back(1.0f);
                        }
                        else if(std::string(attr->Value())=="false" || std::string(attr->Value())=="False"){
                            valores.push_back(0.0f);
                        }
                        
                    }
                    else if(std::string(attr->Name())=="draw"){                     //caso nao seja necessario renderizar a curva mudamos o valor de draw para 0
                        if(std::string(attr->Value())=="false" || std::string(attr->Value())=="False"){
                            draw=0;
                        }
                    }

                }

                for (tinyxml2::XMLElement* child = transformChild->FirstChildElement(); child; child = child->NextSiblingElement()) { //agora percorremos os pontos
                    for (const tinyxml2::XMLAttribute* attr2 = child->FirstAttribute(); attr2; attr2 = attr2->Next()){        //e guardamos tudo
                        valores.push_back(std::stod(attr2->Value()));
                    }
                }
                valores.push_back(0.0f);// variavel current time
                valores.push_back(0.0f);// variavel t
                                                                        //temos que ultimos 6 valores do vetor nao sao pontos 
                valores.push_back(0.0f);// derivada_y.x
                valores.push_back(1.0f);// derivada_y.y
                valores.push_back(0.0f);// derivada_y.z

                valores.push_back(draw); 

                groupTransf.push_back(valores);            


            }    




            else if(elementName=="rotate" && std::string(attr->Name())=="time"){    //rotacao temporal
                std::vector<float> valores; 
                valores.push_back(3);                                      
                for (const tinyxml2::XMLAttribute* attr = transformChild->FirstAttribute(); attr; attr = attr->Next()){
                    valores.push_back(std::stod(attr->Value()));
                }
                groupTransf.push_back(valores); 

            }

            else if(elementName=="translate"){

                std::vector<float> valores;
                valores.push_back(0);
                for (const tinyxml2::XMLAttribute* attr =transformChild->FirstAttribute(); attr; attr = attr->Next()){ //rotacao, escala, translacao
                    valores.push_back(std::stod(attr->Value()));
                }
                groupTransf.push_back(valores); 
            }

            else if(elementName=="rotate"){

                std::vector<float> valores;
                valores.push_back(1);
                for (const tinyxml2::XMLAttribute* attr = transformChild->FirstAttribute(); attr; attr = attr->Next()){ //rotacao, escala, translacao
                    valores.push_back(std::stod(attr->Value()));
                }
                groupTransf.push_back(valores); 
            }

            else if(elementName=="scale"){

                std::vector<float> valores;
                valores.push_back(2);
                for (const tinyxml2::XMLAttribute* attr = transformChild->FirstAttribute(); attr; attr = attr->Next()){ //rotacao, escala, translacao
                    valores.push_back(std::stod(attr->Value()));
                }                
                groupTransf.push_back(valores); 
            }                        




        }
    } 

    for (tinyxml2::XMLElement* model = group->FirstChildElement("models"); model; model = model->NextSiblingElement("models")) {
        for (tinyxml2::XMLElement* modelChild = model->FirstChildElement(); modelChild; modelChild = modelChild->NextSiblingElement()) {
            if (std::string(modelChild->Name()) == "model") {
                const char* file = modelChild->Attribute("file");
                if (file) {
                    groupFiles.push_back(std::make_pair(fileCounter++, "models/"+std::string(file)));
                }
            }
        }
    }

    files.push_back(groupFiles);
    ts.push_back(groupTransf);

    for (tinyxml2::XMLElement* childGroup = group->FirstChildElement("group"); childGroup; childGroup = childGroup->NextSiblingElement("group")) {
        processGroup(childGroup, currentIndex, groupCounter);
    }
}



