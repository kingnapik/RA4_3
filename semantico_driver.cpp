// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "semantico.h"
#include <iostream>

AnalisadorSemantico::AnalisadorSemantico() : linhaAtual(0) {}

const vector<ErroSemantico>& AnalisadorSemantico::getErros() const {
    return erros;
}

void AnalisadorSemantico::limpar() {
    erros.clear();
    tabelaSimbolos.limpar();
    linhaAtual = 0;
}

// Registra um erro semantico
void AnalisadorSemantico::registrarErro(string mensagem, string contexto) {
    erros.push_back(ErroSemantico(linhaAtual, mensagem, contexto));
    std::cerr << "ERRO SEMANTICO [Linha " << linhaAtual << "]: " << mensagem << "\n";
    if (!contexto.empty()) std::cerr << "Contexto: " << contexto << "\n";
}

// Registra uma deducao de tipo
void AnalisadorSemantico::registrarDeducao(string expressao, string tipo, string regra) {
    deducoes.push_back(DeducaoTipo(linhaAtual, expressao, tipo, regra));
}

// Analisadores
bool AnalisadorSemantico::analisarSemantica(NoArvore* raiz, int numeroLinha) {
    linhaAtual = numeroLinha;

    if (!raiz) {
        registrarErro("Arvore sintatica vazia", "");
        return false;
    }

    string tipoResultado = inferirTipo(raiz);
    tabelaSimbolos.adicionarResultadoLinha(tipoResultado, numeroLinha, tipoResultado != "erro");
    return erros.empty();
}

bool AnalisadorSemantico::analisarSemanticaMemoria(NoArvore* arvoreSintatica) {
    if (!arvoreSintatica) return true;

    bool memoriaOk = true;
    string tipoRetorno = "";

    if (processarComandoMEM(arvoreSintatica, tipoRetorno)) {
        return tipoRetorno != "erro";
    }

    if (processarComandoRES(arvoreSintatica, tipoRetorno)) {
        return tipoRetorno != "erro";
    }

    if (arvoreSintatica->simbolo == "E" || arvoreSintatica->simbolo == "E_ESPECIAL") {
        for (auto filho : arvoreSintatica->filhos) {
            if (filho && !filho->simbolo.empty() && isupper(filho->simbolo[0])) {
                string nomeVar = filho->simbolo;

                if (nomeVar == "P" || nomeVar == "CORPO" || nomeVar == "CORPO'" ||
                    nomeVar == "E" || nomeVar == "E_ESPECIAL" || nomeVar == "E_ARITIMETICO" ||
                    nomeVar == "OP" || nomeVar == "OP_ARIT" || nomeVar == "OP_REL" ||
                    nomeVar == "MEM" || nomeVar == "RES" || nomeVar == "IF" || nomeVar == "FOR") {
                    continue;
                }

                if (!tabelaSimbolos.existeSimbolo(nomeVar)) {
                    registrarErro("Variavel '" + nomeVar + "' nao foi declarada", "(" + nomeVar + ")");
                    memoriaOk = false;
                } else if (!tabelaSimbolos.simboloInicializado(nomeVar)) {
                    registrarErro("Variavel '" + nomeVar + "' utilizada sem inicializacao", "(" + nomeVar + ")");
                    memoriaOk = false;
                }
            }
        }
    }

    for (auto filho : arvoreSintatica->filhos)
        if (!analisarSemanticaMemoria(filho)) memoriaOk = false;

    return memoriaOk;
}

bool AnalisadorSemantico::analisarSemanticaControle(NoArvore* arvoreSintatica) {
    if (!arvoreSintatica) return true;

    bool controleOk = true;

    for (size_t i = 0; i < arvoreSintatica->filhos.size(); i++) {
        NoArvore* filho = arvoreSintatica->filhos[i];

        if (filho && filho->simbolo == "IF") {
            registrarDeducao("IF encontrado", "marker", "Marcador IF");
            if (i >= 3) {
                NoArvore* condicao = arvoreSintatica->filhos[i-3];
                string tipoCondicao = inferirTipo(condicao);
                if (tipoCondicao != "booleano" && tipoCondicao != "erro") {
                    registrarErro("IF requer condicao booleana, encontrado: " + tipoCondicao, "IF");
                    controleOk = false;
                }
            }
        }

        if (filho && filho->simbolo == "FOR") {
            registrarDeducao("FOR encontrado", "marker", "Marcador FOR");
            if (i >= 2) {
                NoArvore* inicio = arvoreSintatica->filhos[i-2];
                NoArvore* fim = arvoreSintatica->filhos[i-1];

                string tipoInicio = inferirTipo(inicio);
                string tipoFim = inferirTipo(fim);

                if (tipoInicio != "int" && tipoInicio != "erro") {
                    registrarErro("FOR requer inicio inteiro, encontrado: " + tipoInicio, "FOR");
                    controleOk = false;
                }
                if (tipoFim != "int" && tipoFim != "erro") {
                    registrarErro("FOR requer fim inteiro, encontrado: " + tipoFim, "FOR");
                    controleOk = false;
                }
            }
        }
    }

    for (auto filho : arvoreSintatica->filhos)
        if (!analisarSemanticaControle(filho)) controleOk = false;

    return controleOk;
}

void AnalisadorSemantico::inicializarTabelaSimbolos() {
    tabelaSimbolos.limpar();
}

void AnalisadorSemantico::adicionarSimbolo(string nome, string tipo, int linha) {
    tabelaSimbolos.adicionarSimbolo(nome, tipo, linha);
}

Simbolo AnalisadorSemantico::buscarSimbolo(string nome) const {
    return tabelaSimbolos.buscarSimbolo(nome);
}