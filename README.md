# RA3_2
# Analisador Semântico - Fase 3
## Informações Institucionais
Instituição: Pontificia Universidade do Paraná (PUC-PR)

Ano: 2025

Disciplina: Linguagem Formal de Compiladores 

Professor: Frank de Alcantara

Desenvolvedor: Guilherme Knapik - kingnapik
## Objetivo
Implementação das Fases 1, 2 e 3: gramática LL(1), parser e análise semântica que anota a AST com tipos. A entrada lógica da Fase 3 é a AST da Fase 2, a gramática de atributos e a tabela de símbolos; a saída é a AST anotada ou erro semântico.

---

## Estrutura da Linguagem

### Sintaxe Básica

A linguagem utiliza **notação polonesa reversa (RPN)**, onde operadores aparecem após seus operandos. Todas as expressões devem estar entre parênteses:

```
( operando1 operando2 operador )
```

### Operadores Suportados

#### Operadores Aritméticos
- `+` : Adição
- `-` : Subtração
- `*` : Multiplicação
- `/` : Divisão inteira (trunca para inteiro)
- `|` : Divisão real (mantém casas decimais)
- `%` : Resto da divisão inteira (apenas para inteiros)
- `^` : Potenciação (expoente deve ser inteiro)

#### Operadores de Comparação
- `>` : Maior que
- `<` : Menor que
- `>=` : Maior ou igual
- `<=` : Menor ou igual
- `==` : Igual
- `!=` : Diferente

#### Comandos Especiais
- `(N RES)` : Retorna o resultado da expressão N linhas anteriores
- `(V MEM)` : Armazena o valor V na variável MEM
- `(MEM)` : Retorna o valor armazenado em MEM (retorna 0 se não inicializada)

### Estruturas de Controle

#### IF - Tomada de Decisão

**Sintaxe:**
```
( condição valor_verdadeiro valor_falso IF )
```

**Descrição:**
- `condição`: Expressão que retorna 0 (falso) ou diferente de 0 (verdadeiro)
- `valor_verdadeiro`: Valor retornado se condição for verdadeira
- `valor_falso`: Valor retornado se condição for falsa

#### FOR - Laço de Repetição

**Sintaxe:**
```
( condição corpo FOR )
```

**Descrição:**
- `condição`: Condição que define o numero de execuções.
- `corpo`: Expressão a ser executada

### Expressões Aninhadas

Expressões podem ser aninhadas sem limite:

```
( ( 10 5 + ) ( 3 2 * ) - )              // (10+5) - (3*2) = 9
( ( ( A B + ) ( C D * ) / ) X )         // Armazena (A+B)/(C*D) em X
```

---

## Como compilar
```bash
.\compile_fase3.bat
```
ou diretamente por comando caso o arquivo de compilação esteja faltante.
```bash
g++ -std=c++17 -O2 -Wall \
  main.cpp gramatica.cpp parser.cpp arvore.cpp leitor.cpp \
  tabela_simbolos.cpp semantico_driver.cpp semantico_inferencia.cpp \
  semantico_saida.cpp semantico_comandos.cpp \
  -o AnalisadorCompleto
```
## Como executar
```bash
.\AnalisadorCompleto.exe teste1.txt
```
## Entrada
Um artigo contendo o código-fonte da linguagem utilizada neste projeto (uma expressão RPN por linha) em formato .txt contendo apenas caracteres ASCII.
- Cada linha do arquivo é um programa RPN completo, entre parênteses.
- Tokens separados por espaço.
- Suporta números (int/real), variáveis, operadores aritméticos/relacionais e construtos `IF`/`FOR` em RPN.

Exemplo:
```
( 3.14 2.5 + )
( 40 MEM )
( ( MEM 40 == ) ( MEM 10 + ) ( MEM 10 - ) IF )
```
## Saída
- `analise_gramatica.md` — Relatório da gramática, FIRST/FOLLOW e tabela LL(1) usada pelo parser; inclui exemplos de derivação quando aplicável.
- `gramatica_atributos.md` — Especificação das regras de atributos para a Fase 3 (tipagem, coerções, restrições de uso em `IF`, `FOR`, `MEM`, `RES`).
- `erros_semanticos.md` — Lista de erros semânticos encontrados, com linha, mensagem e, quando disponível, contexto do nó/expressão.
- `julgamento_tipos.md` — Resumo dos passos de tipagem aplicados por linha e por nó (quais regras determinaram cada tipo observado).
- `arvore_atribuida_linha_N.md` — Visualização textual da AST anotada da linha N (nós com token, papel sintático e tipo anotado).
- `arvore_atribuida_linha_N.json` — Estrutura JSON da AST anotada da linha N (campos: símbolo, filhos, tipoAnotado, metadados).
- `arvores_atribuidas.json` — Consolidação JSON com todas as ASTs anotadas das linhas processadas.
- `tabela_ll1.html` — Visualização da tabela LL(1) para consulta durante a correção.
