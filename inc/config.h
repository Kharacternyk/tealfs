#include "set.h"
#include <fcntl.h>

struct config;

struct config *load_config(const char *path, int *error_code,
                           char **error_message);

const struct set *get_configured_editors(const struct config *config);
const char *get_configured_store_root(const struct config *config);
const char *get_configured_queue_path(const struct config *config);
const char *get_configured_version_pattern(const struct config *config);
size_t get_configured_debounce_seconds(const struct config *config);
size_t get_configured_version_max_length(const struct config *config);
size_t get_configured_path_length_guess(const struct config *config);
pid_t get_configured_max_pid_guess(const struct config *config);

void free_config(struct config *config);
