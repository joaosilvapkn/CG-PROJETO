#include <IL/il.h>
#include "xmlparser.h"
#include "catmullrom.h"
#include <GL/glxew.h>


std::vector<std::string> stored;
std::vector<int> mapa;
std::vector<GLuint> vbo(1);
std::vector<GLuint> vbo2(1);
std::vector<GLuint> vbo3(1);
std::vector<std::pair<std::string,int>> te;
std::vector<std::pair<int, std::vector<int>>> fe;
std::vector<GLuint> texturesglu;





//percorre o vetor ficheiros, para cada elemento, percorre o ficheiro e guarda os valores
void get_numbers(std::vector<std::string> fcs){

    for (auto& elem : fcs) { 

        int flag=-1;

        for(int h=0; h<stored.size();h++){    //percorremos o vetor dos ficheiros ja usados, e vemos se ja foi lido e tem os pontos guardados
            if(stored[h]==elem){
                flag=h;                       //se sim, obtemos em flag a posicao em que os pontos estao guardados
            }
        }
        
        if(flag==-1){                       //se ainda nao foi lido, prosseguimos normalmente e guardamos os pontos    
            stored.push_back(elem);         

            std::ifstream f(elem);

            std::vector<float> numbersArray; 
            std::vector<float> normalsArray;
            std::vector<float> texturesArray;

            std::string linha;

            //percorre todas as linhas do ficheiro
            while (std::getline(f, linha)) {
                std::string xstr="";
                std::string normxstr="";
                std::string ystr="";
                std::string normystr="";
                std::string zstr="";
                std::string normzstr="";
                std::string textxstr="";
                std::string textzstr="";
                float x;
                float y;
                float z;
                float normx;
                float normy;
                float normz;
                float textx;
                float textz;
                int contador=0;          //representa o nº de espaços encontrados em cada linha
                for(char a : linha){

                    if(a!=' ' && a!='\n'){ //se o nº de espaços for 0, estamos a copiar o valor de x
                        if(contador==0){
                            xstr+=a;
                        }

                        else if(contador==1){ //se o nº de espaços for 0, estamos a copiar o valor de y
                            ystr+=a;
                        }

                        else if(contador==2){ //se o nº de espaços for 0, estamos a copiar o valor de z
                            zstr+=a;
                        }

                        else if(contador==3){
                            normxstr+=a;
                        }

                        else if(contador==4){
                            normystr+=a;
                        }

                        else if(contador==5){
                            normzstr+=a;
                        }

                        else if(contador==6){
                            textxstr+=a;
                        }

                        else if(contador==7){
                            textzstr+=a;
                        }
                    }

                    else if(a==' '){
                        contador+=1;
                    }
                }

                x = std::stof(xstr);
                y = std::stof(ystr);
                z = std::stof(zstr);
                numbersArray.push_back(x);
                numbersArray.push_back(y);    //guardamos os valores no vetor
                numbersArray.push_back(z);

                if(normxstr.size()>0){
                    normx = std::stof(normxstr);
                    normy = std::stof(normystr);
                    normz = std::stof(normzstr);
                    normalsArray.push_back(normx);
                    normalsArray.push_back(normy);
                    normalsArray.push_back(normz);

                }

                if(textxstr.size()>0){
                    textx = std::stof(textxstr);
                    textz = std::stof(textzstr);
                    texturesArray.push_back(textx);
                    texturesArray.push_back(textz);
                }

            }


            numbersArrays.push_back(numbersArray); //guardamos o vetor no vetor de vetores
            normalsArrays.push_back(normalsArray);
            texturesArrays.push_back(texturesArray);
        }

        else{                                       
            std::vector<float> numbersArray;
            numbersArray.push_back(flag);               //se ja foi lido, guardamos no vetor de vetores, um vetor com um elemento, a posicao em que os pontos se
                                                        //encontram no mesmo vetor
            numbersArrays.push_back(numbersArray); 
            if(normalsArrays.size()>0){  
                normalsArrays.push_back(numbersArray);
            }
            if(texturesArrays.size()>0){
                texturesArrays.push_back(numbersArray);
            }
        }


    }

}



