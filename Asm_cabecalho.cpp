// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA4_3

#include "gerador_assembly.h"

// =============================================================================
// SECAO 1: CABECALHO DO ASSEMBLY
// =============================================================================

void GeradorAssembly::emitirCabecalho() {
    assembly.push_back("; --- CODIGO AVR ASSEMBLY GERADO ---");
    assembly.push_back("; Compilador RPN -> AVR Assembly");
    assembly.push_back("; Arquitetura: ATmega328P (Arduino Uno)");
    assembly.push_back("; Ponto Flutuante: IEEE 754 Half-Precision (16-bit)");
    assembly.push_back("");
    assembly.push_back("#define __SFR_OFFSET 0");
    assembly.push_back("#include <avr/io.h>");
    assembly.push_back("");
    assembly.push_back("; Registradores auxiliares");
    assembly.push_back("#define temp r16");
    assembly.push_back("#define temp2 r17");
    assembly.push_back("");
    assembly.push_back("; Simbolos exportados para o linker");
    assembly.push_back(".global setup");
    assembly.push_back(".global loop");
}

// =============================================================================
// SECAO 2: AREA DE DADOS (.data)
// =============================================================================

void GeradorAssembly::emitirSecaoDados() {
    assembly.push_back("");
    assembly.push_back("; --- SECAO DE DADOS (RAM) ---");
    assembly.push_back(".section .data");
    
    for (const string& var : variaveisGlobais) {
        assembly.push_back(var + ": .skip 2");
    }
}

// =============================================================================
// SECAO 3: AREA DE TEXTO E STRINGS (.text)
// =============================================================================

void GeradorAssembly::emitirSecaoTexto() {
    assembly.push_back("");
    assembly.push_back("; --- SECAO DE CODIGO (.text) ---");
    assembly.push_back(".section .text");
    assembly.push_back("");
    assembly.push_back("; Strings para output serial");
    
    for (const string& var : variaveisGlobais) {
        assembly.push_back("str_" + var + ": .asciz \"" + var + " = 0x\"");
    }
    assembly.push_back("str_newline: .asciz \"\\r\\n\"");
}