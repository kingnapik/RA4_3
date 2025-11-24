// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "semantico.h"

// Funcao auxiliar para detectar e processar comando MEM: (V MEM)
// MEM sintetiza o tipo do valor armazenado
bool AnalisadorSemantico::processarComandoMEM(NoArvore* no, string& tipoRetorno) {
    if (!no || no->simbolo != "CORPO") return false;

    // CORPO -> E CORPO'
    if (no->filhos.size() < 2) return false;

    NoArvore* primeiroE = no->filhos[0];  // valor
    NoArvore* corpoLinha = no->filhos[1]; // CORPO'

    if (!corpoLinha || corpoLinha->filhos.empty()) return false;

    NoArvore* segundoE = corpoLinha->filhos[0]; // possivel var

    // Se existir um segundo filho em CORPO' com subfilhos, entao ha operador/marcador -> nao eh MEM
    if (corpoLinha->filhos.size() >= 2 && !corpoLinha->filhos[1]->filhos.empty()) {
        return false;
    }

    // Verificar se segundo elemento e uma variavel (E -> E_ESPECIAL -> VAR)
    if (segundoE && segundoE->simbolo == "E" &&
        !segundoE->filhos.empty() &&
        segundoE->filhos[0]->simbolo == "E_ESPECIAL" &&
        !segundoE->filhos[0]->filhos.empty()) {

        NoArvore* varNode = segundoE->filhos[0]->filhos[0];
        string nomeVar = varNode->simbolo;

        if (!nomeVar.empty() && isupper(nomeVar[0])) {
            string tipoValor = inferirTipo(primeiroE);

            // Nao aceitar booleano em MEM
            if (tipoValor == "booleano") {
                registrarErro("MEM nao pode armazenar valores booleanos",
                              "(" + nomeVar + " := " + tipoValor + ")");
                tipoRetorno = "erro";
                return true;
            }

            tabelaSimbolos.adicionarSimbolo(nomeVar, tipoValor, linhaAtual);
            // Se houver flag de inicializacao, marque aqui:
            tabelaSimbolos.atualizarSimbolo(nomeVar, true);

            tipoRetorno = tipoValor;

            // anotar
            primeiroE->tipoAnotado = tipoValor;
            segundoE->tipoAnotado = tipoValor;
            varNode->tipoAnotado = tipoValor;

            registrarDeducao(nomeVar + " := valor", tipoValor,
                             "MEM sintetiza tipo: G[" + nomeVar + " |-> " + tipoValor + "] |- MEM : " + tipoValor);
            return true;
        }
    }

    return false;
}

// Funcao auxiliar para detectar e processar comando RES: (N RES)
// RES sintetiza o tipo da linha referenciada
bool AnalisadorSemantico::processarComandoRES(NoArvore* no, string& tipoRetorno) {
    if (!no || no->simbolo != "CORPO") return false;

    if (no->filhos.size() < 2) return false;

    NoArvore* primeiroE = no->filhos[0];  // deve ser numero N
    NoArvore* corpoLinha = no->filhos[1]; // CORPO'

    if (!corpoLinha || corpoLinha->filhos.empty()) return false;

    NoArvore* segundoE = corpoLinha->filhos[0]; // possivel RES

    // se houver um segundo elemento com subfilhos, nao e RES simples
    if (corpoLinha->filhos.size() >= 2 && !corpoLinha->filhos[1]->filhos.empty()) {
        return false;
    }

    if (segundoE && segundoE->simbolo == "E" &&
        !segundoE->filhos.empty() &&
        segundoE->filhos[0]->simbolo == "E_ESPECIAL" &&
        !segundoE->filhos[0]->filhos.empty() &&
        segundoE->filhos[0]->filhos[0]->simbolo == "RES") {

        // Buscar N
        if (primeiroE && !primeiroE->filhos.empty()) {
            NoArvore* valorNode = primeiroE->filhos[0];
            if (valorNode && valorNode->simbolo == "E_ARITIMETICO" && !valorNode->filhos.empty()) {
                string nStr = valorNode->filhos[0]->simbolo;

                if (!ehNumeroInteiro(nStr)) {
                    registrarErro("RES requer um numero inteiro, encontrado: " + nStr, "(" + nStr + " RES)");
                    tipoRetorno = "erro";
                    return true;
                }

                int n = stoi(nStr);
                if (n < 0) {
                    registrarErro("RES com indice negativo: " + nStr, "(" + nStr + " RES)");
                    tipoRetorno = "erro";
                    return true;
                }

                int linhaRef = linhaAtual - n;
                if (linhaRef < 1 || linhaRef >= linhaAtual) {
                    registrarErro("RES referencia linha inexistente: linha " + to_string(linhaRef) + " (N=" + nStr + ")",
                                  "(" + nStr + " RES)");
                    tipoRetorno = "erro";
                    return true;
                }

                if (!tabelaSimbolos.existeResultadoLinha(linhaRef)) {
                    registrarErro("RES referencia linha sem resultado: linha " + to_string(linhaRef),
                                  "(" + nStr + " RES)");
                    tipoRetorno = "erro";
                    return true;
                }

                ResultadoLinha resultado = tabelaSimbolos.buscarResultadoLinha(linhaRef);
                if (!resultado.valido) {
                    registrarErro("RES referencia linha invalida: " + nStr, "(" + nStr + " RES)");
                    tipoRetorno = "erro";
                    return true;
                }

                tipoRetorno = resultado.tipo;

                // anotar
                segundoE->tipoAnotado = resultado.tipo;
                segundoE->filhos[0]->tipoAnotado = resultado.tipo;
                segundoE->filhos[0]->filhos[0]->tipoAnotado = resultado.tipo;

                registrarDeducao(nStr + " RES", resultado.tipo,
                                 "RES sintetiza tipo da linha " + to_string(linhaRef) + ": RES(" + nStr + ") : " + resultado.tipo);
                return true;
            }
        }
    }

    return false;
}