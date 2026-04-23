/**
*   Classe que gera e configura um checkbox
*   Classe pode ser reconfigurada externamente, mudando de rotulos, cores e gatilhos de eventos (quando clicado ou marcado)
*
*   Autor: Miguel Miron Silva
*   UFSM - 2026
*
*   misilva@inf.ufsm.br
*   miguelmironsilva@gmail.com
*
**/

#ifndef __CHECKBOX_H__
#define __CHECKBOX_H__

#include <functional>
#include <string>
#include "Interativo.h"
#include "gl_canvas2d.h"

class Checkbox : public Interativo
{
    float tamanhoCaixa, espacoRotulo;
    bool marcado;
    std::string rotulo;
    std::function<void(bool)> aoAlternar;

    //Vários estados de cor do checkbox

    struct Cor { float r, g, b; };

    Cor corCaixaNormal     = { 0.20f, 0.20f, 0.20f }; // preenchimento desmarcado
    Cor corCaixaMarcado    = { 0.20f, 0.55f, 0.90f }; // preenchimento marcado (azul)
    Cor corCaixaSobMouse   = { 0.30f, 0.30f, 0.30f }; // preenchimento ao passar mouse
    Cor corCaixaPressionado= { 0.10f, 0.10f, 0.10f }; // preenchimento ao pressionar
    Cor corCaixaDesativado = { 0.50f, 0.50f, 0.50f };
    Cor corBorda           = { 0.55f, 0.55f, 0.55f };
    Cor corBordaSobMouse   = { 0.80f, 0.80f, 0.80f };
    Cor corMarca           = { 1.00f, 1.00f, 1.00f }; // cor do símbolo de verificação
    Cor corRotulo          = { 0.90f, 0.90f, 0.90f }; // cor do texto do rótulo

    //construtor do checkbox
    Checkbox(float x, float y, float tamCaixa, const std::string& rot = "Opcao",bool estadoInicial = false)
        : Interativo(x, y,
                     tamCaixa + 6.0f + (float)rot.size() * 8.0f, // largura automática
                     tamCaixa),                                    // altura automática
          marcado(estadoInicial), rotulo(rot), tamanhoCaixa(tamCaixa), espacoRotulo(6.0f)
    {}

    //renderizacao do checkbox
    void Render() override
    {
        //determina a cor do preenchimento
        Cor preenchimento;
        if (!ativo)
            preenchimento = corCaixaDesativado;
        else if (pressionado)
            preenchimento = corCaixaPressionado;
        else if (sobMouse)
            preenchimento = corCaixaSobMouse;
        else if (marcado)
            preenchimento = corCaixaMarcado;
        else
            preenchimento = corCaixaNormal;

        //desenho do fundo da caixa
        float cx1 = x;
        float cy1 = y;
        float cx2 = cx1 + tamanhoCaixa;
        float cy2 = cy1 + tamanhoCaixa;

        CV::color(preenchimento.r, preenchimento.g, preenchimento.b);
        CV::rectFill(cx1, cy1, cx2, cy2);

        //desenho das bordas da caixa
        Cor borda = (sobMouse && ativo) ? corBordaSobMouse : corBorda;
        CV::color(borda.r, borda.g, borda.b);
        CV::rect(cx1, cy1, cx2, cy2);

        //desenho da marca de verificacao
        if (marcado && ativo)
        {
            desenharMarca(cx1, cy1);
        }
        else if (marcado && !ativo)
        {
            // Marca atenuada para estado desativado-mas-marcado
            CV::color(corMarca.r * 0.5f,
                      corMarca.g * 0.5f,
                      corMarca.b * 0.5f);
            desenharMarca(cx1, cy1);
        }

        //desenho do rotulo à direita da caixa
        CV::color(corRotulo.r, corRotulo.g, corRotulo.b);
        float rotuloX = cx2 + espacoRotulo;
        float rotuloY = cy1 + (tamanhoCaixa + 13.0f) * 0.5f - 4.0f;
        CV::text(rotuloX, rotuloY, rotulo.c_str());
    }

    //tratamento do mouse
    bool TratarMouse(int button, int state, int wheel, int direction, int mx, int my) override
     {
        bool estavaPressionado = pressionado;
        Interativo::TratarMouse(button, state, wheel, direction, mx, my);

        // Alterna ao soltar o botão esquerdo dentro do widget
        if (ativo && button == 0 && state == 1 && estavaPressionado && sobMouse)
        {
            marcado = !marcado;
            if (aoAlternar) aoAlternar(marcado);
            return true;
        }

        // Consome eventos de mouse-down que caem dentro do widget
        if (ativo && button == 0 && state == 0 && sobMouse)
            return true;

        return false;
    }

    //tratamento do teclado
    bool TratarTeclado(int key, bool keyUp) override
    {
        if (!ativo || keyUp) return false;

        if (key == ' ') //barra de espaço alterna o checkbox
        {
            marcado = !marcado;
            if (aoAlternar) aoAlternar(marcado);
            return true;
        }
        return false;
    }

    //definicao programatica do estado marcado
    //dispara aoAlternar apenas quando notificar = true (padrão).
    void setValor(bool valor, bool notificar = true)
    {
        if (marcado == valor) return;
        marcado = valor;
        if (notificar && aoAlternar) aoAlternar(marcado);
    }

    //customização fora da classe
    Checkbox* comRotulo(const std::string& rot)
    {
        rotulo  = rot;
        largura = tamanhoCaixa + espacoRotulo + (float)rotulo.size() * 8.0f;
        return this;
    }

    Checkbox* comAoAlternar(std::function<void(bool)> cb)
    {
        aoAlternar = cb;
        return this;
    }

    Checkbox* comMarcado(bool valor)
    {
        marcado = valor;
        return this;
    }

    Checkbox* comCores(Cor caixaNormal, Cor caixaMarcado, Cor rotuloCor)
    {
        corCaixaNormal  = caixaNormal;
        corCaixaMarcado = caixaMarcado;
        corRotulo       = rotuloCor;
        return this;
    }

private:

    //tratamento de desenho do tick do checkbox
    {
        float s = tamanhoCaixa;
        float p = 1.5f; // recuo interno

        float inicioX = cx + p + s * 0.15f;
        float inicioY = cy + p + s * 0.45f;
        float pivoX   = cx + p + s * 0.35f;
        float pivoY   = cy + p + s * 0.72f;
        float fimX    = cx + p + s * 0.82f;
        float fimY    = cy + p + s * 0.18f;

        CV::color(corMarca.r, corMarca.g, corMarca.b);
        CV::line(inicioX, inicioY, pivoX, pivoY);
        CV::line(pivoX,   pivoY,   fimX,  fimY);
    }

};

#endif // __CHECKBOX_H__
