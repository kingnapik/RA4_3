// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>
#include <stack>
#include "arvore.h"

using namespace std;

// Estrutura para uma instrucao TAC
struct InstrucaoTAC {
    string op;       // operador: +, -, *, /, %, |, ^, =, <, >, <=, >=, ==, !=, :=, GOTO, IF_FALSE, LABEL, etc
    string arg1;     // primeiro argumento
    string arg2;     // segundo argumento (pode ser vazio)
    string result;   // resultado (pode ser vazio para GOTO, LABEL, etc)
    
    InstrucaoTAC(string o = "", string a1 = "", string a2 = "", string r = "") 
        : op(o), arg1(a1), arg2(a2), result(r) {}
};

// Classe para geracao de TAC
class GeradorTAC {
private:
    vector<InstrucaoTAC> codigo;    // codigo TAC gerado
    int contadorTemp;                // contador para temporarios (t0, t1, t2...)
    int contadorLabel;               // contador para labels (L0, L1, L2...)
    stack<string> pilhaOperandos;   // pilha para processar RPN
    
    // Funcoes auxiliares
    string novoTemp();               // gera novo temporario
    string novoLabel();              // gera novo label
    void limparPilha();              // limpa pilha de operandos
    
    // Processamento da arvore
    void processar(NoArvore* no);
    void processarExpressao(NoArvore* no);
    void processarOperacao(NoArvore* no);
    void processarLiteral(NoArvore* no);
    void processarVariavel(NoArvore* no);
    void processarMEM(NoArvore* no);
    void processarRES(NoArvore* no);
    void processarIF(NoArvore* no);
    void processarFOR(NoArvore* no);
    void processarCorpo(NoArvore* no);
    
    // Helpers para RPN
    bool ehOperador(const string& simbolo);
    bool ehOperadorBinario(const string& op);
    bool ehOperadorRelacional(const string& op);
    string traduzirOperador(const string& op);
    
public:
    GeradorTAC();
    
    // Funcao principal
    void gerarTAC(NoArvore* arvoreAtribuida);
    
    // Saida
    void imprimirTAC();
    void salvarTAC(const string& nomeArquivo);
    vector<InstrucaoTAC> obterCodigo() const;
    
    // Utilitarios
    void limpar();
    int tamanho() const;
};

#endif