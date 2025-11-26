; --- CODIGO AVR ASSEMBLY GERADO ---
#define __SFR_OFFSET 0
#include <avr/io.h>

#define temp r16
#define temp2 r17

.global setup
.global loop
.section .data
N2: .skip 2
N4: .skip 2
N6: .skip 2
R: .skip 2
T2: .skip 2
T4: .skip 2
T6: .skip 2
X: .skip 2
t0: .skip 2
t1: .skip 2
t10: .skip 2
t2: .skip 2
t3: .skip 2
t4: .skip 2
t5: .skip 2
t6: .skip 2
t7: .skip 2
t8: .skip 2
t9: .skip 2

.section .text

str_N2: .asciz "N2 = 0x"
str_N4: .asciz "N4 = 0x"
str_N6: .asciz "N6 = 0x"
str_R: .asciz "R = 0x"
str_T2: .asciz "T2 = 0x"
str_T4: .asciz "T4 = 0x"
str_T6: .asciz "T6 = 0x"
str_X: .asciz "X = 0x"
str_t0: .asciz "t0 = 0x"
str_t1: .asciz "t1 = 0x"
str_t10: .asciz "t10 = 0x"
str_t2: .asciz "t2 = 0x"
str_t3: .asciz "t3 = 0x"
str_t4: .asciz "t4 = 0x"
str_t5: .asciz "t5 = 0x"
str_t6: .asciz "t6 = 0x"
str_t7: .asciz "t7 = 0x"
str_t8: .asciz "t8 = 0x"
str_t9: .asciz "t9 = 0x"
str_newline: .asciz "\r\n"

loop:
    ret                         

setup:
    ldi     temp, lo8(RAMEND)   
    out     _SFR_IO_ADDR(SPL), temp
    ldi     temp, hi8(RAMEND)   
    out     _SFR_IO_ADDR(SPH), temp
    rcall   USART_Init          

; X = 0.5 := 
    ldi     r24, 0x00           
    ldi     r25, 0x38           
    sts     X, r24              
    sts     X+1, r25            
; t0 = X < 0.0
    lds     r24, X              
    lds     r25, X+1            
    ldi     r22, 0x00           
    ldi     r23, 0x00           
    rcall   __cmphf2            
    cpi     r24, 0xFF           
    breq    CMP_TRUE_0          ; Se r24 == -1, A < B
    clr     r24                 
    clr     r25                 
    rjmp    CMP_END_0           
CMP_TRUE_0:
    ldi     r24, 1              
    clr     r25                 
CMP_END_0:
    sts     t0, r24             
    sts     t0+1, r25           
; L0 = t0 IF_FALSE 
    lds     r24, t0             
    lds     r25, t0+1           
    or      r24, r25            
    brne    IF_SKIP_0           
    rjmp    L0                  
IF_SKIP_0:
; t1 = 0.0 - X
    ldi     r24, 0x00           
    ldi     r25, 0x00           
    lds     r22, X              
    lds     r23, X+1            
    rcall   __subhf3            
    sts     t1, r24             
    sts     t1+1, r25           
; X = t1 := 
    lds     r24, t1             
    lds     r25, t1+1           
    sts     X, r24              
    sts     X+1, r25            
; L1 =  GOTO 
    rjmp    L1                  
; L0 =  LABEL 
L0:
; L1 =  LABEL 
L1:
; R = 1.0 := 
    ldi     r24, 0x00           
    ldi     r25, 0x3C           
    sts     R, r24              
    sts     R+1, r25            
; t2 = X ^ 2
    lds     r24, X              
    lds     r25, X+1            
    ldi     r22, 0x02           
    ldi     r23, 0x00           
    rcall   __powhf3            
    sts     t2, r24             
    sts     t2+1, r25           
; N2 = t2 := 
    lds     r24, t2             
    lds     r25, t2+1           
    sts     N2, r24             
    sts     N2+1, r25           
; t3 = N2 | 2.0
    lds     r24, N2             
    lds     r25, N2+1           
    ldi     r22, 0x00           
    ldi     r23, 0x40           
    rcall   __divhf3            
    sts     t3, r24             
    sts     t3+1, r25           
