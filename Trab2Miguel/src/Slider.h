/**
*   Classe que gera e configura um slider
*   Classe pode ser reconfigurada externamente, mudando de rotulos, cores e gatilhos de eventos (quando clicado ou marcado)
*
*   Autor: Miguel Miron Silva
*   UFSM - 2026
*
*   misilva@inf.ufsm.br
*   miguelmironsilva@gmail.com
*
**/

#ifndef __SLIDER_H__
#define __SLIDER_H__

#include <functional>
#include <string>
#include <cstdio>
#include "Interativo.h"
#include "gl_canvas2d.h"

class Slider : public Interativo
{
    public:
    enum Orientacao { HORIZONTAL, VERTICAL };

    // ----------------------------------------------------------
    //  Intervalo de valores e valor atual
    // ----------------------------------------------------------
    float valorMin, valorMax, valor, passo, raioIndicador;
    Orientacao  orientacao;
    std::string rotulo;
    bool        mostrarValor;
    int         precisaoValor;
    std::function<void(float)> aoMudar;

    //personalização de cores
    struct Cor { float r, g, b; };

    Cor corTrilhaFundo      = { 0.20f, 0.20f, 0.20f }; // trilha não preenchida
    Cor corTrilhaPreenchida = { 0.20f, 0.55f, 0.90f }; // porção preenchida
    Cor corTrilhaDesativada = { 0.40f, 0.40f, 0.40f };
    Cor corIndicadorNormal  = { 0.75f, 0.75f, 0.75f };
    Cor corIndicadorSobMouse= { 1.00f, 1.00f, 1.00f };
    Cor corIndicadorPressio = { 0.50f, 0.50f, 0.50f };
    Cor corIndicadorDesativ = { 0.55f, 0.55f, 0.55f };
    Cor corIndicadorBorda   = { 0.35f, 0.35f, 0.35f };
    Cor corRotulo           = { 0.90f, 0.90f, 0.90f };
    Cor corValor            = { 0.70f, 0.70f, 0.70f };

    //construtor do slider
    Slider(float x, float y, float larg, float alt,
           float minV = 0.0f, float maxV = 1.0f, float initV = 0.5f,
           float passoV = 0.0f, Orientacao orient = HORIZONTAL)
        : Interativo(x, y, larg, alt),
          valorMin(minV), valorMax(maxV),
          valor(limitar(initV, minV, maxV)),
          passo(passoV),
          orientacao(orient),
          rotulo(""),
          mostrarValor(true),
          precisaoValor(1),
          raioIndicador(7.0f),
          arrastando(false),
          deslocamentoArrastePx(0.0f)
    {}

    //renderiza o Slider
     void Render() override
    {
        //desenha o rotulo
        if (!rotulo.empty())
        {
            float alfa = ativo ? 1.0f : 0.55f;
            CV::color(corRotulo.r * alfa,
                      corRotulo.g * alfa,
                      corRotulo.b * alfa);
            CV::text(x, y - 2.0f, rotulo.c_str());
        }

        //calcula extremidades da trilha
        float tx1, ty1, tx2, ty2;
        calcularExtremidadesTrilha(tx1, ty1, tx2, ty2);

        float indX, indY;
        calcularPosIndicador(indX, indY);

        //desenha o fundo da trilha
        Cor corTrilha = ativo ? corTrilhaFundo : corTrilhaDesativada;
        CV::color(corTrilha.r, corTrilha.g, corTrilha.b);

        if (orientacao == HORIZONTAL)
        {
            float altTrilha = 4.0f;
            float yTrilha   = y + altura * 0.5f - altTrilha * 0.5f;
            CV::rectFill(tx1, yTrilha, tx2, yTrilha + altTrilha);
        }
        else
        {
            float largTrilha = 4.0f;
            float xTrilha    = x + largura * 0.5f - largTrilha * 0.5f;
            CV::rectFill(xTrilha, ty1, xTrilha + largTrilha, ty2);
        }

        //desenha a porcao preenchida
        if (ativo)
        {
            CV::color(corTrilhaPreenchida.r,
                      corTrilhaPreenchida.g,
                      corTrilhaPreenchida.b);

            if (orientacao == HORIZONTAL)
            {
                float altTrilha = 4.0f;
                float yTrilha   = y + altura * 0.5f - altTrilha * 0.5f;
                CV::rectFill(tx1, yTrilha, indX, yTrilha + altTrilha);
            }
            else
            {
                // Vertical: preenche do indicador para CIMA (max fica em cima)
                float largTrilha = 4.0f;
                float xTrilha    = x + largura * 0.5f - largTrilha * 0.5f;
                CV::rectFill(xTrilha, indY, xTrilha + largTrilha, ty2);
            }
        }

        //desenha o indicador
        Cor corInd;
        if (!ativo)
            corInd = corIndicadorDesativ;
        else if (pressionado || arrastando)
            corInd = corIndicadorPressio;
        else if (sobMouse)
            corInd = corIndicadorSobMouse;
        else
            corInd = corIndicadorNormal;

        CV::color(corInd.r, corInd.g, corInd.b);
        CV::circleFill(indX, indY, raioIndicador, 16);

        CV::color(corIndicadorBorda.r, corIndicadorBorda.g, corIndicadorBorda.b);
        CV::circle(indX, indY, raioIndicador, 16);

        //desenha o valor numerico
        if (mostrarValor)
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.*f",
                     precisaoValor, (double)valor);

