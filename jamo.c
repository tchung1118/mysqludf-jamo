#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include "jamo.h"

struct JamoDecompState {
  char *original;
  char *locale;
  unsigned int orig_len;
  unsigned int wcs_len;
  int *wcs;
};

struct DecomposedSyllable {
  int length;
  int *jamos;
};

unsigned int utf8_to_unicode(int *dest, char *u8, unsigned int len) {
  int i;
  unsigned int cnt = 0;
  if (dest == NULL) {
    // count total number of wide characters
    for (i = 0; i < len; i++) {
      if (IS_WITHIN_ASCII(u8[i])) {
        cnt++;
      } else if (IS_UTF_BEGINNING(u8[i])) {
        cnt++;
      }
    }
    return cnt;
  }
  // convert utf8 encoded string to array of unicode code points
  for (i = 0; i < len; i++) {
    if (IS_WITHIN_ASCII(u8[i])) {
      dest[cnt++] = (int)u8[i];
    } else if (IS_UTF_BEGINNING(u8[i])) {
      int num_bytes = MB_UTF_CHARS(u8[i]);
      switch (num_bytes) {
        case 2:
          dest[cnt++] = ((int)(u8[i]&0x1F) << 6) + (u8[i+1]&0x3F);
          break;
        case 3:
          dest[cnt++] = ((int)(u8[i]&0x0F) << 12) + ((int)(u8[i+1]&0x3F) << 6) + (u8[i+2]&0x3F);
          break;
        case 4:
          dest[cnt++] = ((int)(u8[i]&0x07) << 18) + ((int)(u8[i+1]&0x3F) << 12) + ((int)(u8[i+2]&0x3F) << 6) + (u8[i+3]&0x3F);
          break;
      }
    }
  }
  return cnt;
}

unsigned int unicode_to_utf8(char *dest, int *unicode, unsigned int len) {
  int i;
  unsigned int cnt = 0;
  if (dest == NULL) {
    // count total number of utf-8 encoded chars
    for (i = 0; i < len; i++) {
      cnt += UNICODE_U8_NUM_CHARS(unicode[i]);
    }
    return cnt;
  }
  // convert unicode code points to utf-8
  for (i = 0; i < len; i++) {
    if (IS_WITHIN_ASCII(unicode[i])) {
      dest[cnt++] = unicode[i];
    } else {
      int cp = unicode[i];
      int num_bytes = UNICODE_U8_NUM_CHARS(cp);
      // skip invalid unicode code points for utf-8
      if (cp > 0x1FFFFF) continue;
      switch (num_bytes) {
        case 2:
          dest[cnt++] = 0xC0|((cp >> 6)&0x1F);
          dest[cnt++] = 0x80|(cp&0x3F);
          break;
        case 3:
          dest[cnt++] = 0xE0|((cp >> 12)&0xF);
          dest[cnt++] = 0x80|((cp >> 6)&0x3F);
          dest[cnt++] = 0x80|(cp&0x3F);
          break;
        case 4:
          dest[cnt++] = 0xF0|((cp >> 18)&0x7);
          dest[cnt++] = 0x80|((cp >> 12)&0x3F);
          dest[cnt++] = 0x80|((cp >> 6)&0x3F);
          dest[cnt++] = 0x80|(cp&0x3F);
          break;
      }
    }
  }
  return cnt;
}

JamoDecompState *jamo_decomp_state_init() {
  JamoDecompState *state = (JamoDecompState *) malloc(sizeof (JamoDecompState));
  if (state == NULL) return NULL;
  // init locale
  state->locale = setlocale(LC_ALL, "en_US");
  return state;
}

void jamo_decomp_state_set_original(JamoDecompState *state, char *original, unsigned int len) {
  // cannot assume original is null-terminated
  state->original = malloc(len+1);
  memcpy(state->original, original, len);
  state->original[len] = 0;
  state->orig_len = len;
  // utf-8 -> unicode code points
  state->wcs_len = utf8_to_unicode(NULL, state->original, state->orig_len); //mbstowcs(NULL, state->original, 0) + 1;
  state->wcs = malloc(state->wcs_len * sizeof (int));
  state->wcs[state->wcs_len-1] = 0;
  utf8_to_unicode(state->wcs, state->original, state->orig_len); //mbstowcs(state->wcs, state->original, state->wcs_len);
}

void jamo_decomp_state_flush(JamoDecompState *state) {
  assert(state->original != NULL);
  free(state->original);
  assert(state->wcs != NULL);
  free(state->wcs);
}

void jamo_decomp_state_deinit(JamoDecompState *state) {
  assert(state != NULL);
  free(state);
}

void append_decomposed_syllable(int *decomposed_syls, int wc, int *current_index) {
  if (IS_HANGUL_SYLLABLE(wc)) {
    int s_index = wc - SYL_BASE;
    int l_index = s_index / N_COUNT;
    int v_index = (s_index % N_COUNT) / TAIL_COUNT;
    int t_index = s_index % TAIL_COUNT;
    int lead = LEAD_BASE + l_index;
    int vowel = VOW_BASE + v_index;
    if (t_index > 0) {
      // tail exists
      int tail = CONV_TAILS(TAIL_BASE + t_index);
      if (tail < TAIL_BASE) {
        // simple tail
        decomposed_syls[(*current_index)++] = lead;
        decomposed_syls[(*current_index)++] = vowel;
        decomposed_syls[(*current_index)++] = tail;
      } else {
        // composite tail
        const int *composite_tail = CONV_COMP_TAILS(tail);
        decomposed_syls[(*current_index)++] = lead;
        decomposed_syls[(*current_index)++] = vowel;
        decomposed_syls[(*current_index)++] = composite_tail[0];
        decomposed_syls[(*current_index)++] = composite_tail[1];
      }
    } else {
      // no tail
      decomposed_syls[(*current_index)++] = lead;
      decomposed_syls[(*current_index)++] = vowel;
    }
  } else if (IS_HANGUL_COMPAT_JAMO(wc)) {
    decomposed_syls[(*current_index)++] = CONV_COMPAT_JAMO(wc);
  } else {
    decomposed_syls[(*current_index)++] = wc;
  }
}

// returns null-terminated, jamo-decomposed string of state->original
unsigned int jamo_decompose_str(char *decomposed, JamoDecompState *state, unsigned int max) {
  int i;
  int jamo_cnt = 0;
  int max_length = state->wcs_len * 4;
  int *decomposed_syls = (int *)malloc(max_length * sizeof (int));
  for (i = 0; i < state->wcs_len; i++) {
    append_decomposed_syllable(decomposed_syls, state->wcs[i], &jamo_cnt);
  }
  decomposed_syls[jamo_cnt] = 0;
  unsigned int mb_len = unicode_to_utf8(decomposed, decomposed_syls, jamo_cnt+1); //wcstombs(decomposed, decomposed_syls, max)+1;
  free(decomposed_syls);
  return mb_len;
}