; T2 = t3 := 
    lds     r24, t3             
    lds     r25, t3+1           
    sts     T2, r24             
    sts     T2+1, r25           
; t4 = R - T2
    lds     r24, R              
    lds     r25, R+1            
    lds     r22, T2             
    lds     r23, T2+1           
    rcall   __subhf3            
    sts     t4, r24             
    sts     t4+1, r25           
; R = t4 := 
    lds     r24, t4             
    lds     r25, t4+1           
    sts     R, r24              
    sts     R+1, r25            
; t5 = X ^ 4
    lds     r24, X              
    lds     r25, X+1            
    ldi     r22, 0x04           
    ldi     r23, 0x00           
    rcall   __powhf3            
    sts     t5, r24             
    sts     t5+1, r25           
; N4 = t5 := 
    lds     r24, t5             
    lds     r25, t5+1           
    sts     N4, r24             
    sts     N4+1, r25           
; t6 = N4 | 24.0
    lds     r24, N4             
    lds     r25, N4+1           
    ldi     r22, 0x00           
    ldi     r23, 0x4E           
    rcall   __divhf3            
    sts     t6, r24             
    sts     t6+1, r25           
; T4 = t6 := 
    lds     r24, t6             
    lds     r25, t6+1           
    sts     T4, r24             
    sts     T4+1, r25           
; t7 = R + T4
    lds     r24, R              
    lds     r25, R+1            
    lds     r22, T4             
    lds     r23, T4+1           
    rcall   __addhf3            
    sts     t7, r24             
    sts     t7+1, r25           
; R = t7 := 
    lds     r24, t7             
    lds     r25, t7+1           
    sts     R, r24              
    sts     R+1, r25            
; t8 = X ^ 6
    lds     r24, X              
    lds     r25, X+1            
    ldi     r22, 0x06           
    ldi     r23, 0x00           
    rcall   __powhf3            
    sts     t8, r24             
    sts     t8+1, r25           
; N6 = t8 := 
    lds     r24, t8             
    lds     r25, t8+1           
    sts     N6, r24             
    sts     N6+1, r25           
; t9 = N6 | 720.0
    lds     r24, N6             
    lds     r25, N6+1           
    ldi     r22, 0xA0           
    ldi     r23, 0x61           
    rcall   __divhf3            
    sts     t9, r24             
    sts     t9+1, r25           
; T6 = t9 := 
    lds     r24, t9             
    lds     r25, t9+1           
    sts     T6, r24             
    sts     T6+1, r25           
; t10 = R - T6
    lds     r24, R              
    lds     r25, R+1            
    lds     r22, T6             
    lds     r23, T6+1           
    rcall   __subhf3            
    sts     t10, r24            
    sts     t10+1, r25          
; R = t10 := 
    lds     r24, t10            
    lds     r25, t10+1          
    sts     R, r24              
    sts     R+1, r25            
    rcall   DUMP_VARS           
    rjmp    FIM_DO_PROGRAMA     

