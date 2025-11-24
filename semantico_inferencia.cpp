// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "semantico.h"
#include <cctype>

// Infere o tipo de um no da arvore recursivamente E ANOTA NA ARVORE
string AnalisadorSemantico::inferirTipo(NoArvore* no) {
    if (!no) return "erro";

    if (!no->tipoAnotado.empty()) {
        return no->tipoAnotado;
    }

    string tipoInferido = "erro";

    // CASO 1: Terminal - numero, variavel ou palavra-chave
    if (no->ehTerminal) {
        if (ehNumeroInteiro(no->simbolo)) {
            tipoInferido = "int";
            registrarDeducao(no->simbolo, "int", "Literal inteiro: " + no->simbolo + " ∈ ℤ");
        }
        else if (ehNumeroReal(no->simbolo)) {
            tipoInferido = "real";
            registrarDeducao(no->simbolo, "real", "Literal real: " + no->simbolo + " ∈ ℝ");
        }
        else if (!no->simbolo.empty() && isupper(no->simbolo[0])) {
            if (!tabelaSimbolos.existeSimbolo(no->simbolo) || !tabelaSimbolos.simboloInicializado(no->simbolo)) {
                registrarErro("Memoria '" + no->simbolo + "' usada sem inicializacao", "(" + no->simbolo + ")");
                tipoInferido = "erro";
            } else {
                Simbolo s = tabelaSimbolos.buscarSimbolo(no->simbolo);
                tipoInferido = s.tipo;
                registrarDeducao(no->simbolo, s.tipo, "Leitura de memoria: G(" + no->simbolo + ") = " + s.tipo);
            }
        }
        else if (no->simbolo == "RES" || no->simbolo == "IF" || no->simbolo == "FOR") {
            tipoInferido = no->simbolo; // marcadores
        }
        else {
            tipoInferido = "terminal";
        }

        no->tipoAnotado = tipoInferido;
        return tipoInferido;
    }

    // CASO 2: Nao-terminal
    string simbolo = no->simbolo;

    // P -> ( CORPO )
    if (simbolo == "P") {
        if (no->filhos.size() >= 3) {
            NoArvore* corpo = no->filhos[1];

            // MEM
            string tipoMEM;
            if (processarComandoMEM(corpo, tipoMEM)) {
                no->tipoAnotado = tipoMEM;
                corpo->tipoAnotado = tipoMEM;
                return tipoMEM;
            }

            // RES
            string tipoRES;
            if (processarComandoRES(corpo, tipoRES)) {
                no->tipoAnotado = tipoRES;
                corpo->tipoAnotado = tipoRES;
                return tipoRES;
            }

            // Outras expressoes
            string tipoCorpo = inferirTipo(corpo);
            tipoInferido = tipoCorpo;
            registrarDeducao("(CORPO)", tipoCorpo, "Expressao parentizada: G |- (e) : " + tipoCorpo);
        }

        no->tipoAnotado = tipoInferido;
        return tipoInferido;
    }

    // CORPO -> E CORPO'
    if (simbolo == "CORPO") {
        if (!no->filhos.empty()) {
            NoArvore* primeiroE = no->filhos[0];
            string tipo1 = inferirTipo(primeiroE);

            // Se houver CORPO' com pelo menos 2 filhos, pode ser binaria, IF ou FOR
            if (no->filhos.size() >= 2 && no->filhos[1]->filhos.size() >= 1) {
                NoArvore* corpoLinha = no->filhos[1];

                // ----- DETECCAO IF/FOR EM RPN -----
                NoArvore* marcadorContainer = (corpoLinha->filhos.size() >= 2) ? corpoLinha->filhos[1] : nullptr;
                if (marcadorContainer && !marcadorContainer->filhos.empty()) {
                    NoArvore* marcadorE = marcadorContainer->filhos[0];
                    if (marcadorE && marcadorE->simbolo == "E" &&
                        !marcadorE->filhos.empty() &&
                        marcadorE->filhos[0]->simbolo == "E_ESPECIAL" &&
                        !marcadorE->filhos[0]->filhos.empty()) {

                        string marcador = marcadorE->filhos[0]->filhos[0]->simbolo;

                        // === IF ===
                        if (marcador == "IF") {
                            // cond = primeiroE; e1 = corpoLinha->filhos[0]; e2 = ultimo E antes do marcador
                            string tcond = tipo1;
                            string t1 = inferirTipo(corpoLinha->filhos[0]);

                            // procurar e2: ultimo E antes do marcador
                            string t2 = "erro";
                            for (int i = (int)corpoLinha->filhos.size() - 1; i >= 0; --i) {
                                if (corpoLinha->filhos[i] == marcadorContainer) continue;
                                if (corpoLinha->filhos[i]->simbolo == "E") {
                                    t2 = inferirTipo(corpoLinha->filhos[i]);
                                    break;
                                }
                            }

                            if (tcond != "booleano") {
                                registrarErro("IF requer condicao booleana, encontrado: " + tcond, "(cond e1 e2 IF)");
                                no->tipoAnotado = "erro";
                                return "erro";
                            }
                            if (t1 == "erro" || t2 == "erro") {
                                no->tipoAnotado = "erro";
                                return "erro";
                            }
                            if (t1 != t2) {
                                registrarErro("Ramos do IF devem ter o mesmo tipo: " + t1 + " e " + t2, "(e1 e2 IF)");
                                no->tipoAnotado = "erro";
                                return "erro";
                            }

                            registrarDeducao("(cond e1 e2 IF)", t1, "IF: cond:booleano; e1:e2:" + t1 + " => " + t1);
                            no->tipoAnotado = t1;
                            corpoLinha->tipoAnotado = t1;
                            marcadorE->tipoAnotado = t1;
                            return t1;
                        }

                        // === FOR ===
                        if (marcador == "FOR") {
                            // inicio = primeiroE; fim = corpoLinha->filhos[0]; corpo = ultimo E antes do marcador
                            string tinicio = tipo1;
                            string tfim = inferirTipo(corpoLinha->filhos[0]);

                            if (tinicio != "int" || tfim != "int") {
                                registrarErro("FOR requer limites inteiros (inicio e fim)", "(inicio fim corpo FOR)");
                                no->tipoAnotado = "erro";
                                return "erro";
                            }

                            string tcorpo = "int";
                            for (int i = (int)corpoLinha->filhos.size() - 1; i >= 0; --i) {
                                if (corpoLinha->filhos[i] == marcadorContainer) continue;
                                if (corpoLinha->filhos[i]->simbolo == "E") {
                                    tcorpo = inferirTipo(corpoLinha->filhos[i]);
                                    break;
                                }
                            }
                            if (tcorpo == "erro") {
                                no->tipoAnotado = "erro";
                                return "erro";
                            }

                            registrarDeducao("(inicio fim corpo FOR)", tcorpo, "FOR: inicio/fim:int; corpo:" + tcorpo + " => " + tcorpo);
                            no->tipoAnotado = tcorpo;
                            corpoLinha->tipoAnotado = tcorpo;
                            marcadorE->tipoAnotado = tcorpo;
                            return tcorpo;
                        }
                    }
                }

                // ----- OPERADORES BINARIOS (+ - * | / % ^, relacionais) -----
                if (corpoLinha->filhos.size() >= 2 && !corpoLinha->filhos[1]->filhos.empty()) {
                    NoArvore* segundoE = corpoLinha->filhos[0];
                    string tipo2 = inferirTipo(segundoE);

                    NoArvore* terceiroE = corpoLinha->filhos[1]->filhos[0];
                    if (terceiroE->simbolo == "E" && !terceiroE->filhos.empty() &&
                        terceiroE->filhos[0]->simbolo == "OP") {

                        NoArvore* opNode = terceiroE->filhos[0];
                        string op = opNode->filhos[0]->simbolo;

                        // Relacionais => booleano (proibido booleano como operando)
                        if (ehOperadorRelacional(op)) {
                            if (tipo1 == "booleano" || tipo2 == "booleano") {
                                registrarErro("Operadores relacionais nao aceitam booleano como operando",
                                              "tipo1=" + tipo1 + ", tipo2=" + tipo2);
                                tipoInferido = "erro";
                            } else if (!tipoCompativel(tipo1, tipo2)) {
                                registrarErro("Tipos incompativeis para operador '" + op + "': " + tipo1 + " e " + tipo2, op);
                                tipoInferido = "erro";
                            } else {
                                tipoInferido = "booleano";
                                registrarDeducao("e1 " + op + " e2", "booleano",
                                                 "Operador relacional sintetiza booleano: G |- e1:" + tipo1 +
                                                 " G |- e2:" + tipo2 + " => booleano");
                            }

                            opNode->tipoAnotado = tipoInferido;
                            opNode->filhos[0]->tipoAnotado = tipoInferido;
                            terceiroE->tipoAnotado = tipoInferido;
                            corpoLinha->tipoAnotado = tipoInferido;
                            no->tipoAnotado = tipoInferido;
                            return tipoInferido;
                        }

                        // Aritmeticos (+ - * | / % ^) (proibido booleano como operando)
                        if (ehOperadorAritmetico(op)) {
                            if (tipo1 == "booleano" || tipo2 == "booleano") {
                                registrarErro("Operadores aritmeticos nao aceitam booleano como operando",
                                              "tipo1=" + tipo1 + ", tipo2=" + tipo2);
                                tipoInferido = "erro";
                            }
                            else if (op == "^") {
                                if (tipo2 != "int") {
                                    registrarErro("Expoente do operador ^ deve ser inteiro, encontrado: " + tipo2,
                                                  "... ^ " + tipo2);
                                    tipoInferido = "erro";
                                } else {
                                    tipoInferido = tipo1; // tipo da base
                                    registrarDeducao("e1 ^ e2", tipo1,
                                                     "Exponenciacao (expoente int): G |- e1:" + tipo1 +
                                                     " G |- e2:int => " + tipo1);
                                }
                            }
                            else if (op == "/" || op == "%") {
                                if (tipo1 != "int" || tipo2 != "int") {
                                    registrarErro("Operador '" + op + "' requer operandos inteiros, encontrado: " +
                                                  tipo1 + " e " + tipo2, op);
                                    tipoInferido = "erro";
                                } else {
                                    tipoInferido = "int";
                                    registrarDeducao("e1 " + op + " e2", "int",
                                                     "Operacao inteira: G |- e1:int G |- e2:int => int");
                                }
                            }
                            else if (op == "|"){
                                if (tipo1 == "booleano" || tipo2 == "booleano"){
                                    registrarErro("Operador '|' nao aceita booleano", "tipo1 = " + tipo1 + ", tipo2 = " + tipo2);
                                    tipoInferido = "erro";
                                }else{
                                    tipoInferido = "real";
                                    registrarDeducao("e1 | e2", "real", "Divisao real: G |- e1:" + tipo1 + " G |- e2:" + tipo2 + " => real");
                                }
                            }
                            else {
                                string tprom = promoverTipo(tipo1, tipo2);
                                if (tprom == "erro") {
                                    registrarErro("Tipos incompativeis para operador '" + op + "': " + tipo1 + " e " + tipo2, op);
                                    tipoInferido = "erro";
                                } else {
                                    tipoInferido = tprom;
                                    registrarDeducao("e1 " + op + " e2", tipoInferido,
                                                     "Operador aritmetico com promocao: G |- e1:" + tipo1 +
                                                     " G |- e2:" + tipo2 +
                                                     " => promover(" + tipo1 + "," + tipo2 + ") = " + tipoInferido);
                                }
                            }

                            opNode->tipoAnotado = tipoInferido;
                            opNode->filhos[0]->tipoAnotado = tipoInferido;
                            terceiroE->tipoAnotado = tipoInferido;
                            corpoLinha->tipoAnotado = tipoInferido;
                            no->tipoAnotado = tipoInferido;
                            return tipoInferido;
                        }
                    }
                }
            }

            // Se nao casou nada, propaga o tipo do primeiro E
            tipoInferido = tipo1;
        }

        no->tipoAnotado = tipoInferido;
        return tipoInferido;
    }

    // CORPO' -> E CORPO' | epsilon
    if (simbolo == "CORPO'") {
        if (no->filhos.empty()) {
            tipoInferido = "epsilon";
        } else if (!no->filhos.empty()) {
            tipoInferido = inferirTipo(no->filhos[0]);
        }

        no->tipoAnotado = tipoInferido;
        return tipoInferido;
    }

    // E -> E_ARITMETICO | E_ESPECIAL | OP
    if (simbolo == "E") {
        if (!no->filhos.empty()) {
            tipoInferido = inferirTipo(no->filhos[0]);
        }
        no->tipoAnotado = tipoInferido;
        return tipoInferido;
    }

    // E_ARITIMETICO -> num | P
    if (simbolo == "E_ARITIMETICO") {
        if (!no->filhos.empty()) {
            tipoInferido = inferirTipo(no->filhos[0]);
        }
        no->tipoAnotado = tipoInferido;
        return tipoInferido;
    }

    // E_ESPECIAL -> var | RES | IF | FOR
    if (simbolo == "E_ESPECIAL") {
        if (no->filhos.empty()) {
            tipoInferido = "erro";
        } else {
            NoArvore* filho = no->filhos[0];
            string valorFilho = filho->simbolo;

            if (valorFilho == "RES") {
                tipoInferido = "res_marker";
            } else if (valorFilho == "IF") {
                tipoInferido = "if_marker";
            } else if (valorFilho == "FOR") {
                tipoInferido = "for_marker";
            } else if (!valorFilho.empty() && isupper(valorFilho[0])) {
                if (!tabelaSimbolos.existeSimbolo(valorFilho)) {
                    registrarErro("Memoria '" + valorFilho + "' usada sem inicializacao", "(" + valorFilho + ")");
                    tipoInferido = "erro";
                } else {
                    Simbolo s = tabelaSimbolos.buscarSimbolo(valorFilho);
                    tipoInferido = s.tipo;
                }
            } else {
                tipoInferido = inferirTipo(filho);
            }
        }

        no->tipoAnotado = tipoInferido;
        return tipoInferido;
    }

    // OP -> operadores aritmeticos e relacionais
    if (simbolo == "OP") {
        if (no->filhos.empty()) {
            tipoInferido = "erro";
        } else {
            string op = no->filhos[0]->simbolo;
            if (ehOperadorRelacional(op)) {
                tipoInferido = "booleano";
            }
            else if (ehOperadorAritmetico(op)) {
                tipoInferido = "terminal"; // decidido pelos operandos
            }
            else {
                tipoInferido = "erro";
            }
        }

        no->tipoAnotado = tipoInferido;
        return tipoInferido;
    }

    no->tipoAnotado = tipoInferido;
    return tipoInferido;
}

