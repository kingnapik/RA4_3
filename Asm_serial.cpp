// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA4_3

#include "gerador_assembly.h"

// =============================================================================
// SECAO 8: DRIVER SERIAL (USART)
// =============================================================================

void GeradorAssembly::emitirDriverSerial() {
    assembly.push_back("");
    assembly.push_back("; ==========================================================================");
    assembly.push_back("; DRIVER SERIAL USART (9600 bps @ 16MHz)");
    assembly.push_back("; ==========================================================================");

    // USART_Init
    emitLabel("USART_Init");
    emit("ldi", "temp, 103", "UBRR = 103 para 9600 bps @ 16MHz");
    emit("sts", "UBRR0L, temp");
    emit("clr", "temp");
    emit("sts", "UBRR0H, temp");
    emit("ldi", "temp, (1<<TXEN0)", "Habilita transmissor");
    emit("sts", "UCSR0B, temp");
    emit("ldi", "temp, (1<<UCSZ01)|(1<<UCSZ00)", "8 bits, sem paridade, 1 stop");
    emit("sts", "UCSR0C, temp");
    emit("ret", "");

    // USART_Tx
    emitLabel("USART_Tx");
    emitLabel("USART_Tx_Wait");
    emit("lds", "temp2, UCSR0A");
    emit("sbrs", "temp2, UDRE0", "Espera buffer vazio");
    emit("rjmp", "USART_Tx_Wait");
    emit("sts", "UDR0, temp", "Envia byte");
    emit("ret", "");

    // USART_PrintStr
    emitLabel("USART_PrintStr");
    emitLabel("PrintStr_Loop");
    emit("lpm", "temp, Z+", "Le byte da Flash");
    emit("cpi", "temp, 0");
    emit("breq", "PrintStr_Done");
    emit("rcall", "USART_Tx");
    emit("rjmp", "PrintStr_Loop");
    emitLabel("PrintStr_Done");
    emit("ret", "");

    // USART_PrintHexByte
    emitLabel("USART_PrintHexByte");
    emit("push", "temp");
    emit("mov",  "temp2, temp");

    // Nibble alto
    emit("swap", "temp2");
    emit("andi", "temp2, 0x0F");
    emit("rcall", "HexToAscii");
    emit("mov", "temp, temp2");
    emit("rcall", "USART_Tx");

    // Nibble baixo
    emit("pop", "temp");
    emit("andi", "temp, 0x0F");
    emit("mov", "temp2, temp");
    emit("rcall", "HexToAscii");
    emit("mov", "temp, temp2");
    emit("rcall", "USART_Tx");
    emit("ret", "");

    // HexToAscii
    emitLabel("HexToAscii");
    emit("cpi", "temp2, 10");
    emit("brlo", "HexDigit");
    emit("subi", "temp2, -7", "A-F: adiciona 7");
    emitLabel("HexDigit");
    emit("subi", "temp2, -48", "Adiciona '0'");
    emit("ret", "");
}