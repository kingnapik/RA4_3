// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "otimizador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <set>
#include <iomanip>
#include <ctime>

using namespace std;

OtimizadorTAC::OtimizadorTAC() : houveOtimizacao(false) {}

void OtimizadorTAC::carregarCodigo(const vector<InstrucaoTAC>& codigo) {
    this->codigo = codigo;
    this->codigoOriginal = codigo;
    this->relatorio.clear();
    this->stats = EstatisticasOtimizacao();
    this->stats.instrucoesOriginais = codigo.size();
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
    return s.length() > 1 && s[0] == 't' && isdigit(s[1]);
}

string OtimizadorTAC::formatarInstrucaoTAC(const InstrucaoTAC& inst) {
    stringstream ss;
    if (inst.op == "LABEL") ss << inst.result << ":";
    else if (inst.op == "GOTO") ss << "GOTO " << inst.result;
    else if (inst.op == "IF_FALSE") ss << "IF_FALSE " << inst.arg1 << " GOTO " << inst.result;
    else if (inst.op == ":=") ss << inst.result << " = " << inst.arg1;
    else if (inst.op == "RES") ss << inst.result << " = RES(" << inst.arg1 << ")";
    else ss << inst.result << " = " << inst.arg1 << " " << inst.op << " " << inst.arg2;
    return ss.str();
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
        redundantJumpElimination();
        
    } while (houveOtimizacao && passadas < 10);
    
    stats.totalPassadas = passadas;
    stats.instrucoesFinais = codigo.size();
}

// =============================================================================
// 1. CONSTANT FOLDING - Calcula expressoes constantes em tempo de compilacao
// =============================================================================
void OtimizadorTAC::constantFolding() {
    for (size_t i = 0; i < codigo.size(); i++) {
        InstrucaoTAC& inst = codigo[i];
        
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
            else if (inst.op == "|") {
                if(v2 != 0) res = v1 / v2; else calculou = false;
            }
            else if (inst.op == "%") {
                if((int)v2 != 0) res = (int)v1 % (int)v2; else calculou = false;
            }
            else if (inst.op == "^") res = pow(v1, v2);
            else calculou = false;
            
            if (calculou) {
                string antigo = inst.result + " = " + inst.arg1 + " " + inst.op + " " + inst.arg2;
                
                string sRes = to_string(res);
                sRes.erase(sRes.find_last_not_of('0') + 1, string::npos); 
                if (sRes.back() == '.') sRes.pop_back();

                inst.op = ":=";
                inst.arg1 = sRes;
                inst.arg2 = "";
                
                log("[Constant Folding] Linha " + to_string(i) + ": " + antigo + " --> " + inst.result + " = " + sRes);
                stats.constantFolding++;
                houveOtimizacao = true;
            }
        }
    }
}

