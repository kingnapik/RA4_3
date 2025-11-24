// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "arvore.h"
#include "tabela_simbolos.h"
#include <string>
#include <vector>
#include <utility>

using namespace std;

// Estrutura para representar um erro semantico
struct ErroSemantico {
    int linha;
    string mensagem;
    string contexto;

    ErroSemantico(int l, string m, string c)
        : linha(l), mensagem(m), contexto(c) {}
};

// Estrutura para representar uma deducao de tipo
struct DeducaoTipo {
    int linha;
    string expressao;
    string tipoInferido;
    string regra;

    DeducaoTipo(int l, string e, string t, string r)
        : linha(l), expressao(e), tipoInferido(t), regra(r) {}
};

// Classe principal do analisador semantico
class AnalisadorSemantico {
private:
    TabelaSimbolos tabelaSimbolos;
    vector<ErroSemantico> erros;
    vector<DeducaoTipo> deducoes;
    int linhaAtual;

    // Funcoes auxiliares privadas
    bool processarComandoMEM(NoArvore* no, string& tipoRetorno);
    bool processarComandoRES(NoArvore* no, string& tipoRetorno);
    string promoverTipo(string tipo1, string tipo2);
    bool tipoCompativel(string tipo1, string tipo2);
    string inferirTipo(NoArvore* no);
    void registrarErro(string mensagem, string contexto);
    void registrarDeducao(string expressao, string tipo, string regra);

public:
    AnalisadorSemantico();

    // >>> Getter publico para o main.cpp
    const vector<ErroSemantico>& getErros() const;

    // Funcao principal de analise semantica (tipagem + anotacao)
    bool analisarSemantica(NoArvore* raiz, int numeroLinha);

    // Validacoes especificas
    bool analisarSemanticaMemoria(NoArvore* arvoreSintatica);
    bool analisarSemanticaControle(NoArvore* arvoreSintatica);

    // Gramática de atributos (gera arquivo)
    void definirGramaticaAtributos();
    void gerarGramaticaAtributos(string nomeArquivo);

    // Saidas de arvore atribuida
    void gerarArvoreAtribuida(NoArvore* arvoreAnotada, int numeroLinha);
    void gerarArvoreAtribuidaJSON(NoArvore* arvoreAnotada, int numeroLinha);

    void gerarArvoresAtribuidasJSONConsolidado(
        const vector<pair<NoArvore*, int>>& arvores,
        const string& nomeArquivo);

    // Relatorios
    void gerarRelatorioErros(string nomeArquivo);
    void gerarRelatorioTipos(string nomeArquivo);

    // Utilitarios
    void limpar();

    // Wrappers com os nomes padrao do enunciado
    void inicializarTabelaSimbolos();
    void adicionarSimbolo(string nome, string tipo, int linha);
    Simbolo buscarSimbolo(string nome) const;
};

// Funcoes auxiliares globais
string obterTipoToken(const string& token);
bool ehNumeroInteiro(const string& token);
bool ehNumeroReal(const string& token);
bool ehOperadorAritmetico(const string& op);
bool ehOperadorRelacional(const string& op);

#endif
