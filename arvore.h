// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#ifndef ARVORE_H
#define ARVORE_H

#include <string>
#include <vector>

using namespace std;

struct NoArvore {
    string simbolo;
    vector<NoArvore*> filhos;
    bool ehTerminal;
    string tipoAnotado;
    
    NoArvore(string s, bool terminal = false);
    ~NoArvore();
};

struct Derivacao {
    vector<string> passos;
    NoArvore* raiz;
    bool sucesso;
    string mensagemErro;
    
    Derivacao();
    ~Derivacao();
};

NoArvore* gerarArvore(Derivacao* derivacao);
void imprimirArvore(NoArvore* no, int nivel = 0);
void gerarHTML(NoArvore* raiz, int numeroLinha);
void salvarArvoreMarkdown(NoArvore* raiz, int numeroLinha);

#endif
