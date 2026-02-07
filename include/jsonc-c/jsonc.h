#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifdef _JSONC_EXPORT
#ifdef _WIN32
#ifdef JSONC_DLL
#define JSONC_API __declspec(dllimport)
#else
#define JSONC_API __declspec(dllexport)
#endif
#else
#define JSONC_API __attribute__((visibility("default"), used))
#endif
#else
#define JSONC_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void* jsonc_variant_t;
typedef void* jsonc_object_t;
typedef void* jsonc_array_t;

#define JSONC_VALUE_TYPE_NULL                 0
#define JSONC_VALUE_TYPE_BOOLEAN              1
#define JSONC_VALUE_TYPE_SIGNED               2
#define JSONC_VALUE_TYPE_UNSIGNED             3
#define JSONC_VALUE_TYPE_FLOAT                4
#define JSONC_VALUE_TYPE_STRING               5
#define JSONC_VALUE_TYPE_OBJECT               6
#define JSONC_VALUE_TYPE_ARRAY                7
#define JSONC_VALUE_TYPE_BIG_INTEGER          8
#define JSONC_VALUE_TYPE_HIGH_PRECISION_FLOAT 9

JSONC_API const char* jsonc_get_library_version_string();

JSONC_API uint8_t jsonc_get_library_version_major();
JSONC_API uint8_t jsonc_get_library_version_minor();
JSONC_API uint8_t jsonc_get_library_version_patch();

JSONC_API jsonc_variant_t jsonc_parse_content(const char* content, bool allow_trailing_comma);

JSONC_API int jsonc_get_variant_type(jsonc_variant_t handle);

JSONC_API bool     jsonc_variant_to_bool(jsonc_variant_t handle);
JSONC_API int64_t  jsonc_variant_to_signed(jsonc_variant_t handle);
JSONC_API uint64_t jsonc_variant_to_unsigned(jsonc_variant_t handle);
JSONC_API double   jsonc_variant_to_float(jsonc_variant_t handle);

JSONC_API const char* jsonc_variant_as_string(jsonc_variant_t handle);
JSONC_API const char* jsonc_variant_as_big_int(jsonc_variant_t handle);
JSONC_API const char* jsonc_variant_as_big_float(jsonc_variant_t handle);

JSONC_API jsonc_object_t jsonc_variant_as_object(jsonc_variant_t handle);
JSONC_API jsonc_array_t  jsonc_variant_as_array(jsonc_variant_t handle);
JSONC_API const char*    jsonc_variant_get_comments_before(jsonc_variant_t handle); // Need release buffer
JSONC_API const char*    jsonc_variant_get_comments_after(jsonc_variant_t handle);  // Need release buffer
JSONC_API void           jsonc_variant_set_comments_before(jsonc_variant_t handle, const char* comments);
JSONC_API void           jsonc_variant_set_comments_after(jsonc_variant_t handle, const char* comments);

JSONC_API void jsonc_variant_merge_comments(jsonc_variant_t lhs, jsonc_variant_t rhs);

// Need release buffer
JSONC_API const char*
jsonc_variant_dump(jsonc_variant_t handle, int indent, bool ensure_ascii, bool ignore_comments, bool multi_line_comments_format);

