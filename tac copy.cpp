// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "tac.h"
#include <iostream>
#include <fstream>
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

// Funcao principal chamada pela main
void GeradorTAC::gerarTAC(NoArvore* arvoreAtribuida) {
    if (!arvoreAtribuida) return;
    limpar();
    processar(arvoreAtribuida);
}

// Achata a arvore recursiva em uma lista sequencial
void GeradorTAC::linearizarCorpo(NoArvore* no, vector<NoArvore*>& lista) {
    if (!no) return;
    
    // Navega na estrutura: CORPO -> E ... CORPO'
    for (auto filho : no->filhos) {
        if (filho->simbolo == "E") {
            lista.push_back(filho);
        } else if (filho->simbolo == "CORPO" || filho->simbolo == "CORPO'") {
            linearizarCorpo(filho, lista);
        }
    }
}

void GeradorTAC::processar(NoArvore* no) {
    if (!no) return;
    
    string sim = no->simbolo;
    
    if (sim == "P") {
        // P -> ( CORPO )
        // Procura o filho CORPO
        for(auto f : no->filhos) {
            if(f->simbolo == "CORPO") {
                processar(f);
                return;
            }
        }
    }
    else if (sim == "CORPO") {
        processarCorpo(no);
    }
    else if (sim == "E") {
        if (!no->filhos.empty()) processar(no->filhos[0]);
    }
    // Tratamento de literais e variaveis
    else if (sim == "E_ARITIMETICO" || sim == "E_ARITMETICO") { // Aceita ambas grafias
        if (!no->filhos.empty()) {
            if (no->filhos[0]->simbolo == "P") processar(no->filhos[0]); // Recursao ( P )
            else pilhaOperandos.push(no->filhos[0]->simbolo); // Numero
        }
    }
    else if (sim == "E_ESPECIAL") {
        if (!no->filhos.empty()) {
            string val = no->filhos[0]->simbolo;
            if (val == "RES") processarRES(no);
            else if (val == "IF" || val == "FOR") { /* processado no corpo */ }
            else if (isupper(val[0])) pilhaOperandos.push(val); // Variavel
        }
    }
    else if (sim == "OP") {
        processarOperacao(no);
    }
}

void GeradorTAC::processarCorpo(NoArvore* no) {
    // 1. Transformar a recursao em lista plana
    vector<NoArvore*> elementos;
    linearizarCorpo(no, elementos);
    
    if (elementos.empty()) return;

    // O ultimo elemento define o comando especial em RPN (sufixo)
    NoArvore* ultimo = elementos.back();
    string comando = "";
    
    // Descobre o que é o ultimo elemento
    if (!ultimo->filhos.empty() && !ultimo->filhos[0]->filhos.empty()) {
         // Navega E -> E_ESPECIAL -> IF/FOR/VAR
         if (ultimo->filhos[0]->simbolo == "E_ESPECIAL") {
             comando = ultimo->filhos[0]->filhos[0]->simbolo;
         }
    }

    // --- CASO 1: IF ---
    // Padrao: (COND) (THEN) (ELSE) IF
    if (comando == "IF" && elementos.size() >= 4) {
        // Elementos anteriores sao blocos P -> ( CORPO )
        NoArvore* condicao = elementos[elementos.size() - 4];
        NoArvore* blocoThen = elementos[elementos.size() - 3];
        NoArvore* blocoElse = elementos[elementos.size() - 2];
        
        string lblElse = novoLabel();
        string lblFim = novoLabel();
        
        // 1. Gera condicao
        processar(condicao);
        string tCond = pilhaOperandos.top(); pilhaOperandos.pop();
        
        // 2. Salta se falso
        codigo.push_back(InstrucaoTAC("IF_FALSE", tCond, "", lblElse));
        
        // 3. Bloco Then
        processar(blocoThen);
        codigo.push_back(InstrucaoTAC("GOTO", "", "", lblFim));
        
        // 4. Bloco Else
        codigo.push_back(InstrucaoTAC("LABEL", "", "", lblElse));
        processar(blocoElse);
        
        // 5. Fim
        codigo.push_back(InstrucaoTAC("LABEL", "", "", lblFim));
        return;
    }

    // --- CASO 2: FOR ---
    // Padrao: (COND) (BODY) FOR
    if (comando == "FOR" && elementos.size() >= 3) {
        NoArvore* condicao = elementos[elementos.size() - 3];
        NoArvore* corpoLoop = elementos[elementos.size() - 2];
        
        string lblInicio = novoLabel();
        string lblFim = novoLabel();
        
        // 1. Label inicio
        codigo.push_back(InstrucaoTAC("LABEL", "", "", lblInicio));
        
        // 2. Condicao
        processar(condicao);
        string tCond = pilhaOperandos.top(); pilhaOperandos.pop();
        
        // 3. Sai se falso
        codigo.push_back(InstrucaoTAC("IF_FALSE", tCond, "", lblFim));
        
        // 4. Corpo
        processar(corpoLoop);
        
        // 5. Volta
        codigo.push_back(InstrucaoTAC("GOTO", "", "", lblInicio));
        
        // 6. Fim
        codigo.push_back(InstrucaoTAC("LABEL", "", "", lblFim));
        return;
    }
    
    // --- CASO 3: MEM (Atribuicao) ---
    // Padrao: ( EXPRESSAO ) VARIAVEL
    // O ultimo elemento é uma Variavel e temos apenas 2 itens
    if (elementos.size() == 2 && comando != "IF" && comando != "FOR" && comando != "RES") {
        // Verifica se é variavel (letra maiuscula)
        if (!comando.empty() && isupper(comando[0])) {
            processar(elementos[0]); // Calcula valor
            string valor = pilhaOperandos.top(); pilhaOperandos.pop();
            
            // Gera: A = valor
            codigo.push_back(InstrucaoTAC(":=", valor, "", comando));
            return;
        }
    }

    // --- CASO 4: Sequencia Normal ---
    // Se nao for especial, processa um por um (ex: calculos matematicos)
    for (auto elem : elementos) {
        processar(elem);
    }
}