; --- DUMP RESULTADOS ---
DUMP_VARS:
    ldi     r30, lo8(str_N2)    
    ldi     r31, hi8(str_N2)    
    rcall   USART_PrintStr      
    lds     r24, N2             
    lds     r25, N2+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_N4)    
    ldi     r31, hi8(str_N4)    
    rcall   USART_PrintStr      
    lds     r24, N4             
    lds     r25, N4+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_N6)    
    ldi     r31, hi8(str_N6)    
    rcall   USART_PrintStr      
    lds     r24, N6             
    lds     r25, N6+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_R)     
    ldi     r31, hi8(str_R)     
    rcall   USART_PrintStr      
    lds     r24, R              
    lds     r25, R+1            
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_T2)    
    ldi     r31, hi8(str_T2)    
    rcall   USART_PrintStr      
    lds     r24, T2             
    lds     r25, T2+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_T4)    
    ldi     r31, hi8(str_T4)    
    rcall   USART_PrintStr      
    lds     r24, T4             
    lds     r25, T4+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_T6)    
    ldi     r31, hi8(str_T6)    
    rcall   USART_PrintStr      
    lds     r24, T6             
    lds     r25, T6+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_X)     
    ldi     r31, hi8(str_X)     
    rcall   USART_PrintStr      
    lds     r24, X              
    lds     r25, X+1            
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t0)    
    ldi     r31, hi8(str_t0)    
    rcall   USART_PrintStr      
    lds     r24, t0             
    lds     r25, t0+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t1)    
    ldi     r31, hi8(str_t1)    
    rcall   USART_PrintStr      
    lds     r24, t1             
    lds     r25, t1+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t10)   
    ldi     r31, hi8(str_t10)   
    rcall   USART_PrintStr      
    lds     r24, t10            
    lds     r25, t10+1          
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t2)    
    ldi     r31, hi8(str_t2)    
    rcall   USART_PrintStr      
    lds     r24, t2             
    lds     r25, t2+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t3)    
    ldi     r31, hi8(str_t3)    
    rcall   USART_PrintStr      
    lds     r24, t3             
    lds     r25, t3+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t4)    
    ldi     r31, hi8(str_t4)    
    rcall   USART_PrintStr      
    lds     r24, t4             
    lds     r25, t4+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t5)    
    ldi     r31, hi8(str_t5)    
    rcall   USART_PrintStr      
    lds     r24, t5             
    lds     r25, t5+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t6)    
    ldi     r31, hi8(str_t6)    
    rcall   USART_PrintStr      
    lds     r24, t6             
    lds     r25, t6+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t7)    
    ldi     r31, hi8(str_t7)    
    rcall   USART_PrintStr      
    lds     r24, t7             
    lds     r25, t7+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t8)    
    ldi     r31, hi8(str_t8)    
    rcall   USART_PrintStr      
    lds     r24, t8             
    lds     r25, t8+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ldi     r30, lo8(str_t9)    
    ldi     r31, hi8(str_t9)    
    rcall   USART_PrintStr      
    lds     r24, t9             
    lds     r25, t9+1           
    mov     temp, r25           
    rcall   USART_PrintHexByte  
    mov     temp, r24           
    rcall   USART_PrintHexByte  
    ldi     r30, lo8(str_newline)
    ldi     r31, hi8(str_newline)
    rcall   USART_PrintStr      
    ret                         
FIM_DO_PROGRAMA:
    rjmp    FIM_DO_PROGRAMA     

; --- DRIVER SERIAL ---
USART_Init:
    ldi     temp, 103           
    sts     UBRR0L, temp        
    clr     temp                
    sts     UBRR0H, temp        
    ldi     temp, (1<<TXEN0)    
    sts     UCSR0B, temp        
    ldi     temp, (1<<UCSZ01)|(1<<UCSZ00)
    sts     UCSR0C, temp        
    ret                         
USART_Tx:
USART_Tx_Wait:
    lds     temp2, UCSR0A       
    sbrs    temp2, UDRE0        
    rjmp    USART_Tx_Wait       
    sts     UDR0, temp          
    ret                         
USART_PrintStr:
PrintStr_Loop:
    lpm     temp, Z+            
    cpi     temp, 0             
    breq    PrintStr_Done       
    rcall   USART_Tx            
    rjmp    PrintStr_Loop       
PrintStr_Done:
    ret                         
USART_PrintHexByte:
    push    temp                
    mov     temp2, temp         
    swap    temp2               
    andi    temp2, 0x0F         
    rcall   HexToAscii          
    mov     temp, temp2         
    rcall   USART_Tx            
    pop     temp                
    andi    temp, 0x0F          
    mov     temp2, temp         
    rcall   HexToAscii          
    mov     temp, temp2         
    rcall   USART_Tx            
    ret                         
HexToAscii:
    cpi     temp2, 10           
    brlo    HexDigit            
    subi    temp2, -7           
HexDigit:
    subi    temp2, -48          
    ret                         

; --- IEEE 754 HALF-PRECISION MATH ---
__subhf3:
    ldi     r20, 0x80           
    eor     r23, r20            
    rjmp    __addhf3            