JSONC_API bool           jsonc_object_contains(jsonc_object_t handle, const char* key);
JSONC_API bool           jsonc_object_value_is_any_int_type(jsonc_object_t handle, const char* key);
JSONC_API bool           jsonc_object_value_is_any_float_type(jsonc_object_t handle, const char* key);
JSONC_API bool           jsonc_object_value_is_any_number_type(jsonc_object_t handle, const char* key);
JSONC_API int            jsonc_object_get_type(jsonc_object_t handle, const char* key);
JSONC_API bool           jsonc_object_get_bool(jsonc_object_t handle, const char* key);
JSONC_API void           jsonc_object_set_bool(jsonc_object_t handle, const char* key, bool value);
JSONC_API int64_t        jsonc_object_get_signed(jsonc_object_t handle, const char* key);
JSONC_API void           jsonc_object_set_signed(jsonc_object_t handle, const char* key, int64_t value);
JSONC_API uint64_t       jsonc_object_get_unsigned(jsonc_object_t handle, const char* key);
JSONC_API void           jsonc_object_set_unsigned(jsonc_object_t handle, const char* key, uint64_t value);
JSONC_API const char*    jsonc_object_get_any_int(jsonc_object_t handle, const char* key); // Need release buffer
JSONC_API bool           jsonc_object_set_any_int(jsonc_object_t handle, const char* key, const char* value);
JSONC_API double         jsonc_object_get_float(jsonc_object_t handle, const char* key);
JSONC_API void           jsonc_object_set_float(jsonc_object_t handle, const char* key, double value);
JSONC_API const char*    jsonc_object_get_any_float(jsonc_object_t handle, const char* key); // Need release buffer
JSONC_API bool           jsonc_object_set_any_float(jsonc_object_t handle, const char* key, const char* value);
JSONC_API const char*    jsonc_object_get_any_number(jsonc_object_t handle, const char* key); // Need release buffer
JSONC_API bool           jsonc_object_set_any_number(jsonc_object_t handle, const char* key, const char* value);
JSONC_API const char*    jsonc_object_get_string(jsonc_object_t handle, const char* key);
JSONC_API void           jsonc_object_set_string(jsonc_object_t handle, const char* key, const char* value);
JSONC_API jsonc_object_t jsonc_object_get_object(jsonc_object_t handle, const char* key);
JSONC_API jsonc_object_t jsonc_object_add_new_object(jsonc_object_t handle, const char* key);
JSONC_API void           jsonc_object_set_object(jsonc_object_t handle, const char* key, jsonc_object_t value);
JSONC_API jsonc_array_t  jsonc_object_get_array(jsonc_object_t handle, const char* key);
JSONC_API jsonc_array_t  jsonc_object_add_new_array(jsonc_object_t handle, const char* key);
JSONC_API void           jsonc_object_set_array(jsonc_object_t handle, const char* key, jsonc_array_t value);
JSONC_API size_t         jsonc_object_get_size(jsonc_object_t handle);
JSONC_API const char*    jsonc_object_get_key_at_index(jsonc_object_t handle, size_t index);
JSONC_API void           jsonc_object_clear(jsonc_object_t handle);
JSONC_API bool           jsonc_object_remove(jsonc_object_t handle, const char* key);

// Need release buffer
JSONC_API const char* jsonc_object_dump(jsonc_object_t handle, int indent, bool ensure_ascii, bool ignore_comments, bool multi_line_comments_format);

JSONC_API const char* jsonc_object_get_key_comments_before(jsonc_object_t handle, const char* key);                       // Need release buffer
JSONC_API const char* jsonc_object_get_key_comments_after(jsonc_object_t handle, const char* key);                        // Need release buffer
JSONC_API const char* jsonc_object_get_value_comments_before(jsonc_object_t handle, const char* key);                     // Need release buffer
JSONC_API const char* jsonc_object_get_value_comments_after(jsonc_object_t handle, const char* key);                      // Need release buffer
JSONC_API void        jsonc_object_set_key_comments_before(jsonc_object_t handle, const char* key, const char* comments); // Need release buffer
JSONC_API void        jsonc_object_set_key_comments_after(jsonc_object_t handle, const char* key, const char* comments);
JSONC_API void        jsonc_object_set_value_comments_before(jsonc_object_t handle, const char* key, const char* comments);
JSONC_API void        jsonc_object_set_value_comments_after(jsonc_object_t handle, const char* key, const char* comments);
JSONC_API bool        jsonc_object_equals(jsonc_object_t lhs, jsonc_object_t rhs);

