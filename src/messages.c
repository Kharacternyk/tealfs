#include "messages.h"

/* clang-format off */
const struct translation messages = {
  .trace = {
    .because_of = "because of",
    .which_is = "which is",
    .message_dropped = "message_dropped",
  },
  .timestamp = {
    .overflow = "Version string exceeds the size limit",
  },
  .parents = {
    .cannot_remove_ancestor = "Cannot remove ancestor directory",
    .cannot_create_ancestor = "Cannot create ancestor directory",
  },
  .store = {
    .copy = {
      .file_does_not_exist = "File does not exist",
      .permission_denied = "Permission denied",
      .version_already_exists = "Version already exists",
    },
  },
  .config = {
    .is_static = "Configuration files are not supported",
  },
  .linq = {
    .invalid_entry = "Queue contains an invalid entry",
  },
  .params = {
    .unknown_option = "Unknown option",
    .stray_option = "Stray option",
  },
  .handler = {
    .version = {
      .has_slashes = "Version string contains slash characters",
    },
    .config = {
      .cannot_load = "Cannot load configuration file",
      .cannot_reload = "Cannot reload configuration file",
    },
    .linq = {
      .cannot_load = "Cannot load debouncing link queue",
      .cannot_reload = "Cannot reload debouncing link queue",
      .cannot_get_head = "Cannot get head of debouncing link queue",
      .cannot_push = "Cannot push file to debouncing link queue",
    },
    .store = {
      .cannot_copy = "Cannot copy file to store",
    },
    .journal = {
      .cannot_open = "Cannot open journal",
      .cannot_write_to = "Cannot write to journal"
    },
  },
};
/* clang-format on */
