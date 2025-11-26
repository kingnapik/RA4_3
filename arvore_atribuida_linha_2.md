# Árvore Sintática Abstrata Atribuída - Linha 2

```
P : booleano
  (
  CORPO : booleano
    E : booleano
      E_ARITIMETICO : booleano
        P : booleano
          (
          CORPO : booleano
            E : real
              E_ESPECIAL : real
                X
            CORPO' : booleano
              E : real
                E_ARITIMETICO : real
                  0.0 : real
              CORPO'
                E : booleano
                  OP : booleano
                    < : booleano
                CORPO'
          )
    CORPO'
      E : real
        E_ARITIMETICO : real
          P : real
            (
            CORPO : real
              E : real
                E_ARITIMETICO : real
                  P : real
                    (
                    CORPO : real
                      E : real
                        E_ARITIMETICO : real
                          0.0 : real
                      CORPO' : real
                        E : real
                          E_ESPECIAL : real
                            X
                        CORPO'
                          E : real
                            OP : real
                              - : real
                          CORPO'
                    )
              CORPO'
                E : real
                  E_ESPECIAL
                    X : real
                CORPO'
            )
      CORPO'
        E
          E_ARITIMETICO
            P
              (
              CORPO
                E
                  E_ARITIMETICO
                    0.0
                CORPO'
              )
        CORPO'
          E
            E_ESPECIAL
              IF
          CORPO'
  )
```
