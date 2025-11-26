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
    int32_t exp   = (int32_t)((bits >> 23) & 0xFF) - 127;  // expoente com viÃ©s removido
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

// --- EMISSORES BÃSICOS ---

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
            // Ignora rÃ³tulos em LABEL/GOTO/IF_FALSE
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
    // 1. CabeÃ§alho
    // ---------------------------------------------------------------------
    assembly.push_back("; --- CODIGO AVR ASSEMBLY GERADO ---");
    assembly.push_back("#define __SFR_OFFSET 0");
    assembly.push_back("#include <avr/io.h>");
    assembly.push_back("");
    
    // Defines de registradores auxiliares
    assembly.push_back("#define temp r16");
    assembly.push_back("#define temp2 r17");
    assembly.push_back("");

    // Exporta sÃ­mbolos de entrada
    //assembly.push_back(".global main");      // para bare-metal
    assembly.push_back(".global setup"); // void setup()
    assembly.push_back(".global loop");  // void loop()

    // ---------------------------------------------------------------------
    // 2. Ãrea de dados (.data) â€“ variÃ¡veis em RAM
    // ---------------------------------------------------------------------
    assembly.push_back(".section .data");
    for (const string& var : variaveisGlobais) {
        assembly.push_back(var + ": .skip 2");
    }

    // ---------------------------------------------------------------------
    // 3. Ãrea de cÃ³digo (.text) + strings em Flash
    // ---------------------------------------------------------------------
    assembly.push_back("");
    assembly.push_back(".section .text");
    assembly.push_back("");

    for (const string& var : variaveisGlobais) {
        assembly.push_back("str_" + var + ": .asciz \"" + var + " = 0x\"");
    }
    assembly.push_back("str_newline: .asciz \"\\r\\n\"");

    // ---------------------------------------------------------------------
    // 4. main â€“ prÃ³logo e inicializaÃ§Ã£o
    // ---------------------------------------------------------------------
    assembly.push_back("");

    // loop() do Arduino: não faz nada (código roda uma vez no setup)
    emitLabel("loop");
    emit("ret", "");

    // setup() do Arduino: contém todo o código
    assembly.push_back("");
    emitLabel("setup");
    // setup() do Arduino: sÃ­mbolo mangled _Z5setupv
    //emitLabel("_Z5setupv");
    //emit("rjmp", "main");

    // loop() do Arduino: sÃ­mbolo mangled _Z4loopv, laÃ§o infinito
    //emitLabel("_Z4loopv");
    //emit("rjmp", "_Z4loopv");

    // main "real" do seu cÃ³digo (para bare-metal ou para ser chamado por setup)
    //assembly.push_back("");
    //emitLabel("main");

    // Stack pointer
    emit("ldi", "temp, lo8(RAMEND)");
    emit("out", "_SFR_IO_ADDR(SPL), temp");
    emit("ldi", "temp, hi8(RAMEND)");
    emit("out", "_SFR_IO_ADDR(SPH), temp");

    // UART
    emit("rcall", "USART_Init");
    assembly.push_back("");

    // ---------------------------------------------------------------------
    // 5. TraduÃ§Ã£o TAC -> ASM
    // ---------------------------------------------------------------------
    for (const auto& inst : codigoTAC) {
        assembly.push_back("; " + inst.result + " = " +
                           inst.arg1 + " " + inst.op + " " + inst.arg2);

        // RÃ³tulo
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
            static int ifCounter = 0;
            string lblSkip = "IF_SKIP_" + to_string(ifCounter++);

            // carrega tCond em r24:r25
            emit("lds", "r24, " + inst.arg1);
            emit("lds", "r25, " + inst.arg1 + "+1");
            emit("or",  "r24, r25");

            // se diferente de zero, pula o rjmp (branch curto, sempre perto)
            emit("brne", lblSkip);

            // se zero, faz o salto longo
            emit("rjmp", inst.result);

            // label local logo depois, sempre dentro do alcance do brne
            emitLabel(lblSkip);
            continue;
        }

        // AtribuiÃ§Ã£o simples: x := y ou x := literal
        if (inst.op == ":=") {
            bool literal = false;
            if (!inst.arg1.empty()) {
                char c = inst.arg1[0];
                if (isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+')
                    literal = true;
            }

            if (literal) {
                // Check if it's an integer (no decimal point) or float
                bool isFloat = (inst.arg1.find('.') != string::npos);
                uint16_t val;
                
                if (isFloat) {
                    float f = std::stof(inst.arg1);
                    val = floatToHalf(f);
                } else {
                    // Raw integer - store directly as 16-bit value
                    int intVal = std::stoi(inst.arg1);
                    val = static_cast<uint16_t>(static_cast<int16_t>(intVal));
                }
                emit("ldi", "r24, " + hex8(static_cast<uint8_t>(val & 0xFF)));
                emit("ldi", "r25, " + hex8(static_cast<uint8_t>((val >> 8) & 0xFF)));
            } else {
                emit("lds", "r24, " + inst.arg1);
                emit("lds", "r25, " + inst.arg1 + "+1");
            }

            emit("sts", inst.result + ", r24");
            emit("sts", inst.result + "+1, r25");
            continue;
        }

        // OperaÃ§Ãµes binÃ¡rias e comparaÃ§Ãµes
        auto loadArg = [&](const string& arg, const string& rL, const string& rH) {
            bool literal = false;
            if (!arg.empty()) {
                char c = arg[0];
                if (isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+')
                    literal = true;
            }

            if (literal) {
                // Check if it's an integer (no decimal point) or float
                bool isFloat = (arg.find('.') != string::npos);
                uint16_t val;
                
                if (isFloat) {
                    float f = std::stof(arg);
                    val = floatToHalf(f);
                } else {
                    // Raw integer - store directly as 16-bit value
                    int intVal = std::stoi(arg);
                    val = static_cast<uint16_t>(static_cast<int16_t>(intVal));
                }
                emit("ldi", rL + ", " + hex8(static_cast<uint8_t>(val & 0xFF)));
                emit("ldi", rH + ", " + hex8(static_cast<uint8_t>((val >> 8) & 0xFF)));
            } else {
                emit("lds", rL + ", " + arg);
                emit("lds", rH + ", " + arg + "+1");
            }
        };

        loadArg(inst.arg1, "r24", "r25");
        loadArg(inst.arg2, "r22", "r23");

        string rotina;
        bool isComparison = false;

        if (inst.op == "+")       rotina = "__addhf3";
        else if (inst.op == "-")  rotina = "__subhf3";
        else if (inst.op == "*")  rotina = "__mulhf3";
        else if (inst.op == "/" || inst.op == "|") rotina = "__divhf3";
        else if (inst.op == "^")  rotina = "__powhf3";
        else if (inst.op == "<"  || inst.op == ">"  ||
                 inst.op == "<=" || inst.op == ">=" ||
                 inst.op == "==" || inst.op == "!=") {
            rotina = "__cmphf2";
            isComparison = true;
        }

        if (!rotina.empty()) {
            emit("rcall", rotina);
        }

        // Para comparações, converter resultado de __cmphf2 para booleano
        // __cmphf2 retorna: -1 (menor), 0 (igual), 1 (maior) em r24
        if (isComparison) {
            static int cmpCounter = 0;
            string lblTrue = "CMP_TRUE_" + to_string(cmpCounter);
            string lblEnd = "CMP_END_" + to_string(cmpCounter);
            cmpCounter++;

            // r24 contém resultado da comparação
            if (inst.op == "==") {
                emit("cpi", "r24, 0");
                emit("breq", lblTrue, "Se r24 == 0, são iguais");
            } else if (inst.op == "!=") {
                emit("cpi", "r24, 0");
                emit("brne", lblTrue, "Se r24 != 0, são diferentes");
            } else if (inst.op == "<") {
                // r24 = -1 (0xFF) significa menor
                emit("cpi", "r24, 0xFF");
                emit("breq", lblTrue, "Se r24 == -1, A < B");
            } else if (inst.op == ">=") {
                // r24 >= 0 significa maior ou igual (r24 = 0 ou r24 = 1)
                emit("cpi", "r24, 0xFF");
                emit("brne", lblTrue, "Se r24 != -1, A >= B");
            } else if (inst.op == ">") {
                // r24 = 1 significa maior
                emit("cpi", "r24, 1");
                emit("breq", lblTrue, "Se r24 == 1, A > B");
            } else if (inst.op == "<=") {
                // r24 <= 0 significa menor ou igual (r24 = -1 ou r24 = 0)
                emit("cpi", "r24, 1");
                emit("brne", lblTrue, "Se r24 != 1, A <= B");
            }

            // Resultado falso: 0
            emit("clr", "r24");
            emit("clr", "r25");
            emit("rjmp", lblEnd);

            // Resultado verdadeiro: 1
            emitLabel(lblTrue);
            emit("ldi", "r24, 1");
            emit("clr", "r25");

            emitLabel(lblEnd);
        }

        // Resultado em r24:r25
        emit("sts", inst.result + ", r24");
        emit("sts", inst.result + "+1, r25");
    }

    // Ao final do TAC, imprime variÃ¡veis e trava no fim
    emit("rcall", "DUMP_VARS");
    emit("rjmp", "FIM_DO_PROGRAMA");

    // ---------------------------------------------------------------------
    // 6. Rotina DUMP_VARS â€“ imprime todas as variÃ¡veis em hexa
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
    // 7. Fim do programa: laÃ§o infinito
    // ---------------------------------------------------------------------
    emitLabel("FIM_DO_PROGRAMA");
    emit("rjmp", "FIM_DO_PROGRAMA");

    // ---------------------------------------------------------------------
    // 8. DRIVER SERIAL â€“ implementaÃ§Ãµes usadas acima
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

    // USART_PrintStr â€“ Z aponta para string em Flash
    emitLabel("USART_PrintStr");
    emitLabel("PrintStr_Loop");
    emit("lpm", "temp, Z+");
    emit("cpi", "temp, 0");
    emit("breq", "PrintStr_Done");
    emit("rcall", "USART_Tx");
    emit("rjmp", "PrintStr_Loop");
    emitLabel("PrintStr_Done");
    emit("ret", "");

    // USART_PrintHexByte â€“ imprime temp em hexa
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

    // HexToAscii â€“ converte 0..15 em '0'..'9','A'..'F' em temp2
    emitLabel("HexToAscii");
    emit("cpi", "temp2, 10");
    emit("brlo", "HexDigit");
    emit("subi", "temp2, -7");     // A-F: +7 em relaÃ§Ã£o a '9'
    emitLabel("HexDigit");
    emit("subi", "temp2, -48");    // + '0'
    emit("ret", "");

    // ---------------------------------------------------------------------
    // 9. Rotinas de ponto flutuante 16-bit
    //    Implementação simplificada para inteiros com escala
    // ---------------------------------------------------------------------
    assembly.push_back("");
    assembly.push_back("; --- MATH ROUTINES 16-BIT ---");
    
    // __addhf3: r24:r25 = r24:r25 + r22:r23
    emitLabel("__addhf3");
    emit("add", "r24, r22");
    emit("adc", "r25, r23");
    emit("ret", "");
    
    // __subhf3: r24:r25 = r24:r25 - r22:r23
    emitLabel("__subhf3");
    emit("sub", "r24, r22");
    emit("sbc", "r25, r23");
    emit("ret", "");
    
    // __mulhf3: r24:r25 = r24:r25 * r22:r23 (simplificado)
    emitLabel("__mulhf3");
    emit("push", "r18");
    emit("push", "r19");
    emit("push", "r20");
    emit("push", "r21");
    emit("clr", "r20");
    emit("clr", "r21");
    // r24*r22
    emit("mul", "r24, r22");
    emit("movw", "r18, r0");
    // r24*r23
    emit("mul", "r24, r23");
    emit("add", "r19, r0");
    emit("adc", "r20, r1");
    // r25*r22
    emit("mul", "r25, r22");
    emit("add", "r19, r0");
    emit("adc", "r20, r1");
    // r25*r23
    emit("mul", "r25, r23");
    emit("add", "r20, r0");
    emit("adc", "r21, r1");
    // Resultado em r19:r18 (ignoramos overflow)
    emit("mov", "r24, r18");
    emit("mov", "r25, r19");
    emit("clr", "r1");
    emit("pop", "r21");
    emit("pop", "r20");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("ret", "");
    
    // __divhf3: r24:r25 = r24:r25 / r22:r23 (simplificado)
    emitLabel("__divhf3");
    emit("push", "r18");
    emit("push", "r19");
    emit("clr", "r18", "Quociente L");
    emit("clr", "r19", "Quociente H");
    emitLabel("__div_loop");
    emit("cp", "r24, r22");
    emit("cpc", "r25, r23");
    emit("brlo", "__div_done", "Se dividendo < divisor, termina");
    emit("sub", "r24, r22");
    emit("sbc", "r25, r23");
    emit("subi", "r18, 0xFF", "Incrementa quociente (subi -1)");
    emit("sbci", "r19, 0xFF");
    emit("rjmp", "__div_loop");
    emitLabel("__div_done");
    emit("mov", "r24, r18");
    emit("mov", "r25, r19");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("ret", "");
    
    // __powhf3: r24:r25 = r24:r25 ^ r22 (expoente inteiro)
    emitLabel("__powhf3");
    emit("push", "r18");
    emit("push", "r19");
    emit("push", "r20");
    emit("push", "r21");
    emit("mov", "r18, r24", "Base L");
    emit("mov", "r19, r25", "Base H");
    emit("mov", "r20, r22", "Expoente");
    emit("cpi", "r20, 0");
    emit("breq", "__pow_one", "Se exp=0, retorna 1");
    emit("cpi", "r20, 1");
    emit("breq", "__pow_done", "Se exp=1, retorna base");
    // Resultado inicial = base
    emit("dec", "r20");
    emitLabel("__pow_loop");
    emit("cpi", "r20, 0");
    emit("breq", "__pow_done");
    // Multiplicar resultado pela base
    emit("mov", "r22, r18");
    emit("mov", "r23, r19");
    emit("rcall", "__mulhf3");
    emit("dec", "r20");
    emit("rjmp", "__pow_loop");
    emitLabel("__pow_one");
    emit("ldi", "r24, 1");
    emit("clr", "r25");
    emitLabel("__pow_done");
    emit("pop", "r21");
    emit("pop", "r20");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("ret", "");
    
    // __cmphf2: Compara r24:r25 com r22:r23
    // Retorna: r24 = -1 se <, 0 se ==, 1 se >
    emitLabel("__cmphf2");
    emit("cp", "r24, r22", "Compara bytes baixos");
    emit("cpc", "r25, r23", "Compara bytes altos com carry");
    emit("breq", "__cmp_equal");
    emit("brlt", "__cmp_less", "Signed: se r24:r25 < r22:r23");
    // Maior
    emit("ldi", "r24, 1");
    emit("ret", "");
    emitLabel("__cmp_less");
    emit("ldi", "r24, 0xFF", "-1 em complemento de 2");
    emit("ret", "");
    emitLabel("__cmp_equal");
    emit("clr", "r24");
    emit("ret", "");

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