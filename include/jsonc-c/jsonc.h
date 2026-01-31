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

typedef void* JsoncTypeVariantHandle;
typedef void* JsoncObjectHandle;
typedef void* JsoncArrayHandle;

enum JsoncValueType {
    JSONC_TYPE_NULL     = 0,
    JSONC_TYPE_BOOLEAN  = 1,
    JSONC_TYPE_SIGNED   = 2,
    JSONC_TYPE_UNSIGNED = 3,
    JSONC_TYPE_FLOAT    = 4,
    JSONC_TYPE_STRING   = 5,
    JSONC_TYPE_OBJECT   = 6,
    JSONC_TYPE_ARRAY    = 7,
    JSONC_TYPE_BIG_INT  = 8,
};

JSONC_API const char* jsonc_get_library_version();

JSONC_API JsoncTypeVariantHandle jsonc_parse_content(const char* content, bool allow_trailing_comma);

JSONC_API enum JsoncValueType jsonc_get_variant_type(JsoncTypeVariantHandle handle);

JSONC_API bool     jsonc_variant_to_bool(JsoncTypeVariantHandle handle);
JSONC_API int64_t  jsonc_variant_to_signed(JsoncTypeVariantHandle handle);
JSONC_API uint64_t jsonc_variant_to_unsigned(JsoncTypeVariantHandle handle);
JSONC_API double   jsonc_variant_to_float(JsoncTypeVariantHandle handle);

JSONC_API const char* jsonc_variant_as_string(JsoncTypeVariantHandle handle);
JSONC_API const char* jsonc_variant_as_big_int(JsoncTypeVariantHandle handle);

JSONC_API JsoncObjectHandle jsonc_variant_as_object(JsoncTypeVariantHandle handle);
JSONC_API JsoncArrayHandle  jsonc_variant_as_array(JsoncTypeVariantHandle handle);
JSONC_API const char*       jsonc_variant_get_comments_before(JsoncTypeVariantHandle handle); // Need release buffer
JSONC_API const char*       jsonc_variant_get_comments_after(JsoncTypeVariantHandle handle);  // Need release buffer
JSONC_API void              jsonc_variant_set_comments_before(JsoncTypeVariantHandle handle, const char* comments);
JSONC_API void              jsonc_variant_set_comments_after(JsoncTypeVariantHandle handle, const char* comments);

JSONC_API void jsonc_variant_merge_comments(JsoncTypeVariantHandle lhs, JsoncTypeVariantHandle rhs);

JSONC_API const char* jsonc_variant_dump(
    JsoncTypeVariantHandle handle,
    int                    indent,
    bool                   ensure_ascii,
    bool                   ignore_comments,
    bool                   multi_line_comments_format
); // Need release buffer

JSONC_API bool                jsonc_object_contains(JsoncObjectHandle handle, const char* key);
JSONC_API bool                jsonc_object_value_is_any_int_type(JsoncObjectHandle handle, const char* key);
JSONC_API enum JsoncValueType jsonc_object_get_type(JsoncObjectHandle handle, const char* key);
JSONC_API bool                jsonc_object_get_bool(JsoncObjectHandle handle, const char* key);
JSONC_API void                jsonc_object_set_bool(JsoncObjectHandle handle, const char* key, bool value);
JSONC_API int64_t             jsonc_object_get_signed(JsoncObjectHandle handle, const char* key);
JSONC_API void                jsonc_object_set_signed(JsoncObjectHandle handle, const char* key, int64_t value);
JSONC_API uint64_t            jsonc_object_get_unsigned(JsoncObjectHandle handle, const char* key);
JSONC_API void                jsonc_object_set_unsigned(JsoncObjectHandle handle, const char* key, uint64_t value);
JSONC_API const char*         jsonc_object_get_any_int(JsoncObjectHandle handle, const char* key); // Need release buffer
JSONC_API bool                jsonc_object_set_any_int(JsoncObjectHandle handle, const char* key, const char* value);
JSONC_API double              jsonc_object_get_float(JsoncObjectHandle handle, const char* key);
JSONC_API void                jsonc_object_set_float(JsoncObjectHandle handle, const char* key, double value);
JSONC_API const char*         jsonc_object_get_string(JsoncObjectHandle handle, const char* key);
JSONC_API void                jsonc_object_set_string(JsoncObjectHandle handle, const char* key, const char* value);
JSONC_API JsoncObjectHandle   jsonc_object_get_object(JsoncObjectHandle handle, const char* key);
JSONC_API JsoncObjectHandle   jsonc_object_add_new_object(JsoncObjectHandle handle, const char* key);
JSONC_API void                jsonc_object_set_object(JsoncObjectHandle handle, const char* key, JsoncObjectHandle value);
JSONC_API JsoncArrayHandle    jsonc_object_get_array(JsoncObjectHandle handle, const char* key);
JSONC_API JsoncArrayHandle    jsonc_object_add_new_array(JsoncObjectHandle handle, const char* key);
JSONC_API void                jsonc_object_set_array(JsoncObjectHandle handle, const char* key, JsoncArrayHandle value);
JSONC_API size_t              jsonc_object_get_size(JsoncObjectHandle handle);
JSONC_API const char*         jsonc_object_get_key_at_index(JsoncObjectHandle handle, size_t index);
JSONC_API void                jsonc_object_clear(JsoncObjectHandle handle);
JSONC_API bool                jsonc_object_remove(JsoncObjectHandle handle, const char* key);
JSONC_API const char*         jsonc_object_dump(
            JsoncObjectHandle handle,
            int               indent,
            bool              ensure_ascii,
            bool              ignore_comments,
            bool              multi_line_comments_format
        );                                                                                                                   // Need release buffer
