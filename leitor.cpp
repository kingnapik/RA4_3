// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA3_2

#include "leitor.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

vector<vector<string>> lerTokens(string arquivo) {
    vector<vector<string>> linhasDeTokens;
    
    ifstream file(arquivo); //abre o arqivo
    if (!file.is_open()) {
        throw runtime_error("Erro ao abrir arquivo: " + arquivo);
    }
    
    string linha;
    while (getline(file, linha)) { //passa linha por linha
        if (linha.empty()) continue;
        
        vector<string> tokens;
        stringstream ss(linha); //split pelo espaco
        string token;
        while (ss >> token) { //tira tkn
            tokens.push_back(token);
        }
        
        if (!tokens.empty()) {
            linhasDeTokens.push_back(tokens); //salva pro futuro
        }
    }
    
    file.close();
    return linhasDeTokens;
}