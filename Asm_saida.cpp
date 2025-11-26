// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA4_3

#include "gerador_assembly.h"

// =============================================================================
// SECAO 6: DUMP DE VARIAVEIS (SAIDA SERIAL)
// =============================================================================

void GeradorAssembly::emitirDumpVars() {
    assembly.push_back("");
    assembly.push_back("; --- ROTINA DE DUMP DE VARIAVEIS ---");
    emitLabel("DUMP_VARS");

    for (const string& var : variaveisGlobais) {
        // Imprime "var = 0x"
        emit("ldi", "r30, lo8(str_" + var + ")");
        emit("ldi", "r31, hi8(str_" + var + ")");
        emit("rcall", "USART_PrintStr");

        // Carrega valor
        emit("lds", "r24, " + var);
        emit("lds", "r25, " + var + "+1");

        // High byte
        emit("mov", "temp, r25");
        emit("rcall", "USART_PrintHexByte");
        // Low byte
        emit("mov", "temp, r24");
        emit("rcall", "USART_PrintHexByte");

        // Quebra de linha
        emit("ldi", "r30, lo8(str_newline)");
        emit("ldi", "r31, hi8(str_newline)");
        emit("rcall", "USART_PrintStr");
    }

    emit("ret", "");
}

// =============================================================================
// SECAO 7: FIM DO PROGRAMA
// =============================================================================

void GeradorAssembly::emitirFimPrograma() {
    assembly.push_back("");
    assembly.push_back("; --- FIM DO PROGRAMA ---");
    emitLabel("FIM_DO_PROGRAMA");
    emit("rjmp", "FIM_DO_PROGRAMA", "Loop infinito");
}