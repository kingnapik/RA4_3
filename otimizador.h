#ifndef OTIMIZADOR_H
#define OTIMIZADOR_H

#include "tac.h" // Precisa conhecer a estrutura InstrucaoTAC
#include <vector>
#include <string>
#include <map>

using namespace std;

class OtimizadorTAC {
private:
    vector<InstrucaoTAC> codigo;
    vector<string> relatorio; // Armazena o log das alterações
    bool houveOtimizacao;     // Flag para repetir passadas

    // Verificadores
    bool ehNumero(const string& s);
    bool ehVariavelTemporaria(const string& s);

    // Passadas de otimização
    void constantFolding();       // Calcula 2 + 2 -> 4
    void constantPropagation();   // Substitui t0 por 5 se t0 = 5
    void algebraicSimplification(); // x + 0 -> x
    void deadCodeElimination();   // Remove t0 = ... se t0 não for usado

    // Utilitário para o relatório
    void log(const string& msg);

public:
    OtimizadorTAC();
    
    // Carrega o código gerado pelo Aluno 1
    void carregarCodigo(const vector<InstrucaoTAC>& codigoOriginal);
    
    // Executa o ciclo de otimizações
    void otimizar();
    
    // Retorna o código otimizado
    vector<InstrucaoTAC> obterCodigo() const;
    
    // Imprime e salva o relatório
    void imprimirRelatorio();
    void salvarRelatorio(const string& nomeArquivo);
    void salvarTACOtimizado(const string& nomeArquivo);
};

#endif