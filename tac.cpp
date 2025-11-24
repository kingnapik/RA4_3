// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "tac.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

GeradorTAC::GeradorTAC() : contadorTemp(0), contadorLabel(0) {}

string GeradorTAC::novoTemp() {
    return "t" + to_string(contadorTemp++);
}

string GeradorTAC::novoLabel() {
    return "L" + to_string(contadorLabel++);
}

void GeradorTAC::limparPilha() {
    while (!pilhaOperandos.empty()) {
        pilhaOperandos.pop();
    }
}

bool GeradorTAC::ehOperador(const string& simbolo) {
    return simbolo == "+" || simbolo == "-" || simbolo == "*" || simbolo == "/" ||
           simbolo == "%" || simbolo == "|" || simbolo == "^" ||
           simbolo == ">" || simbolo == "<" || simbolo == ">=" || 
           simbolo == "<=" || simbolo == "==" || simbolo == "!=";
}

bool GeradorTAC::ehOperadorBinario(const string& op) {
    return ehOperador(op);
}

bool GeradorTAC::ehOperadorRelacional(const string& op) {
    return op == ">" || op == "<" || op == ">=" || 
           op == "<=" || op == "==" || op == "!=";
}

string GeradorTAC::traduzirOperador(const string& op) {
    // Operadores ja estao no formato correto
    return op;
}

void GeradorTAC::gerarTAC(NoArvore* arvoreAtribuida) {
    if (!arvoreAtribuida) return;
    
    limpar();
    processar(arvoreAtribuida);
}

void GeradorTAC::processar(NoArvore* no) {
    if (!no) return;
    
    // P -> ( CORPO )
    if (no->simbolo == "P") {
        if (no->filhos.size() >= 3) {
            processar(no->filhos[1]); // processa CORPO
        }
    }
    // CORPO -> E CORPO'
    else if (no->simbolo == "CORPO") {
        processarCorpo(no);
    }
    // CORPO' -> E CORPO' | epsilon
    else if (no->simbolo == "CORPO'") {
        for (auto filho : no->filhos) {
            processar(filho);
        }
    }
    // E -> E_ARITMETICO | E_ESPECIAL | OP
    else if (no->simbolo == "E") {
        processarExpressao(no);
    }
    // E_ARITIMETICO -> num | P
    else if (no->simbolo == "E_ARITIMETICO") {
        if (!no->filhos.empty()) {
            NoArvore* filho = no->filhos[0];
            if (filho->simbolo == "P") {
                processar(filho);
            } else {
                // Literal numerico
                pilhaOperandos.push(filho->simbolo);
            }
        }
    }
    // E_ESPECIAL -> var | res | if | for
    else if (no->simbolo == "E_ESPECIAL") {
        if (!no->filhos.empty()) {
            NoArvore* filho = no->filhos[0];
            if (filho->simbolo == "RES") {
                processarRES(no);
            } else if (filho->simbolo == "IF") {
                processarIF(no);
            } else if (filho->simbolo == "FOR") {
                processarFOR(no);
            } else if (!filho->simbolo.empty() && isupper(filho->simbolo[0])) {
                // Variavel
                pilhaOperandos.push(filho->simbolo);
            }
        }
    }
    // OP -> operadores
    else if (no->simbolo == "OP") {
        processarOperacao(no);
    }
}

void GeradorTAC::processarExpressao(NoArvore* no) {
    if (!no || no->filhos.empty()) return;
    
    processar(no->filhos[0]);
}

void GeradorTAC::processarOperacao(NoArvore* no) {
    if (!no || no->filhos.empty()) return;
    
    string op = no->filhos[0]->simbolo;
    
    if (ehOperadorBinario(op)) {
        // Operacao binaria em RPN: pop dois operandos, push resultado
        if (pilhaOperandos.size() >= 2) {
            string op2 = pilhaOperandos.top(); pilhaOperandos.pop();
            string op1 = pilhaOperandos.top(); pilhaOperandos.pop();
            
            string temp = novoTemp();
            codigo.push_back(InstrucaoTAC(op, op1, op2, temp));
            pilhaOperandos.push(temp);
        }
    }
}

