// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#ifndef GRAMATICA_H
#define GRAMATICA_H

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

extern const string EPSILON; //carac pro e de vazio
extern const string END_MARKER; // final

using Production = vector<string>; //1 regra
using Grammar = map<string, vector<Production>>; //todas

struct Gramatica {
    Grammar regras; //todas regras
    set<string> naoTerminais; //n-terminais
    set<string> terminais; //terminais
    set<string> nullable; //quais derivam em null
    map<string, set<string>> first; //FIRST
    map<string, set<string>> follow; //FOLLOW
    map<pair<string, string>, Production> tabelaLL1; //tabela
    string simboloInicial; //comeco com o P
};

// Funcoes principais
Gramatica construirGramatica();
void salvarAnaliseGramatica(const Gramatica& g);

#endif