// =============================================================================
// 2. ALGEBRAIC SIMPLIFICATION - Simplifica identidades algebricas
// =============================================================================
void OtimizadorTAC::algebraicSimplification() {
    for (size_t i = 0; i < codigo.size(); i++) {
        InstrucaoTAC& inst = codigo[i];
        string antigo = formatarInstrucaoTAC(inst);
        bool otimizou = false;
        
        // x + 0 = x, 0 + x = x
        if (inst.op == "+") {
            string val = "";
            if (inst.arg1 == "0" || inst.arg1 == "0.0") val = inst.arg2;
            else if (inst.arg2 == "0" || inst.arg2 == "0.0") val = inst.arg1;
            
            if (!val.empty()) {
                inst.op = ":=";
                inst.arg1 = val;
                inst.arg2 = "";
                otimizou = true;
            }
        }
        // x - 0 = x
        else if (inst.op == "-") {
            if (inst.arg2 == "0" || inst.arg2 == "0.0") {
                inst.op = ":=";
                inst.arg2 = "";
                otimizou = true;
            }
        }
        // x * 1 = x, 1 * x = x
        else if (inst.op == "*") {
            string val = "";
            if (inst.arg1 == "1" || inst.arg1 == "1.0") val = inst.arg2;
            else if (inst.arg2 == "1" || inst.arg2 == "1.0") val = inst.arg1;
            // x * 0 = 0
            else if (inst.arg1 == "0" || inst.arg1 == "0.0" || 
                     inst.arg2 == "0" || inst.arg2 == "0.0") val = "0";
            
            if (!val.empty()) {
                inst.op = ":=";
                inst.arg1 = val;
                inst.arg2 = "";
                otimizou = true;
            }
        }
        // x / 1 = x
        else if (inst.op == "/" || inst.op == "|") {
            if (inst.arg2 == "1" || inst.arg2 == "1.0") {
                inst.op = ":=";
                inst.arg2 = "";
                otimizou = true;
            }
        }
        // x ^ 0 = 1, x ^ 1 = x
        else if (inst.op == "^") {
            if (inst.arg2 == "0" || inst.arg2 == "0.0") {
                inst.op = ":=";
                inst.arg1 = "1";
                inst.arg2 = "";
                otimizou = true;
            } else if (inst.arg2 == "1" || inst.arg2 == "1.0") {
                inst.op = ":=";
                inst.arg2 = "";
                otimizou = true;
            }
        }
        
        if (otimizou) {
            log("[Algebraic Simpl.] Linha " + to_string(i) + ": " + antigo + " --> " + formatarInstrucaoTAC(inst));
            stats.algebraicSimplification++;
            houveOtimizacao = true;
        }
    }
}

// =============================================================================
// 3. CONSTANT PROPAGATION - Propaga valores constantes
// =============================================================================
void OtimizadorTAC::constantPropagation() {
    map<string, string> valoresConhecidos;

    for (size_t i = 0; i < codigo.size(); i++) {
        InstrucaoTAC& inst = codigo[i];
        
        // Labels invalidam conhecimento (pode vir de qualquer lugar)
        if (inst.op == "LABEL") {
            valoresConhecidos.clear();
            continue;
        }

        // Substitui arg1 se conhecido (apenas temporarios)
        if (valoresConhecidos.count(inst.arg1) && ehVariavelTemporaria(inst.arg1)) {
            log("[Const. Propagation] Linha " + to_string(i) + ": " + inst.arg1 + " --> " + valoresConhecidos[inst.arg1]);
            inst.arg1 = valoresConhecidos[inst.arg1];
            stats.constantPropagation++;
            houveOtimizacao = true;
        }

        // Substitui arg2 se conhecido (apenas temporarios)
        if (valoresConhecidos.count(inst.arg2) && ehVariavelTemporaria(inst.arg2)) {
            log("[Const. Propagation] Linha " + to_string(i) + ": " + inst.arg2 + " --> " + valoresConhecidos[inst.arg2]);
            inst.arg2 = valoresConhecidos[inst.arg2];
            stats.constantPropagation++;
            houveOtimizacao = true;
        }

        // Guarda valor se for atribuicao de constante ou temporario
        if (inst.op == ":=" && (ehNumero(inst.arg1) || ehVariavelTemporaria(inst.arg1))) {
            valoresConhecidos[inst.result] = inst.arg1;
        }
        else if (!inst.result.empty()) {
            valoresConhecidos.erase(inst.result);
        }
    }
}

// =============================================================================
// 4. DEAD CODE ELIMINATION - Remove codigo morto
// =============================================================================
void OtimizadorTAC::deadCodeElimination() {
    set<string> usados;
    
    // Coleta variaveis usadas
    for (const auto& inst : codigo) {
        if (!inst.arg1.empty()) usados.insert(inst.arg1);
        if (!inst.arg2.empty()) usados.insert(inst.arg2);
    }
    
    vector<InstrucaoTAC> novoCodigo;
    
    for (size_t i = 0; i < codigo.size(); i++) {
        const InstrucaoTAC& inst = codigo[i];
        
        // Remove temporario nao usado
        if (ehVariavelTemporaria(inst.result) && 
            usados.find(inst.result) == usados.end() && 
            inst.op != "LABEL") {
            log("[Dead Code Elim.] Removendo linha " + to_string(i) + ": " + formatarInstrucaoTAC(inst));
            stats.deadCodeElimination++;
            houveOtimizacao = true;
        } else {
            novoCodigo.push_back(inst);
        }
    }
    
    codigo = novoCodigo;
}

