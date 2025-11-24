// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "semantico.h"
#include <fstream>
#include <set>
#include <functional>

void AnalisadorSemantico::gerarRelatorioErros(string nomeArquivo) {
    ofstream file(nomeArquivo);
    if (!file.is_open()) {
        cerr << "Erro ao criar arquivo: " << nomeArquivo << endl;
        return;
    }

    file << "# Relatorio de Erros Semanticos\n\n";

    if (erros.empty()) {
        file << "Nenhum erro semantico encontrado.\n";
    } else {
        file << "Total de erros: " << erros.size() << "\n\n";
        for (const auto& erro : erros) {
            file << "## Erro na Linha " << erro.linha << "\n\n";
            file << "**Mensagem:** " << erro.mensagem << "\n\n";
            if (!erro.contexto.empty()) {
                file << "**Contexto:** `" << erro.contexto << "`\n\n";
            }
            file << "---\n\n";
        }
    }

    file.close();
    cout << "Relatorio de erros gerado: " << nomeArquivo << endl;
}

void AnalisadorSemantico::gerarRelatorioTipos(string nomeArquivo) {
    ofstream file(nomeArquivo);
    if (!file.is_open()) {
        cerr << "Erro ao criar arquivo: " << nomeArquivo << endl;
        return;
    }

    file << "# JULGAMENTO DE TIPOS - ARVORES DE DERIVACAO\n\n";
    file << "Este relatorio mostra ONDE e COMO cada regra de inferencia foi aplicada durante a analise.\n\n";
    file << "---\n\n";

    if (deducoes.empty()) {
        file << "## Resumo\n\n";
        file << "Nenhuma deducao de tipo foi registrada durante a analise.\n\n";
    } else {
        file << "## DEDUCOES POR LINHA\n\n";

        int linha = -1;
        for (const auto& ded : deducoes) {
            if (ded.linha != linha) {
                if (linha != -1) file << "\n---\n\n";
                file << "### LINHA " << ded.linha << "\n\n";
                linha = ded.linha;
            }

            file << "#### Deducao: `" << ded.expressao << "`\n\n";
            file << "**Tipo inferido:** `" << ded.tipoInferido << "`\n\n";
            file << "**Regra aplicada:** " << ded.regra << "\n\n";

            file << "**Arvore de derivacao:**\n";
            file << "```\n";

            if (ded.regra.find("Literal inteiro") != string::npos) {
                file << "           " << ded.expressao << " ∈ ℤ\n";
                file << "        ─────────────────────\n";
                file << "        G |- " << ded.expressao << " : int\n";
            }
            else if (ded.regra.find("Literal real") != string::npos) {
                file << "           " << ded.expressao << " ∈ ℝ\n";
                file << "        ─────────────────────\n";
                file << "        G |- " << ded.expressao << " : real\n";
            }
            else if (ded.regra.find("MEM sintetiza") != string::npos) {
                size_t pos = ded.regra.find("[");
                if (pos != string::npos) {
                    string mem = ded.regra.substr(pos);
                    file << "        G |- valor : " << ded.tipoInferido << "\n";
                    file << "        ─────────────────────────────────\n";
                    file << "        G" << mem << " |- MEM : " << ded.tipoInferido << "\n";
                }
            }
            else if (ded.regra.find("RES sintetiza") != string::npos) {
                file << "        linha existe    resultado valido\n";
                file << "        ──────────────────────────────────\n";
                file << "        G |- " << ded.expressao << " : " << ded.tipoInferido << "\n";
            }
            else if (ded.regra.find("Operador relacional sintetiza booleano") != string::npos) {
                file << "        G |- e₁ : T    G |- e₂ : T\n";
                file << "        ────────────────────────────\n";
                file << "        G |- " << ded.expressao << " : booleano\n";
            }
            else if (ded.regra.find("IF: cond:booleano") != string::npos) {
                file << "        G |- cond : booleano    G |- e1 : T    G |- e2 : T\n";
                file << "        ─────────────────────────────────────────────────\n";
                file << "                G |- (cond e1 e2 IF) : T\n";
            }
            else if (ded.regra.find("FOR: inicio/fim:int") != string::npos) {
                file << "        G |- inicio : int   G |- fim : int   G |- corpo : T\n";
                file << "        ─────────────────────────────────────────────────\n";
                file << "              G |- (inicio fim corpo FOR) : T\n";
            }
            else if (ded.regra.find("promocao") != string::npos) {
                file << "        G |- e₁ : T₁    G |- e₂ : T₂\n";
                file << "        ────────────────────────────\n";
                file << "        G |- " << ded.expressao << " : " << ded.tipoInferido << "\n";
            }
            else {
                file << "        (regra: " << ded.regra << ")\n";
                file << "        ────────────────────────────\n";
                file << "        G |- " << ded.expressao << " : " << ded.tipoInferido << "\n";
            }

            file << "```\n\n";
        }
    }

    file << "\n---\n\n";
    file << "## CATALOGO DE REGRAS APLICADAS\n\n";

    file << "### Regras de Literais\n\n";
    file << "#### (INT-LIT) - Literal Inteiro\n";
    file << "```\n";
    file << "         n ∈ ℤ\n";
    file << "    ───────────────\n";
    file << "     G |- n : int\n";
    file << "```\n\n";

    file << "#### (REAL-LIT) - Literal Real\n";
    file << "```\n";
    file << "         r ∈ ℝ\n";
    file << "    ───────────────\n";
    file << "     G |- r : real\n";
    file << "```\n\n";

    file << "### Regras de Operacoes Aritmeticas\n\n";
    file << "#### (DIV-INT) - Divisao Inteira (requer ambos int)\n";
    file << "```\n";
    file << "    G |- e₁ : int    G |- e₂ : int\n";
    file << "    ─────────────────────────────────\n";
    file << "          G |- e₁ / e₂ : int\n";
    file << "```\n\n";

    file << "#### (MOD-INT) - Modulo (requer ambos int)\n";
    file << "```\n";
    file << "    G |- e₁ : int    G |- e₂ : int\n";
    file << "    ─────────────────────────────────\n";
    file << "          G |- e₁ % e₂ : int\n";
    file << "```\n\n";

    file << "#### (POW) - Exponenciacao (expoente deve ser int)\n";
    file << "```\n";
    file << "    G |- e₁ : T    G |- e₂ : int\n";
    file << "    ────────────────────────────\n";
    file << "          G |- e₁ ^ e₂ : T\n";
    file << "```\n\n";

    file << "#### (PROMO) - Aritmeticos com promocao ( +  -  *  | )\n";
    file << "```\n";
    file << "    G |- e₁ : T₁    G |- e₂ : T₂     T₁,T₂ ∈ {int,real}\n";
    file << "    ───────────────────────────────────────────────────\n";
    file << "        G |- e₁ op e₂ : promover(T₁, T₂)\n";
    file << "```\n\n";

    file << "### Regras de Operacoes Relacionais\n\n";
    file << "#### (REL-CMP) - Comparacao (sintetiza booleano; opera sobre numeros)\n";
    file << "```\n";
    file << "    G |- e₁ : T    G |- e₂ : T    T ∈ {int,real}\n";
    file << "    op ∈ {>, <, >=, <=, ==, !=}\n";
    file << "    ─────────────────────────────────────────────\n";
    file << "             G |- e₁ op e₂ : booleano\n";
    file << "```\n\n";

    file << "### Regras de Memoria\n\n";
    file << "#### (STORE) - Armazenamento de memoria (nao aceita booleano)\n";
    file << "```\n";
    file << "    G |- v : T    T ∈ {int, real}\n";
    file << "    ───────────────────────────────\n";
    file << "      G[x ↦ T] |- (v x) : T\n";
    file << "```\n\n";

    file << "#### (MEM-LOAD) - Leitura de Memoria\n";
    file << "```\n";
    file << "    x ∈ dom(G)    G(x) = T\n";
    file << "    ─────────────────────────\n";
    file << "         G |- x : T\n";
    file << "```\n\n";

    file << "#### (RES) - RES sintetiza tipo da linha referenciada\n";
    file << "```\n";
    file << "    G |- n : int    linha_atual - n > 0    R(linha_atual - n) = T\n";
    file << "    ─────────────────────────────────────────────────────────────\n";
    file << "                         G |- n RES : T\n";
    file << "```\n\n";

    file << "### Regras de Controle\n\n";
    file << "#### (IF-THEN-ELSE) - IF sintetiza o tipo dos ramos (T)\n";
    file << "```\n";
    file << "    G |- cond : booleano    G |- e₁ : T    G |- e₂ : T\n";
    file << "    ─────────────────────────────────────────────────\n";
    file << "            G |- (cond e₁ e₂ IF) : T\n";
    file << "```\n\n";

    file << "#### (FOR-LOOP) - FOR sintetiza o tipo do corpo (T)\n";
    file << "```\n";
    file << "    G |- inicio : int    G |- fim : int    G |- corpo : T\n";
    file << "    ─────────────────────────────────────────────────\n";
    file << "            G |- (inicio fim corpo FOR) : T\n";
    file << "```\n\n";

    file << "---\n\n";
    file << "## FUNCAO DE PROMOCAO DE TIPOS\n\n";
    file << "```\n";
    file << "promover : {int,real} × {int,real} → {int,real}\n";
    file << "\n";
    file << "promover(T₁, T₂) = {\n";
    file << "    real, se T₁ = real ∨ T₂ = real\n";
    file << "    int,  se T₁ = int  ∧ T₂ = int\n";
    file << "}\n";
    file << "```\n\n";

    file << "**Exemplos:** promover(int,real)=real; promover(int,int)=int; promover(real,real)=real.\n\n";

    // estatisticas simples
    file << "---\n\n";
    file << "## ESTATISTICAS\n\n";
    file << "- **Total de deducoes:** " << deducoes.size() << "\n";

    set<int> linhasUnicas;
    for (const auto& ded : deducoes) linhasUnicas.insert(ded.linha);
    file << "- **Linhas analisadas:** " << linhasUnicas.size() << "\n\n";

    file.close();
    cout << "Relatorio de tipos gerado: " << nomeArquivo << endl;
}

