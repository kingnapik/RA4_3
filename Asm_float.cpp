// Integrantes do grupo:
// Guilherme Knapik - kingnapik
// Nome do grupo no Canvas: RA4_3

#include "gerador_assembly.h"

// =============================================================================
// SECAO 9: ROTINAS DE PONTO FLUTUANTE IEEE 754 HALF-PRECISION
// =============================================================================

void GeradorAssembly::emitirRotinasFloat() {
    assembly.push_back("");
    assembly.push_back("; ==========================================================================");
    assembly.push_back("; ROTINAS IEEE 754 HALF-PRECISION (16-bit Float)");
    assembly.push_back("; Convencao: Operando A em r25:r24, Operando B em r23:r22");
    assembly.push_back(";            Resultado em r25:r24");
    assembly.push_back("; ==========================================================================");

    emitirSubtracao();
    emitirAdicao();
    emitirMultiplicacao();
    emitirDivisao();
    emitirComparacaoFloat();
    emitirPotencia();
}

// =============================================================================
// SUBTRACAO: A - B = A + (-B)
// =============================================================================

void GeradorAssembly::emitirSubtracao() {
    assembly.push_back("");
    assembly.push_back("; --- SUBTRACAO: A - B = A + (-B) ---");
    emitLabel("__subhf3");
    emit("ldi", "r20, 0x80");
    emit("eor", "r23, r20", "Inverte sinal de B");
    emit("rjmp", "__addhf3");
}

// =============================================================================
// ADICAO IEEE 754 HALF-PRECISION
// =============================================================================

