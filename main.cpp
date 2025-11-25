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

int main(int argc, char* argv[]) {//verifica argumentos de entrada
    if (argc < 2) {
        cerr << "Uso: ./AnalisadorSintatico <arquivo_tokens.txt>" << endl;
        return 1;
    }

    string nomeArquivo = argv[1];

    try {
        cout << "Construindo gramatica..." << endl;
        Gramatica gramatica = construirGramatica();//chama a construcao da gramatica, a base de tudo

        cout << "\nLendo tokens de: " << nomeArquivo << endl;
        vector<vector<string>> linhasDeTokens = lerTokens(nomeArquivo);//lendo todos os tokens
        cout << "Total de linhas: " << linhasDeTokens.size() << endl;

        int sucessos = 0;
        int falhas = 0;
        vector<pair<NoArvore*, int>> todasArvores; // Armazena todas as arvores validas
        vector<Derivacao*> todasDerivacoes;

        // Analisador semantico global (acumula erros de todas as linhas)
        AnalisadorSemantico analisadorGlobal;

        cout << "\n--- Definindo Gramatica de Atributos ---" << endl;
        analisadorGlobal.definirGramaticaAtributos();
        cout << "Gramatica de atributos definida.\n" << endl;

        for (size_t i = 0; i < linhasDeTokens.size(); ++i) {
            int numeroLinha = i + 1;
            const vector<string>& tokens = linhasDeTokens[i];
            //mostra linha sendo processada por pura organizacao
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

            if (!derivacao->sucesso) {//levanta erro de derivacao e pula
                cerr << "ERRO: " << derivacao->mensagemErro << endl;
                falhas++;
                delete derivacao;
                continue;//pula, n faz arvore
            }

            cout << "SUCESSO!" << endl;//passa o sucesso
            sucessos++;
            todasDerivacoes.push_back(derivacao);//faz a arvore

            cout << "\nPrimeiros passos da derivacao:" << endl;
            for (size_t j = 0; j < derivacao->passos.size() && j < 5; ++j) {
                cout << "  " << derivacao->passos[j] << endl;
            }
            if (derivacao->passos.size() > 5) {
                cout << "  ... (total de " << derivacao->passos.size() << " passos)" << endl;
            }

            NoArvore* arvore = gerarArvore(derivacao);//gera a arvore so se der certo

            if (arvore) {
                cout << "\nArvore Sintatica:" << endl;
                imprimirArvore(arvore);//mostra no console
                gerarHTML(arvore, numeroLinha);//faz o html dela pra ficar mais facil de ler

                // === ANALISE SEMANTICA (FASE 3) ===
                cout << "\n--- ANALISE SEMANTICA ---" << endl;

                bool semanticaOk = analisadorGlobal.analisarSemantica(arvore, numeroLinha);
                bool memoriaOk = analisadorGlobal.analisarSemanticaMemoria(arvore);
                bool controleOk = analisadorGlobal.analisarSemanticaControle(arvore);

                // Verificar se houve erros em qualquer analise
                bool todasAnalisesOk = semanticaOk && memoriaOk && controleOk;

                if (todasAnalisesOk) {
                    cout << "Analise semantica: OK" << endl;
                } else {
                    cout << "Erros semanticos encontrados:" << endl;
                    for (const auto& erro : analisadorGlobal.getErros()) {
                        if (erro.linha == numeroLinha) { // Mostra apenas erros desta linha
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

                // Guarda a arvore para salvar no markdown depois
                todasArvores.push_back(make_pair(arvore, numeroLinha));
            }
        }

        // Salva TODAS as arvores validas no arquivo markdown
        for (const auto& arvoreInfo : todasArvores) {
            NoArvore* arvore = arvoreInfo.first;
            int numeroLinha = arvoreInfo.second;
            salvarArvoreMarkdown(arvore, numeroLinha);
        }

        // Gera um JSON unico com todas as arvores atribuidas
        analisadorGlobal.gerarArvoresAtribuidasJSONConsolidado(todasArvores, "arvores_atribuidas.json");

        // === FASE 4: GERACAO DE CODIGO INTERMEDIARIO (TAC) ===
        cout << "\n========================================" << endl;
        cout << "FASE 4: GERACAO DE CODIGO TAC" << endl;
        cout << "========================================" << endl;
        
        GeradorTAC geradorTAC;
        vector<InstrucaoTAC> codigoCompleto; // Vetor para guardar TUDO
        
        cout << "\n1. Gerando TAC para todas as arvores..." << endl;
        
        // Passo 1: Gerar e Acumular
        for (const auto& arvoreInfo : todasArvores) {
            NoArvore* arvore = arvoreInfo.first;

            // Gera TAC desta linha (contadores tX e LX continuam incrementando)
            geradorTAC.gerarTAC(arvore);
        }

        // Salvar o TAC Bruto por completo
        cout << "Salvando TAC Bruto (todas as instrucoes)..." << endl;
        geradorTAC.salvarTAC("tac_completo_original.txt");

        // Passo 2: Otimizar tudo de uma vez
        cout << "\n2. Executando Otimizacao Global..." << endl;
        OtimizadorTAC otimizador;
        
        otimizador.carregarCodigo(geradorTAC.obterCodigo());
        
        // Otimiza
        otimizador.otimizar();
        
        // Passo 3: Relatórios Finais
        cout << "\n3. Salvando resultados finais..." << endl;
        otimizador.imprimirRelatorio();
        
        otimizador.salvarRelatorio("relatorio_otimizacao_final.txt");
        otimizador.salvarTACOtimizado("tac_final_otimizado.txt");

        cout << "\n4. Gerando Assembly AVR (16-bit)..." << endl;
        GeradorAssembly geradorASM;

        // Usa o código otimizado (do vetor de TACs do otimizador)
        // Nota: Precisamos adicionar um getter no otimizador ou usar o vetor original se não mudou muito
        // Recomendo adicionar "vector<InstrucaoTAC> obterCodigo() const" no otimizador.h
        geradorASM.gerarAssembly(otimizador.obterCodigo(), "codigo.S");
        
        cout << "\nProcesso concluido!" << endl;
        cout << "Arquivos gerados:" << endl;
        cout << "- tac_completo_original.txt" << endl;
        cout << "- tac_completo_otimizado.txt" << endl;
        cout << "- relatorio_otimizacao_global.txt" << endl;
        cout << "- codigo_final.asm" << endl;

        // Limpa memoria de todas as derivacoes (que contem as arvores)
        for (auto deriv : todasDerivacoes) {
            delete deriv;
        }

        cout << "\n========================================" << endl;
        cout << "RESUMO" << endl;
        cout << "========================================" << endl;
        cout << "Total de linhas: " << linhasDeTokens.size() << endl;
        cout << "Sucessos: " << sucessos << endl;
        cout << "Falhas: " << falhas << endl;

        // Gerar relatorios semanticos (FASE 3)
        cout << "\n--- Gerando relatorios semanticos..." << endl;
        analisadorGlobal.gerarRelatorioErros("erros_semanticos.md");
        analisadorGlobal.gerarRelatorioTipos("julgamento_tipos.md");

        cout << "\n=== ANALISE CONCLUIDA ===" << endl;
        cout << "\nArquivos gerados:" << endl;
        cout << "- analise_gramatica.md (gramatica, FIRST, FOLLOW, tabela LL(1) e todas as arvores)" << endl;
        cout << "- tabela_ll1.html (tabela LL(1) - visualizacao extra)" << endl;
        cout << "- arvore_linha_N.html (todas as arvores - visualizacao extra)\n" << endl;
        cout << "- gramatica_atributos.md (FASE 3 - regras semanticas)" << endl;
        cout << "- erros_semanticos.md (FASE 3 - erros encontrados)" << endl;
        cout << "- julgamento_tipos.md (FASE 3 - inferencia de tipos)" << endl;
        cout << "- arvore_atribuida_linha_N.md (FASE 3 - arvore atribuida Markdown)" << endl;
        cout << "- arvore_atribuida_linha_N.json (FASE 3 - arvore atribuida JSON)\n" << endl;
        cout << "- tac_completo_original.txt (FASE 4 - TAC)" << endl;
        cout << "- tac_completo_otimizado.txt (FASE 4 - TAC)" << endl;
        cout << "- relatorio_otimizacao_global.txt (FASE 4 - TAC)\n" << endl;
        cout << "\nAbra os arquivos HTML no navegador para visualizacao grafica!" << endl;

    } catch (const exception& e) {
        cerr << "ERRO: " << e.what() << endl;
        return 1;
    }

    return 0;
}