void AnalisadorSemantico::gerarGramaticaAtributos(string nomeArquivo) {
    ofstream file(nomeArquivo);
    if (!file.is_open()) {
        cerr << "Erro ao criar arquivo: " << nomeArquivo << endl;
        return;
    }

    file << "# GRAMATICA DE ATRIBUTOS - LINGUAGEM RPN\n\n";

    file << "## 1. Tokens e Tipos\n\n";
    file << "- Tokens: num, var, res, if, for, '(', ')', "
            "+, -, *, /, %, |, ^, <, <=, ==, !=, >=, >.\n";
    file << "- Tipos: int, real, booleano.\n\n";

    file << "## 2. EBNF (coerente com o parser da Fase 2)\n\n";
    file << "```\n";
    file << "P        -> '(' CORPO ')'\n";
    file << "CORPO    -> E CORPO'\n";
    file << "CORPO'   -> E CORPO' | epsilon\n";
    file << "\n";
    file << "E        -> TERMO | E E OP\n";
    file << "TERMO    -> num | var | res | IF | FOR\n";
    file << "OP       -> OP_ARIT | OP_REL\n";
    file << "OP_ARIT  -> '+' | '-' | '*' | '/' | '%' | '|' | '^'\n";
    file << "OP_REL   -> '<' | '<=' | '==' | '!=' | '>=' | '>'\n";
    file << "\n";
    file << "IF       -> E E 'if'     // cond bloco (RPN)\n";
    file << "FOR      -> E E 'for'    // iter bloco (RPN)\n";
    file << "```\n\n";

    file << "> Observacao (RPN):\n";
    file << "> - **Store**: uma linha com `E var` (sem operador subsequente) armazena o valor de `E` em `var` (apenas `int`/`real`).\n";
    file << "> - **Load**: `var` sozinho carrega o valor; erro se `var` nao inicializada.\n\n";

    file << "## 3. Atributos (sintetizados por no)\n\n";
    file << "- tipo in {int, real, booleano}\n";
    file << "- valido in {true, false}\n";
    file << "- lvalue in {true, false} (aplica-se a var)\n";
    file << "- inicializada in {true, false} (apenas para var)\n\n";

    file << "## 4. Regras de Tipagem e Erro\n\n";

    file << "### 4.1 Literais e identificadores\n";
    file << "- num: inteiro -> int; real -> real. valido=true; lvalue=false.\n";
    file << "- var x: tipo = tabela[x].tipo; inicializada = tabela[x].inicializada; "
            "valido=true; lvalue=true. ERRO se usada sem inicializar.\n";
    file << "- res N: requer resultado existente para N; tipo = tipoResultado(N); "
            "valido=true; lvalue=false. ERRO se inexistente.\n\n";

    file << "### 4.2 Aritmeticos basicos (+ - *)\n";
    file << "- operandos nao podem ser booleano.\n";
    file << "- promocao numerica: se algum operando for real -> resultado real; "
            "senao int.\n\n";

    file << "### 4.3 Divisoes e potencia\n";
    file << "- '/' e '%' exigem ambos int -> resultado int. ERRO se houver real/booleano.\n";
    file << "- '|' divisao real: exige int/real nos dois lados -> resultado real. "
            "ERRO se houver booleano.\n";
    file << "- '^' potencia: base numerica (int/real) e expoente int -> resultado = tipo da base. "
            "ERRO se expoente nao for int ou houver booleano.\n\n";

    file << "### 4.4 Relacionais (< <= == != >= >)\n";
    file << "- Para < <= >= >: ambos numericos (int/real) -> resultado booleano.\n";
    file << "- Para == !=: int==int, real==real, booleano==booleano -> booleano. "
            "ERRO para misturas (ex.: int==booleano).\n\n";

    file << "### 4.5 MEM (armazenamento)\n";
    file << "- Forma RPN: E var (armazena E em var).\n";
    file << "- Restricoes: E.tipo in {int, real}; var.tipo == E.tipo. "
            "ERRO se booleano ou tipos diferentes.\n";
    file << "- Efeito: marcar var como inicializada na tabela.\n";
    file << "- Tipo do no mem: pode propagar E.tipo (neste projeto, propagamos E.tipo).\n\n";

    file << "### 4.6 IF e FOR\n";
    file << "- IF (cond bloco if): cond.tipo == booleano; bloco valido. "
            "Tipo resultante: opcional/neutro (nao usado para armazenamento).\n";
    file << "- FOR (iter bloco for): iter numerico (ou int, se voce restringiu); bloco valido. "
            "Efeito de controle; tipo neutro.\n\n";

    file << "## 5. Mensagens de erro\n\n";
    file << "- Formato no console: ERRO SEMANTICO [Linha X]: <descricao> "
            "e Contexto: <trecho> (sem acento, por limitacao de codificacao).\n\n";

    file << "## 6. Exemplos RPN\n\n";
    file << "- 3 5 + -> int\n";
    file << "- 2 3 | -> real\n";
    file << "- A B + -> int/real (requer A,B inicializadas)\n";
    file << "- V var -> marca X como inicializada (exige tipos compativeis e nao booleano)\n";
    file << "- cond bloco if -> cond:booleano\n";
    file << "- N bloco for -> N numerico\n";

    file.close();
}