void loadTextures() {

    for(int u=0; u<textures.size(); u++){
        GLuint texture_id;
        unsigned int t, tw, th;
        unsigned char *texData;
        ilGenImages(1, &t);
        ilBindImage(t);
        
        if (!ilLoadImage((ILstring)textures[u].c_str())) {
            printf("Error - Texture file not found: %s\n", textures[u].data());
            exit(1);
        }
        
        tw = ilGetInteger(IL_IMAGE_WIDTH); 
        th = ilGetInteger(IL_IMAGE_HEIGHT);
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        texData = ilGetData();
        glGenTextures(1, &texture_id);


        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
        texturesglu.push_back(texture_id);
    }
}





void draw_group(int i){

        for(int j=0; j<ts[i].size(); j++){
            
            if(ts[i][j][0]==0){ //translacao
                glTranslatef(ts[i][j][1],ts[i][j][2],ts[i][j][3]);
            }

            if(ts[i][j][0]==1){ //rotacao
                glRotatef(ts[i][j][1],ts[i][j][2],ts[i][j][3], ts[i][j][4]);
            }

            if(ts[i][j][0]==2){ //escala
                glScalef(ts[i][j][1],ts[i][j][2],ts[i][j][3]);
            }  


            else if(ts[i][j][0]==3){      //rotacao com tempo
                float tempo=ts[i][j][1]*1000;
                float angulo=glutGet(GLUT_ELAPSED_TIME)*360/tempo;
                glRotatef(angulo,ts[i][j][2],ts[i][j][3],ts[i][j][4]);
            } 





            else if(ts[i][j][0]==4){     //translacao com tempo (catmull-rom)

                int tamanho=ts[i][j].size();
                std::vector<std::vector<float>> pontos; 

                for(int a = 3; a<tamanho-8; a+=3){          //guarda os pontos de controlo no vetor de vetor de floats pontos
                    std::vector<float> ponto;
                    ponto.push_back(ts[i][j][a]);
                    ponto.push_back(ts[i][j][a+1]);   
                    ponto.push_back(ts[i][j][a+2]);
                    pontos.push_back(ponto);
                }

                float draw=ts[i][j][tamanho-1];           //flag que indica se vai ser desenhado
                float pos[3];
                float deriv[3];

                if(hasLight){
                    glDisable(GL_LIGHTING);
                }
                renderCatmullRomCurve(pos,deriv,pontos,draw);   

                if(hasLight){
                    glEnable(GL_LIGHTING);
                }

                float time=ts[i][j][1];
                float t=ts[i][j][tamanho-5];
                float current_time=ts[i][j][tamanho-6];

                getGlobalCatmullRomPoint(t, pos, deriv,pontos);

                glTranslatef(pos[0], pos[1], pos[2]);

                if(ts[i][j][2]==1){      //caso o objeto seja alinhado com a curva

                    float x[3] = {deriv[0], deriv[1], deriv[2]};
                    float y[3];
                    float z[3];
                    float prev_y[3];
                    prev_y[0]=ts[i][j][tamanho-4];
                    prev_y[1]=ts[i][j][tamanho-3];
                    prev_y[2]=ts[i][j][tamanho-2];


                    normalize(x);
                    cross(x, prev_y, z);
                    normalize(z);
                    cross(z,x,y);
                    normalize(y);

                    ts[i][j][tamanho-4]=y[0];
                    ts[i][j][tamanho-3]=y[1];
                    ts[i][j][tamanho-2]=y[2];


                    float m[16];

                    buildRotMatrix(x,y,z,m);

                    glMultMatrixf(m);
                }


                float new_time = glutGet(GLUT_ELAPSED_TIME);
                float diff = new_time - current_time;
                
                
                
                t += diff/time;
                current_time = new_time;
                
                ts[i][j][tamanho-5]=t;;

                ts[i][j][tamanho-6]=current_time;
                
            }



        } 



        for(int j=0; j<files[i].size(); j++){

            int reali=files[i][j].first;  
            int num=mapa[reali]; 

            if(numbersArrays[reali].size()==1){                  //se o tamanho do elemento for igual a um, significa que nao tem pontos, mas que aponta para a posição
                reali=numbersArrays[reali][0];                   //em que os seus pontos estao
            }  
            

            glBindBuffer(GL_ARRAY_BUFFER, vbo[num]);
            glVertexPointer(3, GL_FLOAT, 0, 0);  


            glBindBuffer(GL_ARRAY_BUFFER, vbo2[num]);
            glNormalPointer(GL_FLOAT, 0, 0);  
            


            if(hasColour && hasLight){

                int obj = files[i][j].first;

                for(int a=0; a<luzObj[obj].size(); a++){

                    if(luzObj[obj][a][0]==0){ //diffuse

                        GLfloat dif[]={luzObj[obj][a][1]/255.0f,luzObj[obj][a][2]/255.0f,luzObj[obj][a][3]/255.0f, 1.0f};
                        glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
                    }

                    if(luzObj[obj][a][0]==1){ //ambient

                        GLfloat amb[]={luzObj[obj][a][1]/255.0f,luzObj[obj][a][2]/255.0f,luzObj[obj][a][3]/255.0f, 1.0f};
                        glMaterialfv(GL_FRONT, GL_AMBIENT, amb);  
                    }


                    if(luzObj[obj][a][0]==2){ //specular

                        GLfloat spec[]={luzObj[obj][a][1]/255.0f,luzObj[obj][a][2]/255.0f,luzObj[obj][a][3]/255.0f, 1.0f};
                        glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
                    }

                    if(luzObj[obj][a][0]==3){  //emissive

                        GLfloat emi[]={luzObj[obj][a][1]/255.0f,luzObj[obj][a][2]/255.0f,luzObj[obj][a][3]/255.0f, 1.0f};
                        glMaterialfv(GL_FRONT, GL_EMISSION, emi);
                    }


                    if(luzObj[obj][a][0]==4){ //shininess

                        GLfloat shin=luzObj[obj][a][1];
                        glMaterialf(GL_FRONT, GL_SHININESS, shin);
                    }  
                                                                             
                }


            }


            else if(hasLight){


                float diffuse[] = {200.0f/255.0f, 200.0f/255.0f, 200.0f/255.0f, 1.0f/255.0f};
                float ambient[] = {50.0f/255.0f, 50.0f/255.0f, 50.0f/255.0f, 1.0f/255.0f};
                float specular[] = {0.0f, 0.0f, 0.0f, 1.0f/255.0f};
                float emissive[] = {0.0f, 0.0f, 0.0f, 1.0f/255.0f};
                float shininess = 0.0f;

                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
                glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);



            }



            if(hasText){
                glBindTexture(GL_TEXTURE_2D, texturesglu[files[i][j].first]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo3[num]);
                glTexCoordPointer(2,GL_FLOAT, 0, 0);
            }




            glDrawArrays(GL_TRIANGLES, 0, numbersArrays[reali].size() / 3); 

                

            if(hasText){
                glBindTexture(GL_TEXTURE_2D,0);
            } 

        }
         

}


