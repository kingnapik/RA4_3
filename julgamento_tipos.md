# JULGAMENTO DE TIPOS - ARVORES DE DERIVACAO

Este relatorio mostra ONDE e COMO cada regra de inferencia foi aplicada durante a analise.

---

## DEDUCOES POR LINHA

### LINHA 1

#### Deducao: `0.5`

**Tipo inferido:** `real`

**Regra aplicada:** Literal real: 0.5 ∈ ℝ

**Arvore de derivacao:**
```
           0.5 ∈ ℝ
        ─────────────────────
        G |- 0.5 : real
```

#### Deducao: `X := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[X |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[X |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `X := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[X |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[X |-> real] |- MEM : real |- MEM : real
```


---

### LINHA 2

#### Deducao: `0.0`

**Tipo inferido:** `real`

**Regra aplicada:** Literal real: 0.0 ∈ ℝ

**Arvore de derivacao:**
```
           0.0 ∈ ℝ
        ─────────────────────
        G |- 0.0 : real
```

#### Deducao: `e1 < e2`

**Tipo inferido:** `booleano`

**Regra aplicada:** Operador relacional sintetiza booleano: G |- e1:real G |- e2:real => booleano

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

#### Deducao: `0.0`

**Tipo inferido:** `real`

**Regra aplicada:** Literal real: 0.0 ∈ ℝ

**Arvore de derivacao:**
```
           0.0 ∈ ℝ
        ─────────────────────
        G |- 0.0 : real
