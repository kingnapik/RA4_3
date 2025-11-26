// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA4_3

#include "gerador_assembly.h"

// =============================================================================
// SECAO 4: FUNCAO LOOP (VAZIA) E SETUP
// =============================================================================

void GeradorAssembly::emitirSetupInicio() {
    assembly.push_back("");
    assembly.push_back("; --- FUNCAO LOOP (Arduino) ---");
    emitLabel("loop");
    emit("ret", "", "Loop vazio - codigo roda uma vez no setup");

    assembly.push_back("");
    assembly.push_back("; --- FUNCAO SETUP (Arduino) ---");
    emitLabel("setup");

    // Stack pointer
    emit("ldi", "temp, lo8(RAMEND)", "Inicializa stack pointer");
    emit("out", "_SFR_IO_ADDR(SPL), temp");
    emit("ldi", "temp, hi8(RAMEND)");
    emit("out", "_SFR_IO_ADDR(SPH), temp");

    // UART
    emit("rcall", "USART_Init", "Inicializa comunicacao serial");
    assembly.push_back("");
}