JSONC_API int            jsonc_array_get_type(jsonc_array_t handle, size_t index);
JSONC_API bool           jsonc_array_value_is_any_int_type(jsonc_array_t handle, size_t index);
JSONC_API bool           jsonc_array_value_is_any_float_type(jsonc_array_t handle, size_t index);
JSONC_API bool           jsonc_array_value_is_any_number_type(jsonc_array_t handle, size_t index);
JSONC_API bool           jsonc_array_get_bool(jsonc_array_t handle, size_t index);
JSONC_API void           jsonc_array_set_bool(jsonc_array_t handle, size_t index, bool value);
JSONC_API void           jsonc_array_add_bool(jsonc_array_t handle, bool value);
JSONC_API int64_t        jsonc_array_get_signed(jsonc_array_t handle, size_t index);
JSONC_API void           jsonc_array_set_signed(jsonc_array_t handle, size_t index, int64_t value);
JSONC_API void           jsonc_array_add_signed(jsonc_array_t handle, int64_t value);
JSONC_API uint64_t       jsonc_array_get_unsigned(jsonc_array_t handle, size_t index);
JSONC_API void           jsonc_array_set_unsigned(jsonc_array_t handle, size_t index, uint64_t value);
JSONC_API void           jsonc_array_add_unsigned(jsonc_array_t handle, uint64_t value);
JSONC_API const char*    jsonc_array_get_any_int(jsonc_array_t handle, size_t index); // Need release buffer
JSONC_API bool           jsonc_array_set_any_int(jsonc_array_t handle, size_t index, const char* value);
JSONC_API bool           jsonc_array_add_any_int(jsonc_array_t handle, const char* value);
JSONC_API double         jsonc_array_get_float(jsonc_array_t handle, size_t index);
JSONC_API void           jsonc_array_set_float(jsonc_array_t handle, size_t index, double value);
JSONC_API void           jsonc_array_add_float(jsonc_array_t handle, double value);
JSONC_API const char*    jsonc_array_get_any_float(jsonc_array_t handle, size_t index); // Need release buffer
JSONC_API bool           jsonc_array_set_any_float(jsonc_array_t handle, size_t index, const char* value);
JSONC_API bool           jsonc_array_add_any_float(jsonc_array_t handle, const char* value);
JSONC_API const char*    jsonc_array_get_any_number(jsonc_array_t handle, size_t index); // Need release buffer
JSONC_API bool           jsonc_array_set_any_number(jsonc_array_t handle, size_t index, const char* value);
JSONC_API bool           jsonc_array_add_any_number(jsonc_array_t handle, const char* value);
JSONC_API const char*    jsonc_array_get_string(jsonc_array_t handle, size_t index);
JSONC_API void           jsonc_array_set_string(jsonc_array_t handle, size_t index, const char* value);
JSONC_API void           jsonc_array_add_string(jsonc_array_t handle, const char* value);
JSONC_API jsonc_object_t jsonc_array_get_object(jsonc_array_t handle, size_t index);
JSONC_API jsonc_object_t jsonc_array_add_new_object(jsonc_array_t handle);
JSONC_API void           jsonc_array_set_object(jsonc_array_t handle, size_t index, jsonc_object_t value);
JSONC_API jsonc_array_t  jsonc_array_get_array(jsonc_array_t handle, size_t index);
JSONC_API jsonc_array_t  jsonc_array_add_new_array(jsonc_array_t handle);
JSONC_API void           jsonc_array_set_array(jsonc_array_t handle, size_t index, jsonc_array_t value);
JSONC_API size_t         jsonc_array_get_size(jsonc_array_t handle);
JSONC_API void           jsonc_array_clear(jsonc_array_t handle);
JSONC_API bool           jsonc_array_remove(jsonc_array_t handle, size_t index);

// Need release buffer
JSONC_API const char* jsonc_array_dump(jsonc_array_t handle, int indent, bool ensure_ascii, bool ignore_comments, bool multi_line_comments_format);

JSONC_API const char* jsonc_array_get_comments_before(jsonc_object_t handle, size_t index); // Need release buffer
JSONC_API const char* jsonc_array_get_comments_after(jsonc_object_t handle, size_t index);  // Need release buffer
JSONC_API void        jsonc_array_set_comments_before(jsonc_object_t handle, size_t index, const char* comments);
JSONC_API void        jsonc_array_set_comments_after(jsonc_object_t handle, size_t index, const char* comments);
JSONC_API bool        jsonc_array_equals(jsonc_array_t lhs, jsonc_array_t rhs);

JSONC_API jsonc_object_t jsonc_create_object();
JSONC_API jsonc_array_t  jsonc_create_array();

JSONC_API void jsonc_free_object(jsonc_object_t handle);
JSONC_API void jsonc_free_array(jsonc_array_t handle);
JSONC_API void jsonc_free_type_variant(jsonc_variant_t handle);
JSONC_API void jsonc_free_string(const char* str);

#ifdef __cplusplus
}
#endif