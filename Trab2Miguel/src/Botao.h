/**
*   Classe que gera e configura um botao, com suas dimensoes, cores, gatilhos quando presssionado
*   Classe pode ser reconfigurada externamente, mudando de rotulos, cores e gatilhos de eventos
*
*   Autor: Miguel Miron Silva
*   UFSM - 2026
*
*   misilva@inf.ufsm.br
*   miguelmironsilva@gmail.com
*
**/

#ifndef __BOTAO_H__
#define __BOTAO_H__

#include <functional>
#include <string>
#include "Interativo.h"
#include "gl_canvas2d.h"

class Botao : public Interativo{

public:
    std::string rotulo;
    std::function<void()> clicked; //atribua qualquer chamada à isso

    //Várias cores para vários estados do botão

    struct Cor { float r, g, b; };

    Cor corNormal     = { 0.22f, 0.22f, 0.22f }; // preenchimento cinza-escuro padrão
    Cor corSobMouse   = { 0.32f, 0.32f, 0.32f };
    Cor corPressionado= { 0.12f, 0.12f, 0.12f };
    Cor corDesativado = { 0.55f, 0.55f, 0.55f };
    Cor corRotulo     = { 1.00f, 1.00f, 1.00f }; // texto branco
    Cor corBorda      = { 0.45f, 0.45f, 0.45f };

    //Construtor do botao
    Botao(float x, float y, float larg, float alt,
          const std::string& rot = "Botao")
        : Interativo(x, y, larg, alt), rotulo(rot)
    {}

  void Render() override
  {
      //determina a cor do preenchimento
       Cor preenchimento;
        if (!ativo)
            preenchimento = corDesativado;
        else if (pressionado)
            preenchimento = corPressionado;
        else if (sobMouse)
            preenchimento = corSobMouse;
        else
            preenchimento = corNormal;


       CV::color(preenchimento.r, preenchimento.g, preenchimento.b); //cor do botao
      CV::rectFill(x, y, x + largura, y + altura); //desenha o botao

      CV::color(corBorda.r, corBorda.g, corBorda.b); //adiciona uma borda
      CV::rect(x, y, x + largura, y + altura); //desenha a borda

      CV::color(corRotulo.r, corRotulo.g, corRotulo.b); //cor  do texto
      CV::text(x+5, y+altura/2, rotulo.c_str()); //escreve o label do botao mais ou menos ao centro.
  }

  //tratamento dos estados do mouse
  bool TratarMouse(int button, int state, int wheel, int direction, int mx, int my) override
  {
       //deixa a classe base atualizar sobMouse e pressionado
        bool estavaPressionado = pressionado;
        Interativo::TratarMouse(button, state, wheel, direction, mx, my);

        // Dispara aoClicar: botão esquerdo (0) solto (estado==1)
        // com o cursor ainda dentro e o widget ativo
        if (ativo && button == 0 && state == 1
            && estavaPressionado && sobMouse && aoClicar)
        {
            aoClicar();
            return true; // evento consumido
        }

        // Consome eventos de mouse-down que caem dentro do botão
        // para que widgets abaixo não reajam também
        if (ativo && button == 0 && state == 0 && sobMouse)
            return true;

        return false;
  }

  //tratamento dos estados do teclado
  bool TratarTeclado(int key, bool keyUp) override
    {
        if (!ativo || keyUp) return false;

        if ((key == ' ' || key == '\r' || key == '\n') && aoClicar) //barra de espaço e enter tbm acionam o botao
        {
            aoClicar();
            return true;
        }
        return false;
    }

    //configura externamente rotulos, cores e o que o botao faz ao clicar
    Botao* comRotulo(const std::string& rot)
    {
        rotulo = rot;
        return this;
    }

    Botao* comCores(Cor normal, Cor sobMouse, Cor pressionado)
    {
        corNormal      = normal;
        corSobMouse    = sobMouse;
        corPressionado = pressionado;
        return this;
    }

    Botao* comAoClicar(std::function<void()> cb)
    {
        aoClicar = cb;
        return this;
    }

};

#endif
