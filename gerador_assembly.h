// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA4_3

#ifndef GERADOR_ASSEMBLY_H
#define GERADOR_ASSEMBLY_H

#include "tac.h"
#include <vector>
#include <string>
#include <set>
#include <cstdint>

using namespace std;

class GeradorAssembly {
private:
    vector<string> assembly;
    set<string> variaveisGlobais;

    // Emissores basicos
    void emit(string inst, string args = "", string comment = "");
    void emitLabel(string label);
    
    // Conversao IEEE 754
    uint16_t floatToHalf(float value);

    // Coleta variaveis do TAC
    void coletarVariaveis(const vector<InstrucaoTAC>& codigoTAC);

    // === SECOES DO ASSEMBLY (arquivos separados) ===
    
    // asm_cabecalho.cpp
    void emitirCabecalho();
    void emitirSecaoDados();
    void emitirSecaoTexto();
    
    // asm_setup.cpp
    void emitirSetupInicio();
    
    // asm_traducao.cpp
    void traduzirTAC(const vector<InstrucaoTAC>& codigoTAC);
    void traduzirIfFalse(const InstrucaoTAC& inst);
    void traduzirAtribuicao(const InstrucaoTAC& inst);
    void traduzirOperacaoBinaria(const InstrucaoTAC& inst);
    void carregarOperando(const string& arg, const string& rL, const string& rH);
    void traduzirComparacao(const string& op);
    
    // asm_saida.cpp
    void emitirDumpVars();
    void emitirFimPrograma();
    
    // asm_serial.cpp
    void emitirDriverSerial();
    
    // asm_float.cpp
    void emitirRotinasFloat();
    void emitirSubtracao();
    void emitirAdicao();
    void emitirMultiplicacao();
    void emitirDivisao();
    void emitirComparacaoFloat();
    void emitirPotencia();

public:
    GeradorAssembly();
    
    // Metodo principal
    void gerarAssembly(const vector<InstrucaoTAC>& codigoTAC, string nomeArquivoSaida);
};

#endif