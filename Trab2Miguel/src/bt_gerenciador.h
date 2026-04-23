#ifndef __BT_GERENCIADOR__H__
#define __BT_GERENCIADOR__H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include "Vector2.h"
#include <vector>
#include <algorithm>
#include "Interativo.h"

#define PI_2 6.28318530717
#define PI   3.14159265359

#define Y_CANVAS_CRESCE_PARA_CIMA 1

class GerenciadorUI
{
public:

    // ----------------------------------------------------------
    //  Construtor / Destrutor
    // ----------------------------------------------------------
    GerenciadorUI() {}

    ~GerenciadorUI()
    {
        Limpar();
    }

    // N�o copi�vel � ponteiros brutos com posse n�o devem ser
    // duplicados
    GerenciadorUI(const GerenciadorUI&)             = delete;
    GerenciadorUI& operator=(const GerenciadorUI&)  = delete;

    // ----------------------------------------------------------
    //  Adicionar()
    //  Registra um elemento. GerenciadorUI assume a posse.
    //  Retorna o ponteiro de volta para que o chamador possa
    //  encadear ou armazen�-lo.
    //
    //      Botao* b = gerenciadorUI.Adicionar(new Botao(...));
    //      b->aoClicar = []() { ... };
    // ----------------------------------------------------------
    template<typename T>
    T* Adicionar(T* elemento)
    {
        if (elemento)
            elementos.push_back(elemento);
        return elemento;
    }

    // ----------------------------------------------------------
    //  Remover()
    //  Cancela o registro e DELETA um elemento pelo ponteiro.
    //  Seguro chamar com nullptr ou ponteiro n�o registrado.
    // ----------------------------------------------------------
    void Remover(Interativo* elemento)
    {
        auto it = std::find(elementos.begin(), elementos.end(), elemento);
        if (it != elementos.end())
        {
            delete *it;
            elementos.erase(it);
        }
    }

    // ----------------------------------------------------------
    //  Limpar()
    //  Deleta e remove todos os elementos registrados.
    // ----------------------------------------------------------
    void Limpar()
    {
        for (Interativo* w : elementos)
            delete w;
        elementos.clear();
    }

    // ----------------------------------------------------------
    //  Render()
    //  Desenha todos os elementos vis�veis em ordem de inser��o
    //  (de tr�s para frente). Chame uma vez por frame dentro
    //  de render().
    // ----------------------------------------------------------
    void Render()
    {
        for (Interativo* w : elementos)
        {
            if (w && w->visivel)
                w->Render();
        }
    }

    // ----------------------------------------------------------
    //  TratarMouse()
    //  Despacha um evento de mouse para todos os elementos ativos,
    //  em ordem inversa de inser��o (o do topo primeiro). Para
    //  no primeiro elemento que consumir o evento (retornar true).
    //
    //  Os par�metros espelham exatamente a assinatura de mouse()
    //  do gl_canvas2d � repasse-os diretamente:
    //
    //      void mouse(int button, int state,
    //                 int wheel, int direction, int x, int y)
    //      {
    //          gerenciadorUI.TratarMouse(button, state,
    //                                    wheel, direction, x, y);
    //      }
    // ----------------------------------------------------------
    bool TratarMouse(int botao, int estado,
                     int roda, int direcao,
                     int x, int y)
    {
        for (int i = (int)elementos.size() - 1; i >= 0; --i)
        {
            Interativo* w = elementos[i];
            if (w && w->visivel && w->ativo)
            {
                if (w->TratarMouse(botao, estado, roda, direcao, x, y))
                    return true;
            }
        }
        return false;
    }

