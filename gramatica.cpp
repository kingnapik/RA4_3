// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "gramatica.h"
#include <iostream>
#include <fstream>
#include <algorithm>

const string EPSILON = "vazio";
const string END_MARKER = "$";

bool todosNullable(const Production& prod, const set<string>& nullable, const set<string>& naoTerminais) { //helper
    for (const auto& sym : prod) {
        if (naoTerminais.count(sym)) { //e n-terminal?
            if (!nullable.count(sym)) return false; //e nao nullable
        } else {
            return false; //terminal n pode ser nullable
        }
    }
    return true;// valido e e nullable
}//ex: " corpo' -> e corpo' " se "e" e "corpo'" sao nullable, entao e true, se nao forem, retorna false

void calcularNullable(Gramatica& g) {
    bool changed = true;
    while (changed) { //olha ate nao ter mudancas
        changed = false;
        for (const auto& kv : g.regras) {
            const string& A = kv.first; //nome do n-terminal
            if (g.nullable.count(A)) continue; //se ja nullable, pula
            
            for (const auto& prod : kv.second) { //verifica producao
                // 2 casos possiveis para nullable
                if (prod.empty() || (prod.size() == 1 && prod[0] == EPSILON)) { //deriva direto para epsilon
                    g.nullable.insert(A);
                    changed = true;
                    break;
                }
                
                if (todosNullable(prod, g.nullable, g.naoTerminais)) { //todos os simbolos sao nullable
                    g.nullable.insert(A);
                    changed = true;
                    break;
                }
            }
        }
    }
}

set<string> firstOfSequence(const vector<string>& seq, const Gramatica& g) { //calculando first da sequencia
    set<string> result;
    bool allNullableSoFar = true;
    //se for terminal
    for (const auto& sym : seq) {
        if (g.terminais.count(sym)) {
            result.insert(sym); //add do terminal
            allNullableSoFar = false;
            break; //e para
        }
        //se for nao terminal
        if (g.naoTerminais.count(sym)) {
            for (const auto& s : g.first.at(sym)) { //add o FIRST dele menos epsilon
                if (s != EPSILON) {
                    result.insert(s);
                }
            }
            if (!g.nullable.count(sym)) {//se for n-nullable
                allNullableSoFar = false;
                break;//para
            }
        }//se for nullable, continua
    }
    
    if (allNullableSoFar) {//add epsilon se nullable
        result.insert(EPSILON);
    }
    
    return result;
}

