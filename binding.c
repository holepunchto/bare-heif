#include <assert.h>
#include <bare.h>
#include <js.h>
#include <libheif/heif.h>
#include <utf.h>

static js_value_t *
bare_heif_decode(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  void *data;
  size_t len;
  err = js_get_typedarray_info(env, argv[0], NULL, &data, &len, NULL, NULL);
  assert(err == 0);

  struct heif_error error;

  struct heif_context *ctx = heif_context_alloc();

  error = heif_context_read_from_memory_without_copy(ctx, data, len, NULL);

  if (error.code != heif_error_Ok) {
    err = js_throw_errorf(env, NULL, error.message);
    assert(err == 0);

    heif_context_free(ctx);

    return NULL;
  }

  struct heif_image_handle *handle;
  error = heif_context_get_primary_image_handle(ctx, &handle);

  if (error.code != heif_error_Ok) {
    err = js_throw_errorf(env, NULL, error.message);
    assert(err == 0);

    heif_context_free(ctx);

    return NULL;
  }

  struct heif_image *image;
  error = heif_decode_image(handle, &image, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, NULL);

  if (error.code != heif_error_Ok) {
    err = js_throw_errorf(env, NULL, error.message);
    assert(err == 0);

    heif_image_handle_release(handle);
    heif_context_free(ctx);

    return NULL;
  }

  int width = heif_image_get_width(image, heif_channel_interleaved);
  int height = heif_image_get_height(image, heif_channel_interleaved);
  int stride;

  const uint8_t *plane = heif_image_get_plane_readonly(image, heif_channel_interleaved, &stride);

  assert(plane);

  js_value_t *result;
  err = js_create_object(env, &result);
  assert(err == 0);

#define V(n) \
  { \
    js_value_t *val; \
    err = js_create_int32(env, n, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, result, #n, val); \
    assert(err == 0); \
  }

  V(width);
  V(height);
#undef V

  len = stride * height;

  js_value_t *buffer;
  err = js_create_unsafe_arraybuffer(env, len, &data, &buffer);
  assert(err == 0);

  memcpy(data, plane, len);

  err = js_set_named_property(env, result, "data", buffer);
  assert(err == 0);

  heif_image_release(image);
  heif_image_handle_release(handle);
  heif_context_free(ctx);

  return result;
}

static js_value_t *
bare_heif_exports(js_env_t *env, js_value_t *exports) {
  int err;

#define V(name, fn) \
  { \
    js_value_t *val; \
    err = js_create_function(env, name, -1, fn, NULL, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("decode", bare_heif_decode)
#undef V

  return exports;
}

BARE_MODULE(bare_heif, bare_heif_exports)
