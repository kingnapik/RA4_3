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

// Converte float 32-bit (IEEE754) em half-precision 16-bit (IEEE754)
uint16_t GeradorAssembly::floatToHalf(float value) {
    uint32_t bits;
    static_assert(sizeof(float) == 4, "float deve ter 32 bits");
    std::memcpy(&bits, &value, sizeof(float));

    uint32_t sign = (bits >> 31) & 0x1;
    int32_t exp   = (int32_t)((bits >> 23) & 0xFF) - 127;  // expoente com viés removido
    uint32_t mant = bits & 0x7FFFFF;                       // 23 bits

    // Zero, subnormais e underflow: vira 0 com mesmo sinal
    if (exp <= -15) {
        return static_cast<uint16_t>(sign << 15);
    }

    // Overflow: infinito
    if (exp > 16) {
        return static_cast<uint16_t>((sign << 15) | (0x1F << 10));
    }

    // Normal: reenviesa para half (bias 15) e corta mantissa para 10 bits
    uint16_t hExp  = static_cast<uint16_t>(exp + 15);
    uint16_t hMant = static_cast<uint16_t>(mant >> 13); // 23-10 = 13

    return static_cast<uint16_t>((sign << 15) | (hExp << 10) | hMant);
}

// Retorna "0x3C00"
string GeradorAssembly::hex16(uint16_t val) {
    stringstream ss;
    ss << "0x" << hex << uppercase << setfill('0') << setw(4) << static_cast<int>(val);
    return ss.str();
}

// Helper interno: 8 bits -> "0xFF"
static string hex8(uint8_t val) {
    stringstream ss;
    ss << "0x" << hex << uppercase << setfill('0') << setw(2) << static_cast<int>(val);
    return ss.str();
}

// --- EMISSORES BÁSICOS ---

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
        auto check = [&](const string& s) {
            if (s.empty()) return;
            if (isdigit(static_cast<unsigned char>(s[0])) || s[0] == '-') return;
            // Ignora rótulos em LABEL/GOTO/IF_FALSE
            if (inst.op == "LABEL" || inst.op == "GOTO" || inst.op == "IF_FALSE") return;
            variaveisGlobais.insert(s);
        };

        check(inst.result);
        check(inst.arg1);
        check(inst.arg2);
    }
}

