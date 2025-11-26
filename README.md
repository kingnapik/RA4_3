# Compilador RPN para Assembly AVR

**Grupo:** RA4_3

**Integrante:** Guilherme Knapik (kingnapik)  

**Disciplina:** Linguagens Formais e Compiladores

---

## Descricao

Compilador completo para uma linguagem baseada em Notacao Polonesa Reversa (RPN), gerando codigo Assembly para microcontroladores AVR (Arduino Uno - ATmega328P).

O projeto integra as 4 fases de um compilador:
1. **Analise Lexica** - Tokenizacao do codigo fonte
2. **Analise Sintatica** - Parser LL(1) com construcao de arvore sintatica
3. **Analise Semantica** - Verificacao de tipos e gramatica de atributos
4. **Geracao de Codigo** - TAC, otimizacao e Assembly AVR

---

## Estrutura do Projeto

```
/
├── main.cpp                 # Ponto de entrada do compilador
├── leitor.cpp/.h            # Leitura de tokens do arquivo
├── gramatica.cpp/.h         # Definicao da gramatica LL(1)
├── parser.cpp/.h            # Parser LL(1)
├── arvore.cpp/.h            # Arvore sintatica
├── tabela_simbolos.cpp/.h   # Tabela de simbolos
├── semantico.h              # Header do analisador semantico
├── semantico_driver.cpp     # Driver do analisador semantico
├── semantico_inferencia.cpp # Inferencia de tipos
├── semantico_comandos.cpp   # Comandos MEM e RES
├── semantico_saida.cpp      # Geracao de relatorios semanticos
├── tac.cpp/.h               # Gerador de codigo TAC
├── otimizador.cpp/.h        # Otimizador de TAC
├── gerador_assembly.cpp/.h  # Orquestrador da geracao de Assembly
├── asm_cabecalho.cpp        # Cabecalho e secoes de dados
├── asm_setup.cpp            # Setup do Arduino
├── asm_traducao.cpp         # Traducao TAC -> Assembly
├── asm_saida.cpp            # Rotinas de saida (dump, fim)
├── asm_serial.cpp           # Driver USART
├── asm_float.cpp            # Rotinas IEEE 754 half-precision
├── fatorial.txt             # Exemplo: calculo de fatorial
├── fibonacci.txt            # Exemplo: sequencia de Fibonacci
└── taylor.txt               # Exemplo: serie de Taylor para cosseno
```

---

## Compilacao

### Windows (usando .bat)

```bat
@echo off
g++ -o AnalisadorSintatico ^
    main.cpp ^
    gramatica.cpp ^
    parser.cpp ^
    arvore.cpp ^
    leitor.cpp ^
    tabela_simbolos.cpp ^
    semantico_driver.cpp ^
    semantico_inferencia.cpp ^
    semantico_comandos.cpp ^
    semantico_saida.cpp ^
    tac.cpp ^
    otimizador.cpp ^
    gerador_assembly.cpp ^
    asm_cabecalho.cpp ^
    asm_setup.cpp ^
    asm_traducao.cpp ^
    asm_saida.cpp ^
    asm_serial.cpp ^
    asm_float.cpp

if %errorlevel% equ 0 (
    echo Compilacao OK
) else (
    echo Erro na compilacao
)
pause
```

### Linux/Mac

```bash
g++ -o AnalisadorSintatico \
    main.cpp gramatica.cpp parser.cpp arvore.cpp leitor.cpp \
    tabela_simbolos.cpp semantico_driver.cpp semantico_inferencia.cpp \
    semantico_comandos.cpp semantico_saida.cpp tac.cpp otimizador.cpp \
    gerador_assembly.cpp asm_cabecalho.cpp asm_setup.cpp \
    asm_traducao.cpp asm_saida.cpp asm_serial.cpp asm_float.cpp
```

---

## Execucao

```bash
.\AnalisadorSintatico <arquivo_entrada.txt>
```

**Exemplos:**
```bash
.\AnalisadorSintatico fatorial.txt
.\AnalisadorSintatico fibonacci.txt
.\AnalisadorSintatico taylor.txt
```

---

## Arquivos Gerados

### Fase 2 - Analise Sintatica
| Arquivo | Descricao |
|---------|-----------|
| `analise_gramatica.md` | Gramatica, FIRST, FOLLOW, tabela LL(1) |
| `tabela_ll1.html` | Tabela LL(1) em HTML |
| `arvore_linha_N.html` | Arvore sintatica de cada linha |