void AnalisadorSemantico::definirGramaticaAtributos() {
    gerarGramaticaAtributos("gramatica_atributos.md");
}

void AnalisadorSemantico::gerarArvoreAtribuida(NoArvore* arvoreAnotada, int numeroLinha) {
    string nomeArquivo = "arvore_atribuida_linha_" + to_string(numeroLinha) + ".md";
    ofstream arquivo(nomeArquivo);

    if (!arquivo.is_open()) {
        cerr << "Erro ao criar arquivo: " << nomeArquivo << endl;
        return;
    }

    arquivo << "# Árvore Sintática Abstrata Atribuída - Linha " << numeroLinha << "\n\n";
    arquivo << "```\n";

    function<void(NoArvore*, int)> imprimir = [&](NoArvore* no, int nivel) {
        if (!no) return;

        for (int i = 0; i < nivel; i++) arquivo << "  ";
        arquivo << no->simbolo;

        if (!no->tipoAnotado.empty() &&
            no->tipoAnotado != "erro" &&
            no->tipoAnotado != "terminal" &&
            no->tipoAnotado != "epsilon" &&
            no->tipoAnotado != "res_marker" &&
            no->tipoAnotado != "if_marker" &&
            no->tipoAnotado != "for_marker") {
            arquivo << " : " << no->tipoAnotado;
        }
        arquivo << "\n";

        for (auto filho : no->filhos) imprimir(filho, nivel + 1);
    };

    imprimir(arvoreAnotada, 0);

    arquivo << "```\n";
    arquivo.close();

    cout << "Arvore atribuida gerada: " << nomeArquivo << endl;
}

