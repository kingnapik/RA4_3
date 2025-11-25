#include "gerador_assembly.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <cstring>
#include <cstdint>

using namespace std;

GeradorAssembly::GeradorAssembly() {}

// --- CONVERSÃO IEEE 754 16-BIT ---
uint16_t GeradorAssembly::floatToHalf(float value) {
    uint32_t f32;
    std::memcpy(&f32, &value, sizeof(float));
    
    uint16_t sign = (f32 >> 16) & 0x8000;
    int16_t exponent = ((f32 >> 23) & 0xFF) - 127;
    uint32_t mantissa = f32 & 0x7FFFFF;

    int16_t new_exp = exponent + 15;
    if (new_exp <= 0) return sign; 
    if (new_exp >= 31) return sign | 0x7C00; 

    uint16_t new_mant = (mantissa >> 13) & 0x3FF;
    return sign | (new_exp << 10) | new_mant;
}

// implementação pedida no .h
string GeradorAssembly::hex16(uint16_t val) {
    stringstream ss;
    ss << "0x" << hex << uppercase << setfill('0') << setw(4) << (int)val;
    return ss.str();
}

// Helper para gerar Hex de 8 bits direto (0xFF)
string hex8(uint8_t val) {
    stringstream ss;
    ss << "0x" << hex << uppercase << setfill('0') << setw(2) << (int)val;
    return ss.str();
}

// --- EMISSORES ---
void GeradorAssembly::emit(string inst, string args, string comment) {
    stringstream ss;
    ss << "    " << left << setw(8) << inst << left << setw(20) << args;
    if (!comment.empty()) ss << "; " << comment;
    assembly.push_back(ss.str());
}

void GeradorAssembly::emitLabel(string label) {
    assembly.push_back(label + ":");
}

void GeradorAssembly::coletarVariaveis(const vector<InstrucaoTAC>& codigoTAC) {
    variaveisGlobais.clear();
    for (const auto& inst : codigoTAC) {
        auto check = [&](string s) {
            if (!s.empty() && !isdigit(s[0]) && s[0] != '-' && 
                inst.op != "LABEL" && inst.op != "GOTO" && inst.op != "IF_FALSE") {
                variaveisGlobais.insert(s);
            }
        };
        check(inst.result);
        if(isalpha(inst.arg1[0])) check(inst.arg1);
        if(isalpha(inst.arg2[0])) check(inst.arg2);
    }
}

