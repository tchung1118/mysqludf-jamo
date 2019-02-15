#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "jamo.h"

int main() {
  char *mbstr = u8"정태원";
  char *ret_mbs;
  int wcs[9];
  JamoDecompState *state = jamo_decomp_state_init();
  unsigned int len = strlen(mbstr);
  printf("%d\n", len);
  jamo_decomp_state_set_original(state, mbstr, len);
  ret_mbs = malloc(len * 4);
  unsigned int jamo_len = jamo_decompose_str(ret_mbs, state, len * 4);
  printf("%d\n", jamo_len);
  printf("%s\n", ret_mbs);
  mbstowcs(wcs, ret_mbs, 9);
  for (int i = 0; i < 9; i++) {
    printf("%x ", wcs[i]);
  }
  jamo_decomp_state_flush(state);
  free(ret_mbs);
  jamo_decomp_state_deinit(state);
  return 0;
}