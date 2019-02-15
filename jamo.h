#ifndef JAMO_H
#define JAMO_H

// Hangul unicode constants
#define SYL_BASE 0xAC00
#define LEAD_BASE 0x1100
#define VOW_BASE 0x1161
#define TAIL_BASE 0x11A7

// Hangul jamo constants
#define LEAD_COUNT 19
#define VOW_COUNT 21
#define TAIL_COUNT 28
#define N_COUNT (VOW_COUNT*TAIL_COUNT)
#define SYL_COUNT (LEAD_COUNT*N_COUNT)
#define IS_HANGUL_SYLLABLE(wc) (wc >= SYL_BASE && wc < SYL_BASE + SYL_COUNT)

// Tails to leads
#define CONV_G(c) c-0xA8
#define CONV_GG(c) c-0xA8
#define CONV_N(c) c-0xA9
#define CONV_D(c) c-0xAB
#define CONV_L(c) c-0xAA
#define CONV_M(c) c-0xB1
#define CONV_B(c) c-0xB1
#define CONV_S(c) c-0xB1
#define CONV_SS(c) c-0xB1
#define CONV_NG(c) c-0xB1
#define CONV_J(c) c-0xB1
#define CONV_C(c) c-0xB0
#define CONV_K(c) c-0xB0
#define CONV_T(c) c-0xB0
#define CONV_P(c) c-0xB0
#define CONV_H(c) c-0xB0
#define CONV_TAILS(c) (c == 0x11A8) ? CONV_G(c) : ( \
  (c == 0x11A9) ? CONV_GG(c) : ( \
  (c == 0x11AB) ? CONV_N(c) : ( \
  (c == 0x11AE) ? CONV_D(c) : ( \
  (c == 0x11AF) ? CONV_L(c) : ( \
  (c == 0x11B7) ? CONV_M(c) : ( \
  (c == 0x11B8) ? CONV_B(c) : ( \
  (c == 0x11BA) ? CONV_S(c) : ( \
  (c == 0x11BB) ? CONV_SS(c) : ( \
  (c == 0x11BC) ? CONV_NG(c) : ( \
  (c == 0x11BD) ? CONV_J(c) : ( \
  (c == 0x11BE) ? CONV_C(c) : ( \
  (c == 0x11BF) ? CONV_K(c) : ( \
  (c == 0x11C0) ? CONV_T(c) : ( \
  (c == 0x11C1) ? CONV_P(c) : ( \
  (c == 0x11C2) ? CONV_H(c) : c)))))))))))))))

// Composite tails and their composing leads
static const int TAIL_GS[] = {0x1100, 0x1109};
static const int TAIL_NJ[] = {0x1102, 0x110C};
static const int TAIL_NH[] = {0x1102, 0x1112};
static const int TAIL_LG[] = {0x1105, 0x1100};
static const int TAIL_LM[] = {0x1105, 0x1106};
static const int TAIL_LB[] = {0x1105, 0x1107};
static const int TAIL_LS[] = {0x1105, 0x1109};
static const int TAIL_LT[] = {0x1105, 0x1110};
static const int TAIL_LP[] = {0x1105, 0x1111};
static const int TAIL_LH[] = {0x1105, 0x1112};
static const int TAIL_BS[] = {0x1107, 0x1109};
// Composite tails to two leads
#define CONV_COMP_TAILS(c) (c == 0x11AA) ? TAIL_GS : ( \
  (c == 0x11AC) ? TAIL_NJ : ( \
  (c == 0x11AD) ? TAIL_NH : ( \
  (c == 0x11B0) ? TAIL_LG : ( \
  (c == 0x11B1) ? TAIL_LM : ( \
  (c == 0x11B2) ? TAIL_LB : ( \
  (c == 0x11B3) ? TAIL_LS : ( \
  (c == 0x11B4) ? TAIL_LT : ( \
  (c == 0x11B5) ? TAIL_LP : ( \
  (c == 0x11B6) ? TAIL_LH : TAIL_BS)))))))))

// jamo decomposition state
typedef struct JamoDecompState JamoDecompState;

JamoDecompState *jamo_decomp_state_init();
void jamo_decomp_state_set_original(JamoDecompState *state, char *original, unsigned int len);
void jamo_decomp_state_flush(JamoDecompState *state);
void jamo_decomp_state_deinit(JamoDecompState *state);

// decompose given string containing hangul syllables into jamos
unsigned int jamo_decompose_str(char *decomposed, JamoDecompState *state, unsigned int max);

#endif