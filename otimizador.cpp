#include "otimizador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <set>

using namespace std;

OtimizadorTAC::OtimizadorTAC() : houveOtimizacao(false) {}

void OtimizadorTAC::carregarCodigo(const vector<InstrucaoTAC>& codigoOriginal) {
    this->codigo = codigoOriginal;
    this->relatorio.clear();
}

void OtimizadorTAC::log(const string& msg) {
    relatorio.push_back(msg);
}

bool OtimizadorTAC::ehNumero(const string& s) {
    if (s.empty()) return false;
    char* end = nullptr;
    strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0';
}

bool OtimizadorTAC::ehVariavelTemporaria(const string& s) {
    // Assume que temporários começam com 't' seguido de números (t0, t1...)
    return s.length() > 0 && s[0] == 't' && isdigit(s[1]);
}

void OtimizadorTAC::otimizar() {
    int passadas = 0;
    do {
        passadas++;
        houveOtimizacao = false;
        
        log("--- Inicio da Passada " + to_string(passadas) + " ---");
        
        constantFolding();
        algebraicSimplification();
        constantPropagation();
        deadCodeElimination();
        
    } while (houveOtimizacao); // Repete enquanto houver melhorias
}

// 1. DOBRA DE CONSTANTES (Calcula valores fixos)
void OtimizadorTAC::constantFolding() {
    for (size_t i = 0; i < codigo.size(); i++) {
        InstrucaoTAC& inst = codigo[i];
        
        // Se for operação binária e ambos argumentos forem números
        if (!inst.arg1.empty() && !inst.arg2.empty() && 
            ehNumero(inst.arg1) && ehNumero(inst.arg2)) {
            
            double v1 = stod(inst.arg1);
            double v2 = stod(inst.arg2);
            double res = 0;
            bool calculou = true;
            
            if (inst.op == "+") res = v1 + v2;
            else if (inst.op == "-") res = v1 - v2;
            else if (inst.op == "*") res = v1 * v2;
            else if (inst.op == "/") {
                if(v2 != 0) res = v1 / v2; else calculou = false;
            }
            else if (inst.op == "|") { // Divisao real
                 if(v2 != 0) res = v1 / v2; else calculou = false;
            }
            else if (inst.op == "%") {
                if((int)v2 != 0) res = (int)v1 % (int)v2; else calculou = false;
            }
            else if (inst.op == "^") res = pow(v1, v2);
            else calculou = false; // Operadores relacionais ou desconhecidos ignorados por segurança
            
            if (calculou) {
                // Transforma t0 = 2 + 3 em t0 = 5
                string antigo = inst.result + " = " + inst.arg1 + " " + inst.op + " " + inst.arg2;
                
                // Remove casas decimais desnecessárias (ex: 5.00 -> 5)
                string sRes = to_string(res);
                sRes.erase(sRes.find_last_not_of('0') + 1, string::npos); 
                if (sRes.back() == '.') sRes.pop_back();

                inst.op = ":=";
                inst.arg1 = sRes;
                inst.arg2 = "";
                // result mantem-se o mesmo
                
                log("[Dobra Constantes] Linha " + to_string(i) + ": " + antigo + "  --->  " + inst.result + " = " + sRes);
                houveOtimizacao = true;
            }
        }
    }
}

// 2. SIMPLIFICAÇÃO ALGÉBRICA (x + 0, x * 1, etc)
void OtimizadorTAC::algebraicSimplification() {
    for (size_t i = 0; i < codigo.size(); i++) {
        InstrucaoTAC& inst = codigo[i];
        
        // Soma com 0
        if (inst.op == "+") {
            string val = "";
            if (inst.arg1 == "0") val = inst.arg2;
            else if (inst.arg2 == "0") val = inst.arg1;
            
            if (!val.empty()) {
                log("[Algébrica] " + inst.result + " = " + inst.arg1 + " + " + inst.arg2 + " ---> " + inst.result + " = " + val);
                inst.op = ":=";
                inst.arg1 = val;
                inst.arg2 = "";
                houveOtimizacao = true;
            }
        }
        // Multiplicação por 1
        else if (inst.op == "*") {
            string val = "";
            if (inst.arg1 == "1") val = inst.arg2;
            else if (inst.arg2 == "1") val = inst.arg1;
            
            if (!val.empty()) {
                log("[Algébrica] " + inst.result + " = " + inst.arg1 + " * " + inst.arg2 + " ---> " + inst.result + " = " + val);
                inst.op = ":=";
                inst.arg1 = val;
                inst.arg2 = "";
                houveOtimizacao = true;
            }
        }
    }
}

