/**
*   Classe genérica que denomina um elemento interativo - de botões à checkboxes à sliders
*   Classe pode ser reconfigurada externamente, mudando de posicao, tamanho e lugar
*
*   Autor: Miguel Miron Silva
*   UFSM - 2026
*
*   misilva@inf.ufsm.br
*   miguelmironsilva@gmail.com
*
**/

#ifndef __INTERATIVO_H__
#define __INTERATIVO_H__

#include "Vector2.h"
#include "gl_canvas2d.h"

class Interativo{
    float altura, largura, x, y;

public:
    bool sobMouse, pressionado, ativo, visivel;

    Interativo(float x, float y, float largura, float altura)
        : x(x), y(y), largura(largura), altura(altura),
          sobMouse(false), pressionado(false), ativo(true), visivel(true)
    {}

    virtual void Render() = 0; //Aqui é onde as classes derivadas vão desenhar os seus layouts

    bool TratarMouse(int button, int state, int wheel, int direction, int mx, int my)
    {
        if (!ativo)
        {
            sobMouse    = false;
            pressionado = false;
            return false;
        }

        sobMouse = Colidiu((float)mx, (float)my);

        // Rastreia o estado pressionado para clique com botão esquerdo
        if (button == 0)
        {
            if (state == 0 && sobMouse) // botão esquerdo pressionado dentro do interativo
                pressionado = true;
            if (state == 1)             // botão esquerdo solto em qualquer lugar
                pressionado = false;
        }

        // Movimento do mouse sem botão: limpa pressionado se o cursor saiu
        if (button == -2 && !sobMouse)
            pressionado = false;

        return false;
    }

    bool TratarTeclado(int key, bool keyUp) //chamado pelo bt_gerenciador a cada evento de teclado, mas retorna falso
    {
        (void)key;
        (void)keyUp;
        return false;
    }

    Vector2 getCenter() const //retorna ponto central do elemento interativo
    {
        return Vector2(x + largura * 0.5f, y + altura * 0.5f);
    }

    bool Colidiu(float px, float py) const //detecta se px py está dentro do elemento interativo
    {
        return ( px >= x && px <= (x + largura) && py >= y && py <= (y + altura) );
    }

    bool Sobrepos(const Interativo& outro) const //detecta se a área do elemento interativo intersecciona a área de outro
    {
        return !(x + largura  < outro.x || outro.x + outro.largura  < x ||
                 y + altura < outro.y || outro.y + outro.altura < y);
    }

    //configura externamente o elemento interativo

    void setPosicao(float novoX, float novoY)
    {
        x = novoX;
        y = novoY;
    }

    void setTamanho(float novaLargura, float novaAltura)
    {
        largura = novaLargura;
        altura  = novaAltura;
    }

    void setBounds(float novoX, float novoY,
                   float novaLargura, float novaAltura)
    {
        x       = novoX;
        y       = novoY;
        largura = novaLargura;
        altura  = novaAltura;
    }

};

#endif