    // ----------------------------------------------------------
    //  TratarTeclado()
    //  Despacha um evento de teclado. Se um elemento possui foco
    //  expl�cito (ver DefinirFoco()), apenas esse elemento recebe
    //  o evento. Caso contr�rio, despachado em ordem inversa
    //  at� ser consumido.
    //
    //  solto: false = tecla pressionada, true = tecla solta
    //
    //      void keyboard(int key)   { gerenciadorUI.TratarTeclado(key, false); }
    //      void keyboardUp(int key) { gerenciadorUI.TratarTeclado(key, true);  }
    // ----------------------------------------------------------
    bool TratarTeclado(int tecla, bool solto)
    {
        // elemento com foco recebe acesso exclusivo
        if (elementoEmFoco)
        {
            if (elementoEmFoco->visivel && elementoEmFoco->ativo)
                return elementoEmFoco->TratarTeclado(tecla, solto);
            else
                elementoEmFoco = nullptr; // foco obsoleto � limpa
        }

        // Sem foco: transmite em ordem inversa
        for (int i = (int)elementos.size() - 1; i >= 0; --i)
        {
            Interativo* w = elementos[i];
            if (w && w->visivel && w->ativo)
            {
                if (w->TratarTeclado(tecla, solto))
                    return true;
            }
        }
        return false;
    }

    // ----------------------------------------------------------
    //  DefinirFoco() / LimparFoco()
    //  D� foco de teclado a um elemento espec�fico.
    //  Passe nullptr (ou chame LimparFoco) para liberar o foco.
    // ----------------------------------------------------------
    void DefinirFoco(Interativo* elemento)
    {
        elementoEmFoco = elemento;
    }

    void LimparFoco()
    {
        elementoEmFoco = nullptr;
    }

    Interativo* GetFoco() const
    {
        return elementoEmFoco;
    }

    // ----------------------------------------------------------
    //  DefinirTodosVisiveis() / DefinirTodosAtivos()
    //  Alterna visibilidade ou interatividade de todos os elementos
    //  em massa.
    // ----------------------------------------------------------
    void DefinirTodosVisiveis(bool visivel)
    {
        for (Interativo* w : elementos)
            if (w) w->visivel = visivel;
    }

    void DefinirTodosAtivos(bool ativo)
    {
        for (Interativo* w : elementos)
            if (w) w->ativo = ativo;
    }

    // ----------------------------------------------------------
    //  Helpers de ordem Z
    //  TrarParaFrente() � elemento renderiza por �ltimo (no topo)
    //                     e recebe eventos primeiro
    //  EnviarParaTras() � elemento renderiza primeiro (camada base)
    // ----------------------------------------------------------
    void TrarParaFrente(Interativo* elemento)
    {
        auto it = std::find(elementos.begin(), elementos.end(), elemento);
        if (it != elementos.end() && it != elementos.end() - 1)
            std::rotate(it, it + 1, elementos.end());
    }

    void EnviarParaTras(Interativo* elemento)
    {
        auto it = std::find(elementos.begin(), elementos.end(), elemento);
        if (it != elementos.end() && it != elementos.begin())
            std::rotate(elementos.begin(), it, it + 1);
    }

    // ----------------------------------------------------------
    //  GetWidgetEm()
    //  Retorna o elemento vis�vel mais ao topo cuja caixa
    //  delimitadora cont�m (mx, my), ou nullptr se nenhum.
    //  �til para tooltips ou hit-testing customizado.
    // ----------------------------------------------------------
    Interativo* GetWidgetEm(float mx, float my) const
    {
        for (int i = (int)elementos.size() - 1; i >= 0; --i)
        {
            Interativo* w = elementos[i];
            if (w && w->visivel && w->Colidiu(mx, my))
                return w;
        }
        return nullptr;
    }

    // ----------------------------------------------------------
    //  Quantidade() / Vazio()
    // ----------------------------------------------------------
    size_t Quantidade() const { return elementos.size(); }
    bool   Vazio()      const { return elementos.empty(); }

private:

    std::vector<Interativo*> elementos;               // Todos os elementos registrados, de tr�s para frente
    Interativo*              elementoEmFoco = nullptr; // Elemento com foco de teclado atual
};

#endif // __BT_GERENCIADOR__H__