__addhf3:
    push    r16                 
    push    r17                 
    push    r18                 
    push    r19                 
    push    r20                 
    push    r21                 
    push    r26                 
    push    r27                 
    push    r28                 
    mov     r16, r24            
    andi    r16, 0x7F           
    or      r16, r25            
    brne    __add_a_nz          
    mov     r24, r22            
    mov     r25, r23            
    rjmp    __add_done          
__add_a_nz:
    mov     r16, r22            
    andi    r16, 0x7F           
    or      r16, r23            
    brne    __add_b_nz          
    rjmp    __add_done          
__add_b_nz:
    mov     r16, r25            
    andi    r16, 0x80           
    mov     r17, r23            
    andi    r17, 0x80           
    mov     r18, r25            
    lsr     r18                 
    lsr     r18                 
    andi    r18, 0x1F           
    mov     r19, r23            
    lsr     r19                 
    lsr     r19                 
    andi    r19, 0x1F           
    mov     r20, r24            
    mov     r21, r25            
    andi    r21, 0x03           
    ori     r21, 0x04           
    mov     r26, r22            
    mov     r27, r23            
    andi    r27, 0x03           
    ori     r27, 0x04           
    cp      r18, r19            
    brsh    __add_no_swap       
    mov     r28, r18            
    mov     r18, r19            
    mov     r19, r28            
    mov     r28, r20            
    mov     r20, r26            
    mov     r26, r28            
    mov     r28, r21            
    mov     r21, r27            
    mov     r27, r28            
    mov     r28, r16            
    mov     r16, r17            
    mov     r17, r28            
__add_no_swap:
    mov     r28, r18            
    sub     r28, r19            
    cpi     r28, 11             
    brlo    __add_do_align      
    rjmp    __add_pack_a        
__add_do_align:
    tst     r28                 
    breq    __add_aligned       
__add_align_loop:
    lsr     r27                 
    ror     r26                 
    dec     r28                 
    brne    __add_align_loop    
__add_aligned:
    cp      r16, r17            
    brne    __add_diff_sign     
    add     r20, r26            
    adc     r21, r27            
    sbrs    r21, 3              
    rjmp    __add_norm          
    lsr     r21                 
    ror     r20                 
    inc     r18                 
    rjmp    __add_norm          
__add_diff_sign:
    sub     r20, r26            
    sbc     r21, r27            
    mov     r28, r20            
    or      r28, r21            
    brne    __add_norm          
    clr     r24                 
    clr     r25                 
    rjmp    __add_done          
__add_norm:
    sbrc    r21, 2              
    rjmp    __add_norm_done     
    tst     r18                 
    breq    __add_ret_zero      
    lsl     r20                 
    rol     r21                 
    dec     r18                 
    rjmp    __add_norm          
__add_norm_done:
    cpi     r18, 31             
    brlo    __add_exp_ok        
    ldi     r24, 0x00           
    ldi     r25, 0x7C           
    or      r25, r16            
    rjmp    __add_done          
__add_exp_ok:
    andi    r21, 0x03           
    mov     r24, r20            
    mov     r25, r21            
    lsl     r18                 
    lsl     r18                 
    or      r25, r18            
    or      r25, r16            
    rjmp    __add_done          
__add_pack_a:
    andi    r21, 0x03           
    mov     r24, r20            
    mov     r25, r21            
    lsl     r18                 
    lsl     r18                 
    or      r25, r18            
    or      r25, r16            
    rjmp    __add_done          
__add_ret_zero:
    clr     r24                 
    clr     r25                 
__add_done:
    pop     r28                 
    pop     r27                 
    pop     r26                 
    pop     r21                 
    pop     r20                 
    pop     r19                 
    pop     r18                 
    pop     r17                 
    pop     r16                 
    ret                         
__mulhf3:
    push    r16                 
    push    r17                 
    push    r18                 
    push    r19                 
    push    r20                 
    push    r21                 
    push    r26                 
    push    r27                 
    mov     r16, r24            
    andi    r16, 0x7F           
    or      r16, r25            
    brne    __mul_a_not_zero    
    rjmp    __mul_ret_zero      
__mul_a_not_zero:
    mov     r16, r22            
    andi    r16, 0x7F           
    or      r16, r23            
    brne    __mul_b_not_zero    
    rjmp    __mul_ret_zero      
