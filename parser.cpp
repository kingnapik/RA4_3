// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "parser.h"
#include <cctype>
#include <vector>

string mapearTokenParaTerminal(const string& token) {
    if (!token.empty() && (isdigit(token[0]) || (token[0] == '-' && token.size() > 1))) {//numeros, inteiros com ponto ou negativos, todos associados a num
        return "num";
    }
    //operadores permanecem como sao
    if (token == "+" || token == "-" || token == "*" || token == "/" || 
        token == "|" || token == "%" || token == "^" || 
        token == ">" || token == "<" || token == ">=" || 
        token == "<=" || token == "==" || token == "!=") {
        return token;
    }
    //parenteses tambem, mantidos normal
    if (token == "(" || token == ")") {
        return token;
    }
    //essencialmente, passando tokens especiais para minusculo porque sao terminais
    if (token == "RES") {return "res";}
    if (token == "IF") {return "if";}
    if (token == "FOR") {return "for";}
    // pega qqr char ou sequencia de chars e passa par ser interpretada como variavel
    if (!token.empty() && isupper(token[0])) {return "var";}
    
    return token;
}

string producaoParaString(const Production& prod) { //transforma prod em uma string legivel
    if (prod.empty() || (prod.size() == 1 && prod[0] == EPSILON)) {
        return "epsilon";
    }
    string result;
    for (size_t i = 0; i < prod.size(); ++i) {
        if (i > 0) result += " ";
        result += prod[i];
    }
    return result;
}

Derivacao* parsear(const vector<string>& _tokens_, const Gramatica& gramatica) {//parseando por pilha
    Derivacao* derivacao = new Derivacao();
    
    if (_tokens_.empty()) {//caso tenha uma entrada vazia levanta erro
        derivacao->sucesso = false;
        derivacao->mensagemErro = "Entrada vazia";
        return derivacao;
    }
    
    struct ItemPilha {//setup
        string simbolo;//simbolo da gramatica
        NoArvore* no;
        ItemPilha(string s, NoArvore* n) : simbolo(s), no(n) {}
    };
    
    vector<ItemPilha> pilha;
    //vvv cria um no que comeca com o simbolo inicial, o P
    derivacao->raiz = new NoArvore(gramatica.simboloInicial, false);
    pilha.push_back(ItemPilha(gramatica.simboloInicial, derivacao->raiz)); //e inicia o stack
    
    size_t indiceToken = 0;//guarda posicao atual
    derivacao->passos.push_back(gramatica.simboloInicial);//comeca de verdade, primeiro paco na derivacao
    
    while (!pilha.empty()) {
        ItemPilha topo = pilha.back();//pop da pilha
        pilha.pop_back();
        
        string simboloTopo = topo.simbolo;
        NoArvore* noTopo = topo.no;
        //input token atual
        string tokenAtual = (indiceToken < _tokens_.size()) ? _tokens_[indiceToken] : END_MARKER;
        string terminalAtual = mapearTokenParaTerminal(tokenAtual);
        
        if (gramatica.terminais.count(simboloTopo) || simboloTopo == END_MARKER) {
            if (simboloTopo == terminalAtual) { //se o terminal da match com o input
                if (simboloTopo != END_MARKER) {//atualiza no com valor do token
                    noTopo->simbolo = tokenAtual;//ex: stack = [num] e o input = 5 4 + )
                    noTopo->ehTerminal = true;//                                 ^
                    indiceToken++;
                }
            } else {//se nao ser match, deu erro. esperava-se outra coisa, levanta erro
                derivacao->sucesso = false;
                derivacao->mensagemErro = "Erro sintatico: esperado '" + simboloTopo + //ex: stack = [num] e input = + )
                                          "' mas encontrado '" + tokenAtual + //                                     ^
                                          "' na posicao " + to_string(indiceToken);
                return derivacao;
            }
        }
        else if (gramatica.naoTerminais.count(simboloTopo)) {//se o topo for n-terminal
            auto chave = make_pair(simboloTopo, terminalAtual);//lookup na tabela
            
            if (!gramatica.tabelaLL1.count(chave)) {//levanta erro se nao estiver presente na tabela
                derivacao->sucesso = false;
                derivacao->mensagemErro = "Erro sintatico: nao ha producao para (" + 
                                          simboloTopo + ", " + terminalAtual + 
                                          ") na posicao " + to_string(indiceToken);
                return derivacao;
            }
            
            const Production& producao = gramatica.tabelaLL1.at(chave);//pega a regra da tabela
            //e grava o passo na derivacao
            string passo = simboloTopo + " -> " + producaoParaString(producao);
            derivacao->passos.push_back(passo);
            //constroi a arvore enquanto parseia
            if (!(producao.size() == 1 && producao[0] == EPSILON)) {//so se for diferente de epsilon na producao, porque epsilon nao aparece na arvore
                vector<NoArvore*> filhos;
                for (const auto& sim : producao) {
                    bool ehTerm = gramatica.terminais.count(sim) > 0;
                    NoArvore* filho = new NoArvore(sim, ehTerm);
                    filhos.push_back(filho);
                    noTopo->filhos.push_back(filho);//push na arvore
                }
                //push na ordem inversa para poder ser processado esquerda p/ direita
                for (int i = producao.size() - 1; i >= 0; --i) {
                    pilha.push_back(ItemPilha(producao[i], filhos[i]));
                }
            }//epsilon nao e colocado, pula
        }
        else {
            derivacao->sucesso = false;//se o token for desconhecido, levanta erro
            derivacao->mensagemErro = "Erro interno: simbolo desconhecido '" + simboloTopo + "'";
            return derivacao;
        }
    }
    
    if (indiceToken < _tokens_.size()) {//se de alguma maneira sobrarem tokens, levanta erro que eles nao foram cosnumidos
        derivacao->sucesso = false;
        derivacao->mensagemErro = "Erro sintatico: tokens extras apos analise completa";
        return derivacao;
    }
    
    derivacao->sucesso = true;//deu boa
    return derivacao;
}