            float alfa = ativo ? 1.0f : 0.55f;
            CV::color(corValor.r * alfa,
                      corValor.g * alfa,
                      corValor.b * alfa);

            if (orientacao == HORIZONTAL)
                CV::text(tx2 + raioIndicador + 4.0f,
                         y + (altura + 13.0f) * 0.5f - 4.0f,
                         buf);
            else
                CV::text(x, ty2 + raioIndicador + 14.0f, buf);
        }
    }

    //tratamento do mouse
    bool TratarMouse(int botao, int estado,
                     int roda, int direcao,
                     int mx, int my) override
    {
        Interativo::TratarMouse(button, state, wheel, direction, mx, my);

        if (!ativo) return false;

        //rolagem da roda do mouse
        if (roda != -2 && direcao != -2 && sobMouse)
        {
            float deslocamento = (passo > 0.0f)
                                    ? passo
                                    : (valorMax - valorMin) * 0.01f;
            setValor(valor + direcao * deslocamento);
            return true;
        }

        //botão esquerdo pressionado
        if (botao == 0 && estado == 0 && sobMouse)
        {
            float indX, indY;
            calcularPosIndicador(indX, indY);

            float dist = (orientacao == HORIZONTAL)
                            ? fabsf((float)mx - indX)
                            : fabsf((float)my - indY);

            if (dist <= raioIndicador * 1.5f)
            {
                //arraste iniciado no indicador — registra deslocamento
                arrastando = true;
                deslocamentoArrastePx = (orientacao == HORIZONTAL)
                                            ? (float)mx - indX
                                            : (float)my - indY;
            }
            else
            {
                //clique na trilha — busca imediata
                buscarMouse(mx, my);
            }
            return true;
        }

        //botao solto
        if (botao == 0 && estado == 1)
        {
            arrastando = false;
            return sobMouse;
        }

        //mouse movido
        if (botao == -2 && arrastando)
        {
            buscarMouse(mx - (int)deslocamentoArrastePx,
                        my - (int)deslocamentoArrastePx);
            return true;
        }

        return false;
    }

    //tratamento do teclado
    bool TratarTeclado(int tecla, bool solto) override
    {
        if (!ativo || solto) return false;

        float deslocamento = (passo > 0.0f)
                                ? passo
                                : (valorMax - valorMin) * 0.01f;

        if (orientacao == HORIZONTAL)
        {
            if (tecla == 100) { setValor(valor - deslocamento); return true; } // Esquerda
            if (tecla == 102) { setValor(valor + deslocamento); return true; } // Direita
        }
        else
        {
            if (tecla == 101) { setValor(valor - deslocamento); return true; } // Baixo
            if (tecla == 103) { setValor(valor + deslocamento); return true; } // Cima
        }
        return false;
    }

    //definicao do valor
    void setValor(float v, bool notificar = true)
    {
        float encaixado = aplicarPasso(limitar(v, valorMin, valorMax));
        if (encaixado == valor) return;
        valor = encaixado;
        if (notificar && aoMudar) aoMudar(valor);
    }

    //alteracado do min/max
    void setIntervalo(float novoMin, float novoMax, bool notificar = false)
    {
        valorMin = novoMin;
        valorMax = novoMax;
        setValor(valor, notificar);
    }

    //normalizacao
    float getNormalizado() const
    {
        if (valorMax == valorMin) return 0.0f;
        return (valor - valorMin) / (valorMax - valorMin);
    }

    //configuraveis fora do slider
    Slider* comRotulo(const std::string& rot)
    {
        rotulo = rot;
        return this;
    }

    Slider* comPasso(float p)
    {
        passo = p;
        return this;
    }

    Slider* comMostrarValor(bool mostrar, int precisao = 1)
    {
        mostrarValor   = mostrar;
        precisaoValor  = precisao;
        return this;
    }

    Slider* comAoMudar(std::function<void(float)> cb)
    {
        aoMudar = cb;
        return this;
    }

    Slider* comRaioIndicador(float r)
    {
        raioIndicador = r;
        return this;
    }

