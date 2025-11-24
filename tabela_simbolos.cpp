// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "tabela_simbolos.h"
#include <iomanip>

TabelaSimbolos::TabelaSimbolos() {}

// adiciona simbolo: por padrao NAO inicializada
void TabelaSimbolos::adicionarSimbolo(string nome, string tipo, int linha) {
    Simbolo novo(nome, tipo, false, linha);
    simbolos[nome] = novo;
}

bool TabelaSimbolos::existeSimbolo(string nome) const {
    return simbolos.find(nome) != simbolos.end();
}

Simbolo TabelaSimbolos::buscarSimbolo(string nome) const {
    auto it = simbolos.find(nome);
    if (it != simbolos.end()) return it->second;
    return Simbolo(); // default: nome="", inicializada=false
}

void TabelaSimbolos::atualizarSimbolo(string nome, bool inicializada) {
    auto it = simbolos.find(nome);
    if (it != simbolos.end()) {
        it->second.inicializada = inicializada;
    }
}

bool TabelaSimbolos::simboloInicializado(string nome) const {
    auto it = simbolos.find(nome);
    if (it == simbolos.end()) return false;
    return it->second.inicializada;
}

// ------ RES n ------
void TabelaSimbolos::adicionarResultadoLinha(string tipo, int linha, bool valido) {
    if (linha < 0) return;
    if ((int)resultadosLinhas.size() <= linha) {
        resultadosLinhas.resize(linha + 1);
    }
    resultadosLinhas[linha] = ResultadoLinha(tipo, linha, valido);
}

ResultadoLinha TabelaSimbolos::buscarResultadoLinha(int n) const {
    if (n >= 0 && n < (int)resultadosLinhas.size()) {
        return resultadosLinhas[n];
    }
    return ResultadoLinha(); // valido=false
}

bool TabelaSimbolos::existeResultadoLinha(int n) const {
    return (n >= 0 && n < (int)resultadosLinhas.size() && resultadosLinhas[n].valido);
}

// ------ util ------
void TabelaSimbolos::limpar() {
    simbolos.clear();
    resultadosLinhas.clear();
}

int TabelaSimbolos::quantidadeSimbolos() const {
    return (int)simbolos.size();
}

void TabelaSimbolos::imprimirTabela() const {
    cout << "\n===== Tabela de Simbolos =====\n";
    cout << left << setw(15) << "Nome"
         << setw(10) << "Tipo"
         << setw(15) << "Inicializada"
         << setw(10) << "Linha" << "\n";
    cout << string(50, '-') << "\n";

    for (const auto& par : simbolos) {
        const Simbolo& s = par.second;
        cout << left << setw(15) << s.nome
             << setw(10) << s.tipo
             << setw(15) << (s.inicializada ? "Sim" : "Nao")
             << setw(10) << s.linhaDeclaracao << "\n";
    }

    cout << "Total de simbolos: " << simbolos.size() << "\n";
    cout << "==============================\n";
}