void draw(int i) {
    glPushMatrix();
    draw_group(i);
    for(int j=0; j<childs[i].size(); j++){
        draw(childs[i][j]);
        int x=1;
    }
    glPopMatrix();
    
}




void global_light(){
        
    
    GLenum lightEnums[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};

    for(int c = 0; c < luzglobal.size(); c++) {
        GLenum currentLight = lightEnums[c];


        if(luzglobal[c][0] == 0) { //direction

            GLfloat lightDirection[] = {luzglobal[c][1], luzglobal[c][2], luzglobal[c][3], 0.0f};        
            glLightfv(currentLight, GL_POSITION, lightDirection);


        } else if(luzglobal[c][0] == 1) { //point

            GLfloat lightPosition[] = {luzglobal[c][1], luzglobal[c][2], luzglobal[c][3], 1.0f}; 
            glLightfv(currentLight, GL_POSITION, lightPosition);  



        } else if(luzglobal[c][0] == 2) { // Spot

            GLfloat lightPosition[] = {luzglobal[c][1], luzglobal[c][2], luzglobal[c][3], 1.0f};
            GLfloat spotDirection[] = {luzglobal[c][4], luzglobal[c][5], luzglobal[c][6], 0.0f};
            GLfloat cutoff = luzglobal[c][7];

            glLightfv(currentLight, GL_POSITION, lightPosition);
            glLightfv(currentLight, GL_SPOT_DIRECTION, spotDirection);
            glLightf(currentLight, GL_SPOT_CUTOFF, cutoff);         
        }



    }
    
}









