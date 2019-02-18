#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "jamo.h"
#include <mysql.h>

my_bool jamodecomp_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  JamoDecompState *state
  if (args->arg_count != 1) {
    strcpy(message, "jamodecomp requires exactly one string argument");
    return 1;
  }
  if (args->arg_type[0] != STRING_RESULT) {
    strcpy(message, "jamodecomp requires exactly one string argument");
    return 1;
  }
  initid->max_length = args->lengths[0] * 4;
  state = jamo_decomp_state_init();
  initid->ptr = (char *) state;
  return 0;
}

char * jamodecomp(
  UDF_INIT *initid, UDF_ARGS *args,
  char *result, unsigned long *length,
  char *is_null, char *error
) {
  JamoDecompState *state = (JamoDecompState *) initid->ptr;
  char *decomp_ret;
  jamo_decomp_state_set_original(state, args->args[0], args->lenghts[0]);
  // allocate enough memory for return
  *length = jamo_decompose_str(result, state, args->lenghts[0] * 4) - 1;
  jamo_decomp_state_flush(state);
  return result;
}

void jamodecomp_deinit(UDF_INIT *initid) {
  jamo_decomp_state_deinit((JamoDecompState *) initid->ptr);
  return;
}