### Fase 3 - Analise Semantica
| Arquivo | Descricao |
|---------|-----------|
| `gramatica_atributos.md` | Regras semanticas |
| `erros_semanticos.md` | Erros encontrados |
| `julgamento_tipos.md` | Inferencia de tipos |
| `arvore_atribuida_linha_N.md` | Arvore com tipos anotados |
| `arvore_atribuida_linha_N.json` | Arvore em JSON |
| `arvores_atribuidas.json` | Todas as arvores consolidadas |

### Fase 4 - Geracao de Codigo
| Arquivo | Descricao |
|---------|-----------|
| `tac_completo_original.txt` | Codigo TAC antes da otimizacao |
| `tac_final_otimizado.txt` | Codigo TAC apos otimizacao |
| `relatorio_otimizacao.txt` | Log das otimizacoes |
| `relatorio_otimizacoes.md` | Relatorio completo em Markdown |
| `codigo.S` | Assembly AVR gerado |

---

## Linguagem RPN

A linguagem utiliza Notacao Polonesa Reversa (pos-fixa), onde operadores aparecem apos seus operandos.

### Sintaxe

```
( numero numero op )    # Expressao simples
( valor VARIAVEL )      # Atribuicao (MEM)
( N RES )               # Referencia a linha anterior
( cond e1 e2 IF )       # Condicional
( cond corpo FOR )      # Loop
```

### Operadores

| Operador | Descricao |
|----------|-----------|
| `+` `-` `*` | Aritmeticos basicos |
| `/` | Divisao inteira |
| `\|` | Divisao real |
| `%` | Modulo |
| `^` | Potencia |
| `>` `<` `>=` `<=` `==` `!=` | Relacionais |

### Exemplo: Fatorial

```
( 1.0 N )
( 8.0 L )
( ( N L <= ) ( ( 1.0 F ) ( 1.0 K ) ( ( K N <= ) ( ( ( F K * ) F ) ( ( K 1.0 + ) K ) ) FOR ) ( ( N 1.0 + ) N ) ) FOR )
```

---

## Otimizacoes Implementadas

1. **Constant Folding** - Avalia expressoes constantes em tempo de compilacao
2. **Algebraic Simplification** - Aplica identidades algebricas (x+0=x, x*1=x, etc)
3. **Constant Propagation** - Propaga valores constantes conhecidos
4. **Dead Code Elimination** - Remove codigo cujo resultado nao e usado
5. **Redundant Jump Elimination** - Remove saltos desnecessarios

---

## Assembly AVR

### Arquitetura Alvo
- **MCU:** ATmega328P (Arduino Uno)
- **Clock:** 16 MHz
- **Ponto Flutuante:** IEEE 754 Half-Precision (16-bit)
- **Comunicacao:** USART 9600 bps

### Convencao de Registradores

| Registrador | Uso |
|-------------|-----|
| R0-R1 | Resultado MUL |
| R16 (temp) | Temporario 1 |
| R17 (temp2) | Temporario 2 |
| R18-R21 | Calculos intermediarios |
| R22-R23 | Operando 2 |
| R24-R25 | Operando 1 / Resultado |
| R26-R27 (X) | Ponteiro |
| R28 | Auxiliar |
| R30-R31 (Z) | Ponteiro Flash |

### Rotinas IEEE 754

| Rotina | Operacao |
|--------|----------|
| `__addhf3` | Adicao |
| `__subhf3` | Subtracao |
| `__mulhf3` | Multiplicacao |
| `__divhf3` | Divisao |
| `__cmphf2` | Comparacao |
| `__powhf3` | Potencia |

---

## Compilacao para Arduino

### Usando Platform.io com VSCode

1. **Estrutura do projeto**

   Certifique-se de que o projeto está organizado em formato PlatformIO, com pelo menos:

   - Um arquivo `platformio.ini` na raiz do projeto, por exemplo:
     ```ini
     [env:uno]
     platform = atmelavr
     board = uno
     framework = arduino
     ```
   - O arquivo de assembly `codigo.S` dentro da pasta `src/`:
     ```
     src/
       codigo.S
     ```

2. **Compilar o código**

   No terminal do VS Code (ou terminal do sistema dentro da pasta do projeto), execute:

   ```bash
   pio run

---

## Validacao

Conecte ao Arduino via serial (9600 bps) para ver os resultados:

```bash

# Windows
# Use o Serial Monitor do Arduino IDE
```

A saida mostra os valores das variaveis em hexadecimal (formato half-precision):

```
N = 0x4800
L = 0x4800
F = 0x6320
K = 0x4900
```
