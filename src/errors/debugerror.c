#include <Inventor/C/errors/debugerror.h>
#include <stdlib.h>

extern void cc_default_handler_cb(const cc_error * err, void * data);

static cc_error_cb * callback = cc_default_handler_cb;
static void * callback_data = NULL;

void
cc_debugerror_construct(cc_debugerror * me)
{
  cc_error_construct((cc_error *)me);
}

void
cc_debugerror_clean(cc_debugerror * me)
{
  cc_error_clean((cc_error *)me);
}


CC_DEBUGERROR_SEVERITY
cc_debugerror_get_severity(const cc_debugerror * me)
{
  return me->severity;
}

void
cc_debugerror_set_handler_callback(cc_error_cb * function, void * data)
{
  callback = function;
  callback_data = data;
}

cc_error_cb *
cc_debugerror_get_handler_callback(void)
{
  return callback;
}

void *
cc_debugerror_get_handler_data(void)
{
  return callback_data;
}

#define CC_DEBUGERROR_POST(SEVERITY, TYPE) \
  cc_string s; \
  cc_debugerror error; \
  va_list args; \
 \
  va_start(args, format); \
  cc_string_construct(&s); \
  cc_string_vsprintf(&s, format, args); \
  va_end(args); \
 \
  error.severity = SEVERITY; \
  cc_error_set_debug_string((cc_error *)&error, "Coin "); \
  cc_error_append_to_debug_string((cc_error *)&error, TYPE); \
  cc_error_append_to_debug_string((cc_error *)&error, " in "); \
  cc_error_append_to_debug_string((cc_error *)&error, source); \
  cc_error_append_to_debug_string((cc_error *)&error, "(): "); \
  cc_error_append_to_debug_string((cc_error *)&error, cc_string_get_text(&s)); \
  cc_error_handle((cc_error *)&error); \
  /* FIXME: port to C. 20020524 mortene. */ \
  /* check_breakpoints(source);*/ \
  cc_string_clean(&s)

void
cc_debugerror_post(const char * source, const char * format, ...)
{
  CC_DEBUGERROR_POST(CC_DEBUGERROR_ERROR, "error");
}

void
cc_debugerror_postwarning(const char * source, const char * format, ...)
{
  CC_DEBUGERROR_POST(CC_DEBUGERROR_WARNING, "warning");
}

void
cc_debugerror_postinfo(const char * source, const char * format, ...)
{
  CC_DEBUGERROR_POST(CC_DEBUGERROR_INFO, "info");
}
