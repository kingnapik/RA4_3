# Analise da Gramatica

## Regras de Producao (EBNF)

P -> ( CORPO )

CORPO -> E CORPO'

CORPO' -> E CORPO'
       | epsilon

E -> E_ARITIMETICO
  | E_ESPECIAL
  | OP

E_ARITIMETICO -> num
              | P

E_ESPECIAL -> var
           | res
           | if
           | for

OP -> +
   | -
   | *
   | %
   | /
   | |
   | ^
   | >
   | <
   | >=
   | <=
   | ==
   | !=


## Conjunto NULLABLE

```
NULLABLE = { CORPO' }
```

## Conjuntos FIRST

```
FIRST(P) = { ( }
FIRST(CORPO) = { != % ( * + - / < <= == > >= ^ for if num res var | }
FIRST(CORPO') = { != % ( * + - / < <= == > >= ^ for if num res var vazio | }
FIRST(E) = { != % ( * + - / < <= == > >= ^ for if num res var | }
FIRST(E_ARITIMETICO) = { ( num }
FIRST(E_ESPECIAL) = { for if res var }
FIRST(OP) = { != % * + - / < <= == > >= ^ | }
```

## Conjuntos FOLLOW

```
FOLLOW(P) = { != $ % ( ) * + - / < <= == > >= ^ for if num res var | }
FOLLOW(CORPO) = { ) }
FOLLOW(CORPO') = { ) }
FOLLOW(E) = { != % ( ) * + - / < <= == > >= ^ for if num res var | }
FOLLOW(E_ARITIMETICO) = { != % ( ) * + - / < <= == > >= ^ for if num res var | }
FOLLOW(E_ESPECIAL) = { != % ( ) * + - / < <= == > >= ^ for if num res var | }
FOLLOW(OP) = { != % ( ) * + - / < <= == > >= ^ for if num res var | }
```

## Tabela de Analise LL(1)

| Nao-Terminal | != | % | ( | ) | * | + | - | / | < | <= | == | > | >= | ^ | for | if | num | res | var | | | $ |
|--------------|------|------|------|------|------|------|------|------|------|------|------|------|------|------|------|------|------|------|------|------|------|
| P |  |  | P -> ( CORPO ) |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| CORPO | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' |  | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' | CORPO -> E CORPO' |  |
| CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> epsilon | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' | CORPO' -> E CORPO' |  |
| E | E -> OP | E -> OP | E -> E_ARITIMETICO |  | E -> OP | E -> OP | E -> OP | E -> OP | E -> OP | E -> OP | E -> OP | E -> OP | E -> OP | E -> OP | E -> E_ESPECIAL | E -> E_ESPECIAL | E -> E_ARITIMETICO | E -> E_ESPECIAL | E -> E_ESPECIAL | E -> OP |  |
| E_ARITIMETICO |  |  | E_ARITIMETICO -> P |  |  |  |  |  |  |  |  |  |  |  |  |  | E_ARITIMETICO -> num |  |  |  |  |
| E_ESPECIAL |  |  |  |  |  |  |  |  |  |  |  |  |  |  | E_ESPECIAL -> for | E_ESPECIAL -> if |  | E_ESPECIAL -> res | E_ESPECIAL -> var |  |  |
| OP | OP -> != | OP -> % |  |  | OP -> * | OP -> + | OP -> - | OP -> / | OP -> < | OP -> <= | OP -> == | OP -> > | OP -> >= | OP -> ^ |  |  |  |  |  | OP -> | |  |

---
*Nota: As arvores sintaticas de todas as linhas validas serao adicionadas ao final deste arquivo.*