__mul_b_not_zero:
    mov     r16, r25            
    eor     r16, r23            
    andi    r16, 0x80           
    mov     r17, r25            
    lsr     r17                 
    lsr     r17                 
    andi    r17, 0x1F           
    mov     r21, r23            
    lsr     r21                 
    lsr     r21                 
    andi    r21, 0x1F           
    add     r17, r21            
    subi    r17, 15             
    mov     r18, r24            
    mov     r19, r25            
    andi    r19, 0x03           
    ori     r19, 0x04           
    mov     r26, r22            
    mov     r27, r23            
    andi    r27, 0x03           
    ori     r27, 0x04           
    clr     r20                 
    clr     r21                 
    mul     r18, r26            
    movw    r20, r0             
    mul     r18, r27            
    add     r21, r0             
    clr     r18                 
    adc     r18, r1             
    mul     r19, r26            
    add     r21, r0             
    adc     r18, r1             
    mul     r19, r27            
    add     r18, r0             
    clr     r19                 
    adc     r19, r1             
    clr     r1                  
    tst     r19                 
    brne    __mul_shift_down    
    sbrc    r18, 5              
    rjmp    __mul_norm_5        
    sbrc    r18, 4              
    rjmp    __mul_norm_4        
    sbrc    r18, 3              
    rjmp    __mul_norm_3        
    sbrc    r18, 2              
    rjmp    __mul_norm_2        
    rjmp    __mul_ret_zero      
__mul_shift_down:
    lsr     r19                 
    ror     r18                 
    ror     r21                 
    inc     r17                 
    tst     r19                 
    brne    __mul_shift_down    
    sbrc    r18, 5              
    rjmp    __mul_norm_5        
__mul_norm_5:
    inc     r17                 
    lsr     r18                 
    ror     r21                 
__mul_norm_4:
    rjmp    __mul_pack          
__mul_norm_3:
    dec     r17                 
    lsl     r21                 
    rol     r18                 
    rjmp    __mul_pack          
__mul_norm_2:
    dec     r17                 
    lsl     r21                 
    rol     r18                 
    dec     r17                 
    lsl     r21                 
    rol     r18                 
__mul_pack:
    cpi     r17, 31             
    brlo    __mul_no_ovf        
    rjmp    __mul_ret_inf       
__mul_no_ovf:
    cpi     r17, 1              
    brsh    __mul_no_udf        
    rjmp    __mul_ret_zero      
__mul_no_udf:
    mov     r19, r18            
    andi    r19, 0x03           
    mov     r24, r21            
    mov     r25, r19            
    lsl     r17                 
    lsl     r17                 
    or      r25, r17            
    or      r25, r16            
    rjmp    __mul_done          
__mul_ret_zero:
    clr     r24                 
    clr     r25                 
    rjmp    __mul_done          
__mul_ret_inf:
    ldi     r24, 0x00           
    ldi     r25, 0x7C           
    or      r25, r16            
__mul_done:
    pop     r27                 
    pop     r26                 
    pop     r21                 
    pop     r20                 
    pop     r19                 
    pop     r18                 
    pop     r17                 
    pop     r16                 
    ret                         
__divhf3:
    push    r16                 
    push    r17                 
    push    r18                 
    push    r19                 
    push    r20                 
    push    r21                 
    push    r26                 
    push    r27                 
    push    r28                 
    mov     r16, r24            
    andi    r16, 0x7F           
    or      r16, r25            
    brne    __div_a_ok          
    rjmp    __div_ret_zero      
__div_a_ok:
    mov     r16, r22            
    andi    r16, 0x7F           
    or      r16, r23            
    brne    __div_b_ok          
    rjmp    __div_ret_inf       
__div_b_ok:
    mov     r16, r25            
    eor     r16, r23            
    andi    r16, 0x80           
    mov     r17, r25            
    lsr     r17                 
    lsr     r17                 
    andi    r17, 0x1F           
    mov     r21, r23            
    lsr     r21                 
    lsr     r21                 
    andi    r21, 0x1F           
    sub     r17, r21            
    subi    r17, -15            
    mov     r18, r24            
    mov     r19, r25            
    andi    r19, 0x03           
    ori     r19, 0x04           
    mov     r26, r22            
    mov     r27, r23            
    andi    r27, 0x03           
    ori     r27, 0x04           
    clr     r20                 
    clr     r21                 
    ldi     r28, 11             