void GeradorAssembly::emitirAdicao() {
    assembly.push_back("");
    assembly.push_back("; --- ADICAO IEEE 754 HALF-PRECISION ---");
    emitLabel("__addhf3");
    emit("push", "r16");
    emit("push", "r17");
    emit("push", "r18");
    emit("push", "r19");
    emit("push", "r20");
    emit("push", "r21");
    emit("push", "r26");
    emit("push", "r27");
    emit("push", "r28");

    // Check A == 0
    emit("mov", "r16, r24");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r25");
    emit("brne", "__add_a_nz");
    emit("mov", "r24, r22");
    emit("mov", "r25, r23");
    emit("rjmp", "__add_done");
    emitLabel("__add_a_nz");

    // Check B == 0
    emit("mov", "r16, r22");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r23");
    emit("brne", "__add_b_nz");
    emit("rjmp", "__add_done");
    emitLabel("__add_b_nz");

    // Extract signs
    emit("mov", "r16, r25");
    emit("andi", "r16, 0x80");
    emit("mov", "r17, r23");
    emit("andi", "r17, 0x80");

    // Extract exponents
    emit("mov", "r18, r25");
    emit("lsr", "r18");
    emit("lsr", "r18");
    emit("andi", "r18, 0x1F");
    emit("mov", "r19, r23");
    emit("lsr", "r19");
    emit("lsr", "r19");
    emit("andi", "r19, 0x1F");

    // Extract mantissas with implicit 1
    emit("mov", "r20, r24");
    emit("mov", "r21, r25");
    emit("andi", "r21, 0x03");
    emit("ori", "r21, 0x04");
    emit("mov", "r26, r22");
    emit("mov", "r27, r23");
    emit("andi", "r27, 0x03");
    emit("ori", "r27, 0x04");

    // Swap if needed (expA >= expB)
    emit("cp", "r18, r19");
    emit("brsh", "__add_no_swap");
    emit("mov", "r28, r18");
    emit("mov", "r18, r19");
    emit("mov", "r19, r28");
    emit("mov", "r28, r20");
    emit("mov", "r20, r26");
    emit("mov", "r26, r28");
    emit("mov", "r28, r21");
    emit("mov", "r21, r27");
    emit("mov", "r27, r28");
    emit("mov", "r28, r16");
    emit("mov", "r16, r17");
    emit("mov", "r17, r28");

    emitLabel("__add_no_swap");
    emit("mov", "r28, r18");
    emit("sub", "r28, r19");
    emit("cpi", "r28, 11");
    emit("brlo", "__add_do_align");
    emit("rjmp", "__add_pack_a");

    emitLabel("__add_do_align");
    emit("tst", "r28");
    emit("breq", "__add_aligned");

    emitLabel("__add_align_loop");
    emit("lsr", "r27");
    emit("ror", "r26");
    emit("dec", "r28");
    emit("brne", "__add_align_loop");

    emitLabel("__add_aligned");
    emit("cp", "r16, r17");
    emit("brne", "__add_diff_sign");

    // Same sign: add
    emit("add", "r20, r26");
    emit("adc", "r21, r27");
    emit("sbrs", "r21, 3");
    emit("rjmp", "__add_norm");
    emit("lsr", "r21");
    emit("ror", "r20");
    emit("inc", "r18");
    emit("rjmp", "__add_norm");

    emitLabel("__add_diff_sign");
    emit("sub", "r20, r26");
    emit("sbc", "r21, r27");
    emit("mov", "r28, r20");
    emit("or", "r28, r21");
    emit("brne", "__add_norm");
    emit("clr", "r24");
    emit("clr", "r25");
    emit("rjmp", "__add_done");

    emitLabel("__add_norm");
    emit("sbrc", "r21, 2");
    emit("rjmp", "__add_norm_done");
    emit("tst", "r18");
    emit("breq", "__add_ret_zero");
    emit("lsl", "r20");
    emit("rol", "r21");
    emit("dec", "r18");
    emit("rjmp", "__add_norm");

    emitLabel("__add_norm_done");
    emit("cpi", "r18, 31");
    emit("brlo", "__add_exp_ok");
    emit("ldi", "r24, 0x00");
    emit("ldi", "r25, 0x7C");
    emit("or", "r25, r16");
    emit("rjmp", "__add_done");

    emitLabel("__add_exp_ok");
    emit("andi", "r21, 0x03");
    emit("mov", "r24, r20");
    emit("mov", "r25, r21");
    emit("lsl", "r18");
    emit("lsl", "r18");
    emit("or", "r25, r18");
    emit("or", "r25, r16");
    emit("rjmp", "__add_done");

    emitLabel("__add_pack_a");
    emit("andi", "r21, 0x03");
    emit("mov", "r24, r20");
    emit("mov", "r25, r21");
    emit("lsl", "r18");
    emit("lsl", "r18");
    emit("or", "r25, r18");
    emit("or", "r25, r16");
    emit("rjmp", "__add_done");

    emitLabel("__add_ret_zero");
    emit("clr", "r24");
    emit("clr", "r25");

    emitLabel("__add_done");
    emit("pop", "r28");
    emit("pop", "r27");
    emit("pop", "r26");
    emit("pop", "r21");
    emit("pop", "r20");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("pop", "r17");
    emit("pop", "r16");
    emit("ret", "");
}

// =============================================================================
// MULTIPLICACAO IEEE 754 HALF-PRECISION
// =============================================================================

