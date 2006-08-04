
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "libscript.h"
#include "internals.h"
#include "data.h"

#include <stdio.h>
#include <assert.h>

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

void script_reset_params(script_env* env) {
   int size = env->n_params;
   int i;
   for (i = 0; i < size; i++) {
      script_data* data = &(env->params[i]);
      if (data->type == SCRIPT_STRING) {
         free(data->u.string_value);
      }
   }
   env->n_params = 0;
   env->next_get = 0;
}

int script_param_count(script_env* env) {
   return env->n_params;
}

script_type script_get_type(script_env* env, int i) {
   script_data* data;
   assert (i == env->next_get);
   if (i >= env->n_params) return SCRIPT_NONE;
   data = &(env->params[i]);
   return data->type;
}

INLINE static script_data* script_get_data(script_env* env, int i, script_type type) {
   script_data* data;
   assert (i == env->next_get);
   script_check_ret(i >= env->n_params, SCRIPT_ERRPARMISSING, NULL);
   data = &(env->params[i]);
   script_check_ret(data->type != type, SCRIPT_ERRPARTYPE, NULL);
   env->next_get++;
   return data;
}

static script_data* script_put_data(script_env* env, int i, script_type type) {
   script_data* data;

   if (i == 0 && env->n_params > 0)
      script_reset_params(env);
   assert(i == env->n_params);
   script_check_ret(i != env->n_params, SCRIPT_ERRPAREXCESS, NULL);
   script_check_ret(i >= SCRIPT_MAX_PARAMS, SCRIPT_ERRPAREXCESS, NULL);
   data = &(env->params[i]);
   data->type = type;
   env->n_params = i + 1;
   return data;
}

/**
 * @return The string, now owned by the caller.
 */
char* script_get_string(script_env* env, int i) {
   script_data* data = script_get_data(env, i, SCRIPT_STRING);
   char* result;
   if (!data) return NULL;
   result = data->u.string_value;
   data->u.string_value = NULL;
   assert(result);
   return result;
}

double script_get_double(script_env* env, int i) {
   script_data* data = script_get_data(env, i, SCRIPT_DOUBLE);
   if (!data) return 0;
   return data->u.double_value;
}

int script_get_int(script_env* env, int i) {
   script_data* data = script_get_data(env, i, SCRIPT_DOUBLE);
   if (!data) return 0;
   return (int) data->u.double_value;
}

int script_get_bool(script_env* env, int i) {
   script_data* data = script_get_data(env, i, SCRIPT_BOOL);
   if (!data) return 0;
   return (int) data->u.bool_value;
}

/**
 * @param value The string value -- libscript stores its own copy of the string.
 */
void script_put_string(script_env* env, int i, const char* value) {
   script_data* data = script_put_data(env, i, SCRIPT_STRING);
   if (!data) return;
   data->u.string_value = strdup(value);
}

void script_put_double(script_env* env, int i, double value) {
   script_data* data = script_put_data(env, i, SCRIPT_DOUBLE);
   if (!data) return;
   data->u.double_value = value;
}

void script_put_int(script_env* env, int i, int value) {
   script_data* data = script_put_data(env, i, SCRIPT_DOUBLE);
   if (!data) return;
   data->u.double_value = value;
}

void script_put_bool(script_env* env, int i, int value) {
   script_data* data = script_put_data(env, i, SCRIPT_BOOL);
   if (!data) return;
   data->u.bool_value = value;
}