void calcularFirst(Gramatica& g) { //calculo do FIRST
    for (const auto& A : g.naoTerminais) {
        g.first[A] = {};//comeca vazio
    }
    
    bool changed = true;
    while (changed) {//enquanto tiver trocando, nao e first, quando parar e o firts
        changed = false;
        for (const auto& kv : g.regras) {
            const string& A = kv.first;//n-terminal
            for (const auto& prod : kv.second) {//para cada producao
                for (size_t i = 0; i < prod.size(); ++i) {//para cada simbolo naquela producao
                    const string& Xi = prod[i];
                    //caso 1: Xi e termina
                    if (g.terminais.count(Xi)) {
                        if (!g.first[A].count(Xi)) {
                            g.first[A].insert(Xi);
                            changed = true;
                        }
                        break;//feito com essa producao
                    }
                    //caso 2: se Xi e n-terminal
                    if (g.naoTerminais.count(Xi)) {
                        for (const auto& s : g.first[Xi]) {//pega first do Xi menos epsilon
                            if (s != EPSILON) {
                                if (!g.first[A].count(s)) {
                                    g.first[A].insert(s);
                                    changed = true;
                                }
                            }
                        }
                        if (!g.nullable.count(Xi)) {//se nao nullable
                            break;//para
                        }
                    }
                    
                    if (i == prod.size() - 1) {//se chegar no fim, e nullable e adiciona epsilon
                        if (!g.first[A].count(EPSILON)) {
                            g.first[A].insert(EPSILON);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

void calcularFollow(Gramatica& g) {
    for (const auto& A : g.naoTerminais) {//igual, inicia vazio
        g.follow[A] = {};
    }
    
    g.follow[g.simboloInicial].insert(END_MARKER);//inicia P com $
    
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& kv : g.regras) {
            const string& A = kv.first;//olha o lado direito da expressao
            for (const auto& prod : kv.second) {//pra cada producao
                for (size_t i = 0; i < prod.size(); ++i) {//para cada simbolo
                    const string& B = prod[i];
                    if (!g.naoTerminais.count(B)) continue;//pula terminais
                    
                    vector<string> beta;//beta e tudo depois de B
                    for (size_t j = i + 1; j < prod.size(); ++j) {
                        beta.push_back(prod[j]);
                    }
                    
                    if (!beta.empty()) {//FIRST(beta) sem o epsilon aŠ† FOLLOW(B)
                        auto firstBeta = firstOfSequence(beta, g);
                        
                        for (const auto& s : firstBeta) {
                            if (s != EPSILON) {
                                if (!g.follow[B].count(s)) {
                                    g.follow[B].insert(s);
                                    changed = true;
                                }
                            }
                        }
                        
                        if (firstBeta.count(EPSILON)) {//se tiver epsilon no first beta FOLLOW(A) aŠ† FOLLOW(B)
                            for (const auto& s : g.follow[A]) {
                                if (!g.follow[B].count(s)) {
                                    g.follow[B].insert(s);
                                    changed = true;
                                }
                            }
                        }
                    } else {
                        for (const auto& s : g.follow[A]) { //B no final, FOLLOW(A) aŠ† FOLLOW(B)
                            if (!g.follow[B].count(s)) {
                                g.follow[B].insert(s);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

void construirTabelaLL1(Gramatica& g) {
    for (const auto& kv : g.regras) {
        const string& A = kv.first;
        for (const auto& prod : kv.second) {
            set<string> firstProd = firstOfSequence(prod, g);//pega o first da sequencia atual
            
            for (const auto& t : firstProd) {//para cada terminal nesta first
                if (t != EPSILON && g.terminais.count(t)) {
                    auto key = make_pair(A, t);
                    if (g.tabelaLL1.count(key)) {
                        cerr << "CONFLITO LL(1): (" << A << ", " << t << ")" << endl;
                    }
                    g.tabelaLL1[key] = prod;//prod e a coordenada tabela[A,t]
                }
            }
            
            if (firstProd.count(EPSILON)) { //se tiver epsilon no first
                for (const auto& t : g.follow[A]) {//para cada terminal em follow do item
                    if (g.terminais.count(t) || t == END_MARKER) {
                        auto key = make_pair(A, t);
                        if (g.tabelaLL1.count(key)) {
                            cerr << "CONFLITO LL(1): (" << A << ", " << t << ")" << endl;
                        }
                        g.tabelaLL1[key] = prod;
                    }
                }
            }
        }
    }
}

void salvarAnaliseGramatica(const Gramatica& g) {//salvando no arquivo
    vector<string> ordem = {"P", "CORPO", "CORPO'", "E", "E_ARITIMETICO", "E_ESPECIAL", "OP"};
    
    // ARQUIVO MARKDOWN
    ofstream fileMd("analise_gramatica.md");
    if (!fileMd.is_open()) {
        cerr << "Erro ao criar arquivo: analise_gramatica.md" << endl;
        return;
    }
    
    fileMd << "# Analise da Gramatica\n\n";
    fileMd << "## Regras de Producao (EBNF)\n\n";
    
    for (const auto& naoTerminal : ordem) {
        if (!g.regras.count(naoTerminal)) continue;
        
        const auto& producoes = g.regras.at(naoTerminal);
        bool first = true;
        for (const auto& prod : producoes) {
            if (first) {
                fileMd << naoTerminal << " -> ";
                first = false;
            } else {
                fileMd << string(naoTerminal.length(), ' ') << " | ";
            }
            
            if (prod.empty() || (prod.size() == 1 && prod[0] == EPSILON)) {
                fileMd << "epsilon";
            } else {
                for (size_t i = 0; i < prod.size(); ++i) {
                    if (i > 0) fileMd << " ";
                    fileMd << prod[i];
                }
            }
            fileMd << "\n";
        }
        fileMd << "\n";
    }
    
    fileMd << "\n## Conjunto NULLABLE\n\n```\n";
    fileMd << "NULLABLE = { ";
    for (const auto& n : g.nullable) {
        fileMd << n << " ";
    }
    fileMd << "}\n```\n\n";
    
    fileMd << "## Conjuntos FIRST\n\n```\n";
    for (const auto& nt : ordem) {
        if (!g.first.count(nt)) continue;
        fileMd << "FIRST(" << nt << ") = { ";
        for (const auto& t : g.first.at(nt)) {
            fileMd << t << " ";
        }
        fileMd << "}\n";
    }
    fileMd << "```\n\n";
    
    fileMd << "## Conjuntos FOLLOW\n\n```\n";
    for (const auto& nt : ordem) {
        if (!g.follow.count(nt)) continue;
        fileMd << "FOLLOW(" << nt << ") = { ";
        for (const auto& t : g.follow.at(nt)) {
            fileMd << t << " ";
        }
        fileMd << "}\n";
    }
    fileMd << "```\n\n";
    
    fileMd << "## Tabela de Analise LL(1)\n\n";
    
    vector<string> terminaisOrdenados(g.terminais.begin(), g.terminais.end());
    sort(terminaisOrdenados.begin(), terminaisOrdenados.end());
    
    fileMd << "| Nao-Terminal |";
    for (const auto& t : terminaisOrdenados) {
        fileMd << " " << t << " |";
    }
    fileMd << " $ |\n";
    
    fileMd << "|--------------|";
    for (size_t i = 0; i < terminaisOrdenados.size(); ++i) {
        fileMd << "------|";
    }
    fileMd << "------|\n";
    
    for (const auto& nt : ordem) {
        fileMd << "| " << nt << " |";
        
        for (const auto& t : terminaisOrdenados) {
            fileMd << " ";
            auto key = make_pair(nt, t);
            if (g.tabelaLL1.count(key)) {
                const auto& prod = g.tabelaLL1.at(key);
                fileMd << nt << " -> ";
                if (prod.empty() || (prod.size() == 1 && prod[0] == EPSILON)) {
                    fileMd << "epsilon";
                } else {
                    for (size_t i = 0; i < prod.size(); ++i) {
                        if (i > 0) fileMd << " ";
                        fileMd << prod[i];
                    }
                }
            }
            fileMd << " |";
        }
        
        fileMd << " ";
        auto key = make_pair(nt, END_MARKER);
        if (g.tabelaLL1.count(key)) {
            const auto& prod = g.tabelaLL1.at(key);
            fileMd << nt << " -> ";
            if (prod.empty() || (prod.size() == 1 && prod[0] == EPSILON)) {
                fileMd << "epsilon";
            } else {
                for (size_t i = 0; i < prod.size(); ++i) {
                    if (i > 0) fileMd << " ";
                    fileMd << prod[i];
                }
            }
        }
        fileMd << " |\n";
    }
    
    fileMd << "\n---\n*Nota: As arvores sintaticas de todas as linhas validas serao adicionadas ao final deste arquivo.*\n\n";
    fileMd.close();
    cout << "Analise da gramatica salva em: analise_gramatica.md" << endl;
    
    // ARQUIVO HTML para poder ver a tabela melhor
    ofstream fileHtml("tabela_ll1.html");
    if (!fileHtml.is_open()) {
        cerr << "Erro ao criar arquivo: tabela_ll1.html" << endl;
        return;
    }
    
    fileHtml << "<!DOCTYPE html>\n<html>\n<head>\n";
    fileHtml << "<meta charset=\"UTF-8\">\n";
    fileHtml << "<title>Tabela de Analise LL(1)</title>\n";
    fileHtml << "<style>\n";
    fileHtml << "body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }\n";
    fileHtml << "h1 { color: #333; }\n";
    fileHtml << "table { border-collapse: collapse; background: white; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n";
    fileHtml << "th { background: #4CAF50; color: white; padding: 12px; text-align: center; border: 1px solid #ddd; }\n";
    fileHtml << "td { padding: 10px; border: 1px solid #ddd; text-align: center; }\n";
    fileHtml << "td.empty { background: #f9f9f9; }\n";
    fileHtml << "td.filled { background: #e8f5e9; }\n";
    fileHtml << ".producao { font-family: 'Courier New', monospace; font-size: 13px; }\n";
    fileHtml << "</style>\n</head>\n<body>\n";
    fileHtml << "<h1>Tabela de Analise LL(1)</h1>\n";
    fileHtml << "<table>\n<tr>\n<th>Nao-Terminal</th>\n";
    
    for (const auto& t : terminaisOrdenados) {
        fileHtml << "<th>" << t << "</th>\n";
    }
    fileHtml << "<th>$</th>\n</tr>\n";
    
    for (const auto& nt : ordem) {
        fileHtml << "<tr>\n<th>" << nt << "</th>\n";
        
        for (const auto& t : terminaisOrdenados) {
            auto key = make_pair(nt, t);
            if (g.tabelaLL1.count(key)) {
                const auto& prod = g.tabelaLL1.at(key);
                fileHtml << "<td class=\"filled\"><span class=\"producao\">" << nt << " &rarr; ";
                if (prod.empty() || (prod.size() == 1 && prod[0] == EPSILON)) {
                    fileHtml << "&epsilon;";
                } else {
                    for (size_t i = 0; i < prod.size(); ++i) {
                        if (i > 0) fileHtml << " ";
                        fileHtml << prod[i];
                    }
                }
                fileHtml << "</span></td>\n";
            } else {
                fileHtml << "<td class=\"empty\"></td>\n";
            }
        }
        
        auto key = make_pair(nt, END_MARKER);
        if (g.tabelaLL1.count(key)) {
            const auto& prod = g.tabelaLL1.at(key);
            fileHtml << "<td class=\"filled\"><span class=\"producao\">" << nt << " &rarr; ";
            if (prod.empty() || (prod.size() == 1 && prod[0] == EPSILON)) {
                fileHtml << "&epsilon;";
            } else {
                for (size_t i = 0; i < prod.size(); ++i) {
                    if (i > 0) fileHtml << " ";
                    fileHtml << prod[i];
                }
            }
            fileHtml << "</span></td>\n";
        } else {
            fileHtml << "<td class=\"empty\"></td>\n";
        }
        
        fileHtml << "</tr>\n";
    }
    
    fileHtml << "</table>\n</body>\n</html>\n";
    fileHtml.close();
    cout << "Tabela LL(1) salva em: tabela_ll1.html" << endl;
}

Gramatica construirGramatica() { //gramatica hard coded
    Gramatica g;
    
    g.simboloInicial = "P";
    
    g.regras["P"] = { {"(", "CORPO", ")"} };
    g.regras["CORPO"] = { {"E", "CORPO'"} };
    g.regras["CORPO'"] = { 
        {"E", "CORPO'"},
        {EPSILON}
    };
    g.regras["E"] = {
        {"E_ARITIMETICO"},
        {"E_ESPECIAL"},
        {"OP"}
    };
    g.regras["E_ARITIMETICO"] = {
        {"num"},
        {"P"}
    };
    g.regras["E_ESPECIAL"] = {
        {"var"},
        {"res"},
        {"if"},
        {"for"}
    };
    g.regras["OP"] = {
        {"+"},
        {"-"},
        {"*"},
        {"%"},
        {"/"},
        {"|"},
        {"^"},
        {">"},
        {"<"},
        {">="},
        {"<="},
        {"=="},
        {"!="}
    };
    
    for (const auto& kv : g.regras) {
        g.naoTerminais.insert(kv.first);
    }
    
    for (const auto& kv : g.regras) {
        for (const auto& prod : kv.second) {
            for (const auto& sym : prod) {
                if (!g.naoTerminais.count(sym) && sym != EPSILON) {
                    g.terminais.insert(sym);
                }
            }
        }
    }
    
    calcularNullable(g);
    calcularFirst(g);
    calcularFollow(g);
    construirTabelaLL1(g);
    salvarAnaliseGramatica(g);
    
    cout << "\n=== GRAMATICA CONSTRUIDA ===" << endl;
    cout << "Nao-terminais: " << g.naoTerminais.size() << endl;
    cout << "Terminais: " << g.terminais.size() << endl;
    cout << "Nullable: { ";
    for (const auto& n : g.nullable) cout << n << " ";
    cout << "}" << endl;
    
    return g;
}