void GeradorAssembly::emitirMultiplicacao() {
    assembly.push_back("");
    assembly.push_back("; --- MULTIPLICACAO IEEE 754 HALF-PRECISION ---");
    emitLabel("__mulhf3");
    emit("push", "r16");
    emit("push", "r17");
    emit("push", "r18");
    emit("push", "r19");
    emit("push", "r20");
    emit("push", "r21");
    emit("push", "r26");
    emit("push", "r27");

    emit("mov", "r16, r24");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r25");
    emit("brne", "__mul_a_not_zero");
    emit("rjmp", "__mul_ret_zero");
    emitLabel("__mul_a_not_zero");

    emit("mov", "r16, r22");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r23");
    emit("brne", "__mul_b_not_zero");
    emit("rjmp", "__mul_ret_zero");
    emitLabel("__mul_b_not_zero");

    emit("mov", "r16, r25");
    emit("eor", "r16, r23");
    emit("andi", "r16, 0x80");

    emit("mov", "r17, r25");
    emit("lsr", "r17");
    emit("lsr", "r17");
    emit("andi", "r17, 0x1F");

    emit("mov", "r21, r23");
    emit("lsr", "r21");
    emit("lsr", "r21");
    emit("andi", "r21, 0x1F");

    emit("add", "r17, r21");
    emit("subi", "r17, 15");

    emit("mov", "r18, r24");
    emit("mov", "r19, r25");
    emit("andi", "r19, 0x03");
    emit("ori", "r19, 0x04");

    emit("mov", "r26, r22");
    emit("mov", "r27, r23");
    emit("andi", "r27, 0x03");
    emit("ori", "r27, 0x04");

    emit("clr", "r20");
    emit("clr", "r21");

    emit("mul", "r18, r26");
    emit("movw", "r20, r0");

    emit("mul", "r18, r27");
    emit("add", "r21, r0");
    emit("clr", "r18");
    emit("adc", "r18, r1");

    emit("mul", "r19, r26");
    emit("add", "r21, r0");
    emit("adc", "r18, r1");

    emit("mul", "r19, r27");
    emit("add", "r18, r0");
    emit("clr", "r19");
    emit("adc", "r19, r1");
    emit("clr", "r1");

    emit("tst", "r19");
    emit("brne", "__mul_shift_down");

    emit("sbrc", "r18, 5");
    emit("rjmp", "__mul_norm_5");
    emit("sbrc", "r18, 4");
    emit("rjmp", "__mul_norm_4");
    emit("sbrc", "r18, 3");
    emit("rjmp", "__mul_norm_3");
    emit("sbrc", "r18, 2");
    emit("rjmp", "__mul_norm_2");
    emit("rjmp", "__mul_ret_zero");

    emitLabel("__mul_shift_down");
    emit("lsr", "r19");
    emit("ror", "r18");
    emit("ror", "r21");
    emit("inc", "r17");
    emit("tst", "r19");
    emit("brne", "__mul_shift_down");
    emit("sbrc", "r18, 5");
    emit("rjmp", "__mul_norm_5");

    emitLabel("__mul_norm_5");
    emit("inc", "r17");
    emit("lsr", "r18");
    emit("ror", "r21");

    emitLabel("__mul_norm_4");
    emit("rjmp", "__mul_pack");

    emitLabel("__mul_norm_3");
    emit("dec", "r17");
    emit("lsl", "r21");
    emit("rol", "r18");
    emit("rjmp", "__mul_pack");

    emitLabel("__mul_norm_2");
    emit("dec", "r17");
    emit("lsl", "r21");
    emit("rol", "r18");
    emit("dec", "r17");
    emit("lsl", "r21");
    emit("rol", "r18");

    emitLabel("__mul_pack");
    emit("cpi", "r17, 31");
    emit("brlo", "__mul_no_ovf");
    emit("rjmp", "__mul_ret_inf");
    emitLabel("__mul_no_ovf");

    emit("cpi", "r17, 1");
    emit("brsh", "__mul_no_udf");
    emit("rjmp", "__mul_ret_zero");
    emitLabel("__mul_no_udf");

    emit("mov", "r19, r18");
    emit("andi", "r19, 0x03");
    emit("mov", "r24, r21");
    emit("mov", "r25, r19");
    emit("lsl", "r17");
    emit("lsl", "r17");
    emit("or", "r25, r17");
    emit("or", "r25, r16");
    emit("rjmp", "__mul_done");

    emitLabel("__mul_ret_zero");
    emit("clr", "r24");
    emit("clr", "r25");
    emit("rjmp", "__mul_done");

    emitLabel("__mul_ret_inf");
    emit("ldi", "r24, 0x00");
    emit("ldi", "r25, 0x7C");
    emit("or", "r25, r16");

    emitLabel("__mul_done");
    emit("pop", "r27");
    emit("pop", "r26");
    emit("pop", "r21");
    emit("pop", "r20");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("pop", "r17");
    emit("pop", "r16");
    emit("ret", "");
}