void AnalisadorSemantico::gerarArvoreAtribuidaJSON(NoArvore* arvoreAnotada, int numeroLinha) {
    string nomeArquivo = "arvore_atribuida_linha_" + to_string(numeroLinha) + ".json";
    ofstream arquivo(nomeArquivo);

    if (!arquivo.is_open()) {
        cerr << "Erro ao criar arquivo: " << nomeArquivo << endl;
        return;
    }

    function<void(NoArvore*)> gerarJSON = [&](NoArvore* no) {
        if (!no) { arquivo << "null"; return; }

        arquivo << "{";
        arquivo << "\"simbolo\":\"" << no->simbolo << "\",";
        arquivo << "\"tipo\":\"" << no->tipoAnotado << "\",";
        arquivo << "\"linha\":" << numeroLinha << ",";
        arquivo << "\"filhos\":[";
        for (size_t i = 0; i < no->filhos.size(); i++) {
            gerarJSON(no->filhos[i]);
            if (i + 1 < no->filhos.size()) arquivo << ",";
        }
        arquivo << "]}";
    };

    gerarJSON(arvoreAnotada);
    arquivo << "\n";
    arquivo.close();

    cout << "Arvore atribuida JSON gerada: " << nomeArquivo << endl;
}

void AnalisadorSemantico::gerarArvoresAtribuidasJSONConsolidado(
    const vector<pair<NoArvore*, int>>& arvores, const string& nomeArquivo) {

    ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao criar arquivo: " << nomeArquivo << endl;
        return;
    }

    // Gerador recursivo para um nó
    function<void(NoArvore*)> gerarJSON = [&](NoArvore* no) {
        if (!no) { arquivo << "null"; return; }
        arquivo << "{";
        arquivo << "\"simbolo\":\"" << no->simbolo << "\",";
        arquivo << "\"tipo\":\"" << no->tipoAnotado << "\",";
        arquivo << "\"filhos\":[";
        for (size_t i = 0; i < no->filhos.size(); i++) {
            gerarJSON(no->filhos[i]);
            if (i + 1 < no->filhos.size()) arquivo << ",";
        }
        arquivo << "]}";
    };

    // Topo: lista de objetos { "linha": N, "arvore": {...} }
    arquivo << "[\n";
    for (size_t i = 0; i < arvores.size(); ++i) {
        NoArvore* raiz = arvores[i].first;
        int linha = arvores[i].second;
        arquivo << "  {\"linha\":" << linha << ",\"arvore\":";
        gerarJSON(raiz);
        arquivo << "}";
        if (i + 1 < arvores.size()) arquivo << ",\n";
        else arquivo << "\n";
    }
    arquivo << "]\n";
    arquivo.close();

    cout << "JSON consolidado gerado: " << nomeArquivo << endl;
}
