// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#ifndef PARSER_H
#define PARSER_H

#include "gramatica.h"
#include "arvore.h"
#include <string>
#include <vector>

using namespace std;

// Funcoes auxiliares
string mapearTokenParaTerminal(const string& token); //mapeia como " 5 --> num"
string producaoParaString(const Production& prod);

// Funcao principal de parsing
Derivacao* parsear(const vector<string>& _tokens_, const Gramatica& gramatica); //chama parser

#endif