// =============================================================================
// DIVISAO IEEE 754 HALF-PRECISION
// =============================================================================

void GeradorAssembly::emitirDivisao() {
    assembly.push_back("");
    assembly.push_back("; --- DIVISAO IEEE 754 HALF-PRECISION ---");
    emitLabel("__divhf3");
    emit("push", "r16");
    emit("push", "r17");
    emit("push", "r18");
    emit("push", "r19");
    emit("push", "r20");
    emit("push", "r21");
    emit("push", "r26");
    emit("push", "r27");
    emit("push", "r28");

    emit("mov", "r16, r24");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r25");
    emit("brne", "__div_a_ok");
    emit("rjmp", "__div_ret_zero");
    emitLabel("__div_a_ok");

    emit("mov", "r16, r22");
    emit("andi", "r16, 0x7F");
    emit("or", "r16, r23");
    emit("brne", "__div_b_ok");
    emit("rjmp", "__div_ret_inf");
    emitLabel("__div_b_ok");

    emit("mov", "r16, r25");
    emit("eor", "r16, r23");
    emit("andi", "r16, 0x80");

    emit("mov", "r17, r25");
    emit("lsr", "r17");
    emit("lsr", "r17");
    emit("andi", "r17, 0x1F");

    emit("mov", "r21, r23");
    emit("lsr", "r21");
    emit("lsr", "r21");
    emit("andi", "r21, 0x1F");

    emit("sub", "r17, r21");
    emit("subi", "r17, -15");

    emit("mov", "r18, r24");
    emit("mov", "r19, r25");
    emit("andi", "r19, 0x03");
    emit("ori", "r19, 0x04");

    emit("mov", "r26, r22");
    emit("mov", "r27, r23");
    emit("andi", "r27, 0x03");
    emit("ori", "r27, 0x04");

    emit("clr", "r20");
    emit("clr", "r21");

    emit("ldi", "r28, 11");

    emitLabel("__div_loop");
    emit("cp", "r18, r26");
    emit("cpc", "r19, r27");
    emit("brlo", "__div_lt");

    emit("sub", "r18, r26");
    emit("sbc", "r19, r27");
    emit("sec");
    emit("rjmp", "__div_shift");

    emitLabel("__div_lt");
    emit("clc");

    emitLabel("__div_shift");
    emit("rol", "r20");
    emit("rol", "r21");
    emit("lsl", "r18");
    emit("rol", "r19");
    emit("dec", "r28");
    emit("brne", "__div_loop");

    emit("mov", "r18, r20");
    emit("mov", "r19, r21");

    emit("mov", "r20, r18");
    emit("or", "r20, r19");
    emit("brne", "__div_norm");
    emit("rjmp", "__div_ret_zero");

    emitLabel("__div_norm");
    emit("sbrc", "r19, 2");
    emit("rjmp", "__div_norm_done");
    emit("cpi", "r17, 2");
    emit("brlo", "__div_ret_zero");
    emit("lsl", "r18");
    emit("rol", "r19");
    emit("dec", "r17");
    emit("rjmp", "__div_norm");

    emitLabel("__div_norm_done");
    emit("cpi", "r17, 31");
    emit("brlo", "__div_no_ovf");
    emit("rjmp", "__div_ret_inf");
    emitLabel("__div_no_ovf");
    emit("cpi", "r17, 1");
    emit("brsh", "__div_pack");
    emit("rjmp", "__div_ret_zero");

    emitLabel("__div_pack");
    emit("andi", "r19, 0x03");
    emit("mov", "r24, r18");
    emit("mov", "r25, r19");
    emit("lsl", "r17");
    emit("lsl", "r17");
    emit("or", "r25, r17");
    emit("or", "r25, r16");
    emit("rjmp", "__div_done");

    emitLabel("__div_ret_zero");
    emit("clr", "r24");
    emit("clr", "r25");
    emit("rjmp", "__div_done");

    emitLabel("__div_ret_inf");
    emit("ldi", "r24, 0x00");
    emit("ldi", "r25, 0x7C");
    emit("or", "r25, r16");

    emitLabel("__div_done");
    emit("pop", "r28");
    emit("pop", "r27");
    emit("pop", "r26");
    emit("pop", "r21");
    emit("pop", "r20");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("pop", "r17");
    emit("pop", "r16");
    emit("ret", "");
}

