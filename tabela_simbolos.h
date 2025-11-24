// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

// simbolo de variavel/memoria
struct Simbolo {
    string nome;         // ex: A, B, CONTADOR
    string tipo;         // "int", "real", "booleano"
    bool   inicializada; // foi inicializada?
    int    linhaDeclaracao;

    Simbolo() : nome(""), tipo("int"), inicializada(false), linhaDeclaracao(-1) {}
    Simbolo(string n, string t, bool ini, int ln)
        : nome(n), tipo(t), inicializada(ini), linhaDeclaracao(ln) {}
};

// resultado por linha (para RES n)
struct ResultadoLinha {
    string tipo; // "int", "real", "booleano"
    int    linha;
    bool   valido;

    ResultadoLinha() : tipo("int"), linha(-1), valido(false) {}
    ResultadoLinha(string t, int l, bool v) : tipo(t), linha(l), valido(v) {}
};

class TabelaSimbolos {
private:
    map<string, Simbolo> simbolos;
    // resultados por numero de linha (0..N-1)
    vector<ResultadoLinha> resultadosLinhas;

public:
    TabelaSimbolos();

    // variaveis/memorias
    void adicionarSimbolo(string nome, string tipo, int linha);
    bool existeSimbolo(string nome) const;
    Simbolo buscarSimbolo(string nome) const; // retorna copia; se nao existir, retorna default
    void atualizarSimbolo(string nome, bool inicializada);
    bool simboloInicializado(string nome) const;

    // RES n
    void adicionarResultadoLinha(string tipo, int linha, bool valido);
    ResultadoLinha buscarResultadoLinha(int n) const; // se nao existir -> default (valido=false)
    bool existeResultadoLinha(int n) const;

    // utilitarios
    void limpar();
    int  quantidadeSimbolos() const;
    void imprimirTabela() const;
};

#endif