void GeradorAssembly::gerarAssembly(const vector<InstrucaoTAC>& codigoTAC,
                                    string nomeArquivoSaida) {
    assembly.clear();
    coletarVariaveis(codigoTAC);

    // ---------------------------------------------------------------------
    // 1. Cabeçalho
    // ---------------------------------------------------------------------
    assembly.push_back("; --- CODIGO AVR ASSEMBLY GERADO ---");
    assembly.push_back("#define __SFR_OFFSET 0");
    assembly.push_back("#include <avr/io.h>");
    assembly.push_back("");
    
    // Defines de registradores auxiliares
    assembly.push_back("#define temp r16");
    assembly.push_back("#define temp2 r17");
    assembly.push_back("");

    // Exporta símbolos de entrada
    assembly.push_back(".global main");      // para bare-metal
    assembly.push_back(".global _Z5setupv"); // void setup()
    assembly.push_back(".global _Z4loopv");  // void loop()

    // ---------------------------------------------------------------------
    // 2. Área de dados (.data) – variáveis em RAM
    // ---------------------------------------------------------------------
    assembly.push_back(".section .data");
    for (const string& var : variaveisGlobais) {
        assembly.push_back(var + ": .skip 2");
    }

    // ---------------------------------------------------------------------
    // 3. Área de código (.text) + strings em Flash
    // ---------------------------------------------------------------------
    assembly.push_back("");
    assembly.push_back(".section .text");
    assembly.push_back("");

    for (const string& var : variaveisGlobais) {
        assembly.push_back("str_" + var + ": .asciz \"" + var + " = 0x\"");
    }
    assembly.push_back("str_newline: .asciz \"\\r\\n\"");

    // ---------------------------------------------------------------------
    // 4. main – prólogo e inicialização
    // ---------------------------------------------------------------------
    assembly.push_back("");

    // setup() do Arduino: símbolo mangled _Z5setupv
    emitLabel("_Z5setupv");
    emit("rjmp", "main");

    // loop() do Arduino: símbolo mangled _Z4loopv, laço infinito
    emitLabel("_Z4loopv");
    emit("rjmp", "_Z4loopv");

    // main "real" do seu código (para bare-metal ou para ser chamado por setup)
    assembly.push_back("");
    emitLabel("main");

    // Stack pointer
    emit("ldi", "temp, lo8(RAMEND)");
    emit("out", "_SFR_IO_ADDR(SPL), temp");
    emit("ldi", "temp, hi8(RAMEND)");
    emit("out", "_SFR_IO_ADDR(SPH), temp");

    // UART
    emit("rcall", "USART_Init");
    assembly.push_back("");

    // ---------------------------------------------------------------------
    // 5. Tradução TAC -> ASM
    // ---------------------------------------------------------------------
    for (const auto& inst : codigoTAC) {
        assembly.push_back("; " + inst.result + " = " +
                           inst.arg1 + " " + inst.op + " " + inst.arg2);

        // Rótulo
        if (inst.op == "LABEL") {
            emitLabel(inst.result);
            continue;
        }

        // GOTO incondicional
        if (inst.op == "GOTO") {
            emit("rjmp", inst.result);
            continue;
        }

        // IF_FALSE t  goto Lx
        if (inst.op == "IF_FALSE") {
            emit("lds", "r24, " + inst.arg1);
            emit("lds", "r25, " + inst.arg1 + "+1");
            emit("or",  "r24, r25");
            emit("breq", inst.result);
            continue;
        }

        // Atribuição simples: x := y ou x := literal
        if (inst.op == ":=") {
            bool literal = false;
            if (!inst.arg1.empty()) {
                char c = inst.arg1[0];
                if (isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+')
                    literal = true;
            }

            if (literal) {
                float f = std::stof(inst.arg1);
                uint16_t hf = floatToHalf(f);
                emit("ldi", "r24, " + hex8(static_cast<uint8_t>(hf & 0xFF)));
                emit("ldi", "r25, " + hex8(static_cast<uint8_t>((hf >> 8) & 0xFF)));
            } else {
                emit("lds", "r24, " + inst.arg1);
                emit("lds", "r25, " + inst.arg1 + "+1");
            }

            emit("sts", inst.result + ", r24");
            emit("sts", inst.result + "+1, r25");
            continue;
        }

        // Operações binárias e comparações
        auto loadArg = [&](const string& arg, const string& rL, const string& rH) {
            bool literal = false;
            if (!arg.empty()) {
                char c = arg[0];
                if (isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+')
                    literal = true;
            }

            if (literal) {
                float f = std::stof(arg);
                uint16_t hf = floatToHalf(f);
                emit("ldi", rL + ", " + hex8(static_cast<uint8_t>(hf & 0xFF)));
                emit("ldi", rH + ", " + hex8(static_cast<uint8_t>((hf >> 8) & 0xFF)));
            } else {
                emit("lds", rL + ", " + arg);
                emit("lds", rH + ", " + arg + "+1");
            }
        };

        loadArg(inst.arg1, "r24", "r25");
        loadArg(inst.arg2, "r22", "r23");

        string rotina;

        if (inst.op == "+")       rotina = "__addhf3";
        else if (inst.op == "-")  rotina = "__subhf3";
        else if (inst.op == "*")  rotina = "__mulhf3";
        else if (inst.op == "/" || inst.op == "|") rotina = "__divhf3";
        else if (inst.op == "^")  rotina = "__powhf3";
        else if (inst.op == "<"  || inst.op == ">"  ||
                 inst.op == "<=" || inst.op == ">=" ||
                 inst.op == "==" || inst.op == "!=")
            rotina = "__cmphf2";

        if (!rotina.empty()) {
            emit("rcall", rotina);
        }

        // Resultado em r24:r25
        emit("sts", inst.result + ", r24");
        emit("sts", inst.result + "+1, r25");
    }

    // Ao final do TAC, imprime variáveis e trava no fim
    emit("rcall", "DUMP_VARS");
    emit("rjmp", "FIM_DO_PROGRAMA");

    // ---------------------------------------------------------------------
    // 6. Rotina DUMP_VARS – imprime todas as variáveis em hexa
    // ---------------------------------------------------------------------
    assembly.push_back("");
    assembly.push_back("; --- DUMP RESULTADOS ---");
    emitLabel("DUMP_VARS");

    for (const string& var : variaveisGlobais) {
        // imprime "var = 0x"
        emit("ldi", "r30, lo8(str_" + var + ")");
        emit("ldi", "r31, hi8(str_" + var + ")");
        emit("rcall", "USART_PrintStr");

        // carrega valor
        emit("lds", "r24, " + var);
        emit("lds", "r25, " + var + "+1");

        // high byte
        emit("mov", "temp, r25");
        emit("rcall", "USART_PrintHexByte");
        // low byte
        emit("mov", "temp, r24");
        emit("rcall", "USART_PrintHexByte");

        // quebra de linha
        emit("ldi", "r30, lo8(str_newline)");
        emit("ldi", "r31, hi8(str_newline)");
        emit("rcall", "USART_PrintStr");
    }

    emit("ret", "");

    // ---------------------------------------------------------------------
    // 7. Fim do programa: laço infinito
    // ---------------------------------------------------------------------
    emitLabel("FIM_DO_PROGRAMA");
    emit("rjmp", "FIM_DO_PROGRAMA");

    // ---------------------------------------------------------------------
    // 8. DRIVER SERIAL – implementações usadas acima
    // ---------------------------------------------------------------------
    assembly.push_back("");
    assembly.push_back("; --- DRIVER SERIAL ---");

    // USART_Init
    emitLabel("USART_Init");
    emit("ldi", "temp, 103");             // 9600 bps @16MHz
    emit("sts", "UBRR0L, temp");
    emit("clr", "temp");
    emit("sts", "UBRR0H, temp");
    emit("ldi", "temp, (1<<TXEN0)");      // habilita TX
    emit("sts", "UCSR0B, temp");
    emit("ldi", "temp, (1<<UCSZ01)|(1<<UCSZ00)"); // 8N1
    emit("sts", "UCSR0C, temp");
    emit("ret", "");

    // USART_Tx
    emitLabel("USART_Tx");
    emitLabel("USART_Tx_Wait");
    emit("lds", "temp2, UCSR0A");
    emit("sbrs", "temp2, UDRE0");
    emit("rjmp", "USART_Tx_Wait");
    emit("sts", "UDR0, temp");
    emit("ret", "");

    // USART_PrintStr – Z aponta para string em Flash
    emitLabel("USART_PrintStr");
    emitLabel("PrintStr_Loop");
    emit("lpm", "temp, Z+");
    emit("cpi", "temp, 0");
    emit("breq", "PrintStr_Done");
    emit("rcall", "USART_Tx");
    emit("rjmp", "PrintStr_Loop");
    emitLabel("PrintStr_Done");
    emit("ret", "");

    // USART_PrintHexByte – imprime temp em hexa
    emitLabel("USART_PrintHexByte");
    emit("push", "temp");          // salva original
    emit("mov",  "temp2, temp");

    // nibble alto
    emit("swap", "temp2");
    emit("andi", "temp2, 0x0F");
    emit("rcall", "HexToAscii");
    emit("mov", "temp, temp2");
    emit("rcall", "USART_Tx");

    // nibble baixo
    emit("pop", "temp");           // recupera original
    emit("andi", "temp, 0x0F");
    emit("mov", "temp2, temp");
    emit("rcall", "HexToAscii");
    emit("mov", "temp, temp2");
    emit("rcall", "USART_Tx");
    emit("ret", "");

    // HexToAscii – converte 0..15 em '0'..'9','A'..'F' em temp2
    emitLabel("HexToAscii");
    emit("cpi", "temp2, 10");
    emit("brlo", "HexDigit");
    emit("subi", "temp2, -7");     // A-F: +7 em relação a '9'
    emitLabel("HexDigit");
    emit("subi", "temp2, -48");    // + '0'
    emit("ret", "");

    // ---------------------------------------------------------------------
    // 9. STUBS de ponto flutuante 16-bit
    //    (para compilar sem linkar nenhuma lib externa)
    // ---------------------------------------------------------------------
    assembly.push_back("");
    assembly.push_back("; --- MATH STUBS 16-BIT (PLACEHOLDER) ---");
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

    // ---------------------------------------------------------------------
    // 10. Grava arquivo
    // ---------------------------------------------------------------------
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