```

#### Deducao: `e1 - e2`

**Tipo inferido:** `real`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:real G |- e2:real => promover(real,real) = real

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

#### Deducao: `X := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[X |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[X |-> real] |- MEM : real |- MEM : real
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

#### Deducao: `X := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[X |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[X |-> real] |- MEM : real |- MEM : real
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

### LINHA 3

#### Deducao: `1.0`

**Tipo inferido:** `real`

**Regra aplicada:** Literal real: 1.0 ∈ ℝ

**Arvore de derivacao:**
```
           1.0 ∈ ℝ
        ─────────────────────
        G |- 1.0 : real
```

#### Deducao: `R := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[R |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[R |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `R := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[R |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[R |-> real] |- MEM : real |- MEM : real
```


---

### LINHA 4

#### Deducao: `2`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 2 ∈ ℤ

**Arvore de derivacao:**
```
           2 ∈ ℤ
        ─────────────────────
        G |- 2 : int
```

#### Deducao: `e1 ^ e2`

**Tipo inferido:** `real`

**Regra aplicada:** Exponenciacao (expoente int): G |- e1:real G |- e2:int => real

**Arvore de derivacao:**
```
        (regra: Exponenciacao (expoente int): G |- e1:real G |- e2:int => real)
        ────────────────────────────
        G |- e1 ^ e2 : real
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

#### Deducao: `N2 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[N2 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[N2 |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `N2 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[N2 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[N2 |-> real] |- MEM : real |- MEM : real
```


---

### LINHA 5

#### Deducao: `2.0`

**Tipo inferido:** `real`

**Regra aplicada:** Literal real: 2.0 ∈ ℝ

**Arvore de derivacao:**
```
           2.0 ∈ ℝ
        ─────────────────────
        G |- 2.0 : real
```

#### Deducao: `e1 | e2`

**Tipo inferido:** `real`

**Regra aplicada:** Divisao real: G |- e1:real G |- e2:real => real

**Arvore de derivacao:**
```
        (regra: Divisao real: G |- e1:real G |- e2:real => real)
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

#### Deducao: `T2 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[T2 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[T2 |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `T2 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[T2 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[T2 |-> real] |- MEM : real |- MEM : real
```


---

### LINHA 6

#### Deducao: `e1 - e2`

**Tipo inferido:** `real`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:real G |- e2:real => promover(real,real) = real

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

#### Deducao: `R := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[R |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[R |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `R := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[R |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[R |-> real] |- MEM : real |- MEM : real
```


---

### LINHA 7

#### Deducao: `4`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 4 ∈ ℤ

**Arvore de derivacao:**
```
           4 ∈ ℤ
        ─────────────────────
        G |- 4 : int
```

#### Deducao: `e1 ^ e2`

**Tipo inferido:** `real`

**Regra aplicada:** Exponenciacao (expoente int): G |- e1:real G |- e2:int => real

**Arvore de derivacao:**
```
        (regra: Exponenciacao (expoente int): G |- e1:real G |- e2:int => real)
        ────────────────────────────
        G |- e1 ^ e2 : real
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

#### Deducao: `N4 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[N4 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[N4 |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `N4 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[N4 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[N4 |-> real] |- MEM : real |- MEM : real
```


---

### LINHA 8

#### Deducao: `24.0`

**Tipo inferido:** `real`

**Regra aplicada:** Literal real: 24.0 ∈ ℝ

**Arvore de derivacao:**
```
           24.0 ∈ ℝ
        ─────────────────────
        G |- 24.0 : real
```

#### Deducao: `e1 | e2`

**Tipo inferido:** `real`

**Regra aplicada:** Divisao real: G |- e1:real G |- e2:real => real

**Arvore de derivacao:**
```
        (regra: Divisao real: G |- e1:real G |- e2:real => real)
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

#### Deducao: `T4 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[T4 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[T4 |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `T4 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[T4 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[T4 |-> real] |- MEM : real |- MEM : real
```


---

### LINHA 9

#### Deducao: `e1 + e2`

**Tipo inferido:** `real`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:real G |- e2:real => promover(real,real) = real

**Arvore de derivacao:**
```
        G |- e₁ : T₁    G |- e₂ : T₂
        ────────────────────────────
        G |- e1 + e2 : real
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

#### Deducao: `R := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[R |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[R |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `R := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[R |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[R |-> real] |- MEM : real |- MEM : real
```


---

### LINHA 10

#### Deducao: `6`

**Tipo inferido:** `int`

**Regra aplicada:** Literal inteiro: 6 ∈ ℤ

**Arvore de derivacao:**
```
           6 ∈ ℤ
        ─────────────────────
        G |- 6 : int
```

#### Deducao: `e1 ^ e2`

**Tipo inferido:** `real`

**Regra aplicada:** Exponenciacao (expoente int): G |- e1:real G |- e2:int => real

**Arvore de derivacao:**
```
        (regra: Exponenciacao (expoente int): G |- e1:real G |- e2:int => real)
        ────────────────────────────
        G |- e1 ^ e2 : real
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

#### Deducao: `N6 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[N6 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[N6 |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `N6 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[N6 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[N6 |-> real] |- MEM : real |- MEM : real
```


---

### LINHA 11

#### Deducao: `720.0`

**Tipo inferido:** `real`

**Regra aplicada:** Literal real: 720.0 ∈ ℝ

**Arvore de derivacao:**
```
           720.0 ∈ ℝ
        ─────────────────────
        G |- 720.0 : real
```

#### Deducao: `e1 | e2`

**Tipo inferido:** `real`

**Regra aplicada:** Divisao real: G |- e1:real G |- e2:real => real

**Arvore de derivacao:**
```
        (regra: Divisao real: G |- e1:real G |- e2:real => real)
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

#### Deducao: `T6 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[T6 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[T6 |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `T6 := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[T6 |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[T6 |-> real] |- MEM : real |- MEM : real
```


---

### LINHA 12

#### Deducao: `e1 - e2`

**Tipo inferido:** `real`

**Regra aplicada:** Operador aritmetico com promocao: G |- e1:real G |- e2:real => promover(real,real) = real

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

#### Deducao: `R := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[R |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[R |-> real] |- MEM : real |- MEM : real
```

#### Deducao: `R := valor`

**Tipo inferido:** `real`

**Regra aplicada:** MEM sintetiza tipo: G[R |-> real] |- MEM : real

**Arvore de derivacao:**
```
        G |- valor : real
        ─────────────────────────────────
        G[R |-> real] |- MEM : real |- MEM : real
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

- **Total de deducoes:** 58
- **Linhas analisadas:** 12

