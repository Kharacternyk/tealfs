#include "config.h"
#include "deref.h"
#include "store.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fanotify.h>
#include <unistd.h>

#define ERROR_STORE 1
#define ERROR_FANOTIFY 2
#define ERROR_PROC 3

static void typed_error_callback_function(const char **message) {
  perror(*message);
  *message = NULL;
}

static void error_callback_function(void *message) {
  typed_error_callback_function(message);
}

int main(int argc, const char **argv) {
  const char *error_message = NULL;
  struct callback *error_callback =
      create_callback(error_callback_function, &error_message);

  struct set *editors = get_editors();
  const char *store_root = argc > 1 ? argv[1] : "./klunok-store";
  error_message = "Cannot create store";
  struct store *store = create_store(store_root, error_callback);

  if (!error_message) {
    return ERROR_STORE;
  }

  int status = 0;
  /* TODO Why does it behave strange with O_RDWR? */
  int fanotify_fd = fanotify_init(FAN_CLASS_CONTENT, O_RDONLY);

  if (fanotify_fd < 0) {
    perror("Cannot init fanotirfy");
    return ERROR_FANOTIFY;
  }

  status = fanotify_mark(fanotify_fd, FAN_MARK_ADD | FAN_MARK_FILESYSTEM,
                         FAN_OPEN_PERM, 0, "/home");

  if (status < 0) {
    perror("Cannot mark /home");
    return ERROR_FANOTIFY;
  }

  for (;;) {
    struct fanotify_event_metadata event;
    status = read(fanotify_fd, &event, sizeof event);

    if (status < sizeof event) {
      perror("Cannot read an event");
      return ERROR_FANOTIFY;
    }

    error_message = "Cannot resolve executable path";
    char *exe_path = deref_pid(event.pid, error_callback);

    if (!error_message) {
      return ERROR_PROC;
    }

    char *exe_filename = strrchr(exe_path, '/');

    if (!exe_filename) {
      perror("Cannot resolve executable path");
      return ERROR_PROC;
    }

    ++exe_filename;

    if (is_in_set(exe_filename, editors)) {
      error_message = "Cannot resolve file path";
      char *file_path = deref_fd(event.fd, error_callback);
      if (!error_message) {
        return ERROR_PROC;
      }

      error_message = "Cannot link file to store";
      link_to_store(file_path, store, error_callback);
      if (!error_message) {
        return ERROR_STORE;
      }

      free(file_path);
    }

    struct fanotify_response response = {
        .fd = event.fd,
        .response = FAN_ALLOW,
    };
    write(fanotify_fd, &response, sizeof response);
    close(event.fd);
    free(exe_path);
  }

  return fanotify_fd;
}
