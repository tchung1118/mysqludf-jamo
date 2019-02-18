#ifndef JAMO_H
#define JAMO_H

#define IS_WITHIN_ASCII(c) (c >= 0 && c <= 0x7F)
#define IS_UTF_BEGINNING(c) ((c&0xC0)!=0x80)
#define MB_UTF_CHARS(c) ((c&0xff) >= 0xC0 && (c&0xff) <= 0xDF) ? 2 : ( \
  ((c&0xff) >= 0xE0 && (c&0xff) <= 0xEF) ? 3 : 4)

#define UNICODE_U8_NUM_CHARS(cp) (cp >= 0 && cp <= 0x7F) ? 1 : (\
  (cp >= 0x80 && cp <= 0x7FF) ? 2 : (\
  (cp >= 0x800 && cp <= 0xFFFF) ? 3 : 4))

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
#define IS_HANGUL_COMPAT_JAMO(wc) (wc >= 0x3131 && wc <= 0x3163)

// Tails to leads
#define CONV_G_GG(c) c-0xA8
#define CONV_N(c) c-0xA9
#define CONV_D(c) c-0xAB
#define CONV_L(c) c-0xAA
#define CONV_M_J(c) c-0xB1
#define CONV_C_H(c) c-0xB0
#define CONV_TAILS(c) (c == 0x11A8 || c == 0x11A9) ? CONV_G_GG(c) : ( \
  (c == 0x11AB) ? CONV_N(c) : ( \
  (c == 0x11AE) ? CONV_D(c) : ( \
  (c == 0x11AF) ? CONV_L(c) : ( \
  (c == 0x11B7 || c == 0x11B8 || (c >= 0x11BA && c <= 0x11BD)) ? CONV_M_J(c) : ( \
  (c >= 0x11BE && c <= 0x11C2) ? CONV_C_H(c) : c)))))

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

// Convert compatibility jamo
#define CONV_COMPAT_G_GG(c) c-0x2031
#define CONV_COMPAT_N(c) c-0x2032
#define CONV_COMPAT_D_L(c) c-0x2034
#define CONV_COMPAT_M_BB(c) c-0x203B
#define CONV_COMPAT_S_H(c) c-0x203C
#define CONV_COMPAT_VOW(c) c-0x1FEE
#define CONV_COMPAT_JAMO(c) (c == 0x3131 || c == 0x3132) ? CONV_COMPAT_G_GG(c) : (\
  (c == 0x3134) ? CONV_COMPAT_N(c) : (\
  (c >= 0x3137 && c <= 0x3139) ? CONV_COMPAT_D_L(c) : (\
  (c >= 0x3141 && c <= 0x3143) ? CONV_COMPAT_M_BB(c) : (\
  (c >= 0x3145 && c <= 0x314E) ? CONV_COMPAT_S_H(c) : (\
  (c >= 0x314F && c <= 0x3163) ? CONV_COMPAT_VOW(c) : c)))))

// jamo decomposition state
typedef struct JamoDecompState JamoDecompState;

JamoDecompState *jamo_decomp_state_init();
void jamo_decomp_state_set_original(JamoDecompState *state, char *original, unsigned int len);
void jamo_decomp_state_flush(JamoDecompState *state);
void jamo_decomp_state_deinit(JamoDecompState *state);

// decompose given string containing hangul syllables into jamos
unsigned int jamo_decompose_str(char *decomposed, JamoDecompState *state, unsigned int max);

#endif