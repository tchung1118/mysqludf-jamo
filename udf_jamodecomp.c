#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "jamo.h"
#include <mysql.h>

my_bool jamodecomp_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  if (args->arg_count != 1) {
    strcpy(message, "jamodecomp requires exactly one string argument");
    return 1;
  }
  if (args->arg_type[0] != STRING_RESULT) {
    strcpy(message, "jamodecomp requires exactly one string argument");
    return 1;
  }
  initid->max_length = args->lengths[0] * 4;
  // TODO: jamo_decomp_state_init
  return 0;
}

char * jamodecomp(
  UDF_INIT *initid, UDF_ARGS *args,
  char *result, unsigned long *length,
  char *is_null, char *error
) {
  // TODO: jamo_decomp_state_set_original
  // TODO: jamo_decomp_state_flush
  return result;
}

void jamodecomp_deinit(UDF_INIT *initid) {
  // TODO: jamo_decomp_state_deinit
  return;
}