void GeradorAssembly::gerarAssembly(const vector<InstrucaoTAC>& codigoTAC, string nomeArquivoSaida) {
    assembly.clear();
    coletarVariaveis(codigoTAC);

    // 1. CABEÇALHO
    assembly.push_back("; --- CODIGO AVR ASSEMBLY GERADO ---");
    assembly.push_back("#define __SFR_OFFSET 0");
    assembly.push_back("#include <avr/io.h>");
    assembly.push_back("");
    
    assembly.push_back("#define temp r16");
    assembly.push_back("#define temp2 r17");
    assembly.push_back("");
    assembly.push_back(".global main");
    
    // 2. RAM (Variáveis em .data)
    assembly.push_back(".section .data");
    for (const string& var : variaveisGlobais) {
        assembly.push_back(var + string(": .skip 2"));
    }
    assembly.push_back("");

    // Strings para Debug
    assembly.push_back("");
    for (const string& var : variaveisGlobais) {
        // GCC usa .asciz para strings com null terminator
        assembly.push_back("str_" + var + ": .asciz \"" + var + " = 0x\"");
    }
    assembly.push_back("str_newline: .asciz \"\\r\\n\"");

    // 4. MAIN
    assembly.push_back("");
    emitLabel("main");
    
    // Inicialização de Stack (Copiado do seu main43.cpp)
    // Usando _SFR_IO_ADDR para garantir compatibilidade
    emit("ldi", "temp, lo8(RAMEND)");
    emit("out", "_SFR_IO_ADDR(SPL), temp");
    emit("ldi", "temp, hi8(RAMEND)");
    emit("out", "_SFR_IO_ADDR(SPH), temp");
    
    // Inicializa UART
    emit("rcall", "USART_Init");
    assembly.push_back("");

    // 5. TRADUÇÃO TAC -> ASM
    for (const auto& inst : codigoTAC) {
        assembly.push_back("; " + inst.result + " = " + inst.arg1 + " " + inst.op + " " + inst.arg2);

        if (inst.op == "LABEL") {
            emitLabel(inst.result);
        }
        else if (inst.op == "GOTO") {
            emit("rjmp", inst.result);
        }
        else if (inst.op == "IF_FALSE") {
            emit("lds", "r24, " + inst.arg1);
            emit("lds", "r25, " + inst.arg1 + "+1");
            emit("or",  "r24, r25");
            emit("breq", inst.result);
        }
        else if (inst.op == ":=") {
            // Atribuição (literal ou variável)
            if (!inst.arg1.empty() && (isdigit(inst.arg1[0]) || inst.arg1[0] == '-')) {
                uint16_t hf = floatToHalf(stof(inst.arg1));
                emit("ldi", "r24, " + hex8(hf & 0xFF));
                emit("ldi", "r25, " + hex8((hf >> 8) & 0xFF));
            } else {
                emit("lds", "r24, " + inst.arg1);
                emit("lds", "r25, " + inst.arg1 + "+1");
            }
            // CORREÇÃO: incluir registrador como segundo operando de STS
            emit("sts", inst.result + ", r24");
            emit("sts", inst.result + "+1, r25");
        }
        else {
            // Operações
            auto loadArg = [&](string arg, string rL, string rH) {
                if (!arg.empty() && (isdigit(arg[0]) || arg[0] == '-')) {
                    uint16_t hf = floatToHalf(stof(arg));
                    emit("ldi", rL + ", " + hex8(hf & 0xFF));
                    emit("ldi", rH + ", " + hex8((hf >> 8) & 0xFF));
                } else {
                    emit("lds", rL + ", " + arg);
                    emit("lds", rH + ", " + arg + "+1");
                }
            };

            loadArg(inst.arg1, "r24", "r25");
            loadArg(inst.arg2, "r22", "r23");

            string rotina = "";
            if (inst.op == "+")      rotina = "__addhf3";
            else if (inst.op == "-") rotina = "__subhf3";
            else if (inst.op == "*") rotina = "__mulhf3";
            else if (inst.op == "/") rotina = "__divhf3";
            else if (inst.op == "|") rotina = "__divhf3";
            else if (inst.op == "^") rotina = "__powhf3";
            else if (inst.op == "<" || inst.op == ">" || inst.op == "<=" ||
                     inst.op == ">=" || inst.op == "==" || inst.op == "!=")
                rotina = "__cmphf2";
            
            if (!rotina.empty()) emit("rcall", rotina);

            // CORREÇÃO: incluir registrador como segundo operando de STS
            emit("sts", inst.result + ", r24");
            emit("sts", inst.result + "+1, r25");
        }
    }

    // 6. DUMP DE RESULTADOS
    assembly.push_back("");
    assembly.push_back("; --- DUMP RESULTADOS ---");
    // Aqui assume que o TAC gera um rótulo de saída (ex: L1) e o fluxo cai no DUMP
    // diretamente após esse rótulo, como no codigo.S que você mandou.
    // Se quiser forçar sempre, pode-se inserir um "rjmp" para o primeiro label aqui.

    // Imprime todas as variáveis em ordem
    for (const string& var : variaveisGlobais) {
        emit("ldi", "r30, lo8(str_" + var + ")");
        emit("ldi", "r31, hi8(str_" + var + ")");
        emit("rcall", "USART_PrintStr");
        emit("lds", "r24, " + var);
        emit("lds", "r25, " + var + "+1");
        emit("mov", "temp, r25");
        emit("rcall", "USART_PrintHexByte");
        emit("mov", "temp, r24");
        emit("rcall", "USART_PrintHexByte");
        emit("ldi", "r30, lo8(str_newline)");
        emit("ldi", "r31, hi8(str_newline)");
        emit("rcall", "USART_PrintStr");
    }

    emitLabel("FIM_DO_PROGRAMA");
    emit("rjmp", "FIM_DO_PROGRAMA");

    // 7. DRIVER SERIAL (mesmo estilo do seu código antigo)
    assembly.push_back("");
    assembly.push_back("; --- DRIVER SERIAL ---");
    
    emitLabel("USART_Init");
    emit("ldi", "temp, 103");
    emit("sts", "UBRR0L, temp");
    emit("clr", "temp");
    emit("sts", "UBRR0H, temp");
    emit("ldi", "temp, (1<<TXEN0)");
    emit("sts", "UCSR0B, temp");
    emit("ldi", "temp, (1<<UCSZ01)|(1<<UCSZ00)");
    emit("sts", "UCSR0C, temp");
    emit("ret", "");

    emitLabel("USART_Tx");
    emitLabel("USART_Tx_Wait");
    emit("lds", "temp2, UCSR0A");
    emit("sbrs", "temp2, UDRE0");
    emit("rjmp", "USART_Tx_Wait");
    emit("sts", "UDR0, temp");
    emit("ret", "");

    emitLabel("USART_PrintStr");
    emitLabel("PrintStr_Loop");
    emit("lpm", "temp, Z+");
    emit("cpi", "temp, 0");
    emit("breq", "PrintStr_Done");
    emit("rcall", "USART_Tx");
    emit("rjmp", "PrintStr_Loop");
    emitLabel("PrintStr_Done");
    emit("ret", "");

    emitLabel("USART_PrintHexByte");
    emit("push", "temp"); // Salva original
    emit("mov", "temp2, temp");
    // Parte Alta
    emit("swap", "temp2");
    emit("andi", "temp2, 0x0F");
    emit("rcall", "HexToAscii");
    emit("mov", "temp, temp2");
    emit("rcall", "USART_Tx");
    // Parte Baixa
    emit("pop", "temp"); // Recupera original
    emit("andi", "temp, 0x0F");
    emit("mov", "temp2, temp"); // Otimizacao
    emit("rcall", "HexToAscii");
    emit("mov", "temp, temp2");
    emit("rcall", "USART_Tx");
    emit("ret", "");

    emitLabel("HexToAscii");
    emit("cpi", "temp2, 10");
    emit("brlo", "HexDigit");
    emit("subi", "temp2, -7"); // A-F
    emitLabel("HexDigit");
    emit("subi", "temp2, -48"); // 0-9
    emit("ret", "");

    // STUBS (Mantidos vazios pois a biblioteca float é complexa)
    assembly.push_back("");
    assembly.push_back("; --- MATH STUBS ---");
    auto gerarStub = [&](const string& nome) {
        emitLabel(nome);
        emit("ret", "");
    };
    gerarStub("__addhf3");
    gerarStub("__subhf3");
    gerarStub("__mulhf3");
    gerarStub("__divhf3");
    gerarStub("__powhf3");
    gerarStub("__cmphf2");

    ofstream file(nomeArquivoSaida);
    for (const auto& linha : assembly) {
        file << linha << endl;
    }
    file.close();
    cout << "Assembly AVR Gerado com Sucesso: " << nomeArquivoSaida << endl;
}
