// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "gramatica.h"
#include "parser.h"
#include "arvore.h"
#include "leitor.h"
#include "tabela_simbolos.h"
#include "semantico.h"
#include "tac.h"
#include "otimizador.h"
#include "gerador_assembly.h"
#include <iostream>
#include <utility>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: ./AnalisadorSintatico <arquivo_tokens.txt>" << endl;
        return 1;
    }

    string nomeArquivo = argv[1];

    try {
        cout << "Construindo gramatica..." << endl;
        Gramatica gramatica = construirGramatica();

        cout << "\nLendo tokens de: " << nomeArquivo << endl;
        vector<vector<string>> linhasDeTokens = lerTokens(nomeArquivo);
        cout << "Total de linhas: " << linhasDeTokens.size() << endl;

        int sucessos = 0;
        int falhas = 0;
        vector<pair<NoArvore*, int>> todasArvores;
        vector<Derivacao*> todasDerivacoes;

        // Analisador semantico global
        AnalisadorSemantico analisadorGlobal;

        cout << "\n--- Definindo Gramatica de Atributos ---" << endl;
        analisadorGlobal.definirGramaticaAtributos();
        cout << "Gramatica de atributos definida.\n" << endl;

        for (size_t i = 0; i < linhasDeTokens.size(); ++i) {
            int numeroLinha = i + 1;
            const vector<string>& tokens = linhasDeTokens[i];
            
            cout << "\n========================================" << endl;
            cout << "LINHA " << numeroLinha << endl;
            cout << "========================================" << endl;

            cout << "Tokens: ";
            for (const auto& token : tokens) {
                cout << token << " ";
            }
            cout << endl;

            cout << "\nAnalisando..." << endl;
            Derivacao* derivacao = parsear(tokens, gramatica);

            if (!derivacao->sucesso) {
                cerr << "ERRO: " << derivacao->mensagemErro << endl;
                falhas++;
                delete derivacao;
                continue;
            }

            cout << "SUCESSO!" << endl;
            sucessos++;
            todasDerivacoes.push_back(derivacao);

            cout << "\nPrimeiros passos da derivacao:" << endl;
            for (size_t j = 0; j < derivacao->passos.size() && j < 5; ++j) {
                cout << "  " << derivacao->passos[j] << endl;
            }
            if (derivacao->passos.size() > 5) {
                cout << "  ... (total de " << derivacao->passos.size() << " passos)" << endl;
            }

            NoArvore* arvore = gerarArvore(derivacao);

            if (arvore) {
                cout << "\nArvore Sintatica:" << endl;
                imprimirArvore(arvore);
                gerarHTML(arvore, numeroLinha);

                // === ANALISE SEMANTICA (FASE 3) ===
                cout << "\n--- ANALISE SEMANTICA ---" << endl;

                bool semanticaOk = analisadorGlobal.analisarSemantica(arvore, numeroLinha);
                bool memoriaOk = analisadorGlobal.analisarSemanticaMemoria(arvore);
                bool controleOk = analisadorGlobal.analisarSemanticaControle(arvore);

                bool todasAnalisesOk = semanticaOk && memoriaOk && controleOk;

                if (todasAnalisesOk) {
                    cout << "Analise semantica: OK" << endl;
                } else {
                    cout << "Erros semanticos encontrados:" << endl;
                    for (const auto& erro : analisadorGlobal.getErros()) {
                        if (erro.linha == numeroLinha) {
                            cerr << "  ERRO [Linha " << erro.linha << "]: "
                                 << erro.mensagem << endl;
                            if (!erro.contexto.empty()) {
                                cerr << "  Contexto: " << erro.contexto << endl;
                            }
                        }
                    }
                }

                analisadorGlobal.gerarArvoreAtribuida(arvore, numeroLinha);
                analisadorGlobal.gerarArvoreAtribuidaJSON(arvore, numeroLinha);

                cout << "-------------------------\n" << endl;

                todasArvores.push_back(make_pair(arvore, numeroLinha));
            }
        }

        // Salva todas as arvores no markdown
        for (const auto& arvoreInfo : todasArvores) {
            NoArvore* arvore = arvoreInfo.first;
            int numeroLinha = arvoreInfo.second;
            salvarArvoreMarkdown(arvore, numeroLinha);
        }

        // JSON consolidado
        analisadorGlobal.gerarArvoresAtribuidasJSONConsolidado(todasArvores, "arvores_atribuidas.json");

        // === FASE 4: GERACAO DE CODIGO INTERMEDIARIO (TAC) ===
        cout << "\n========================================" << endl;
        cout << "FASE 4: GERACAO DE CODIGO TAC" << endl;
        cout << "========================================" << endl;
        
        GeradorTAC geradorTAC;
        
        cout << "\n1. Gerando TAC para todas as arvores..." << endl;
        
        for (const auto& arvoreInfo : todasArvores) {
            NoArvore* arvore = arvoreInfo.first;
            geradorTAC.gerarTAC(arvore);
        }

        cout << "Salvando TAC Bruto..." << endl;
        geradorTAC.salvarTAC("tac_completo_original.txt");

        // Passo 2: Otimizar
        cout << "\n2. Executando Otimizacao Global..." << endl;
        OtimizadorTAC otimizador;
        
        otimizador.carregarCodigo(geradorTAC.obterCodigo());
        otimizador.otimizar();
        
        // Passo 3: Relatorios Finais
        cout << "\n3. Salvando resultados finais..." << endl;
        otimizador.imprimirRelatorio();
        
        otimizador.salvarRelatorio("relatorio_otimizacao.txt");
        otimizador.salvarTACOtimizado("tac_final_otimizado.txt");
        
        // Gera relatorio em Markdown (NOVO)
        otimizador.gerarRelatorioMarkdown("relatorio_otimizacoes.md");

        // Passo 4: Assembly
        cout << "\n4. Gerando Assembly AVR (16-bit)..." << endl;
        GeradorAssembly geradorASM;
        geradorASM.gerarAssembly(otimizador.obterCodigo(), "codigo.S");
        
        // Limpa memoria
        for (auto deriv : todasDerivacoes) {
            delete deriv;
        }

        // === RESUMO ===
        cout << "\n========================================" << endl;
        cout << "RESUMO" << endl;
        cout << "========================================" << endl;
        cout << "Total de linhas: " << linhasDeTokens.size() << endl;
        cout << "Sucessos: " << sucessos << endl;
        cout << "Falhas: " << falhas << endl;

        // Estatisticas de otimizacao
        EstatisticasOtimizacao stats = otimizador.obterEstatisticas();
        cout << "\nEstatisticas de Otimizacao:" << endl;
        cout << "  Instrucoes originais: " << stats.instrucoesOriginais << endl;
        cout << "  Instrucoes finais: " << stats.instrucoesFinais << endl;
        cout << "  Reducao: " << (stats.instrucoesOriginais - stats.instrucoesFinais) << " instrucoes" << endl;

        // Gerar relatorios semanticos
        cout << "\n--- Gerando relatorios semanticos..." << endl;
        analisadorGlobal.gerarRelatorioErros("erros_semanticos.md");
        analisadorGlobal.gerarRelatorioTipos("julgamento_tipos.md");

        cout << "\n=== COMPILACAO CONCLUIDA ===" << endl;
        cout << "\nArquivos gerados:" << endl;
        cout << "\n[FASE 2 - Analise Sintatica]" << endl;
        cout << "  - analise_gramatica.md" << endl;
        cout << "  - tabela_ll1.html" << endl;
        cout << "  - arvore_linha_N.html" << endl;
        cout << "\n[FASE 3 - Analise Semantica]" << endl;
        cout << "  - gramatica_atributos.md" << endl;
        cout << "  - erros_semanticos.md" << endl;
        cout << "  - julgamento_tipos.md" << endl;
        cout << "  - arvore_atribuida_linha_N.md" << endl;
        cout << "  - arvore_atribuida_linha_N.json" << endl;
        cout << "  - arvores_atribuidas.json" << endl;
        cout << "\n[FASE 4 - Geracao de Codigo]" << endl;
        cout << "  - tac_completo_original.txt" << endl;
        cout << "  - tac_final_otimizado.txt" << endl;
        cout << "  - relatorio_otimizacao.txt" << endl;
        cout << "  - relatorio_otimizacoes.md" << endl;
        cout << "  - codigo.S" << endl;

        cout << "\nAbra os arquivos HTML no navegador para visualizacao grafica!" << endl;

    } catch (const exception& e) {
        cerr << "ERRO: " << e.what() << endl;
        return 1;
    }

    return 0;
}