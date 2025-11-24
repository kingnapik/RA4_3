# GRAMATICA DE ATRIBUTOS - LINGUAGEM RPN

## 1. Tokens e Tipos

- Tokens: num, var, res, if, for, '(', ')', +, -, *, /, %, |, ^, <, <=, ==, !=, >=, >.
- Tipos: int, real, booleano.

## 2. EBNF (coerente com o parser da Fase 2)

```
P        -> '(' CORPO ')'
CORPO    -> E CORPO'
CORPO'   -> E CORPO' | epsilon

E        -> TERMO | E E OP
TERMO    -> num | var | res | IF | FOR
OP       -> OP_ARIT | OP_REL
OP_ARIT  -> '+' | '-' | '*' | '/' | '%' | '|' | '^'
OP_REL   -> '<' | '<=' | '==' | '!=' | '>=' | '>'

IF       -> E E 'if'     // cond bloco (RPN)
FOR      -> E E 'for'    // iter bloco (RPN)
```

> Observacao (RPN):
> - **Store**: uma linha com `E var` (sem operador subsequente) armazena o valor de `E` em `var` (apenas `int`/`real`).
> - **Load**: `var` sozinho carrega o valor; erro se `var` nao inicializada.

## 3. Atributos (sintetizados por no)

- tipo in {int, real, booleano}
- valido in {true, false}
- lvalue in {true, false} (aplica-se a var)
- inicializada in {true, false} (apenas para var)

## 4. Regras de Tipagem e Erro

### 4.1 Literais e identificadores
- num: inteiro -> int; real -> real. valido=true; lvalue=false.
- var x: tipo = tabela[x].tipo; inicializada = tabela[x].inicializada; valido=true; lvalue=true. ERRO se usada sem inicializar.
- res N: requer resultado existente para N; tipo = tipoResultado(N); valido=true; lvalue=false. ERRO se inexistente.

### 4.2 Aritmeticos basicos (+ - *)
- operandos nao podem ser booleano.
- promocao numerica: se algum operando for real -> resultado real; senao int.

### 4.3 Divisoes e potencia
- '/' e '%' exigem ambos int -> resultado int. ERRO se houver real/booleano.
- '|' divisao real: exige int/real nos dois lados -> resultado real. ERRO se houver booleano.
- '^' potencia: base numerica (int/real) e expoente int -> resultado = tipo da base. ERRO se expoente nao for int ou houver booleano.

### 4.4 Relacionais (< <= == != >= >)
- Para < <= >= >: ambos numericos (int/real) -> resultado booleano.
- Para == !=: int==int, real==real, booleano==booleano -> booleano. ERRO para misturas (ex.: int==booleano).

### 4.5 MEM (armazenamento)
- Forma RPN: E var (armazena E em var).
- Restricoes: E.tipo in {int, real}; var.tipo == E.tipo. ERRO se booleano ou tipos diferentes.
- Efeito: marcar var como inicializada na tabela.
- Tipo do no mem: pode propagar E.tipo (neste projeto, propagamos E.tipo).

### 4.6 IF e FOR
- IF (cond bloco if): cond.tipo == booleano; bloco valido. Tipo resultante: opcional/neutro (nao usado para armazenamento).
- FOR (iter bloco for): iter numerico (ou int, se voce restringiu); bloco valido. Efeito de controle; tipo neutro.

## 5. Mensagens de erro

- Formato no console: ERRO SEMANTICO [Linha X]: <descricao> e Contexto: <trecho> (sem acento, por limitacao de codificacao).

## 6. Exemplos RPN

- 3 5 + -> int
- 2 3 | -> real
- A B + -> int/real (requer A,B inicializadas)
- V var -> marca X como inicializada (exige tipos compativeis e nao booleano)
- cond bloco if -> cond:booleano
- N bloco for -> N numerico