void GeradorTAC::processarCorpo(NoArvore* no) {
    if (!no) return;
    
    // Detectar padroes especiais: MEM, IF, FOR
    
    // Padrao MEM: valor variavel (sem operador depois)
    if (no->filhos.size() >= 2) {
        NoArvore* primeiroE = no->filhos[0];
        NoArvore* corpoLinha = no->filhos[1];
        
        // Verificar se e MEM (valor var)
        bool ehMEM = false;
        string nomeVar = "";
        
        if (corpoLinha && corpoLinha->filhos.size() >= 1) {
            NoArvore* segundoE = corpoLinha->filhos[0];
            
            // Verificar se nao ha mais operacoes depois
            bool temMaisOperacoes = false;
            if (corpoLinha->filhos.size() >= 2 && 
                !corpoLinha->filhos[1]->filhos.empty()) {
                NoArvore* possivelOp = corpoLinha->filhos[1]->filhos[0];
                if (possivelOp && possivelOp->simbolo == "E" &&
                    !possivelOp->filhos.empty() &&
                    possivelOp->filhos[0]->simbolo == "OP") {
                    temMaisOperacoes = true;
                }
            }
            
            if (!temMaisOperacoes && segundoE && segundoE->simbolo == "E" &&
                !segundoE->filhos.empty() &&
                segundoE->filhos[0]->simbolo == "E_ESPECIAL" &&
                !segundoE->filhos[0]->filhos.empty()) {
                
                NoArvore* varNode = segundoE->filhos[0]->filhos[0];
                if (!varNode->simbolo.empty() && isupper(varNode->simbolo[0])) {
                    ehMEM = true;
                    nomeVar = varNode->simbolo;
                }
            }
        }
        
        if (ehMEM) {
            // Processar valor
            processar(primeiroE);
            if (!pilhaOperandos.empty()) {
                string valor = pilhaOperandos.top();
                pilhaOperandos.pop();
                // Gerar atribuicao
                codigo.push_back(InstrucaoTAC(":=", valor, "", nomeVar));
            }
            return;
        }
        
        // Verificar IF/FOR em RPN
        if (corpoLinha && corpoLinha->filhos.size() >= 2) {
            //NoArvore* ultimoE = nullptr;
            for (int i = corpoLinha->filhos.size() - 1; i >= 0; i--) {
                NoArvore* elem = corpoLinha->filhos[i];
                if (elem && elem->simbolo == "E" &&
                    !elem->filhos.empty() &&
                    elem->filhos[0]->simbolo == "E_ESPECIAL" &&
                    !elem->filhos[0]->filhos.empty()) {
                    
                    string marcador = elem->filhos[0]->filhos[0]->simbolo;
                    
                    if (marcador == "IF") {
                        // Padrao IF: cond then else IF
                        processar(primeiroE); // condicao
                        string cond = "";
                        if (!pilhaOperandos.empty()) {
                            cond = pilhaOperandos.top();
                            pilhaOperandos.pop();
                        }
                        
                        string labelElse = novoLabel();
                        string labelFim = novoLabel();
                        
                        // IF_FALSE cond GOTO labelElse
                        codigo.push_back(InstrucaoTAC("IF_FALSE", cond, "", labelElse));
                        
                        // Processar bloco then (primeiro elemento de corpo')
                        if (corpoLinha->filhos.size() > 0) {
                            processar(corpoLinha->filhos[0]);
                        }
                        
                        // GOTO labelFim
                        codigo.push_back(InstrucaoTAC("GOTO", "", "", labelFim));
                        
                        // LABEL labelElse
                        codigo.push_back(InstrucaoTAC("LABEL", "", "", labelElse));
                        
                        // Processar bloco else (segundo elemento antes do IF)
                        for (int j = 1; j < i; j++) {
                            if (corpoLinha->filhos[j]->simbolo == "E") {
                                processar(corpoLinha->filhos[j]);
                                break;
                            }
                        }
                        
                        // LABEL labelFim
                        codigo.push_back(InstrucaoTAC("LABEL", "", "", labelFim));
                        return;
                    }
                    
                    if (marcador == "FOR") {
                        // Padrao FOR: inicio fim corpo FOR
                        processar(primeiroE); // inicio
                        string inicio = "";
                        if (!pilhaOperandos.empty()) {
                            inicio = pilhaOperandos.top();
                            pilhaOperandos.pop();
                        }
                        
                        if (corpoLinha->filhos.size() > 0) {
                            processar(corpoLinha->filhos[0]); // fim
                        }
                        string fim = "";
                        if (!pilhaOperandos.empty()) {
                            fim = pilhaOperandos.top();
                            pilhaOperandos.pop();
                        }
                        
                        string labelLoop = novoLabel();
                        string labelFim = novoLabel();
                        string contador = novoTemp();
                        
                        // contador = inicio
                        codigo.push_back(InstrucaoTAC(":=", inicio, "", contador));
                        
                        // LABEL labelLoop
                        codigo.push_back(InstrucaoTAC("LABEL", "", "", labelLoop));
                        
                        // t = contador <= fim
                        string tempCond = novoTemp();
                        codigo.push_back(InstrucaoTAC("<=", contador, fim, tempCond));
                        
                        // IF_FALSE t GOTO labelFim
                        codigo.push_back(InstrucaoTAC("IF_FALSE", tempCond, "", labelFim));
                        
                        // Processar corpo do loop
                        for (int j = 1; j < i; j++) {
                            if (corpoLinha->filhos[j]->simbolo == "E") {
                                processar(corpoLinha->filhos[j]);
                                break;
                            }
                        }
                        
                        // contador = contador + 1
                        string tempInc = novoTemp();
                        codigo.push_back(InstrucaoTAC("+", contador, "1", tempInc));
                        codigo.push_back(InstrucaoTAC(":=", tempInc, "", contador));
                        
                        // GOTO labelLoop
                        codigo.push_back(InstrucaoTAC("GOTO", "", "", labelLoop));
                        
                        // LABEL labelFim
                        codigo.push_back(InstrucaoTAC("LABEL", "", "", labelFim));
                        return;
                    }
                }
            }
        }
        
        // Processamento normal de expressoes binarias
        processar(primeiroE);
        if (corpoLinha) {
            for (auto filho : corpoLinha->filhos) {
                processar(filho);
            }
        }
    }
}

