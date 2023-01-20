#include "trace.h"
#include <stddef.h>

struct builder;

struct builder *create_builder(struct trace *trace);
void concat_string(const char *string, struct builder *builder,
                   struct trace *trace);
void concat_size(size_t size, struct builder *builder, struct trace *trace);
const char *build_string(const struct builder *builder);
size_t get_builder_length(const struct builder *builder);
void truncate_builder(size_t length, struct builder *builder);
void free_builder(struct builder *builder);
