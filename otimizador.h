// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#ifndef OTIMIZADOR_H
#define OTIMIZADOR_H

#include "tac.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

// Estrutura para estatisticas de otimizacao
struct EstatisticasOtimizacao {
    int constantFolding;
    int constantPropagation;
    int algebraicSimplification;
    int deadCodeElimination;
    int redundantJumpElimination;
    int totalPassadas;
    int instrucoesOriginais;
    int instrucoesFinais;
    
    EstatisticasOtimizacao() : 
        constantFolding(0), constantPropagation(0), 
        algebraicSimplification(0), deadCodeElimination(0),
        redundantJumpElimination(0), totalPassadas(0),
        instrucoesOriginais(0), instrucoesFinais(0) {}
};

class OtimizadorTAC {
private:
    vector<InstrucaoTAC> codigo;
    vector<InstrucaoTAC> codigoOriginal;
    vector<string> relatorio;
    bool houveOtimizacao;
    EstatisticasOtimizacao stats;

    // Verificadores
    bool ehNumero(const string& s);
    bool ehVariavelTemporaria(const string& s);

    // Passadas de otimizacao
    void constantFolding();
    void constantPropagation();
    void algebraicSimplification();
    void deadCodeElimination();
    void redundantJumpElimination();

    // Utilitario para o relatorio
    void log(const string& msg);
    
    // Helpers para geracao do relatorio MD
    string formatarInstrucaoTAC(const InstrucaoTAC& inst);

public:
    OtimizadorTAC();
    
    // Carrega o codigo gerado
    void carregarCodigo(const vector<InstrucaoTAC>& codigoOriginal);
    
    // Executa o ciclo de otimizacoes
    void otimizar();
    
    // Retorna o codigo otimizado
    vector<InstrucaoTAC> obterCodigo() const;
    
    // Imprime e salva o relatorio
    void imprimirRelatorio();
    void salvarRelatorio(const string& nomeArquivo);
    void salvarTACOtimizado(const string& nomeArquivo);
    
    // Gera relatorio completo em Markdown
    void gerarRelatorioMarkdown(const string& nomeArquivo);
    
    // Retorna estatisticas
    EstatisticasOtimizacao obterEstatisticas() const;
};

#endif