void GeradorTAC::processarMEM(NoArvore* no) {
    // Implementado em processarCorpo
}

void GeradorTAC::processarRES(NoArvore* no) {
    // RES N - referencia resultado de N linhas atras
    string temp = novoTemp();
    codigo.push_back(InstrucaoTAC("RES", "N", "", temp));
    pilhaOperandos.push(temp);
}

void GeradorTAC::processarIF(NoArvore* no) {
    // Implementado em processarCorpo
}

void GeradorTAC::processarFOR(NoArvore* no) {
    // Implementado em processarCorpo
}

void GeradorTAC::imprimirTAC() {
    cout << "\n=== CODIGO DE TRES ENDERECOS (TAC) ===" << endl;
    cout << "---------------------------------------" << endl;
    
    for (size_t i = 0; i < codigo.size(); i++) {
        const InstrucaoTAC& inst = codigo[i];
        
        cout << i << ": ";
        
        if (inst.op == ":=") {
            cout << inst.result << " = " << inst.arg1;
        }
        else if (inst.op == "LABEL") {
            cout << inst.result << ":";
        }
        else if (inst.op == "GOTO") {
            cout << "GOTO " << inst.result;
        }
        else if (inst.op == "IF_FALSE") {
            cout << "IF_FALSE " << inst.arg1 << " GOTO " << inst.result;
        }
        else if (inst.op == "RES") {
            cout << inst.result << " = RES(" << inst.arg1 << ")";
        }
        else if (ehOperadorBinario(inst.op)) {
            cout << inst.result << " = " << inst.arg1 << " " << inst.op << " " << inst.arg2;
        }
        else {
            cout << inst.op << " " << inst.arg1 << " " << inst.arg2 << " " << inst.result;
        }
        
        cout << endl;
    }
    
    cout << "---------------------------------------" << endl;
    cout << "Total de instrucoes: " << codigo.size() << endl;
}

void GeradorTAC::salvarTAC(const string& nomeArquivo) {
    ofstream file(nomeArquivo);
    if (!file.is_open()) {
        cerr << "Erro ao criar arquivo: " << nomeArquivo << endl;
        return;
    }
    
    file << "# CODIGO DE TRES ENDERECOS (TAC)\n";
    file << "# Gerado pelo compilador RPN\n\n";
    
    for (size_t i = 0; i < codigo.size(); i++) {
        const InstrucaoTAC& inst = codigo[i];
        
        file << i << ": ";
        
        if (inst.op == ":=") {
            file << inst.result << " = " << inst.arg1;
        }
        else if (inst.op == "LABEL") {
            file << inst.result << ":";
        }
        else if (inst.op == "GOTO") {
            file << "GOTO " << inst.result;
        }
        else if (inst.op == "IF_FALSE") {
            file << "IF_FALSE " << inst.arg1 << " GOTO " << inst.result;
        }
        else if (inst.op == "RES") {
            file << inst.result << " = RES(" << inst.arg1 << ")";
        }
        else if (ehOperadorBinario(inst.op)) {
            file << inst.result << " = " << inst.arg1 << " " << inst.op << " " << inst.arg2;
        }
        else {
            file << inst.op << " " << inst.arg1 << " " << inst.arg2 << " " << inst.result;
        }
        
        file << "\n";
    }
    
    file.close();
    cout << "Codigo TAC salvo em: " << nomeArquivo << endl;
}

vector<InstrucaoTAC> GeradorTAC::obterCodigo() const {
    return codigo;
}

void GeradorTAC::limpar() {
    codigo.clear();
    contadorTemp = 0;
    contadorLabel = 0;
    limparPilha();
}

int GeradorTAC::tamanho() const {
    return codigo.size();
}