private:

    bool  arrastando;               // true enquanto o usuário arrasta o indicador
    float deslocamentoArrastePx;    // deslocamento em pixels do centro do indicador no início do arraste


    //mantém v dentro de [minVal, maxVal]

    static float limitar(float v, float minVal, float maxVal)
    {
        if (v < minVal) return minVal;
        if (v > maxVal) return maxVal;
        return v;
    }

    //encaixa v no múltiplo mais próximo de passo relativo a valorMin. Sem efeito se passo == 0.

    float aplicarPasso(float v) const
    {
        if (passo <= 0.0f) return v;
        float deslocamento = v - valorMin;
        float encaixado    = roundf(deslocamento / passo) * passo;
        return limitar(valorMin + encaixado, valorMin, valorMax);
    }

    //  Retorna as coordenadas em pixels dos dois extremos da linha da trilha, recuadas pelo raioIndicador para que o
    //  indicador nunca saia da caixa delimitadora do elemento interativo.
    void calcularExtremidadesTrilha(float& x1, float& y1,
                                    float& x2, float& y2) const
    {
        if (orientacao == HORIZONTAL)
        {
            float cy = y + altura * 0.5f;
            x1 = x + raioIndicador;
            y1 = cy;
            x2 = x + largura - raioIndicador;
            y2 = cy;
        }
        else
        {
            float cx = x + largura * 0.5f;
            x1 = cx;
            y1 = y + raioIndicador;
            x2 = cx;
            y2 = y + altura - raioIndicador;
        }
    }


    //mapeia o valor atual para uma posição em pixels na trilha.
    void calcularPosIndicador(float& indX, float& indY) const
    {
        float tx1, ty1, tx2, ty2;
        calcularExtremidadesTrilha(tx1, ty1, tx2, ty2);

        float t = getNormalizado();

        if (orientacao == HORIZONTAL)
        {
            indX = tx1 + t * (tx2 - tx1);
            indY = ty1;
        }
        else
        {
            // Vertical: valorMin embaixo (ty2), valorMax em cima (ty1)
            indX = tx1;
            indY = ty2 - t * (ty2 - ty1);
        }
    }

    //Pega a posicao do mouse
    void buscarMouse(int mx, int my)
    {
        float tx1, ty1, tx2, ty2;
        calcularExtremidadesTrilha(tx1, ty1, tx2, ty2);

        float t;
        if (orientacao == HORIZONTAL)
        {
            float compTrilha = tx2 - tx1;
            if (compTrilha <= 0.0f) return;
            t = ((float)mx - tx1) / compTrilha;
        }
        else
        {
            float compTrilha = ty2 - ty1;
            if (compTrilha <= 0.0f) return;
            // Invertido: baixo = min, cima = max
            t = 1.0f - ((float)my - ty1) / compTrilha;
        }

        setValor(valorMin + limitar(t, 0.0f, 1.0f) * (valorMax - valorMin));
    }

};

#endif