// 3. PROPAGAÇÃO DE CONSTANTES E CÓPIAS
void OtimizadorTAC::constantPropagation() {
    // Mapa de valores conhecidos: "t0" -> "5" ou "N" -> "1"
    map<string, string> valoresConhecidos;

    // Varredura linear
    for (size_t i = 0; i < codigo.size(); i++) {
        InstrucaoTAC& inst = codigo[i];
        
        // --- CORREÇÃO DE SEGURANÇA ---
        // Se encontramos um LABEL, significa que o fluxo de controle pode 
        // vir de qualquer lugar (um loop, um if). 
        // Não podemos garantir que as constantes anteriores ainda valem.
        if (inst.op == "LABEL") {
            valoresConhecidos.clear(); // Esquece tudo o que sabia
            continue;
        }
        // -----------------------------

        // Tenta substituir arg1 pelo valor conhecido
        if (valoresConhecidos.count(inst.arg1)) {
            // Só substitui se for seguro (ex: não substituir N dentro de um loop se N muda)
            // Para simplificar: Temporários (tX) são unicos (Static Single Assignment parcial), 
            // então é seguro propagar. Variáveis globais (N, L) são perigosas.
            
            // Regra segura para Fase 4: Só propaga temporários (t0, t1...)
            // OU literais se tivermos certeza absoluta.
            
            // Para consertar seu Fatorial agora, vamos propagar APENAS se for temporário
            if (ehVariavelTemporaria(inst.arg1)) {
                 log("[Propagação] Linha " + to_string(i) + ": Substituindo " + inst.arg1 + " por " + valoresConhecidos[inst.arg1]);
                 inst.arg1 = valoresConhecidos[inst.arg1];
                 houveOtimizacao = true;
            }
        }

        // Tenta substituir arg2
        if (valoresConhecidos.count(inst.arg2)) {
             if (ehVariavelTemporaria(inst.arg2)) {
                 log("[Propagação] Linha " + to_string(i) + ": Substituindo " + inst.arg2 + " por " + valoresConhecidos[inst.arg2]);
                 inst.arg2 = valoresConhecidos[inst.arg2];
                 houveOtimizacao = true;
             }
        }

        // Se a instrução define uma constante (t0 = 5), guarda no mapa
        if (inst.op == ":=" && (ehNumero(inst.arg1) || ehVariavelTemporaria(inst.arg1))) {
            valoresConhecidos[inst.result] = inst.arg1;
        }
        // Se a instrução define um valor complexo (t0 = A + B), 
        // remove t0 do mapa de conhecidos, pois ele mudou
        else if (!inst.result.empty()) {
            if (valoresConhecidos.count(inst.result)) {
                valoresConhecidos.erase(inst.result);
            }
        }
    }
}

// 4. ELIMINAÇÃO DE CÓDIGO MORTO
void OtimizadorTAC::deadCodeElimination() {
    set<string> usados;
    
    // 1. Identificar todas as variáveis usadas (lidas)
    // Se aparece em arg1, arg2, ou em um IF_FALSE/RES
    for (const auto& inst : codigo) {
        if (!inst.arg1.empty()) usados.insert(inst.arg1);
        if (!inst.arg2.empty()) usados.insert(inst.arg2);
    }
    
    vector<InstrucaoTAC> novoCodigo;
    
    for (size_t i = 0; i < codigo.size(); i++) {
        const InstrucaoTAC& inst = codigo[i];
        
        // Se for definição de temporário (tX = ...)
        // E tX não estiver na lista de usados
        // E não for uma Label ou GOTO
        if (ehVariavelTemporaria(inst.result) && usados.find(inst.result) == usados.end() && inst.op != "LABEL") {
            log("[Dead Code] Removendo linha " + to_string(i) + ": " + inst.result + " (nunca usado)");
            houveOtimizacao = true;
            // Não adiciona no novo vetor (remove)
        } else {
            novoCodigo.push_back(inst);
        }
    }
    
    codigo = novoCodigo;
}

vector<InstrucaoTAC> OtimizadorTAC::obterCodigo() const {
    return codigo;
}

void OtimizadorTAC::imprimirRelatorio() {
    cout << "\n=== RELATORIO DE OTIMIZACAO ===" << endl;
    for (const auto& linha : relatorio) {
        cout << linha << endl;
    }
    cout << "===============================" << endl;
}

void OtimizadorTAC::salvarRelatorio(const string& nomeArquivo) {
    ofstream file(nomeArquivo);
    file << "=== RELATORIO DE OTIMIZACAO ===\n";
    for (const auto& linha : relatorio) {
        file << linha << "\n";
    }
    file.close();
    cout << "Relatorio salvo em: " << nomeArquivo << endl;
}

void OtimizadorTAC::salvarTACOtimizado(const string& nomeArquivo) {
    ofstream file(nomeArquivo);
    
    for (size_t i = 0; i < codigo.size(); i++) {
        const InstrucaoTAC& inst = codigo[i];
        file << i << ": ";
        if (inst.op == ":=") file << inst.result << " = " << inst.arg1;
        else if (inst.op == "LABEL") file << inst.result << ":";
        else if (inst.op == "GOTO") file << "GOTO " << inst.result;
        else if (inst.op == "IF_FALSE") file << "IF_FALSE " << inst.arg1 << " GOTO " << inst.result;
        else if (inst.op == "RES") file << inst.result << " = RES(" << inst.arg1 << ")";
        else file << inst.result << " = " << inst.arg1 << " " << inst.op << " " << inst.arg2;
        file << "\n";
    }
    file.close();
    cout << "TAC Otimizado salvo em: " << nomeArquivo << endl;
}