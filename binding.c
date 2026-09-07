#include <assert.h>
#include <bare.h>
#include <js.h>
#include <libheif/heif.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
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
    err = js_throw_errorf(env, NULL, "%s", error.message);
    assert(err == 0);

    heif_context_free(ctx);

    return NULL;
  }

  struct heif_image_handle *handle;
  error = heif_context_get_primary_image_handle(ctx, &handle);

  if (error.code != heif_error_Ok) {
    err = js_throw_errorf(env, NULL, "%s", error.message);
    assert(err == 0);

    heif_context_free(ctx);

    return NULL;
  }

  struct heif_image *image;
  error = heif_decode_image(handle, &image, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, NULL);

  if (error.code != heif_error_Ok) {
    err = js_throw_errorf(env, NULL, "%s", error.message);
    assert(err == 0);

    heif_image_handle_release(handle);
    heif_context_free(ctx);

    return NULL;
  }

  int stride;

  const uint8_t *plane = heif_image_get_plane_readonly(image, heif_channel_interleaved, &stride);

  int width = stride / 4;
  int height = heif_image_get_height(image, heif_channel_interleaved);

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

  // Widen before multiplying: int * int overflows for images past ~23k pixels per side.
  len = (size_t) stride * (size_t) height;

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
bare_heif_get_metadata(js_env_t *env, js_callback_info_t *info) {
  int err;

#define V(target, name, value) \
  { \
    js_value_t *val; \
    err = js_create_string_utf8(env, (const utf8_t *) (value), -1, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, target, name, val); \
    assert(err == 0); \
  }

  size_t argc = 2;
  js_value_t *argv[2];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 2);

  void *input;
  size_t input_len;
  err = js_get_typedarray_info(env, argv[0], NULL, &input, &input_len, NULL, NULL);
  assert(err == 0);

  char filter_buffer[5];
  const char *filter = NULL;

  if (argc == 2) {
    js_value_type_t type;
    err = js_typeof(env, argv[1], &type);
    assert(err == 0);

    if (type != js_undefined && type != js_string) {
      err = js_throw_type_errorf(env, NULL, "type must be a string");
      assert(err == 0);

      return NULL;
    }

    if (type == js_string) {
      size_t filter_len;
      err = js_get_value_string_utf8(env, argv[1], NULL, 0, &filter_len);
      assert(err == 0);

      if (filter_len != 4) {
        err = js_throw_type_errorf(env, NULL, "type must be a four-character string");
        assert(err == 0);

        return NULL;
      }

      err = js_get_value_string_utf8(env, argv[1], (utf8_t *) filter_buffer, sizeof(filter_buffer), NULL);
      assert(err == 0);

      filter = filter_buffer;
    }
  }

  struct heif_context *ctx = heif_context_alloc();
  assert(ctx);

  struct heif_error error = heif_context_read_from_memory_without_copy(ctx, input, input_len, NULL);

  if (error.code != heif_error_Ok) {
    err = js_throw_errorf(env, NULL, "%s", error.message);
    assert(err == 0);

    heif_context_free(ctx);

    return NULL;
  }

  struct heif_image_handle *handle;
  error = heif_context_get_primary_image_handle(ctx, &handle);

  if (error.code != heif_error_Ok) {
    err = js_throw_errorf(env, NULL, "%s", error.message);
    assert(err == 0);

    heif_context_free(ctx);

    return NULL;
  }

  int count = heif_image_handle_get_number_of_metadata_blocks(handle, filter);
  heif_item_id ids[count > 0 ? count : 1];

  if (count > 0) {
    count = heif_image_handle_get_list_of_metadata_block_IDs(handle, filter, ids, count);
  }

  js_value_t *result;
  err = js_create_array_with_length(env, count, &result);
  assert(err == 0);

  for (int i = 0; i < count; i++) {
    js_value_t *block;
    err = js_create_object(env, &block);
    assert(err == 0);

    const char *type = heif_image_handle_get_metadata_type(handle, ids[i]);
    bool is_uri = type && strcmp(type, "uri ") == 0;
    V(block, "type", type ? type : "")

    const char *content_type = heif_image_handle_get_metadata_content_type(handle, ids[i]);
    if (content_type && content_type[0]) {
      V(block, "contentType", content_type)
    }

    if (is_uri) {
      const char *uri_type = heif_image_handle_get_metadata_item_uri_type(handle, ids[i]);
      if (uri_type && uri_type[0]) {
        V(block, "uriType", uri_type)
      }
    }

    size_t metadata_len = heif_image_handle_get_metadata_size(handle, ids[i]);
    void *metadata;
    js_value_t *data;
    err = js_create_arraybuffer(env, metadata_len, &metadata, &data);
    assert(err == 0);

    error = heif_image_handle_get_metadata(handle, ids[i], metadata);
    if (error.code != heif_error_Ok) {
      err = js_throw_errorf(env, NULL, "%s", error.message);
      assert(err == 0);

      heif_image_handle_release(handle);
      heif_context_free(ctx);

      return NULL;
    }

    err = js_set_named_property(env, block, "data", data);
    assert(err == 0);

    err = js_set_element(env, result, i, block);
    assert(err == 0);
  }

  heif_image_handle_release(handle);
  heif_context_free(ctx);

#undef V

  return result;
}

static void
bare_heif__on_teardown(void *data) {
  heif_deinit();
}

static js_value_t *
bare_heif_exports(js_env_t *env, js_value_t *exports) {
  int err;

  struct heif_error error = heif_init(NULL);
  assert(error.code == heif_error_Ok);

  err = js_add_teardown_callback(env, bare_heif__on_teardown, NULL);
  assert(err == 0);

#define V(name, fn) \
  { \
    js_value_t *val; \
    err = js_create_function(env, name, -1, fn, NULL, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("decode", bare_heif_decode)
  V("getMetadata", bare_heif_get_metadata)
#undef V

  return exports;
}

BARE_MODULE(bare_heif, bare_heif_exports)
