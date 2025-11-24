# JULGAMENTO DE TIPOS - ARVORES DE DERIVACAO

Este relatorio mostra ONDE e COMO cada regra de inferencia foi aplicada durante a analise.

---

## DEDUCOES POR LINHA

### LINHA 1

#### Deducao: `10`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 10 ∈ ℤ

**Arvore de derivacao:**
```
           10 ∈ ℤ
        ─────────────────────
        G |- 10 : int
```

#### Deducao: `5`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 5 ∈ ℤ

**Arvore de derivacao:**
```
           5 ∈ ℤ
        ─────────────────────
        G |- 5 : int
```

#### Deducao: `e1 + e2`

**Tipo inferido:** `int`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:int G |- e2:int => promover(int,int) = int

**Arvore de derivacao:**
```
        G |- e₁ : T₁    G |- e₂ : T₂
        ────────────────────────────
        G |- e1 + e2 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `int`

**Regra aplicada:** Expressao parentizada: G |- (e) : int

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : int)
        ────────────────────────────
        G |- (CORPO) : int
```


---

### LINHA 2

#### Deducao: `20.1`

**Tipo inferido:** `real`

**Regra aplicada:** Literal real: 20.1 ∈ ℝ

**Arvore de derivacao:**
```
           20.1 ∈ ℝ
        ─────────────────────
        G |- 20.1 : real
```

#### Deducao: `4`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 4 ∈ ℤ

**Arvore de derivacao:**
```
           4 ∈ ℤ
        ─────────────────────
        G |- 4 : int
```

#### Deducao: `e1 - e2`

**Tipo inferido:** `real`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:real G |- e2:int => promover(real,int) = real

**Arvore de derivacao:**
```
        G |- e₁ : T₁    G |- e₂ : T₂
        ────────────────────────────
        G |- e1 - e2 : real
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `real`

**Regra aplicada:** Expressao parentizada: G |- (e) : real

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : real)
        ────────────────────────────
        G |- (CORPO) : real
```


---

### LINHA 3

#### Deducao: `7`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 7 ∈ ℤ

**Arvore de derivacao:**
```
           7 ∈ ℤ
        ─────────────────────
        G |- 7 : int
```

#### Deducao: `3`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 3 ∈ ℤ

**Arvore de derivacao:**
```
           3 ∈ ℤ
        ─────────────────────
        G |- 3 : int
```

#### Deducao: `e1 * e2`

**Tipo inferido:** `int`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:int G |- e2:int => promover(int,int) = int

**Arvore de derivacao:**
```
        G |- e₁ : T₁    G |- e₂ : T₂
        ────────────────────────────
        G |- e1 * e2 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `int`

**Regra aplicada:** Expressao parentizada: G |- (e) : int

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : int)
        ────────────────────────────
        G |- (CORPO) : int
```


---

### LINHA 4

#### Deducao: `15`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 15 ∈ ℤ

**Arvore de derivacao:**
```
           15 ∈ ℤ
        ─────────────────────
        G |- 15 : int
```

#### Deducao: `3`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 3 ∈ ℤ

**Arvore de derivacao:**
```
           3 ∈ ℤ
        ─────────────────────
        G |- 3 : int
```

#### Deducao: `e1 / e2`

**Tipo inferido:** `int`

**Regra aplicada:** Operacao inteira: G |- e1:int G |- e2:int => int

**Arvore de derivacao:**
```
        (regra: Operacao inteira: G |- e1:int G |- e2:int => int)
        ────────────────────────────
        G |- e1 / e2 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `int`

**Regra aplicada:** Expressao parentizada: G |- (e) : int

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : int)
        ────────────────────────────
        G |- (CORPO) : int
```


---

### LINHA 5

#### Deducao: `22`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 22 ∈ ℤ

**Arvore de derivacao:**
```
           22 ∈ ℤ
        ─────────────────────
        G |- 22 : int
```

#### Deducao: `7`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 7 ∈ ℤ

**Arvore de derivacao:**
```
           7 ∈ ℤ
        ─────────────────────
        G |- 7 : int
```

