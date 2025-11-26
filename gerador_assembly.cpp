// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "gerador_assembly.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>

using namespace std;

GeradorAssembly::GeradorAssembly() {}

// =============================================================================
// EMISSORES BASICOS
// =============================================================================

void GeradorAssembly::emit(string inst, string args, string comment) {
    stringstream ss;
    ss << "    " << left << setw(8) << inst << left << setw(20) << args;
    if (!comment.empty()) ss << "; " << comment;
    assembly.push_back(ss.str());
}

void GeradorAssembly::emitLabel(string label) {
    assembly.push_back(label + ":");
}

// =============================================================================
// CONVERSAO IEEE 754 FLOAT -> HALF-PRECISION
// =============================================================================

uint16_t GeradorAssembly::floatToHalf(float value) {
    uint32_t bits;
    static_assert(sizeof(float) == 4, "float deve ter 32 bits");
    std::memcpy(&bits, &value, sizeof(float));

    uint32_t sign = (bits >> 31) & 0x1;
    int32_t exp   = (int32_t)((bits >> 23) & 0xFF) - 127;
    uint32_t mant = bits & 0x7FFFFF;

    // Zero, subnormais e underflow
    if (exp <= -15) {
        return static_cast<uint16_t>(sign << 15);
    }

    // Overflow: infinito
    if (exp > 16) {
        return static_cast<uint16_t>((sign << 15) | (0x1F << 10));
    }

    // Normal: reenviesa para half (bias 15) e corta mantissa para 10 bits
    uint16_t hExp  = static_cast<uint16_t>(exp + 15);
    uint16_t hMant = static_cast<uint16_t>(mant >> 13);

    return static_cast<uint16_t>((sign << 15) | (hExp << 10) | hMant);
}

// =============================================================================
// COLETA DE VARIAVEIS
// =============================================================================

void GeradorAssembly::coletarVariaveis(const vector<InstrucaoTAC>& codigoTAC) {
    variaveisGlobais.clear();

    for (const auto& inst : codigoTAC) {
        auto check = [&](const string& s) {
            if (s.empty()) return;
            if (isdigit(static_cast<unsigned char>(s[0])) || s[0] == '-') return;
            if (inst.op == "LABEL" || inst.op == "GOTO" || inst.op == "IF_FALSE") return;
            variaveisGlobais.insert(s);
        };

        check(inst.result);
        check(inst.arg1);
        check(inst.arg2);
    }
}

// =============================================================================
// FUNCAO PRINCIPAL: ORQUESTRA A GERACAO
// =============================================================================

void GeradorAssembly::gerarAssembly(const vector<InstrucaoTAC>& codigoTAC,
                                    string nomeArquivoSaida) {
    assembly.clear();
    coletarVariaveis(codigoTAC);

    // 1. Cabecalho e secoes de dados
    emitirCabecalho();
    emitirSecaoDados();
    emitirSecaoTexto();
    
    // 2. Setup do Arduino
    emitirSetupInicio();
    
    // 3. Traducao do codigo TAC
    traduzirTAC(codigoTAC);
    
    // 4. Finaliza setup: chama dump e trava
    emit("rcall", "DUMP_VARS", "Imprime resultados");
    emit("rjmp", "FIM_DO_PROGRAMA");
    
    // 5. Rotinas de saida
    emitirDumpVars();
    emitirFimPrograma();
    
    // 6. Driver serial
    emitirDriverSerial();
    
    // 7. Rotinas de ponto flutuante
    emitirRotinasFloat();

    // 8. Grava arquivo
    ofstream file(nomeArquivoSaida);
    if (!file.is_open()) {
        cerr << "Erro ao criar arquivo de Assembly: " << nomeArquivoSaida << endl;
        return;
    }

    for (const auto& linha : assembly) {
        file << linha << '\n';
    }

    file.close();
    cout << "Assembly AVR gerado em: " << nomeArquivoSaida << endl;
}