## Arvore Sintatica - Linha 1

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        0.5 (terminal)
    CORPO'
      E
        E_ESPECIAL
          X (terminal)
      CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 2

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        P
          ( (terminal)
          CORPO
            E
              E_ESPECIAL
                X (terminal)
            CORPO'
              E
                E_ARITIMETICO
                  0.0 (terminal)
              CORPO'
                E
                  OP
                    < (terminal)
                CORPO'
          ) (terminal)
    CORPO'
      E
        E_ARITIMETICO
          P
            ( (terminal)
            CORPO
              E
                E_ARITIMETICO
                  P
                    ( (terminal)
                    CORPO
                      E
                        E_ARITIMETICO
                          0.0 (terminal)
                      CORPO'
                        E
                          E_ESPECIAL
                            X (terminal)
                        CORPO'
                          E
                            OP
                              - (terminal)
                          CORPO'
                    ) (terminal)
              CORPO'
                E
                  E_ESPECIAL
                    X (terminal)
                CORPO'
            ) (terminal)
      CORPO'
        E
          E_ARITIMETICO
            P
              ( (terminal)
              CORPO
                E
                  E_ARITIMETICO
                    0.0 (terminal)
                CORPO'
              ) (terminal)
        CORPO'
          E
            E_ESPECIAL
              IF (terminal)
          CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 3

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        1.0 (terminal)
    CORPO'
      E
        E_ESPECIAL
          R (terminal)
      CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 4

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        P
          ( (terminal)
          CORPO
            E
              E_ESPECIAL
                X (terminal)
            CORPO'
              E
                E_ARITIMETICO
                  2 (terminal)
              CORPO'
                E
                  OP
                    ^ (terminal)
                CORPO'
          ) (terminal)
    CORPO'
      E
        E_ESPECIAL
          N2 (terminal)
      CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 5

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        P
          ( (terminal)
          CORPO
            E
              E_ESPECIAL
                N2 (terminal)
            CORPO'
              E
                E_ARITIMETICO
                  2.0 (terminal)
              CORPO'
                E
                  OP
                    | (terminal)
                CORPO'
          ) (terminal)
    CORPO'
      E
        E_ESPECIAL
          T2 (terminal)
      CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 6

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        P
          ( (terminal)
          CORPO
            E
              E_ESPECIAL
                R (terminal)
            CORPO'
              E
                E_ESPECIAL
                  T2 (terminal)
              CORPO'
                E
                  OP
                    - (terminal)
                CORPO'
          ) (terminal)
    CORPO'
      E
        E_ESPECIAL
          R (terminal)
      CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 7

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        P
          ( (terminal)
          CORPO
            E
              E_ESPECIAL
                X (terminal)
            CORPO'
              E
                E_ARITIMETICO
                  4 (terminal)
              CORPO'
                E
                  OP
                    ^ (terminal)
                CORPO'
          ) (terminal)
    CORPO'
      E
        E_ESPECIAL
          N4 (terminal)
      CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 8

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        P
          ( (terminal)
          CORPO
            E
              E_ESPECIAL
                N4 (terminal)
            CORPO'
              E
                E_ARITIMETICO
                  24.0 (terminal)
              CORPO'
                E
                  OP
                    | (terminal)
                CORPO'
          ) (terminal)
    CORPO'
      E
        E_ESPECIAL
          T4 (terminal)
      CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 9

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        P
          ( (terminal)
          CORPO
            E
              E_ESPECIAL
                R (terminal)
            CORPO'
              E
                E_ESPECIAL
                  T4 (terminal)
              CORPO'
                E
                  OP
                    + (terminal)
                CORPO'
          ) (terminal)
    CORPO'
      E
        E_ESPECIAL
          R (terminal)
      CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 10

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        P
          ( (terminal)
          CORPO
            E
              E_ESPECIAL
                X (terminal)
            CORPO'
              E
                E_ARITIMETICO
                  6 (terminal)
              CORPO'
                E
                  OP
                    ^ (terminal)
                CORPO'
          ) (terminal)
    CORPO'
      E
        E_ESPECIAL
          N6 (terminal)
      CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 11

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        P
          ( (terminal)
          CORPO
            E
              E_ESPECIAL
                N6 (terminal)
            CORPO'
              E
                E_ARITIMETICO
                  720.0 (terminal)
              CORPO'
                E
                  OP
                    | (terminal)
                CORPO'
          ) (terminal)
    CORPO'
      E
        E_ESPECIAL
          T6 (terminal)
      CORPO'
  ) (terminal)
```

## Arvore Sintatica - Linha 12

```
P
  ( (terminal)
  CORPO
    E
      E_ARITIMETICO
        P
          ( (terminal)
          CORPO
            E
              E_ESPECIAL
                R (terminal)
            CORPO'
              E
                E_ESPECIAL
                  T6 (terminal)
              CORPO'
                E
                  OP
                    - (terminal)
                CORPO'
          ) (terminal)
    CORPO'
      E
        E_ESPECIAL
          R (terminal)
      CORPO'
  ) (terminal)
```