#### Deducao: `e1 % e2`

**Tipo inferido:** `int`

**Regra aplicada:** Operacao inteira: G |- e1:int G |- e2:int => int

**Arvore de derivacao:**
```
        (regra: Operacao inteira: G |- e1:int G |- e2:int => int)
        ────────────────────────────
        G |- e1 % e2 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `int`

**Regra aplicada:** Expressao parentizada: G |- (e) : int

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : int)
        ────────────────────────────
        G |- (CORPO) : int
```


---

### LINHA 6

#### Deducao: `2`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 2 ∈ ℤ

**Arvore de derivacao:**
```
           2 ∈ ℤ
        ─────────────────────
        G |- 2 : int
```

#### Deducao: `8.1`

**Tipo inferido:** `real`

**Regra aplicada:** Literal real: 8.1 ∈ ℝ

**Arvore de derivacao:**
```
           8.1 ∈ ℝ
        ─────────────────────
        G |- 8.1 : real
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `erro`

**Regra aplicada:** Expressao parentizada: G |- (e) : erro

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : erro)
        ────────────────────────────
        G |- (CORPO) : erro
```


---

### LINHA 7

#### Deducao: `100`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 100 ∈ ℤ

**Arvore de derivacao:**
```
           100 ∈ ℤ
        ─────────────────────
        G |- 100 : int
```

#### Deducao: `25`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 25 ∈ ℤ

**Arvore de derivacao:**
```
           25 ∈ ℤ
        ─────────────────────
        G |- 25 : int
```

#### Deducao: `e1 | e2`

**Tipo inferido:** `real`

**Regra aplicada:** Divisao real: G |- e1:int G |- e2:int => real

**Arvore de derivacao:**
```
        (regra: Divisao real: G |- e1:int G |- e2:int => real)
        ────────────────────────────
        G |- e1 | e2 : real
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `real`

**Regra aplicada:** Expressao parentizada: G |- (e) : real

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : real)
        ────────────────────────────
        G |- (CORPO) : real
```


---

### LINHA 8

#### Deducao: `50`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 50 ∈ ℤ

**Arvore de derivacao:**
```
           50 ∈ ℤ
        ─────────────────────
        G |- 50 : int
```

#### Deducao: `A := valor`

**Tipo inferido:** `int`

**Regra aplicada:** MEM sintetiza tipo: G[A |-> int] |- MEM : int

**Arvore de derivacao:**
```
        G |- valor : int
        ─────────────────────────────────
        G[A |-> int] |- MEM : int |- MEM : int
```

#### Deducao: `A := valor`

**Tipo inferido:** `int`

**Regra aplicada:** MEM sintetiza tipo: G[A |-> int] |- MEM : int

**Arvore de derivacao:**
```
        G |- valor : int
        ─────────────────────────────────
        G[A |-> int] |- MEM : int |- MEM : int
```


---

### LINHA 9

#### Deducao: `10`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 10 ∈ ℤ

**Arvore de derivacao:**
```
           10 ∈ ℤ
        ─────────────────────
        G |- 10 : int
```

#### Deducao: `e1 > e2`

**Tipo inferido:** `booleano`

**Regra aplicada:** Operador relacional sintetiza booleano: G |- e1:int G |- e2:int => booleano

**Arvore de derivacao:**
```
        G |- e₁ : T    G |- e₂ : T
        ────────────────────────────
        G |- e1 > e2 : booleano
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `booleano`

**Regra aplicada:** Expressao parentizada: G |- (e) : booleano

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : booleano)
        ────────────────────────────
        G |- (CORPO) : booleano
```


---

### LINHA 10

#### Deducao: `10`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 10 ∈ ℤ

**Arvore de derivacao:**
```
           10 ∈ ℤ
        ─────────────────────
        G |- 10 : int
```

#### Deducao: `e1 > e2`

**Tipo inferido:** `booleano`

**Regra aplicada:** Operador relacional sintetiza booleano: G |- e1:int G |- e2:int => booleano

