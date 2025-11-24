// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "arvore.h"
#include <iostream>
#include <fstream>
#include <functional>
#include <map>

NoArvore::NoArvore(string s, bool terminal) : simbolo(s), ehTerminal(terminal) {}

NoArvore::~NoArvore() {
    for (auto filho : filhos) {
        delete filho;
    }
}

Derivacao::Derivacao() : raiz(nullptr), sucesso(false) {}

Derivacao::~Derivacao() {
    if (raiz) delete raiz;
}

NoArvore* gerarArvore(Derivacao* derivacao) { //retorna a arvore
    if (!derivacao || !derivacao->sucesso) {
        return nullptr; //n tem se a arvore falhou
    }
    return derivacao->raiz; //volta para raiz da arvore
}

void imprimirArvore(NoArvore* no, int nivel) {
    if (!no) return; //base e o no vazio
    
    for (int i = 0; i < nivel; ++i) { //arruma indent, 2 por nivel
        cout << "  ";
    }
    
    cout << no->simbolo; //print do simbolo por linha
    if (no->ehTerminal) {
        cout << " (terminal)";
    }
    cout << endl;
    
    for (size_t i = 0; i < no->filhos.size(); ++i) { //imprime todas as criancas
        imprimirArvore(no->filhos[i], nivel + 1);
    }
}

void gerarHTML(NoArvore* raiz, int numeroLinha) {
    string filename = "arvore_linha_" + to_string(numeroLinha) + ".html";
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro ao criar arquivo HTML: " << filename << endl;
        return;
    }
    
    int maxNivel = 0;
    int totalNos = 0;
    
    function<void(NoArvore*, int)> calcularDimensoes = [&](NoArvore* no, int nivel) { //contagem recursiva de no e altura
        if (!no) return;
        totalNos++;
        if (nivel > maxNivel) maxNivel = nivel; //marca nivel mais baixo
        for (auto filho : no->filhos) {
            calcularDimensoes(filho, nivel + 1); //repete recursivamente nas crias
        }
    };
    calcularDimensoes(raiz, 0);
    
    //definicoes do html pra arvore
    int largura = 1200;
    int altura = (maxNivel + 1) * 100 + 100;
    
    file << "<!DOCTYPE html>\n";
    file << "<html>\n<head>\n";
    file << "<meta charset=\"UTF-8\">\n";
    file << "<title>Arvore Sintatica - Linha " << numeroLinha << "</title>\n";
    file << "<style>\n";
    file << "body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }\n";
    file << "h1 { color: #333; }\n";
    file << "svg { background: white; border: 1px solid #ddd; display: block; margin: 20px auto; }\n";
    file << ".no-terminal { fill: #e8e8e8; stroke: #666; stroke-width: 2; }\n";
    file << ".no-terminal-text { fill: #000; font-size: 14px; font-weight: bold; }\n";
    file << ".no-folha { fill: #add8e6; stroke: #4682b4; stroke-width: 2; }\n";
    file << ".no-folha-text { fill: #000; font-size: 12px; }\n";
    file << ".aresta { stroke: #666; stroke-width: 2; fill: none; }\n";
    file << "</style>\n";
    file << "</head>\n<body>\n";
    file << "<h1>Arvore Sintatica - Linha " << numeroLinha << "</h1>\n";
    file << "<svg width=\"" << largura << "\" height=\"" << altura << "\">\n";
    
    map<NoArvore*, pair<int, int>> posicoes;
    
    function<int(NoArvore*)> calcularLargura = [&](NoArvore* no) -> int {
        if (!no) return 0;
        if (no->filhos.empty()) return 1; //folha tem largura 1
        
        int larguraTotal = 0;
        for (auto filho : no->filhos) {
            larguraTotal += calcularLargura(filho); //soma larguras
        }
        return larguraTotal;
    };
    
    function<void(NoArvore*, int, int, int, int)> desenharNo = [&](
        NoArvore* no, int x, int y, int larguraDisponivel, int nivel) {
        
        if (!no) return;
        
        posicoes[no] = make_pair(x, y);
        
        string classe = no->ehTerminal ? "no-folha" : "no-terminal";
        string classeTexto = no->ehTerminal ? "no-folha-text" : "no-terminal-text";
        
        int larguraNo = no->simbolo.length() * 8 + 20;
        int alturaNo = 30;
        // desenha o retangulo
        file << "<rect class=\"" << classe << "\" x=\"" << (x - larguraNo/2) 
             << "\" y=\"" << (y - alturaNo/2) << "\" width=\"" << larguraNo 
             << "\" height=\"" << alturaNo << "\" rx=\"5\"/>\n";

        // com o texto dentro
        file << "<text class=\"" << classeTexto << "\" x=\"" << x 
             << "\" y=\"" << (y + 5) << "\" text-anchor=\"middle\">" 
             << no->simbolo << "</text>\n";
        
        if (!no->filhos.empty()) {
            int yFilho = y + 80; //espacamento vertical fixo
            int larguraUsada = 0;
            
            for (auto filho : no->filhos) { //desenha linha para as crianAÂ§as
                int larguraFilho = calcularLargura(filho);
                int proporcao = larguraDisponivel * larguraFilho / calcularLargura(no);
                
                //calc posicao das criancas
                int xFilho = x - larguraDisponivel/2 + larguraUsada + proporcao/2;
                // distibui ^^ horizontal

                //linhazinha curva
                file << "<path class=\"aresta\" d=\"M " << x << " " << (y + alturaNo/2)
                     << " Q " << x << " " << (y + 40) 
                     << " " << xFilho << " " << (yFilho - alturaNo/2) << "\"/>\n";

                //desenho recursivo
                desenharNo(filho, xFilho, yFilho, proporcao, nivel + 1);
                larguraUsada += proporcao;
            }
        }
    };
    
    int larguraInicial = calcularLargura(raiz) * 100;
    if (larguraInicial > largura - 100) larguraInicial = largura - 100;
    
    desenharNo(raiz, largura/2, 50, larguraInicial, 0);
    
    file << "</svg>\n";
    file << "</body>\n</html>\n";
    
    file.close();
    
    cout << "Arquivo HTML gerado: " << filename << endl;
}

void salvarArvoreMarkdown(NoArvore* raiz, int numeroLinha) { //salvando ela no markdown
    ofstream file("analise_gramatica.md", ios::app); //append
    if (!file.is_open()) {
        cerr << "Erro ao abrir arquivo para salvar arvore" << endl;
        return;
    }
    
    function<void(NoArvore*, int)> salvarNo = [&](NoArvore* no, int nivel) { //recursivo para salvar
        if (!no) return;
        
        for (int i = 0; i < nivel; ++i) { //indenta
            file << "  ";
        }
        
        file << no->simbolo; //escreve simbolo
        if (no->ehTerminal) {
            file << " (terminal)";
        }
        file << "\n";
        
        for (auto filho : no->filhos) { //recursiva para a crianca
            salvarNo(filho, nivel + 1);
        }
    };
    
    file << "## Arvore Sintatica - Linha " << numeroLinha << "\n\n";
    file << "```\n";
    salvarNo(raiz, 0); //comeca da base/raiz
    file << "```\n\n";
    
    file.close();
    cout << "Arvore salva em: analise_gramatica.md" << endl;
}