JSONC_API const char* jsonc_object_get_key_comments_before(JsoncObjectHandle handle, const char* key);                       // Need release buffer
JSONC_API const char* jsonc_object_get_key_comments_after(JsoncObjectHandle handle, const char* key);                        // Need release buffer
JSONC_API const char* jsonc_object_get_value_comments_before(JsoncObjectHandle handle, const char* key);                     // Need release buffer
JSONC_API const char* jsonc_object_get_value_comments_after(JsoncObjectHandle handle, const char* key);                      // Need release buffer
JSONC_API void        jsonc_object_set_key_comments_before(JsoncObjectHandle handle, const char* key, const char* comments); // Need release buffer
JSONC_API void        jsonc_object_set_key_comments_after(JsoncObjectHandle handle, const char* key, const char* comments);
JSONC_API void        jsonc_object_set_value_comments_before(JsoncObjectHandle handle, const char* key, const char* comments);
JSONC_API void        jsonc_object_set_value_comments_after(JsoncObjectHandle handle, const char* key, const char* comments);
JSONC_API bool        jsonc_object_equals(JsoncObjectHandle lhs, JsoncObjectHandle rhs);

JSONC_API enum JsoncValueType jsonc_array_get_type(JsoncArrayHandle handle, size_t index);
JSONC_API bool                jsonc_array_value_is_any_int_type(JsoncObjectHandle handle, size_t index);
JSONC_API bool                jsonc_array_get_bool(JsoncArrayHandle handle, size_t index);
JSONC_API void                jsonc_array_set_bool(JsoncArrayHandle handle, size_t index, bool value);
JSONC_API void                jsonc_array_add_bool(JsoncArrayHandle handle, bool value);
JSONC_API int64_t             jsonc_array_get_signed(JsoncArrayHandle handle, size_t index);
JSONC_API void                jsonc_array_set_signed(JsoncArrayHandle handle, size_t index, int64_t value);
JSONC_API void                jsonc_array_add_signed(JsoncArrayHandle handle, int64_t value);
JSONC_API uint64_t            jsonc_array_get_unsigned(JsoncArrayHandle handle, size_t index);
JSONC_API void                jsonc_array_set_unsigned(JsoncArrayHandle handle, size_t index, uint64_t value);
JSONC_API void                jsonc_array_add_unsigned(JsoncArrayHandle handle, uint64_t value);
JSONC_API const char*         jsonc_array_get_any_int(JsoncObjectHandle handle, size_t index); // Need release buffer
JSONC_API bool                jsonc_array_set_any_int(JsoncObjectHandle handle, size_t index, const char* value);
JSONC_API bool                jsonc_array_add_any_int(JsoncArrayHandle handle, const char* value);
JSONC_API double              jsonc_array_get_float(JsoncArrayHandle handle, size_t index);
JSONC_API void                jsonc_array_set_float(JsoncArrayHandle handle, size_t index, double value);
JSONC_API void                jsonc_array_add_float(JsoncArrayHandle handle, double value);
JSONC_API const char*         jsonc_array_get_string(JsoncArrayHandle handle, size_t index);
JSONC_API void                jsonc_array_set_string(JsoncArrayHandle handle, size_t index, const char* value);
JSONC_API void                jsonc_array_add_string(JsoncArrayHandle handle, const char* value);
JSONC_API JsoncObjectHandle   jsonc_array_get_object(JsoncArrayHandle handle, size_t index);
JSONC_API JsoncObjectHandle   jsonc_array_add_new_object(JsoncArrayHandle handle);
JSONC_API void                jsonc_array_set_object(JsoncArrayHandle handle, size_t index, JsoncObjectHandle value);
JSONC_API JsoncArrayHandle    jsonc_array_get_array(JsoncArrayHandle handle, size_t index);
JSONC_API JsoncArrayHandle    jsonc_array_add_new_array(JsoncArrayHandle handle);
JSONC_API void                jsonc_array_set_array(JsoncArrayHandle handle, size_t index, JsoncArrayHandle value);
JSONC_API size_t              jsonc_array_get_size(JsoncArrayHandle handle);
JSONC_API void                jsonc_array_clear(JsoncArrayHandle handle);
JSONC_API bool                jsonc_array_remove(JsoncArrayHandle handle, size_t index);
JSONC_API const char*         jsonc_array_dump(
            JsoncArrayHandle handle,
            int              indent,
            bool             ensure_ascii,
            bool             ignore_comments,
            bool             multi_line_comments_format
        );                                                                                     // Need release buffer
JSONC_API const char* jsonc_array_get_comments_before(JsoncObjectHandle handle, size_t index); // Need release buffer
JSONC_API const char* jsonc_array_get_comments_after(JsoncObjectHandle handle, size_t index);  // Need release buffer
JSONC_API void        jsonc_array_set_comments_before(JsoncObjectHandle handle, size_t index, const char* comments);
JSONC_API void        jsonc_array_set_comments_after(JsoncObjectHandle handle, size_t index, const char* comments);
JSONC_API bool        jsonc_array_equals(JsoncArrayHandle lhs, JsoncArrayHandle rhs);

JSONC_API JsoncObjectHandle jsonc_create_object();
JSONC_API JsoncArrayHandle  jsonc_create_array();

JSONC_API void jsonc_free_object(JsoncObjectHandle handle);
JSONC_API void jsonc_free_array(JsoncArrayHandle handle);
JSONC_API void jsonc_free_type_variant(JsoncTypeVariantHandle handle);
JSONC_API void jsonc_free_string(const char* str);

#ifdef __cplusplus
}
#endif