// Promove tipos em operacoes numericas (int/real). Booleano NUNCA entra aqui.
string AnalisadorSemantico::promoverTipo(string tipo1, string tipo2) {
    if ((tipo1 == "real" && (tipo2 == "int" || tipo2 == "real")) ||
        (tipo2 == "real" && (tipo1 == "int" || tipo1 == "real"))) {
        return "real";
    }
    if (tipo1 == "int" && tipo2 == "int") return "int";
    return "erro"; // qualquer caso com booleano cai aqui e sera tratado pelo chamador
}

// auxiliares
string obterTipoToken(const string& token) {
    if (ehNumeroInteiro(token)) return "int";
    if (ehNumeroReal(token)) return "real";
    return "desconhecido";
}

bool ehNumeroInteiro(const string& token) {
    if (token.empty()) return false;
    size_t inicio = (token[0] == '-') ? 1 : 0;
    for (size_t i = inicio; i < token.size(); ++i) if (!isdigit(token[i])) return false;
    return true;
}

bool ehNumeroReal(const string& token) {
    if (token.empty()) return false;
    bool temPonto = false;
    size_t inicio = (token[0] == '-') ? 1 : 0;
    for (size_t i = inicio; i < token.size(); ++i) {
        if (token[i] == '.') {
            if (temPonto) return false;
            temPonto = true;
        } else if (!isdigit(token[i])) {
            return false;
        }
    }
    return temPonto;
}

bool ehOperadorAritmetico(const string& op) {
    return (op == "+" || op == "-" || op == "*" || op == "/" ||
            op == "|" || op == "%" || op == "^");
}

bool ehOperadorRelacional(const string& op) {
    return (op == ">" || op == "<" || op == ">=" ||
            op == "<=" || op == "==" || op == "!=");
}

// Verifica se dois tipos sao compativeis (para relacionais: apenas numericos iguais/compativeis)
bool AnalisadorSemantico::tipoCompativel(string tipo1, string tipo2) {
    if ((tipo1 == "int" || tipo1 == "real") && (tipo2 == "int" || tipo2 == "real")) {
        return true;
    }
    return tipo1 == tipo2;
}