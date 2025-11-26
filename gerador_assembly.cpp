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
    // 9. IEEE 754 Half-Precision (16-bit) Float Routines
    // ---------------------------------------------------------------------
    assembly.push_back("");
    assembly.push_back("; --- IEEE 754 HALF-PRECISION MATH ---");

    // __subhf3: A - B = A + (-B)
    emitLabel("__subhf3");
    emit("ldi", "r20, 0x80");
    emit("eor", "r23, r20");    // Flip sign of B
    emit("rjmp", "__addhf3");

    // __addhf3: IEEE 754 half-precision addition
    emitLabel("__addhf3");
    emit("push", "r16");
    emit("push", "r17");
    emit("push", "r18");
    emit("push", "r19");
    emit("push", "r20");
    emit("push", "r21");
    emit("push", "r26");
    emit("push", "r27");
    emit("push", "r28");

    // Check A == 0
    emit("mov", "r16, r24");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r25");
    emit("brne", "__add_a_nz");
    // A is zero, return B
    emit("mov", "r24, r22");
    emit("mov", "r25, r23");
    emit("rjmp", "__add_done");
    emitLabel("__add_a_nz");

    // Check B == 0
    emit("mov", "r16, r22");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r23");
    emit("brne", "__add_b_nz");
    // B is zero, return A (already in r25:r24)
    emit("rjmp", "__add_done");
    emitLabel("__add_b_nz");

    // Extract sign A -> r16 bit 7
    emit("mov", "r16, r25");
    emit("andi", "r16, 0x80");

    // Extract sign B -> r17 bit 7
    emit("mov", "r17, r23");
    emit("andi", "r17, 0x80");

    // Extract expA -> r18
    emit("mov", "r18, r25");
    emit("lsr", "r18");
    emit("lsr", "r18");
    emit("andi", "r18, 0x1F");

    // Extract expB -> r19
    emit("mov", "r19, r23");
    emit("lsr", "r19");
    emit("lsr", "r19");
    emit("andi", "r19, 0x1F");

    // Extract mantA -> r21:r20 (with implicit 1)
    emit("mov", "r20, r24");
    emit("mov", "r21, r25");
    emit("andi", "r21, 0x03");
    emit("ori", "r21, 0x04");

    // Extract mantB -> r27:r26 (with implicit 1)
    emit("mov", "r26, r22");
    emit("mov", "r27, r23");
    emit("andi", "r27, 0x03");
    emit("ori", "r27, 0x04");

    // Compare exponents, make expA >= expB
    emit("cp", "r18, r19");
    emit("brsh", "__add_no_swap");

    // Swap A and B (so expA >= expB)
    emit("mov", "r28, r18");
    emit("mov", "r18, r19");
    emit("mov", "r19, r28");
    emit("mov", "r28, r20");
    emit("mov", "r20, r26");
    emit("mov", "r26, r28");
    emit("mov", "r28, r21");
    emit("mov", "r21, r27");
    emit("mov", "r27, r28");
    emit("mov", "r28, r16");
    emit("mov", "r16, r17");
    emit("mov", "r17, r28");

    emitLabel("__add_no_swap");
    // r18 = larger exp (result exp), r19 = smaller exp
    // r21:r20 = mantissa of larger, r27:r26 = mantissa of smaller
    // r16 = sign of larger, r17 = sign of smaller

    // Align mantissas: shift r27:r26 right by (r18 - r19)
    emit("mov", "r28, r18");
    emit("sub", "r28, r19");

    // If shift >= 11, smaller operand is negligible
    emit("cpi", "r28, 11");
    emit("brlo", "__add_do_align");
    // Return larger operand
    emit("rjmp", "__add_pack_a");

    emitLabel("__add_do_align");
    emit("tst", "r28");
    emit("breq", "__add_aligned");

    emitLabel("__add_align_loop");
    emit("lsr", "r27");
    emit("ror", "r26");
    emit("dec", "r28");
    emit("brne", "__add_align_loop");

    emitLabel("__add_aligned");
    // Check if same sign -> add, different sign -> subtract
    emit("cp", "r16, r17");
    emit("brne", "__add_diff_sign");

    // Same sign: add mantissas
    emit("add", "r20, r26");
    emit("adc", "r21, r27");

    // Check overflow (bit 3 set means we need to shift right)
    emit("sbrs", "r21, 3");
    emit("rjmp", "__add_norm");
    emit("lsr", "r21");
    emit("ror", "r20");
    emit("inc", "r18");
    emit("rjmp", "__add_norm");

    emitLabel("__add_diff_sign");
    // Different signs: subtract smaller from larger
    emit("sub", "r20, r26");
    emit("sbc", "r21, r27");

    // Result might be zero
    emit("mov", "r28, r20");
    emit("or", "r28, r21");
    emit("brne", "__add_norm");
    emit("clr", "r24");
    emit("clr", "r25");
    emit("rjmp", "__add_done");

    // Normalize: shift left until bit 2 of r21 is set
    emitLabel("__add_norm");
    emit("sbrc", "r21, 2");
    emit("rjmp", "__add_norm_done");
    emit("tst", "r18");
    emit("breq", "__add_ret_zero");
    emit("lsl", "r20");
    emit("rol", "r21");
    emit("dec", "r18");
    emit("rjmp", "__add_norm");

    emitLabel("__add_norm_done");
    // Check overflow
    emit("cpi", "r18, 31");
    emit("brlo", "__add_exp_ok");
    emit("ldi", "r24, 0x00");
    emit("ldi", "r25, 0x7C");
    emit("or", "r25, r16");
    emit("rjmp", "__add_done");

    emitLabel("__add_exp_ok");
    // Pack result
    emit("andi", "r21, 0x03");
    emit("mov", "r24, r20");
    emit("mov", "r25, r21");
    emit("lsl", "r18");
    emit("lsl", "r18");
    emit("or", "r25, r18");
    emit("or", "r25, r16");
    emit("rjmp", "__add_done");

    emitLabel("__add_pack_a");
    // Return A (larger operand)
    emit("andi", "r21, 0x03");
    emit("mov", "r24, r20");
    emit("mov", "r25, r21");
    emit("lsl", "r18");
    emit("lsl", "r18");
    emit("or", "r25, r18");
    emit("or", "r25, r16");
    emit("rjmp", "__add_done");

    emitLabel("__add_ret_zero");
    emit("clr", "r24");
    emit("clr", "r25");

    emitLabel("__add_done");
    emit("pop", "r28");
    emit("pop", "r27");
    emit("pop", "r26");
    emit("pop", "r21");
    emit("pop", "r20");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("pop", "r17");
    emit("pop", "r16");
    emit("ret", "");

    // __mulhf3
    emitLabel("__mulhf3");
    emit("push", "r16");
    emit("push", "r17");
    emit("push", "r18");
    emit("push", "r19");
    emit("push", "r20");
    emit("push", "r21");
    emit("push", "r26");
    emit("push", "r27");

    emit("mov", "r16, r24");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r25");
    emit("brne", "__mul_a_not_zero");
    emit("rjmp", "__mul_ret_zero");
    emitLabel("__mul_a_not_zero");

    emit("mov", "r16, r22");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r23");
    emit("brne", "__mul_b_not_zero");
    emit("rjmp", "__mul_ret_zero");
    emitLabel("__mul_b_not_zero");

    // Result sign = XOR of signs
    emit("mov", "r16, r25");
    emit("eor", "r16, r23");
    emit("andi", "r16, 0x80");

    // Extract exp A -> r17
    emit("mov", "r17, r25");
    emit("lsr", "r17");
    emit("lsr", "r17");
    emit("andi", "r17, 0x1F");

    // Extract exp B -> r21
    emit("mov", "r21, r23");
    emit("lsr", "r21");
    emit("lsr", "r21");
    emit("andi", "r21, 0x1F");

    // Result exp = expA + expB - 15
    emit("add", "r17, r21");
    emit("subi", "r17, 15");

    // Extract mant A with implicit 1 -> r19:r18
    emit("mov", "r18, r24");
    emit("mov", "r19, r25");
    emit("andi", "r19, 0x03");
    emit("ori", "r19, 0x04");

    // Extract mant B with implicit 1 -> r27:r26
    emit("mov", "r26, r22");
    emit("mov", "r27, r23");
    emit("andi", "r27, 0x03");
    emit("ori", "r27, 0x04");

    // Multiply mantissas: 11-bit x 11-bit
    emit("clr", "r20");
    emit("clr", "r21");

    emit("mul", "r18, r26");
    emit("movw", "r20, r0");

    emit("mul", "r18, r27");
    emit("add", "r21, r0");
    emit("clr", "r18");
    emit("adc", "r18, r1");

    emit("mul", "r19, r26");
    emit("add", "r21, r0");
    emit("adc", "r18, r1");

    emit("mul", "r19, r27");
    emit("add", "r18, r0");
    emit("clr", "r19");
    emit("adc", "r19, r1");
    emit("clr", "r1");

    // Result in r19:r18:r21:r20, leading 1 should be around bit 5 of r18
    // Normalize
    emit("tst", "r19");
    emit("brne", "__mul_shift_down");

    emit("sbrc", "r18, 5");
    emit("rjmp", "__mul_norm_5");
    emit("sbrc", "r18, 4");
    emit("rjmp", "__mul_norm_4");
    emit("sbrc", "r18, 3");
    emit("rjmp", "__mul_norm_3");
    emit("sbrc", "r18, 2");
    emit("rjmp", "__mul_norm_2");
    emit("rjmp", "__mul_ret_zero");

    emitLabel("__mul_shift_down");
    emit("lsr", "r19");
    emit("ror", "r18");
    emit("ror", "r21");
    emit("inc", "r17");
    emit("tst", "r19");
    emit("brne", "__mul_shift_down");
    emit("sbrc", "r18, 5");
    emit("rjmp", "__mul_norm_5");

    emitLabel("__mul_norm_5");
    emit("inc", "r17");
    emit("lsr", "r18");
    emit("ror", "r21");

    emitLabel("__mul_norm_4");
    // Bit 4 is the implicit 1, this is normal position
    emit("rjmp", "__mul_pack");

    emitLabel("__mul_norm_3");
    emit("dec", "r17");
    emit("lsl", "r21");
    emit("rol", "r18");
    emit("rjmp", "__mul_pack");

    emitLabel("__mul_norm_2");
    emit("dec", "r17");
    emit("lsl", "r21");
    emit("rol", "r18");
    emit("dec", "r17");
    emit("lsl", "r21");
    emit("rol", "r18");

    emitLabel("__mul_pack");
    // Check exp overflow/underflow
    emit("cpi", "r17, 31");
    emit("brlo", "__mul_no_ovf");
    emit("rjmp", "__mul_ret_inf");
    emitLabel("__mul_no_ovf");

    emit("cpi", "r17, 1");
    emit("brsh", "__mul_no_udf");
    emit("rjmp", "__mul_ret_zero");
    emitLabel("__mul_no_udf");

    // Pack: mantissa is in r18:r21, bits 3:2 of r18 are high mantissa
    emit("mov", "r19, r18");
    emit("andi", "r19, 0x03");
    emit("mov", "r24, r21");
    emit("mov", "r25, r19");
    emit("lsl", "r17");
    emit("lsl", "r17");
    emit("or", "r25, r17");
    emit("or", "r25, r16");
    emit("rjmp", "__mul_done");

    emitLabel("__mul_ret_zero");
    emit("clr", "r24");
    emit("clr", "r25");
    emit("rjmp", "__mul_done");

    emitLabel("__mul_ret_inf");
    emit("ldi", "r24, 0x00");
    emit("ldi", "r25, 0x7C");
    emit("or", "r25, r16");

    emitLabel("__mul_done");
    emit("pop", "r27");
    emit("pop", "r26");
    emit("pop", "r21");
    emit("pop", "r20");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("pop", "r17");
    emit("pop", "r16");
    emit("ret", "");

    // __divhf3
    emitLabel("__divhf3");
    emit("push", "r16");
    emit("push", "r17");
    emit("push", "r18");
    emit("push", "r19");
    emit("push", "r20");
    emit("push", "r21");
    emit("push", "r26");
    emit("push", "r27");
    emit("push", "r28");

    // Check A == 0
    emit("mov", "r16, r24");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r25");
    emit("brne", "__div_a_ok");
    emit("rjmp", "__div_ret_zero");
    emitLabel("__div_a_ok");

    // Check B == 0
    emit("mov", "r16, r22");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r23");
    emit("brne", "__div_b_ok");
    emit("rjmp", "__div_ret_inf");
    emitLabel("__div_b_ok");

    // Result sign
    emit("mov", "r16, r25");
    emit("eor", "r16, r23");
    emit("andi", "r16, 0x80");

    // Extract expA -> r17
    emit("mov", "r17, r25");
    emit("lsr", "r17");
    emit("lsr", "r17");
    emit("andi", "r17, 0x1F");

    // Extract expB -> r21
    emit("mov", "r21, r23");
    emit("lsr", "r21");
    emit("lsr", "r21");
    emit("andi", "r21, 0x1F");

    // exp_result = expA - expB + 15
    emit("sub", "r17, r21");
    emit("subi", "r17, -15");

    // Extract mantA -> r19:r18
    emit("mov", "r18, r24");
    emit("mov", "r19, r25");
    emit("andi", "r19, 0x03");
    emit("ori", "r19, 0x04");

    // Extract mantB -> r27:r26
    emit("mov", "r26, r22");
    emit("mov", "r27, r23");
    emit("andi", "r27, 0x03");
    emit("ori", "r27, 0x04");

    // Clear quotient
    emit("clr", "r20");
    emit("clr", "r21");

    // 11-bit restoring division
    emit("ldi", "r28, 11");

    emitLabel("__div_loop");
    // Compare remainder to divisor
    emit("cp", "r18, r26");
    emit("cpc", "r19, r27");
    emit("brlo", "__div_lt");

    // remainder >= divisor: subtract, set carry
    emit("sub", "r18, r26");
    emit("sbc", "r19, r27");
    emit("sec");
    emit("rjmp", "__div_shift");

    emitLabel("__div_lt");
    // remainder < divisor: clear carry
    emit("clc");

    emitLabel("__div_shift");
    // Shift carry into quotient (new bit at position 0)
    emit("rol", "r20");
    emit("rol", "r21");
    // Shift remainder for next iteration
    emit("lsl", "r18");
    emit("rol", "r19");
    emit("dec", "r28");
    emit("brne", "__div_loop");

    // Quotient now in r21:r20, copy to r19:r18
    emit("mov", "r18, r20");
    emit("mov", "r19, r21");

    // Check zero
    emit("mov", "r20, r18");
    emit("or", "r20, r19");
    emit("brne", "__div_norm");
    emit("rjmp", "__div_ret_zero");

    // Normalize: bit 10 (bit 2 of r19) must be set
    emitLabel("__div_norm");
    emit("sbrc", "r19, 2");
    emit("rjmp", "__div_norm_done");
    // Check underflow before shifting
    emit("cpi", "r17, 2");
    emit("brlo", "__div_ret_zero");
    emit("lsl", "r18");
    emit("rol", "r19");
    emit("dec", "r17");
    emit("rjmp", "__div_norm");

    emitLabel("__div_norm_done");
    // Check exp bounds
    emit("cpi", "r17, 31");
    emit("brlo", "__div_no_ovf");
    emit("rjmp", "__div_ret_inf");
    emitLabel("__div_no_ovf");
    emit("cpi", "r17, 1");
    emit("brsh", "__div_pack");
    emit("rjmp", "__div_ret_zero");

    emitLabel("__div_pack");
    emit("andi", "r19, 0x03");
    emit("mov", "r24, r18");
    emit("mov", "r25, r19");
    emit("lsl", "r17");
    emit("lsl", "r17");
    emit("or", "r25, r17");
    emit("or", "r25, r16");
    emit("rjmp", "__div_done");

    emitLabel("__div_ret_zero");
    emit("clr", "r24");
    emit("clr", "r25");
    emit("rjmp", "__div_done");

    emitLabel("__div_ret_inf");
    emit("ldi", "r24, 0x00");
    emit("ldi", "r25, 0x7C");
    emit("or", "r25, r16");

    emitLabel("__div_done");
    emit("pop", "r28");
    emit("pop", "r27");
    emit("pop", "r26");
    emit("pop", "r21");
    emit("pop", "r20");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("pop", "r17");
    emit("pop", "r16");
    emit("ret", "");

    // __cmphf2
    emitLabel("__cmphf2");
    emit("mov", "r26, r25");
    emit("eor", "r26, r23");
    emit("sbrs", "r26, 7");
    emit("rjmp", "__cmp_same_sign");
    emit("sbrc", "r25, 7");
    emit("rjmp", "__cmp_ret_less");
    emit("rjmp", "__cmp_ret_greater");

    emitLabel("__cmp_same_sign");
    emit("sbrc", "r25, 7");
    emit("rjmp", "__cmp_both_neg");
    emit("cp", "r24, r22");
    emit("cpc", "r25, r23");
    emit("breq", "__cmp_ret_equal");
    emit("brlo", "__cmp_ret_less");
    emit("rjmp", "__cmp_ret_greater");

    emitLabel("__cmp_both_neg");
    emit("cp", "r22, r24");
    emit("cpc", "r23, r25");
    emit("breq", "__cmp_ret_equal");
    emit("brlo", "__cmp_ret_less");
    emit("rjmp", "__cmp_ret_greater");

    emitLabel("__cmp_ret_less");
    emit("ldi", "r24, 0xFF");
    emit("ret", "");
    emitLabel("__cmp_ret_equal");
    emit("clr", "r24");
    emit("ret", "");
    emitLabel("__cmp_ret_greater");
    emit("ldi", "r24, 1");
    emit("ret", "");

    // __powhf3
    emitLabel("__powhf3");
    emit("push", "r16");
    emit("push", "r17");
    emit("push", "r18");
    emit("push", "r19");

    emit("mov", "r18, r24");
    emit("mov", "r19, r25");
    emit("mov", "r16, r22");

    emit("cpi", "r16, 0");
    emit("brne", "__pow_not_zero");
    emit("rjmp", "__pow_ret_one");
    emitLabel("__pow_not_zero");

    emit("cpi", "r16, 1");
    emit("brne", "__pow_not_one");
    emit("rjmp", "__pow_ret_base");
    emitLabel("__pow_not_one");

    // Start with 1.0
    emit("ldi", "r24, 0x00");
    emit("ldi", "r25, 0x3C");

    emitLabel("__pow_loop");
    emit("cpi", "r16, 0");
    emit("breq", "__pow_done");
    emit("mov", "r22, r18");
    emit("mov", "r23, r19");
    emit("rcall", "__mulhf3");
    emit("dec", "r16");
    emit("rjmp", "__pow_loop");

    emitLabel("__pow_ret_one");
    emit("ldi", "r24, 0x00");
    emit("ldi", "r25, 0x3C");
    emit("rjmp", "__pow_done");

    emitLabel("__pow_ret_base");
    emit("mov", "r24, r18");
    emit("mov", "r25, r19");

    emitLabel("__pow_done");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("pop", "r17");
    emit("pop", "r16");
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