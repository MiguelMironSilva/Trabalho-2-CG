/*********************************************************************
// Canvas para desenho, criada sobre a API OpenGL. Nao eh necessario conhecimentos de OpenGL para usar.
//  Autor: Miguel Miron Silva
//         03/2026
//
//  Pode ser utilizada para fazer desenhos, animacoes, e jogos simples.
//  Tem tratamento de mouse e teclado
//  Estude o OpenGL antes de tentar compreender o arquivo gl_canvas.cpp
//
//  Versao 2.1
//
// *********************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "bt_gerenciador.h" // puxa Interativo.h automaticamente

GerenciadorUI gerenciadorUI;

#include "gl_canvas2d.h"

//largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 500, screenHeight = 500;


int opcao  = 50;//variavel global para selecao do que sera exibido na canvas.
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().


//Substituição dos numeros magicos no programa
const int POS_X_TITULO = 20;
const int POS_Y_TITULO = 500;
const int TECLA_ESC = 27;


//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa função com poucas linhas de codigo.
void render()
{
   CV::text(POS_X_TITULO, POS_Y_TITULO, "Programa Demo Canvas2D");

   Sleep(10); //nao eh controle de FPS. Somente um limitador de FPS.
}



//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   if( key < 200 )
   {
      opcao = key;
   }

   switch(key)
   {
      TECLA_ESC;
	  break;
   }
}

//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
   printf("\nLiberou: %d" , key);
}



//funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   mouseX = x; //guarda as coordenadas do mouse para exibir dentro da render()
   mouseY = y;

}



int main(void)
{
    gerenciadorUI.Adicionar(new Botao(20, 460, 100, 30, "Botao 1"))->comAoClicar(..);
    gerenciadorUI.Adicionar(new Botao(20, 460, 100, 30, "Botao 2"))->comAoClicar(..);
    gerenciadorUI.Adicionar(new Checkbox(20, 420, 16, "Checkbox")))->comAoAlternar(..);
    gerenciadorUI.Adicionar(new Slider(20, 370, 200, 20, 0.0f, 100.0f, 50.0f))->comAoMudar(..);

   CV::init(&screenWidth, &screenHeight, "Trabalho 1 - Miguel Miron Silva");
   CV::run();
}