// =============================================================================
// 5. REDUNDANT JUMP ELIMINATION - Remove saltos redundantes
// =============================================================================
void OtimizadorTAC::redundantJumpElimination() {
    // Coleta labels usados
    set<string> labelsUsados;
    for (const auto& inst : codigo) {
        if (inst.op == "GOTO") labelsUsados.insert(inst.result);
        if (inst.op == "IF_FALSE") labelsUsados.insert(inst.result);
    }
    
    vector<InstrucaoTAC> novoCodigo;
    
    for (size_t i = 0; i < codigo.size(); i++) {
        const InstrucaoTAC& inst = codigo[i];
        
        // GOTO para proxima instrucao (label logo apos)
        if (inst.op == "GOTO" && i + 1 < codigo.size()) {
            if (codigo[i + 1].op == "LABEL" && codigo[i + 1].result == inst.result) {
                log("[Jump Elim.] Removendo GOTO redundante linha " + to_string(i) + ": " + formatarInstrucaoTAC(inst));
                stats.redundantJumpElimination++;
                houveOtimizacao = true;
                continue;
            }
        }
        
        // Label nao referenciado
        if (inst.op == "LABEL" && labelsUsados.find(inst.result) == labelsUsados.end()) {
            log("[Jump Elim.] Removendo label nao usado linha " + to_string(i) + ": " + inst.result);
            stats.redundantJumpElimination++;
            houveOtimizacao = true;
            continue;
        }
        
        novoCodigo.push_back(inst);
    }
    
    codigo = novoCodigo;
}

vector<InstrucaoTAC> OtimizadorTAC::obterCodigo() const {
    return codigo;
}

EstatisticasOtimizacao OtimizadorTAC::obterEstatisticas() const {
    return stats;
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
        file << i << ": " << formatarInstrucaoTAC(codigo[i]) << "\n";
    }
    file.close();
    cout << "TAC Otimizado salvo em: " << nomeArquivo << endl;
}

