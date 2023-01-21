#include "store.h"
#include "builder.h"
#include "messages.h"
#include "parents.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>

struct store {
  char *root;
  size_t root_length;
};

struct store *create_store(const char *root, struct trace *trace) {
  struct store *store = TNULL(malloc(sizeof(struct store)), trace);
  char *root_copy = TNULL(strdup(root), trace);
  if (!ok(trace)) {
    free(store);
    return NULL;
  }

  store->root_length = strlen(root);
  store->root = root_copy;
  return store;
}

static struct builder *get_store_path_builder(const char *filesystem_path,
                                              const char *version,
                                              const struct store *store,
                                              struct trace *trace) {
  assert(*filesystem_path == '/');
  struct builder *builder = create_builder(trace);
  concat_string(store->root, builder, trace);
  concat_string(filesystem_path, builder, trace);
  concat_string("/", builder, trace);
  concat_string(version, builder, trace);
  return builder;
}

static void cleanup(struct builder *path_builder) {
  /*FIXME cleanup error reporting*/
  struct trace *cleanup_trace = create_trace();
  if (cleanup_trace) {
    remove_empty_parents(build_string(path_builder), cleanup_trace);
  }
  catch_all(cleanup_trace);
  free(cleanup_trace);
  free_builder(path_builder);
}

void copy_to_store(const char *filesystem_path, const char *version,
                   const struct store *store, struct trace *trace) {
  struct builder *path_builder =
      get_store_path_builder(filesystem_path, version, store, trace);
  create_parents(build_string(path_builder),
                 S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH, trace);
  if (!ok(trace)) {
    return cleanup(path_builder);
  }

  int in_fd = open(filesystem_path, O_RDONLY);
  if (in_fd < 0) {
    if (errno == ENOENT) {
      throw_static(messages.store.copy.file_does_not_exist, trace);
    } else if (errno == EACCES) {
      throw_static(messages.store.copy.permission_denied, trace);
    } else {
      throw_errno(trace);
    }
    return cleanup(path_builder);
  }

  int out_fd = open(build_string(path_builder), O_CREAT | O_WRONLY | O_EXCL,
                    S_IRUSR | S_IRGRP | S_IROTH);
  if (out_fd < 0) {
    if (errno == EEXIST) {
      throw_static(messages.store.copy.version_already_exists, trace);
    } else {
      throw_errno(trace);
    }
    close(in_fd);
    return cleanup(path_builder);
  }

  struct stat in_fd_stat;
  if (fstat(in_fd, &in_fd_stat) < 0) {
    throw_errno(trace);
    close(out_fd);
    close(in_fd);
    return cleanup(path_builder);
  }

  while (in_fd_stat.st_size) {
    size_t written_size = sendfile(out_fd, in_fd, NULL, in_fd_stat.st_size);
    if (written_size > 0) {
      in_fd_stat.st_size -= written_size;
    } else {
      throw_errno(trace);
      close(out_fd);
      close(in_fd);
      return cleanup(path_builder);
    }
  }

  if (close(out_fd) < 0) {
    throw_errno(trace);
    close(in_fd);
    return cleanup(path_builder);
  }

  close(in_fd);
  free_builder(path_builder);
}

void free_store(struct store *store) {
  if (store) {
    free(store->root);
    free(store);
  }
}