**Arvore de derivacao:**
```
        G |- e₁ : T    G |- e₂ : T
        ────────────────────────────
        G |- e1 > e2 : booleano
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `booleano`

**Regra aplicada:** Expressao parentizada: G |- (e) : booleano

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : booleano)
        ────────────────────────────
        G |- (CORPO) : booleano
```

#### Deducao: `2`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 2 ∈ ℤ

**Arvore de derivacao:**
```
           2 ∈ ℤ
        ─────────────────────
        G |- 2 : int
```

#### Deducao: `e1 * e2`

**Tipo inferido:** `int`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:int G |- e2:int => promover(int,int) = int

**Arvore de derivacao:**
```
        G |- e₁ : T₁    G |- e₂ : T₂
        ────────────────────────────
        G |- e1 * e2 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `int`

**Regra aplicada:** Expressao parentizada: G |- (e) : int

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : int)
        ────────────────────────────
        G |- (CORPO) : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `booleano`

**Regra aplicada:** Expressao parentizada: G |- (e) : booleano

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : booleano)
        ────────────────────────────
        G |- (CORPO) : booleano
```

#### Deducao: `IF encontrado`

**Tipo inferido:** `marker`

**Regra aplicada:** Marcador IF

**Arvore de derivacao:**
```
        (regra: Marcador IF)
        ────────────────────────────
        G |- IF encontrado : marker
```


---

### LINHA 11

#### Deducao: `1`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 1 ∈ ℤ

**Arvore de derivacao:**
```
           1 ∈ ℤ
        ─────────────────────
        G |- 1 : int
```

#### Deducao: `RES := valor`

**Tipo inferido:** `int`

**Regra aplicada:** MEM sintetiza tipo: G[RES |-> int] |- MEM : int

**Arvore de derivacao:**
```
        G |- valor : int
        ─────────────────────────────────
        G[RES |-> int] |- MEM : int |- MEM : int
```

#### Deducao: `RES := valor`

**Tipo inferido:** `int`

**Regra aplicada:** MEM sintetiza tipo: G[RES |-> int] |- MEM : int

**Arvore de derivacao:**
```
        G |- valor : int
        ─────────────────────────────────
        G[RES |-> int] |- MEM : int |- MEM : int
```


---

### LINHA 12

#### Deducao: `100`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 100 ∈ ℤ

**Arvore de derivacao:**
```
           100 ∈ ℤ
        ─────────────────────
        G |- 100 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `erro`

**Regra aplicada:** Expressao parentizada: G |- (e) : erro

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : erro)
        ────────────────────────────
        G |- (CORPO) : erro
```

#### Deducao: `4`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 4 ∈ ℤ

**Arvore de derivacao:**
```
           4 ∈ ℤ
        ─────────────────────
        G |- 4 : int
```

#### Deducao: `4`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 4 ∈ ℤ

**Arvore de derivacao:**
```
           4 ∈ ℤ
        ─────────────────────
        G |- 4 : int
```

#### Deducao: `e1 * e2`

**Tipo inferido:** `int`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:int G |- e2:int => promover(int,int) = int

**Arvore de derivacao:**
```
        G |- e₁ : T₁    G |- e₂ : T₂
        ────────────────────────────
        G |- e1 * e2 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `int`

**Regra aplicada:** Expressao parentizada: G |- (e) : int

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : int)
        ────────────────────────────
        G |- (CORPO) : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `erro`

**Regra aplicada:** Expressao parentizada: G |- (e) : erro

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : erro)
        ────────────────────────────
        G |- (CORPO) : erro
```

#### Deducao: `FOR encontrado`

**Tipo inferido:** `marker`

**Regra aplicada:** Marcador FOR

**Arvore de derivacao:**
```
        (regra: Marcador FOR)
        ────────────────────────────
        G |- FOR encontrado : marker
```


---

### LINHA 13

#### Deducao: `8`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 8 ∈ ℤ

**Arvore de derivacao:**
```
           8 ∈ ℤ
        ─────────────────────
        G |- 8 : int
```

#### Deducao: `4`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 4 ∈ ℤ