// =============================================================================
// GERACAO DO RELATORIO EM MARKDOWN
// =============================================================================
void OtimizadorTAC::gerarRelatorioMarkdown(const string& nomeArquivo) {
    ofstream file(nomeArquivo);
    if (!file.is_open()) {
        cerr << "Erro ao criar arquivo: " << nomeArquivo << endl;
        return;
    }
    
    // Cabecalho
    file << "# Relatorio de Otimizacoes do Codigo TAC\n\n";
    file << "**Grupo:** RA3_2  \n";
    file << "**Integrante:** Guilherme Knapik (kingnapik)  \n\n";
    
    file << "---\n\n";
    
    // Estatisticas gerais
    file << "## Resumo das Otimizacoes\n\n";
    file << "| Metrica | Valor |\n";
    file << "|---------|-------|\n";
    file << "| Instrucoes originais | " << stats.instrucoesOriginais << " |\n";
    file << "| Instrucoes finais | " << stats.instrucoesFinais << " |\n";
    file << "| Instrucoes removidas | " << (stats.instrucoesOriginais - stats.instrucoesFinais) << " |\n";
    file << "| Reducao | " << fixed << setprecision(1) 
         << (stats.instrucoesOriginais > 0 ? 
             100.0 * (stats.instrucoesOriginais - stats.instrucoesFinais) / stats.instrucoesOriginais : 0)
         << "% |\n";
    file << "| Total de passadas | " << stats.totalPassadas << " |\n\n";
    
    // Contagem por tecnica
    file << "## Otimizacoes por Tecnica\n\n";
    file << "| Tecnica | Aplicacoes |\n";
    file << "|---------|------------|\n";
    file << "| Constant Folding | " << stats.constantFolding << " |\n";
    file << "| Algebraic Simplification | " << stats.algebraicSimplification << " |\n";
    file << "| Constant Propagation | " << stats.constantPropagation << " |\n";
    file << "| Dead Code Elimination | " << stats.deadCodeElimination << " |\n";
    file << "| Redundant Jump Elimination | " << stats.redundantJumpElimination << " |\n";
    file << "| **Total** | **" << (stats.constantFolding + stats.algebraicSimplification + 
                                    stats.constantPropagation + stats.deadCodeElimination + 
                                    stats.redundantJumpElimination) << "** |\n\n";
    
    // Descricao das tecnicas
    file << "---\n\n";
    file << "## Descricao das Tecnicas Implementadas\n\n";
    
    file << "### 1. Constant Folding (Dobra de Constantes)\n\n";
    file << "Avalia expressoes com operandos constantes em tempo de compilacao.\n\n";
    file << "**Exemplo:**\n";
    file << "```\n";
    file << "ANTES:  t0 = 2.0 + 3.0\n";
    file << "DEPOIS: t0 = 5\n";
    file << "```\n\n";
    
    file << "### 2. Algebraic Simplification (Simplificacao Algebrica)\n\n";
    file << "Aplica identidades algebricas para simplificar expressoes.\n\n";
    file << "**Identidades aplicadas:**\n";
    file << "- `x + 0 = x`\n";
    file << "- `x - 0 = x`\n";
    file << "- `x * 1 = x`\n";
    file << "- `x * 0 = 0`\n";
    file << "- `x / 1 = x`\n";
    file << "- `x ^ 0 = 1`\n";
    file << "- `x ^ 1 = x`\n\n";
    
    file << "### 3. Constant Propagation (Propagacao de Constantes)\n\n";
    file << "Substitui variaveis por seus valores constantes conhecidos.\n\n";
    file << "**Exemplo:**\n";
    file << "```\n";
    file << "ANTES:  t0 = 5\n";
    file << "        t1 = t0 + 3\n";
    file << "DEPOIS: t0 = 5\n";
    file << "        t1 = 5 + 3\n";
    file << "```\n\n";
    
    file << "### 4. Dead Code Elimination (Eliminacao de Codigo Morto)\n\n";
    file << "Remove instrucoes cujos resultados nunca sao utilizados.\n\n";
    file << "**Exemplo:**\n";
    file << "```\n";
    file << "ANTES:  t0 = A + B    ; t0 nunca e usado depois\n";
    file << "        t1 = C + D\n";
    file << "DEPOIS: t1 = C + D\n";
    file << "```\n\n";
    
    file << "### 5. Redundant Jump Elimination (Eliminacao de Saltos Redundantes)\n\n";
    file << "Remove saltos desnecessarios e labels nao referenciados.\n\n";
    file << "**Casos tratados:**\n";
    file << "- GOTO para a proxima instrucao\n";
    file << "- Labels nao referenciados por nenhum GOTO ou IF_FALSE\n\n";
    
    // Log detalhado
    file << "---\n\n";
    file << "## Log Detalhado das Otimizacoes\n\n";
    file << "```\n";
    for (const auto& linha : relatorio) {
        file << linha << "\n";
    }
    file << "```\n\n";
    
    // Codigo original vs otimizado
    file << "---\n\n";
    file << "## Comparacao: Codigo Original vs Otimizado\n\n";
    
    file << "### Codigo TAC Original\n\n";
    file << "```\n";
    for (size_t i = 0; i < codigoOriginal.size(); i++) {
        file << i << ": " << formatarInstrucaoTAC(codigoOriginal[i]) << "\n";
    }
    file << "```\n\n";
    
    file << "### Codigo TAC Otimizado\n\n";
    file << "```\n";
    for (size_t i = 0; i < codigo.size(); i++) {
        file << i << ": " << formatarInstrucaoTAC(codigo[i]) << "\n";
    }
    file << "```\n\n";
    
    file << "---\n\n";
    file << "*Relatorio gerado automaticamente pelo compilador RPN.*\n";
    
    file.close();
    cout << "Relatorio Markdown gerado: " << nomeArquivo << endl;
}