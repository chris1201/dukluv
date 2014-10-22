#include "duv.h"

static uv_timer_t* duv_require_timer(duk_context *ctx, int index) {
  // TODO: verify pointer type somehow.
  uv_timer_t* handle = duk_require_pointer(ctx, 0);
  return handle;
}

static duk_ret_t duv_new_timer(duk_context *ctx) {
  uv_timer_t* handle = duk_alloc(ctx, sizeof(*handle));
  duv_check(ctx, uv_timer_init(duv_loop(ctx), handle));
  duv_setup_handle(ctx, (uv_handle_t*)handle);
  return 1;
}

static void duv_timer_cb(uv_timer_t* handle) {
  duk_context *ctx = handle->loop->data;
  duv_emit_event(ctx, (uv_handle_t*)handle, DUV_TIMEOUT, 0);
}

static duk_ret_t duv_timer_start(duk_context *ctx) {
  uv_timer_t* handle = duv_require_timer(ctx, 0);
  duv_handle_t* data = handle->data;
  uint64_t timeout = duk_require_int(ctx, 1);
  uint64_t repeat = duk_require_int(ctx, 2);
  if (!duk_is_function(ctx, 3)) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "callback required");
  }
  duk_dup(ctx, 3);
  data->callbacks[DUV_TIMEOUT] = duv_ref(ctx);

  duv_check(ctx, uv_timer_start(handle, duv_timer_cb, timeout, repeat));
  return 0;
}

static duk_ret_t duv_timer_stop(duk_context *ctx) {
  uv_timer_t* handle = duv_require_timer(ctx, 0);
  duv_check(ctx, uv_timer_stop(handle));
  return 0;
}

static duk_ret_t duv_timer_again(duk_context *ctx) {
  uv_timer_t* handle = duv_require_timer(ctx, 0);
  duv_check(ctx, uv_timer_again(handle));
  return 0;
}

static duk_ret_t duv_timer_set_repeat(duk_context *ctx) {
  uv_timer_t* handle = duv_require_timer(ctx, 0);
  uint64_t repeat = duk_require_int(ctx, 1);
  uv_timer_set_repeat(handle, repeat);
  return 0;
}

static duk_ret_t duv_timer_get_repeat(duk_context *ctx) {
  uv_timer_t* handle = duv_require_timer(ctx, 0);
  uint64_t repeat = uv_timer_get_repeat(handle);
  duk_push_int(ctx, repeat);
  return 1;
}