**Arvore de derivacao:**
```
           4 ∈ ℤ
        ─────────────────────
        G |- 4 : int
```

#### Deducao: `e1 + e2`

**Tipo inferido:** `int`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:int G |- e2:int => promover(int,int) = int

**Arvore de derivacao:**
```
        G |- e₁ : T₁    G |- e₂ : T₂
        ────────────────────────────
        G |- e1 + e2 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `int`

**Regra aplicada:** Expressao parentizada: G |- (e) : int

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : int)
        ────────────────────────────
        G |- (CORPO) : int
```

#### Deducao: `3`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 3 ∈ ℤ

**Arvore de derivacao:**
```
           3 ∈ ℤ
        ─────────────────────
        G |- 3 : int
```

#### Deducao: `2`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 2 ∈ ℤ

**Arvore de derivacao:**
```
           2 ∈ ℤ
        ─────────────────────
        G |- 2 : int
```

#### Deducao: `e1 * e2`

**Tipo inferido:** `int`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:int G |- e2:int => promover(int,int) = int

**Arvore de derivacao:**
```
        G |- e₁ : T₁    G |- e₂ : T₂
        ────────────────────────────
        G |- e1 * e2 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `int`

**Regra aplicada:** Expressao parentizada: G |- (e) : int

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : int)
        ────────────────────────────
        G |- (CORPO) : int
```

#### Deducao: `e1 - e2`

**Tipo inferido:** `int`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:int G |- e2:int => promover(int,int) = int

**Arvore de derivacao:**
```
        G |- e₁ : T₁    G |- e₂ : T₂
        ────────────────────────────
        G |- e1 - e2 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `int`

**Regra aplicada:** Expressao parentizada: G |- (e) : int

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : int)
        ────────────────────────────
        G |- (CORPO) : int
```


---

### LINHA 14

#### Deducao: `100`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 100 ∈ ℤ

**Arvore de derivacao:**
```
           100 ∈ ℤ
        ─────────────────────
        G |- 100 : int
```

#### Deducao: `B := valor`

**Tipo inferido:** `int`

**Regra aplicada:** MEM sintetiza tipo: G[B |-> int] |- MEM : int

**Arvore de derivacao:**
```
        G |- valor : int
        ─────────────────────────────────
        G[B |-> int] |- MEM : int |- MEM : int
```

#### Deducao: `B := valor`

**Tipo inferido:** `int`

**Regra aplicada:** MEM sintetiza tipo: G[B |-> int] |- MEM : int

**Arvore de derivacao:**
```
        G |- valor : int
        ─────────────────────────────────
        G[B |-> int] |- MEM : int |- MEM : int
```


---

### LINHA 15

#### Deducao: `e1 < e2`

**Tipo inferido:** `booleano`

**Regra aplicada:** Operador relacional sintetiza booleano: G |- e1:int G |- e2:int => booleano

**Arvore de derivacao:**
```
        G |- e₁ : T    G |- e₂ : T
        ────────────────────────────
        G |- e1 < e2 : booleano
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `booleano`

**Regra aplicada:** Expressao parentizada: G |- (e) : booleano

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : booleano)
        ────────────────────────────
        G |- (CORPO) : booleano
```


---

### LINHA 16

#### Deducao: `e1 < e2`

**Tipo inferido:** `booleano`

**Regra aplicada:** Operador relacional sintetiza booleano: G |- e1:int G |- e2:int => booleano

**Arvore de derivacao:**
```
        G |- e₁ : T    G |- e₂ : T
        ────────────────────────────
        G |- e1 < e2 : booleano
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `booleano`

**Regra aplicada:** Expressao parentizada: G |- (e) : booleano

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : booleano)
        ────────────────────────────
        G |- (CORPO) : booleano
```

#### Deducao: `e1 + e2`

**Tipo inferido:** `int`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:int G |- e2:int => promover(int,int) = int

