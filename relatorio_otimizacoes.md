# Relatorio de Otimizacoes do Codigo TAC

**Grupo:** RA3_2  
**Integrante:** Guilherme Knapik (kingnapik)  

---

## Resumo das Otimizacoes

| Metrica | Valor |
|---------|-------|
| Instrucoes originais | 27 |
| Instrucoes finais | 27 |
| Instrucoes removidas | 0 |
| Reducao | 0.0% |
| Total de passadas | 1 |

## Otimizacoes por Tecnica

| Tecnica | Aplicacoes |
|---------|------------|
| Constant Folding | 0 |
| Algebraic Simplification | 0 |
| Constant Propagation | 0 |
| Dead Code Elimination | 0 |
| Redundant Jump Elimination | 0 |
| **Total** | **0** |

---

## Descricao das Tecnicas Implementadas

### 1. Constant Folding (Dobra de Constantes)

Avalia expressoes com operandos constantes em tempo de compilacao.

**Exemplo:**
```
ANTES:  t0 = 2.0 + 3.0
DEPOIS: t0 = 5
```

### 2. Algebraic Simplification (Simplificacao Algebrica)

Aplica identidades algebricas para simplificar expressoes.

**Identidades aplicadas:**
- `x + 0 = x`
- `x - 0 = x`
- `x * 1 = x`
- `x * 0 = 0`
- `x / 1 = x`
- `x ^ 0 = 1`
- `x ^ 1 = x`

### 3. Constant Propagation (Propagacao de Constantes)

Substitui variaveis por seus valores constantes conhecidos.

**Exemplo:**
```
ANTES:  t0 = 5
        t1 = t0 + 3
DEPOIS: t0 = 5
        t1 = 5 + 3
```

### 4. Dead Code Elimination (Eliminacao de Codigo Morto)

Remove instrucoes cujos resultados nunca sao utilizados.

**Exemplo:**
```
ANTES:  t0 = A + B    ; t0 nunca e usado depois
        t1 = C + D
DEPOIS: t1 = C + D
```

### 5. Redundant Jump Elimination (Eliminacao de Saltos Redundantes)

Remove saltos desnecessarios e labels nao referenciados.

**Casos tratados:**
- GOTO para a proxima instrucao
- Labels nao referenciados por nenhum GOTO ou IF_FALSE

---

## Log Detalhado das Otimizacoes

```
--- Inicio da Passada 1 ---
```

---

## Comparacao: Codigo Original vs Otimizado

### Codigo TAC Original

```
0: X = 0.5
1: t0 = X < 0.0
2: IF_FALSE t0 GOTO L0
3: t1 = 0.0 - X
4: X = t1
5: GOTO L1
6: L0:
7: L1:
8: R = 1.0
9: t2 = X ^ 2
10: N2 = t2
11: t3 = N2 | 2.0
12: T2 = t3
13: t4 = R - T2
14: R = t4
15: t5 = X ^ 4
16: N4 = t5
17: t6 = N4 | 24.0
18: T4 = t6
19: t7 = R + T4
20: R = t7
21: t8 = X ^ 6
22: N6 = t8
23: t9 = N6 | 720.0
24: T6 = t9
25: t10 = R - T6
26: R = t10
```

### Codigo TAC Otimizado

```
0: X = 0.5
1: t0 = X < 0.0
2: IF_FALSE t0 GOTO L0
3: t1 = 0.0 - X
4: X = t1
5: GOTO L1
6: L0:
7: L1:
8: R = 1.0
9: t2 = X ^ 2
10: N2 = t2
11: t3 = N2 | 2.0
12: T2 = t3
13: t4 = R - T2
14: R = t4
15: t5 = X ^ 4
16: N4 = t5
17: t6 = N4 | 24.0
18: T4 = t6
19: t7 = R + T4
20: R = t7
21: t8 = X ^ 6
22: N6 = t8
23: t9 = N6 | 720.0
24: T6 = t9
25: t10 = R - T6
26: R = t10
```

---

*Relatorio gerado automaticamente pelo compilador RPN.*
