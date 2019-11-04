#include <stdio.h>
#include <stdlib.h>
#include <string.h>        // Para usar strings
#include <math.h>

#ifdef WIN32
#include <windows.h>    // Apenas para Windows
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>     // Funções da OpenGL
#include <GL/glu.h>    // Funções da GLU
#include <GL/glut.h>   // Funções da FreeGLUT
#endif

// SOIL é a biblioteca para leitura das imagens
#include "SOIL.h"

// Um pixel RGB (24 bits)
typedef struct {
    unsigned char r, g, b;
} RGB;

// Uma imagem RGB
typedef struct {
    int width, height;
    RGB* img;
} Img;

// Protótipos
void load(char* name, Img* pic);
void uploadTexture();

// Funções da interface gráfica e OpenGL
void init();
void draw();
void keyboard(unsigned char key, int x, int y);

// Largura e altura da janela
int width, height;

// Identificadores de textura
GLuint tex[3];

// As 3 imagens
Img pic[3];

// Imagem selecionada (0,1,2)
int sel;

// Carrega uma imagem para a struct Img
void load(char* name, Img* pic)
{
    int chan;
    pic->img = (RGB*) SOIL_load_image(name, &pic->width, &pic->height, &chan, SOIL_LOAD_RGB);
    if(!pic->img)
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
        exit(1);
    }
    printf("Load: %d x %d x %d\n", pic->width, pic->height, chan);
}