// =============================================================================
// COMPARACAO IEEE 754 HALF-PRECISION
// Retorna: -1 (menor), 0 (igual), 1 (maior) em r24
// =============================================================================

void GeradorAssembly::emitirComparacaoFloat() {
    assembly.push_back("");
    assembly.push_back("; --- COMPARACAO IEEE 754 HALF-PRECISION ---");
    assembly.push_back("; Retorna: -1 (menor), 0 (igual), 1 (maior) em r24");
    emitLabel("__cmphf2");
    emit("mov", "r26, r25");
    emit("eor", "r26, r23");
    emit("sbrs", "r26, 7");
    emit("rjmp", "__cmp_same_sign");
    emit("sbrc", "r25, 7");
    emit("rjmp", "__cmp_ret_less");
    emit("rjmp", "__cmp_ret_greater");

    emitLabel("__cmp_same_sign");
    emit("sbrc", "r25, 7");
    emit("rjmp", "__cmp_both_neg");
    emit("cp", "r24, r22");
    emit("cpc", "r25, r23");
    emit("breq", "__cmp_ret_equal");
    emit("brlo", "__cmp_ret_less");
    emit("rjmp", "__cmp_ret_greater");

    emitLabel("__cmp_both_neg");
    emit("cp", "r22, r24");
    emit("cpc", "r23, r25");
    emit("breq", "__cmp_ret_equal");
    emit("brlo", "__cmp_ret_less");
    emit("rjmp", "__cmp_ret_greater");

    emitLabel("__cmp_ret_less");
    emit("ldi", "r24, 0xFF");
    emit("ret", "");
    emitLabel("__cmp_ret_equal");
    emit("clr", "r24");
    emit("ret", "");
    emitLabel("__cmp_ret_greater");
    emit("ldi", "r24, 1");
    emit("ret", "");
}

// =============================================================================
// POTENCIA (expoente inteiro)
// =============================================================================

void GeradorAssembly::emitirPotencia() {
    assembly.push_back("");
    assembly.push_back("; --- POTENCIA (expoente inteiro) ---");
    emitLabel("__powhf3");
    emit("push", "r16");
    emit("push", "r17");
    emit("push", "r18");
    emit("push", "r19");

    emit("mov", "r18, r24");
    emit("mov", "r19, r25");
    emit("mov", "r16, r22");

    emit("cpi", "r16, 0");
    emit("brne", "__pow_not_zero");
    emit("rjmp", "__pow_ret_one");
    emitLabel("__pow_not_zero");

    emit("cpi", "r16, 1");
    emit("brne", "__pow_not_one");
    emit("rjmp", "__pow_ret_base");
    emitLabel("__pow_not_one");

    emit("ldi", "r24, 0x00", "Inicia com 1.0");
    emit("ldi", "r25, 0x3C");

    emitLabel("__pow_loop");
    emit("cpi", "r16, 0");
    emit("breq", "__pow_done");
    emit("mov", "r22, r18");
    emit("mov", "r23, r19");
    emit("rcall", "__mulhf3");
    emit("dec", "r16");
    emit("rjmp", "__pow_loop");

    emitLabel("__pow_ret_one");
    emit("ldi", "r24, 0x00");
    emit("ldi", "r25, 0x3C");
    emit("rjmp", "__pow_done");

    emitLabel("__pow_ret_base");
    emit("mov", "r24, r18");
    emit("mov", "r25, r19");

    emitLabel("__pow_done");
    emit("pop", "r19");
    emit("pop", "r18");
    emit("pop", "r17");
    emit("pop", "r16");
    emit("ret", "");
}