#ifndef GERADOR_ASSEMBLY_H
#define GERADOR_ASSEMBLY_H

#include "tac.h"
#include <vector>
#include <string>
#include <set>
#include <map>
#include <cstdint>

using namespace std;

class GeradorAssembly {
private:
    vector<string> assembly;      // Buffer de linhas do código gerado
    set<string> variaveisGlobais; // Armazena nomes de variáveis para alocar na RAM (.dseg)

    // --- MAPEAMENTO DE REGISTRADORES (AVR GCC Standard) ---
    // Usaremos R25:R24 para o Operando 1 e Resultado
    // Usaremos R23:R22 para o Operando 2
    
    // Funções Auxiliares
    void emit(string inst, string args = "", string comment = "");
    void emitLabel(string label);
    
    // Converte float C++ para representação binária IEEE 754 Half-Precision (16-bit)
    uint16_t floatToHalf(float value);
    string hex16(uint16_t val); // Retorna "0x3C00"

    // Analisa o TAC para descobrir quais variáveis precisam de memória
    void coletarVariaveis(const vector<InstrucaoTAC>& codigoTAC);

public:
    GeradorAssembly();
    
    // Método principal
    void gerarAssembly(const vector<InstrucaoTAC>& codigoTAC, string nomeArquivoSaida);
};

#endif