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

JamoDecompState *jamo_decomp_state_init() {
  JamoDecompState *state = (JamoDecompState *) malloc(sizeof (JamoDecompState));
  if (state == NULL) return NULL;
  // init locale
  state->locale = setlocale(LC_ALL, "");
  return state;
}

void jamo_decomp_state_set_original(JamoDecompState *state, char *original, unsigned int len) {
  // cannot assume original is null-terminated
  state->original = malloc(len+1);
  memcpy(state->original, original, len);
  state->original[len] = 0;
  state->orig_len = len;
  // utf-8 -> unicode code points
  state->wcs_len = mbstowcs(NULL, state->original, 0) + 1;
  state->wcs = malloc(state->wcs_len);
  mbstowcs(state->wcs, state->original, state->wcs_len);
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
  } else {
    decomposed_syls[(*current_index)++] = wc;
  }
}

// returns null-terminated, jamo-decomposed string of state->original
unsigned int jamo_decompose_str(char *decomposed, JamoDecompState *state, unsigned int max) {
  int i, j;
  int jamo_cnt = 0;
  int max_length = state->wcs_len * 4;
  int *decomposed_syls = (int *)malloc(max_length * sizeof (int *));
  for (i = 0; i < state->wcs_len; i++) {
    append_decomposed_syllable(decomposed_syls, state->wcs[i], &jamo_cnt);
  }
  decomposed_syls[jamo_cnt] = 0;
  unsigned int mb_len = wcstombs(NULL, decomposed_syls, 0)+1;
  wcstombs(decomposed, decomposed_syls, max);
  free(decomposed_syls);
  return mb_len;
}