void changeSize(int w, int h) {
    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(h == 0)
        h = 1;

    // compute window's aspect ratio 
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();
    
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(fov ,ratio, near ,far);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}



void keyboardSpecial(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
    alpha -= 0.1f;
    break;
    case GLUT_KEY_RIGHT:
    alpha += 0.1f;
    break;
    case GLUT_KEY_UP:
    beta += 0.1f;
    break;
    case GLUT_KEY_DOWN:
    beta -= 0.1f;
    break;
    }
    //alpha = std::clamp(alpha, -1.5, 1.5);
    //beta = std::clamp(beta, -1.5, 1.5);
    //glutPostRedisplay();
}



void processKeys(unsigned char key, int x, int y){
    switch (key) {
    case 'a':
        dist -= 0.1;
        break;
    case 'q':
        dist += 0.1;
        break;
    case 's':
        dist -= 0.5;
        break;
    case 'w':
        dist += 0.5;
        break;
    case 'd':
        dist -= 1;
        break;
    case 'e':
        dist += 1;
        break;
    case 'r':
        dist += 10;
        break;
    case 'f':
        dist -= 10;
        break;


    }
}





void draw_eixos(){
    if(hasLight){
        glDisable(GL_LIGHTING);
    }
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f); // X-axis (red)
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f); // Y-axis (green)
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f); // Z-axis (blue)
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
    if(hasLight){
        glEnable(GL_LIGHTING);
    }
}


void renderScene(void) {

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set camera
    glLoadIdentity();

    camz = dist * cos(beta) * cos(alpha);
    camx = dist * cos(beta) * sin(alpha);
    camy = dist * sin(beta);

    gluLookAt(camx, camy, camz, lookx, looky, lookz, upx, upy, upz);

    if(eixos==1){
        draw_eixos();
    }

    if(hasLight){
        global_light();
    }

    glColor3f(1.0f, 1.0f, 1.0f); 
    draw(0);
    
    glutSwapBuffers();
}






