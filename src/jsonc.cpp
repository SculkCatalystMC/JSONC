#include "jsonc/jsonc.hpp"
#include "jsonc-c/jsonc.h"

#ifdef __cplusplus
extern "C" {
#endif

JsoncTypeVariantHandle jsonc_parse_content(const char* content, bool allow_trailing_comma);
/*
 {
    if (auto result = jsonc::parse(content, allow_trailing_comma)) { return &result.value(); }
    return nullptr;
}
*/

JsoncValueType jsonc_get_variant_type(JsoncTypeVariantHandle handle);

bool        jsonc_variant_to_bool(JsoncTypeVariantHandle handle);
int64_t     jsonc_variant_to_signed(JsoncTypeVariantHandle handle);
uint64_t    jsonc_variant_to_unsigned(JsoncTypeVariantHandle handle);
double      jsonc_variant_to_float(JsoncTypeVariantHandle handle);
const char* jsonc_variant_to_string(JsoncTypeVariantHandle handle);

JsoncObjectHandle jsonc_variant_to_object(JsoncTypeVariantHandle handle);
JsoncArrayHandle  jsonc_variant_to_array(JsoncTypeVariantHandle handle);
const char*       jsonc_variant_dump(JsoncTypeVariantHandle handle, int indent, bool ensure_ascii, bool ignore_comments);

bool              jsonc_object_contains(JsoncObjectHandle handle, const char* key);
JsoncValueType    jsonc_object_get_type(JsoncObjectHandle handle, const char* key);
bool              jsonc_object_get_bool(JsoncObjectHandle handle, const char* key);
void              jsonc_object_set_bool(JsoncObjectHandle handle, const char* key, bool value);
int64_t           jsonc_object_get_signed(JsoncObjectHandle handle, const char* key);
void              jsonc_object_set_signed(JsoncObjectHandle handle, const char* key, int64_t value);
uint64_t          jsonc_object_get_unsigned(JsoncObjectHandle handle, const char* key);
void              jsonc_object_set_unsigned(JsoncObjectHandle handle, const char* key, uint64_t value);
double            jsonc_object_get_double(JsoncObjectHandle handle, const char* key);
void              jsonc_object_set_double(JsoncObjectHandle handle, const char* key, double value);
const char*       jsonc_object_get_string(JsoncObjectHandle handle, const char* key);
void              jsonc_object_set_string(JsoncObjectHandle handle, const char* key, const char* value);
JsoncObjectHandle jsonc_object_get_object(JsoncObjectHandle handle, const char* key);
JsoncObjectHandle jsonc_object_add_new_object(JsoncObjectHandle handle, const char* key);
void              jsonc_object_set_object(JsoncObjectHandle handle, const char* key, JsoncObjectHandle value);
JsoncArrayHandle  jsonc_object_get_array(JsoncObjectHandle handle, const char* key);
JsoncArrayHandle  jsonc_object_add_new_array(JsoncObjectHandle handle, const char* key);
void              jsonc_object_set_array(JsoncObjectHandle handle, const char* key, JsoncArrayHandle value);
size_t            jsonc_object_get_size(JsoncObjectHandle handle);
const char*       jsonc_object_get_key_at_index(JsoncObjectHandle handle, size_t index);
void              jsonc_object_clear(JsoncObjectHandle handle);
bool              jsonc_object_remove(JsoncObjectHandle handle, const char* key);
const char*       jsonc_object_dump(JsoncObjectHandle handle, int indent, bool ensure_ascii, bool ignore_comments);
const char*       jsonc_object_get_key_comments_before(JsoncObjectHandle handle, const char* key);
const char*       jsonc_object_get_key_comments_after(JsoncObjectHandle handle, const char* key);
const char*       jsonc_object_get_value_comments_before(JsoncObjectHandle handle, const char* key);
const char*       jsonc_object_get_value_comments_after(JsoncObjectHandle handle, const char* key);
void              jsonc_object_set_key_comments_before(JsoncObjectHandle handle, const char* key, const char* comments);
void              jsonc_object_set_key_comments_after(JsoncObjectHandle handle, const char* key, const char* comments);
void              jsonc_object_set_value_comments_before(JsoncObjectHandle handle, const char* key, const char* comments);
void              jsonc_object_set_value_comments_after(JsoncObjectHandle handle, const char* key, const char* comments);

JsoncValueType    jsonc_array_get_type(JsoncArrayHandle handle, size_t index, JsoncValueType type);
bool              jsonc_array_get_bool(JsoncArrayHandle handle, size_t index);
void              jsonc_array_set_bool(JsoncArrayHandle handle, size_t index, bool value);
void              jsonc_array_add_bool(JsoncArrayHandle handle, bool value);
int64_t           jsonc_array_get_signed(JsoncArrayHandle handle, size_t index);
void              jsonc_array_set_signed(JsoncArrayHandle handle, size_t index, int64_t value);
void              jsonc_array_add_signed(JsoncArrayHandle handle, int64_t value);
uint64_t          jsonc_array_get_unsigned(JsoncArrayHandle handle, size_t index);
void              jsonc_array_set_unsigned(JsoncArrayHandle handle, size_t index, uint64_t value);
void              jsonc_array_add_unsigend(JsoncArrayHandle handle, uint64_t value);
double            jsonc_array_get_double(JsoncArrayHandle handle, size_t index);
void              jsonc_array_set_double(JsoncArrayHandle handle, size_t index, double value);
void              jsonc_array_add_double(JsoncArrayHandle handle, double value);
const char*       jsonc_array_get_string(JsoncArrayHandle handle, size_t index);
void              jsonc_array_set_string(JsoncArrayHandle handle, size_t index, const char* value);
JsoncObjectHandle jsonc_array_get_object(JsoncArrayHandle handle, size_t index);
void              jsonc_array_add_string(JsoncArrayHandle handle, const char* value);
JsoncObjectHandle jsonc_array_add_new_object(JsoncArrayHandle handle, size_t index);
void              jsonc_array_set_object(JsoncArrayHandle handle, size_t index, JsoncObjectHandle value);
JsoncArrayHandle  jsonc_array_get_array(JsoncArrayHandle handle, size_t index);
JsoncArrayHandle  jsonc_array_add_new_array(JsoncArrayHandle handle, size_t index);
void              jsonc_array_set_array(JsoncArrayHandle handle, size_t index, JsoncArrayHandle value);
size_t            jsonc_array_get_size(JsoncArrayHandle handle);
void              jsonc_array_clear(JsoncArrayHandle handle);
bool              jsonc_array_remove(JsoncArrayHandle handle, size_t index);
const char*       jsonc_array_dump(JsoncArrayHandle handle, int indent, bool ensure_ascii, bool ignore_comments);
const char*       jsonc_array_get_comments_before(JsoncObjectHandle handle, size_t index);
const char*       jsonc_array_get_comments_after(JsoncObjectHandle handle, size_t index);
void              jsonc_array_set_comments_before(JsoncObjectHandle handle, size_t index, const char* comments);
void              jsonc_array_set_comments_after(JsoncObjectHandle handle, size_t index, const char* comments);

#ifdef __cplusplus
}
#endif