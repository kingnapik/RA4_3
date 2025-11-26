// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA4_3

#include "gerador_assembly.h"
#include <sstream>
#include <iomanip>

// Helper local: 8 bits -> "0xFF"
static string hex8(uint8_t val) {
    stringstream ss;
    ss << "0x" << hex << uppercase << setfill('0') << setw(2) << static_cast<int>(val);
    return ss.str();
}

// =============================================================================
// SECAO 5: TRADUCAO TAC -> ASSEMBLY
// =============================================================================

void GeradorAssembly::traduzirTAC(const vector<InstrucaoTAC>& codigoTAC) {
    assembly.push_back("; --- CODIGO DO PROGRAMA (traduzido do TAC) ---");
    
    for (const auto& inst : codigoTAC) {
        // Comentario indicando instrucao TAC original
        assembly.push_back("; TAC: " + inst.result + " = " + inst.arg1 + " " + inst.op + " " + inst.arg2);

        // Rotulo
        if (inst.op == "LABEL") {
            emitLabel(inst.result);
            continue;
        }

        // GOTO incondicional
        if (inst.op == "GOTO") {
            emit("rjmp", inst.result);
            continue;
        }

        // IF_FALSE cond GOTO label
        if (inst.op == "IF_FALSE") {
            traduzirIfFalse(inst);
            continue;
        }

        // Atribuicao simples: x := y ou x := literal
        if (inst.op == ":=") {
            traduzirAtribuicao(inst);
            continue;
        }

        // Operacoes binarias
        traduzirOperacaoBinaria(inst);
    }
}

void GeradorAssembly::traduzirIfFalse(const InstrucaoTAC& inst) {
    static int ifCounter = 0;
    string lblSkip = "IF_SKIP_" + to_string(ifCounter++);

    emit("lds", "r24, " + inst.arg1);
    emit("lds", "r25, " + inst.arg1 + "+1");
    emit("or",  "r24, r25");
    emit("brne", lblSkip, "Se != 0, pula o jump");
    emit("rjmp", inst.result, "Se == 0, salta para " + inst.result);
    emitLabel(lblSkip);
}

void GeradorAssembly::traduzirAtribuicao(const InstrucaoTAC& inst) {
    bool literal = false;
    if (!inst.arg1.empty()) {
        char c = inst.arg1[0];
        if (isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+')
            literal = true;
    }

    if (literal) {
        bool isFloat = (inst.arg1.find('.') != string::npos);
        uint16_t val;
        
        if (isFloat) {
            float f = std::stof(inst.arg1);
            val = floatToHalf(f);
        } else {
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
}

void GeradorAssembly::traduzirOperacaoBinaria(const InstrucaoTAC& inst) {
    // Carrega operandos
    carregarOperando(inst.arg1, "r24", "r25");
    carregarOperando(inst.arg2, "r22", "r23");

    // Determina rotina a chamar
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

    // Para comparacoes, converter resultado para booleano
    if (isComparison) {
        traduzirComparacao(inst.op);
    }

    // Armazena resultado
    emit("sts", inst.result + ", r24");
    emit("sts", inst.result + "+1, r25");
}

void GeradorAssembly::carregarOperando(const string& arg, const string& rL, const string& rH) {
    bool literal = false;
    if (!arg.empty()) {
        char c = arg[0];
        if (isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+')
            literal = true;
    }

    if (literal) {
        bool isFloat = (arg.find('.') != string::npos);
        uint16_t val;
        
        if (isFloat) {
            float f = std::stof(arg);
            val = floatToHalf(f);
        } else {
            int intVal = std::stoi(arg);
            val = static_cast<uint16_t>(static_cast<int16_t>(intVal));
        }
        emit("ldi", rL + ", " + hex8(static_cast<uint8_t>(val & 0xFF)));
        emit("ldi", rH + ", " + hex8(static_cast<uint8_t>((val >> 8) & 0xFF)));
    } else {
        emit("lds", rL + ", " + arg);
        emit("lds", rH + ", " + arg + "+1");
    }
}

void GeradorAssembly::traduzirComparacao(const string& op) {
    static int cmpCounter = 0;
    string lblTrue = "CMP_TRUE_" + to_string(cmpCounter);
    string lblEnd = "CMP_END_" + to_string(cmpCounter);
    cmpCounter++;

    if (op == "==") {
        emit("cpi", "r24, 0");
        emit("breq", lblTrue, "Se r24 == 0, sao iguais");
    } else if (op == "!=") {
        emit("cpi", "r24, 0");
        emit("brne", lblTrue, "Se r24 != 0, sao diferentes");
    } else if (op == "<") {
        emit("cpi", "r24, 0xFF");
        emit("breq", lblTrue, "Se r24 == -1, A < B");
    } else if (op == ">=") {
        emit("cpi", "r24, 0xFF");
        emit("brne", lblTrue, "Se r24 != -1, A >= B");
    } else if (op == ">") {
        emit("cpi", "r24, 1");
        emit("breq", lblTrue, "Se r24 == 1, A > B");
    } else if (op == "<=") {
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