void GeradorTAC::processarOperacao(NoArvore* no) {
    string op = no->filhos[0]->simbolo;
    
    if (ehOperadorBinario(op) && pilhaOperandos.size() >= 2) {
        string b = pilhaOperandos.top(); pilhaOperandos.pop(); // Inverte ordem na pilha
        string a = pilhaOperandos.top(); pilhaOperandos.pop();
        
        string t = novoTemp();
        codigo.push_back(InstrucaoTAC(op, a, b, t));
        pilhaOperandos.push(t);
    }
}

void GeradorTAC::processarRES(NoArvore* no) {
    // RES funciona como uma funcao: pega argumento da pilha
    string arg = "1";
    if (!pilhaOperandos.empty()) {
        arg = pilhaOperandos.top(); pilhaOperandos.pop();
    }
    
    string t = novoTemp();
    // Gera: tX = RES(arg)
    codigo.push_back(InstrucaoTAC("RES", arg, "", t));
    pilhaOperandos.push(t);
}

bool GeradorTAC::ehOperadorBinario(const string& op) {
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "%" || op == "^" || op == "|" || // Aritméticos 
           op == ">" || op == "<" || op == ">=" || op == "<=" || op == "==" || op == "!="; // Relacionais
}

void GeradorTAC::imprimirTAC() {
    cout << "\n=== TAC ===" << endl;
    for (size_t i = 0; i < codigo.size(); i++) {
        const auto& inst = codigo[i];
        cout << i << ": ";
        if (inst.op == "LABEL") cout << inst.result << ":";
        else if (inst.op == "GOTO") cout << "GOTO " << inst.result;
        else if (inst.op == "IF_FALSE") cout << "IF_FALSE " << inst.arg1 << " GOTO " << inst.result;
        else if (inst.op == ":=") cout << inst.result << " = " << inst.arg1;
        else if (inst.op == "RES") cout << inst.result << " = RES(" << inst.arg1 << ")";
        else cout << inst.result << " = " << inst.arg1 << " " << inst.op << " " << inst.arg2;
        cout << endl;
    }
}

void GeradorTAC::salvarTAC(const string& nomeArquivo) {
    ofstream file(nomeArquivo);
    for (size_t i = 0; i < codigo.size(); i++) {
        const auto& inst = codigo[i];
        file << i << ": ";
        if (inst.op == "LABEL") file << inst.result << ":";
        else if (inst.op == "GOTO") file << "GOTO " << inst.result;
        else if (inst.op == "IF_FALSE") file << "IF_FALSE " << inst.arg1 << " GOTO " << inst.result;
        else if (inst.op == ":=") file << inst.result << " = " << inst.arg1;
        else if (inst.op == "RES") file << inst.result << " = RES(" << inst.arg1 << ")";
        else file << inst.result << " = " << inst.arg1 << " " << inst.op << " " << inst.arg2;
        file << endl;
    }
    file.close();
}

void GeradorTAC::limpar() {
    codigo.clear();
    limparPilha();
}

vector<InstrucaoTAC> GeradorTAC::obterCodigo() const {
    return codigo;
}