void init_vbos(){
    glClearColor(0.0, 0.0, 0.0, 1.0);

    int tamanho=0;
    for(int j=0; j<numbersArrays.size(); j++){ 
        if (numbersArrays[j].size()>1){                 //tamanho necessário para o vetor de vbos, ignoramos elementos com comprimento=1, nao tem pontos
            tamanho++;
        }
    }
    
    vbo.resize(tamanho);
    vbo2.resize(tamanho);
    vbo3.resize(tamanho);
    int nrm=0;
    int tx=0;

    // Gerar buffer IDs
    glGenBuffers(tamanho, vbo.data());
    if(hasNorm){
        if(normalsArrays[0].size()>0){
            nrm=1;
            glGenBuffers(tamanho, vbo2.data());
        }
    }

    if(hasText){
        if(texturesArrays[0].size()>0){
            tx=1;
            glGenBuffers(tamanho, vbo3.data());
        }
    }

    int countervbo=0;  //indice do vetor de vbos
    
    for (size_t i = 0; i < numbersArrays.size(); i++) {                         //percorremos o vetor

        if (numbersArrays[i].size()>1){                                 //se o elemento tiver pontos, transferimos os dados para o vetor vbo e incrementamos       
            //std::pair<int, int> par = std::make_pair(i, countervbo);  //a posicao do vetor vbo


            mapa.push_back(countervbo);                                     //guardamos no vetor mapa os indices correspondentes aos dois vetores

            glBindBuffer(GL_ARRAY_BUFFER, vbo[countervbo]);
            glBufferData(GL_ARRAY_BUFFER, numbersArrays[i].size() * sizeof(float), numbersArrays[i].data(), GL_STATIC_DRAW);

            if(nrm==1){
                glBindBuffer(GL_ARRAY_BUFFER, vbo2[countervbo]);
                glBufferData(GL_ARRAY_BUFFER, normalsArrays[i].size()* sizeof(float), normalsArrays[i].data(), GL_STATIC_DRAW);
            }
            if(tx==1){
                glBindBuffer(GL_ARRAY_BUFFER, vbo3[countervbo]);
                glBufferData(GL_ARRAY_BUFFER, texturesArrays[i].size()* sizeof(float), texturesArrays[i].data(), GL_STATIC_DRAW); 
            }
            countervbo++;
        }

        else{
            mapa.push_back(numbersArrays[i][0]);
        }
    }

  //  glBindBuffer(GL_ARRAY_BUFFER, 0);

}



int main(int argc, char **argv) {

    if(argc!=2){
        std::cout << "numero incorreto de argumentos\n";
        return 1;
    }


    tinyxml2::XMLDocument doc;
    std::string xmlFilePath = "tests/" + std::string(argv[1]);
    doc.LoadFile(xmlFilePath.c_str());


    tinyxml2::XMLElement* root = doc.RootElement();
    if (root) {
        traverse_elements(root);
    }




    root = doc.RootElement();
    if (!root) {
        std::cout << "Elemento raiz ausente no arquivo XML.\n";
        return 1;
    }

    if(normalsArrays.size()>0){
        hasNorm=true;
    }

    if(textures.size()>0){
        hasText=true;
    }
    if(luzglobal.size()>0){
        hasLight=true;
        hasNorm=true;
    }
    if(luzObj.size()>0){
        hasColour=true;
        hasNorm=true;

    }


    

    std::vector<std::string> fcs;               //vetor com os nomes dos ficheiros .3d
    for(int i=0; i<files.size(); i++){
        for(int j=0; j<files[i].size(); j++){
            fcs.push_back(files[i][j].second);
        }
    }


    get_numbers(fcs);  //obter pontos, normais, coordenadas de textura


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("lcc_gr15_fase4");

    glewInit();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    init_vbos();

    // Required callback registry 
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutSpecialFunc(keyboardSpecial);
    glutKeyboardFunc(processKeys);


    //OpenGL settings

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    if(hasText){
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_TEXTURE_2D);
        ilInit();
        loadTextures();
    }

    if(hasLight){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_RESCALE_NORMAL);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_NORMALIZE);
        glEnable(GL_RESCALE_NORMAL);

        float white[4] = {1.0, 1.0, 1.0, 1.0};
        
        glEnable(GL_LIGHTING);
        for (int i=0; i<luzglobal.size(); i++) {
            glEnable(GL_LIGHT0 + i);
            glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, white);
            glLightfv(GL_LIGHT0 + i, GL_SPECULAR, white);
        }
        float amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
        


    }


    else{
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);  
    }

    

    glutPostRedisplay();
    
    glutMainLoop();
    return 0;
}