__div_loop:
    cp      r18, r26            
    cpc     r19, r27            
    brlo    __div_lt            
    sub     r18, r26            
    sbc     r19, r27            
    sec                         
    rjmp    __div_shift         
__div_lt:
    clc                         
__div_shift:
    rol     r20                 
    rol     r21                 
    lsl     r18                 
    rol     r19                 
    dec     r28                 
    brne    __div_loop          
    mov     r18, r20            
    mov     r19, r21            
    mov     r20, r18            
    or      r20, r19            
    brne    __div_norm          
    rjmp    __div_ret_zero      
__div_norm:
    sbrc    r19, 2              
    rjmp    __div_norm_done     
    cpi     r17, 2              
    brlo    __div_ret_zero      
    lsl     r18                 
    rol     r19                 
    dec     r17                 
    rjmp    __div_norm          
__div_norm_done:
    cpi     r17, 31             
    brlo    __div_no_ovf        
    rjmp    __div_ret_inf       
__div_no_ovf:
    cpi     r17, 1              
    brsh    __div_pack          
    rjmp    __div_ret_zero      
__div_pack:
    andi    r19, 0x03           
    mov     r24, r18            
    mov     r25, r19            
    lsl     r17                 
    lsl     r17                 
    or      r25, r17            
    or      r25, r16            
    rjmp    __div_done          
__div_ret_zero:
    clr     r24                 
    clr     r25                 
    rjmp    __div_done          
__div_ret_inf:
    ldi     r24, 0x00           
    ldi     r25, 0x7C           
    or      r25, r16            
__div_done:
    pop     r28                 
    pop     r27                 
    pop     r26                 
    pop     r21                 
    pop     r20                 
    pop     r19                 
    pop     r18                 
    pop     r17                 
    pop     r16                 
    ret                         
__cmphf2:
    mov     r26, r25            
    eor     r26, r23            
    sbrs    r26, 7              
    rjmp    __cmp_same_sign     
    sbrc    r25, 7              
    rjmp    __cmp_ret_less      
    rjmp    __cmp_ret_greater   
__cmp_same_sign:
    sbrc    r25, 7              
    rjmp    __cmp_both_neg      
    cp      r24, r22            
    cpc     r25, r23            
    breq    __cmp_ret_equal     
    brlo    __cmp_ret_less      
    rjmp    __cmp_ret_greater   
__cmp_both_neg:
    cp      r22, r24            
    cpc     r23, r25            
    breq    __cmp_ret_equal     
    brlo    __cmp_ret_less      
    rjmp    __cmp_ret_greater   
__cmp_ret_less:
    ldi     r24, 0xFF           
    ret                         
__cmp_ret_equal:
    clr     r24                 
    ret                         
__cmp_ret_greater:
    ldi     r24, 1              
    ret                         
__powhf3:
    push    r16                 
    push    r17                 
    push    r18                 
    push    r19                 
    mov     r18, r24            
    mov     r19, r25            
    mov     r16, r22            
    cpi     r16, 0              
    brne    __pow_not_zero      
    rjmp    __pow_ret_one       
__pow_not_zero:
    cpi     r16, 1              
    brne    __pow_not_one       
    rjmp    __pow_ret_base      
__pow_not_one:
    ldi     r24, 0x00           
    ldi     r25, 0x3C           
__pow_loop:
    cpi     r16, 0              
    breq    __pow_done          
    mov     r22, r18            
    mov     r23, r19            
    rcall   __mulhf3            
    dec     r16                 
    rjmp    __pow_loop          
__pow_ret_one:
    ldi     r24, 0x00           
    ldi     r25, 0x3C           
    rjmp    __pow_done          
__pow_ret_base:
    mov     r24, r18            
    mov     r25, r19            
__pow_done:
    pop     r19                 
    pop     r18                 
    pop     r17                 
    pop     r16                 
    ret                         