int main(int argc, char** argv)
{
    if(argc < 2) {
        printf("seamcarving [origem] [mascara]\n");
        printf("Origem é a imagem original, mascara é a máscara desejada\n");
        exit(1);
    }
    glutInit(&argc,argv);

    // Define do modo de operacao da GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // pic[0] -> imagem original
    // pic[1] -> máscara desejada
    // pic[2] -> resultado do algoritmo

    // Carrega as duas imagens
    load(argv[1], &pic[0]);
    load(argv[2], &pic[1]);

    if(pic[0].width != pic[1].width || pic[0].height != pic[1].height) {
        printf("Imagem e máscara com dimensões diferentes!\n");
        exit(1);
    }

    // A largura e altura da janela são calculadas de acordo com a maior
    // dimensão de cada imagem
    width = pic[0].width;
    height = pic[0].height;

    // A largura e altura da imagem de saída são iguais às da imagem original (1)
    pic[2].width  = pic[1].width;
    pic[2].height = pic[1].height;

    // Especifica o tamanho inicial em pixels da janela GLUT
    glutInitWindowSize(width, height);

    // Cria a janela passando como argumento o titulo da mesma
    glutCreateWindow("Seam Carving");

    // Registra a funcao callback de redesenho da janela de visualizacao
    glutDisplayFunc(draw);

    // Registra a funcao callback para tratamento das teclas ASCII
    glutKeyboardFunc (keyboard);

    // Cria texturas em memória a partir dos pixels das imagens
    tex[0] = SOIL_create_OGL_texture((unsigned char*) pic[0].img, pic[0].width, pic[0].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
    tex[1] = SOIL_create_OGL_texture((unsigned char*) pic[1].img, pic[1].width, pic[1].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    // Exibe as dimensões na tela, para conferência
    printf("Origem  : %s %d x %d\n", argv[1], pic[0].width, pic[0].height);
    printf("Destino : %s %d x %d\n", argv[2], pic[1].width, pic[0].height);
    sel = 0; // pic1

    // Define a janela de visualizacao 2D
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0,width,height,0.0);
    glMatrixMode(GL_MODELVIEW);

    // Aloca memória para a imagem de saída
    pic[2].img = malloc(pic[1].width * pic[1].height * 3); // W x H x 3 bytes (RGB)
    // Pinta a imagem resultante de preto!
    memset(pic[2].img, 0, width*height*3);

    // Cria textura para a imagem de saída
    tex[2] = SOIL_create_OGL_texture((unsigned char*) pic[2].img, pic[2].width, pic[2].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    // Entra no loop de eventos, não retorna
    glutMainLoop();
}

boolean isBorda(int i, int j, int width, int height) {
    if(i == 0) {
        return 1;
    }
    if(j == 0) {
        return 1;
    }
    if(i == width) {
        return 1;
    }
    if(j == (height -1)) {
        return 1;
    }
    else {
        return 0;
    }
}
// código trabalho
void pixEnergy() {
    int width = pic[0].width;
    int height = pic[0].height;
    int energy[width][height];

    boolean ctrl = 1;
    boolean ctrl2 = 1;

    printf("width: %i \n", width);
    printf("height: %i \n", height);
    if ( ctrl == 1) {
        for ( int i = 0; i < width; i++ ) {
        //printf("sizeW: %i \n",pic[0].width);
            for ( int j = 0; j < height; j++ ) {

                if(isBorda(i, j, width, height) ) {
                    energy[i][j] = 100000;
                }
                else {
                    int rx = (pic[0].img[j * (width) + i+1].r) - (pic[0].img[j * (width) + i-1].r);
                    int gx = (pic[0].img[j * (width) + i+1].g) - (pic[0].img[j * (width) + i-1].g);
                    int bx = (pic[0].img[j * (width) + i+1].b) - (pic[0].img[j * (width) + i-1].b);
                    int deltaX = pow(rx, 2) + pow(gx, 2) + pow(bx, 2);
                    //printf("deltax: %d \n", deltaX);
                    int ry = (pic[0].img[(j+1) * (width) + i].r) - (pic[0].img[(j-1) * (width) + i].r);
                    int gy = (pic[0].img[(j+1) * (width) + i].g) - (pic[0].img[(j-1) * (width) + i].g);
                    int by = (pic[0].img[(j+1) * (width) + i].b) - (pic[0].img[(j-1) * (width) + i].b);
                    int deltaY = pow(ry, 2) + pow(gy, 2) + pow(by, 2);
                    //printf("deltay: %d \n", deltaY);
                    energy[i][j] = deltaX + deltaY;
                }

            }

        }
        ctrl = 0;
    }


        // debuger para print da energia de cada pixel
//        for( int l = 0; l < width; l++ ) {
//            for( int c = 0; c < height; c++) {
//                printf("linha: %i, col: %i, valor: %i\n", l, c, energy[l][c]);
//            }
//        }


    // matriz de somas
    int matrizSoma[width][height];
    for( int a = 0; a < width; a++ ) {
        for( int b = 0; b < height; b++ ) {
            matrizSoma[a][b] = energy[a][b];
//            printf("linha: %i, coluna: %i, valor: %i\n", a, b, matrizSoma[a][b]);
        }
    }
    if(ctrl2 == 1) {
        for( int a = 0; a < width; a++ ) {
            for( int b = 0; b < height; b++ ) {

                if (a == 0) {
                    matrizSoma[a][b] = energy[a][b];
                }
                else {
                    if (b == 0) {

                        int vlrAtual = matrizSoma[a][b];
                        vlrAtual += matrizSoma[a-1][b];
                        if( (matrizSoma[a][b] + matrizSoma[a-1][b+1]) < vlrAtual ) {
                            vlrAtual = matrizSoma[a][b] + matrizSoma[a-1][b+1];
                            matrizSoma[a][b] = vlrAtual;
                        }
                        else {
                            matrizSoma[a][b] = vlrAtual;
                        }

                    }
                    else {
                        if (b == height-1) {
                            int aux = matrizSoma[a][b];
                            aux += matrizSoma[a-1][b];
                            if( (matrizSoma[a][b] + matrizSoma[a-1][b-1]) < aux ) {
                                aux = matrizSoma[a][b] + matrizSoma[a-1][b-1];
                                matrizSoma[a][b] = aux;
                            }
                            else {
                                matrizSoma[a][b] = aux;
                            }
                        }
                        else {
                            int aux1 = matrizSoma[a][b];
                            aux1 += matrizSoma[a-1][b-1];
                            if( matrizSoma[a][b] + matrizSoma[a-1][b] < aux1 ) {
                                aux1 = matrizSoma[a][b] + matrizSoma[a-1][b];
                            }
                            if( matrizSoma[a][b] + matrizSoma[a-1][b+1] < aux1 ) {
                                aux1 = matrizSoma[a][b] + matrizSoma[a-1][b+1];
                            }
                            matrizSoma[a][b] = aux1;
                        }
                    }

                }


            }
        }
        ctrl2 = 0;
    }
//    imprime matriz de somas
//    for( int l = 0; l < width; l++ ) {
//            for( int c = 0; c < height; c++) {
//                printf("linha: %i, col: %i, valor: %i\n", l, c, matrizSoma[l][c]);
//            }
//        }

//    for( int c = 0; c < height; c++) {
//        printf("linha: %i, col: %i, valor: %i\n", width, c, matrizSoma[width][c]);
//    }
    int positions[width];

    for(int m = width; m >= 0; m--) {
        int menor = matrizSoma[m][0];
        for(int n = 0; n < height; n++) {
            if( matrizSoma[m][n] < menor ) {
                menor = matrizSoma[m][n];
            }
        }

        positions[m] = menor;
    }
//    for(int teste = 0; teste < width; teste++) {
//        printf("posicao: %i menor: %i\n",teste, positions[teste]);
//    }




}




// Gerencia eventos de teclado
void keyboard(unsigned char key, int x, int y)
{
    if(key==27) {
      // ESC: libera memória e finaliza
      free(pic[0].img);
      free(pic[1].img);
      free(pic[2].img);
      exit(1);
    }
    if(key >= '1' && key <= '3')
        // 1-3: seleciona a imagem correspondente (origem, máscara e resultado)
        sel = key - '1';
    if(key == 's') {
        pixEnergy();
        // Aplica o algoritmo e gera a saida em pic[2].img...
        // ...
        // ... (crie uma função para isso!)

        // Exemplo: pintando tudo de amarelo
        for(int i=0; i<pic[2].height*pic[2].width; i++)
            pic[2].img[i].r = pic[2].img[i].g = 98;

        // Chame uploadTexture a cada vez que mudar
        // a imagem (pic[2])
        uploadTexture();
    }
    glutPostRedisplay();
}

// Faz upload da imagem para a textura,
// de forma a exibi-la na tela
void uploadTexture()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        pic[2].width, pic[2].height, 0,
        GL_RGB, GL_UNSIGNED_BYTE, pic[2].img);
    glDisable(GL_TEXTURE_2D);
}

// Callback de redesenho da tela
void draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Preto
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Para outras cores, veja exemplos em /etc/X11/rgb.txt

    glColor3ub(255, 255, 255);  // branco

    // Ativa a textura corresponde à imagem desejada
    glBindTexture(GL_TEXTURE_2D, tex[sel]);
    // E desenha um retângulo que ocupa toda a tela
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glTexCoord2f(0,0);
    glVertex2f(0,0);

    glTexCoord2f(1,0);
    glVertex2f(pic[sel].width,0);

    glTexCoord2f(1,1);
    glVertex2f(pic[sel].width, pic[sel].height);

    glTexCoord2f(0,1);
    glVertex2f(0,pic[sel].height);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Exibe a imagem
    glutSwapBuffers();
}