**Arvore de derivacao:**
```
        G |- e₁ : T₁    G |- e₂ : T₂
        ────────────────────────────
        G |- e1 + e2 : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `int`

**Regra aplicada:** Expressao parentizada: G |- (e) : int

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : int)
        ────────────────────────────
        G |- (CORPO) : int
```

#### Deducao: `(CORPO)`

**Tipo inferido:** `booleano`

**Regra aplicada:** Expressao parentizada: G |- (e) : booleano

**Arvore de derivacao:**
```
        (regra: Expressao parentizada: G |- (e) : booleano)
        ────────────────────────────
        G |- (CORPO) : booleano
```

#### Deducao: `IF encontrado`

**Tipo inferido:** `marker`

**Regra aplicada:** Marcador IF

**Arvore de derivacao:**
```
        (regra: Marcador IF)
        ────────────────────────────
        G |- IF encontrado : marker
```


---

## CATALOGO DE REGRAS APLICADAS

### Regras de Literais

#### (INT-LIT) - Literal Inteiro
```
         n ∈ ℤ
    ───────────────
     G |- n : int
```

#### (REAL-LIT) - Literal Real
```
         r ∈ ℝ
    ───────────────
     G |- r : real
```

### Regras de Operacoes Aritmeticas

#### (DIV-INT) - Divisao Inteira (requer ambos int)
```
    G |- e₁ : int    G |- e₂ : int
    ─────────────────────────────────
          G |- e₁ / e₂ : int
```

#### (MOD-INT) - Modulo (requer ambos int)
```
    G |- e₁ : int    G |- e₂ : int
    ─────────────────────────────────
          G |- e₁ % e₂ : int
```

#### (POW) - Exponenciacao (expoente deve ser int)
```
    G |- e₁ : T    G |- e₂ : int
    ────────────────────────────
          G |- e₁ ^ e₂ : T
```

#### (PROMO) - Aritmeticos com promocao ( +  -  *  | )
```
    G |- e₁ : T₁    G |- e₂ : T₂     T₁,T₂ ∈ {int,real}
    ───────────────────────────────────────────────────
        G |- e₁ op e₂ : promover(T₁, T₂)
```

### Regras de Operacoes Relacionais

#### (REL-CMP) - Comparacao (sintetiza booleano; opera sobre numeros)
```
    G |- e₁ : T    G |- e₂ : T    T ∈ {int,real}
    op ∈ {>, <, >=, <=, ==, !=}
    ─────────────────────────────────────────────
             G |- e₁ op e₂ : booleano
```

### Regras de Memoria

#### (STORE) - Armazenamento de memoria (nao aceita booleano)
```
    G |- v : T    T ∈ {int, real}
    ───────────────────────────────
      G[x ↦ T] |- (v x) : T
```

#### (MEM-LOAD) - Leitura de Memoria
```
    x ∈ dom(G)    G(x) = T
    ─────────────────────────
         G |- x : T
```

#### (RES) - RES sintetiza tipo da linha referenciada
```
    G |- n : int    linha_atual - n > 0    R(linha_atual - n) = T
    ─────────────────────────────────────────────────────────────
                         G |- n RES : T
```

### Regras de Controle

#### (IF-THEN-ELSE) - IF sintetiza o tipo dos ramos (T)
```
    G |- cond : booleano    G |- e₁ : T    G |- e₂ : T
    ─────────────────────────────────────────────────
            G |- (cond e₁ e₂ IF) : T
```

#### (FOR-LOOP) - FOR sintetiza o tipo do corpo (T)
```
    G |- inicio : int    G |- fim : int    G |- corpo : T
    ─────────────────────────────────────────────────
            G |- (inicio fim corpo FOR) : T
```

---

## FUNCAO DE PROMOCAO DE TIPOS

```
promover : {int,real} × {int,real} → {int,real}

promover(T₁, T₂) = {
    real, se T₁ = real ∨ T₂ = real
    int,  se T₁ = int  ∧ T₂ = int
}
```

**Exemplos:** promover(int,real)=real; promover(int,int)=int; promover(real,real)=real.

---

## ESTATISTICAS

- **Total de deducoes:** 73
- **Linhas analisadas:** 16

