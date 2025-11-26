// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA4_3

#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>
#include <stack>
#include "arvore.h"

using namespace std;

// Estrutura simples para TAC
struct InstrucaoTAC {
    string op;       // Ex: "+", "IF_FALSE", "GOTO", ":="
    string arg1;     // Ex: "t0", "A"
    string arg2;     // Ex: "t1", "L1"
    string result;   // Ex: "t2", "L2", "A"
    
    InstrucaoTAC(string o = "", string a1 = "", string a2 = "", string r = "") 
        : op(o), arg1(a1), arg2(a2), result(r) {}
};

class GeradorTAC {
private:
    vector<InstrucaoTAC> codigo;
    int contadorTemp;
    int contadorLabel;
    stack<string> pilhaOperandos;
    
    // Auxiliares
    string novoTemp();
    string novoLabel();
    void limparPilha();
    
    // Transforma a recursao "CORPO -> E -> CORPO'" em uma lista plana
    void linearizarCorpo(NoArvore* no, vector<NoArvore*>& lista);
    
    // Processamento
    void processar(NoArvore* no);
    void processarCorpo(NoArvore* no); // Aqui acontece a interpretacao do IF/FOR
    void processarOperacao(NoArvore* no);
    void processarRES(NoArvore* no);
    
    bool ehOperadorBinario(const string& op);
    
public:
    GeradorTAC();
    
    // Funcao principal
    void gerarTAC(NoArvore* arvoreAtribuida);
    
    void imprimirTAC();
    void salvarTAC(const string& nomeArquivo);
    void limpar();
    vector<InstrucaoTAC> obterCodigo() const;
};

#endif