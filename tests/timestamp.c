#include "timestamp.h"
#include "messages.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main() {
  struct trace *trace = create_trace();
  char *timestamp = get_timestamp("abc", 7, trace);
  assert(ok(trace));
  assert(!strcmp(timestamp, "abc"));
  free(timestamp);

  timestamp = get_timestamp("abc", 3, trace);
  assert(ok(trace));
  assert(!strcmp(timestamp, "abc"));
  free(timestamp);

  timestamp = get_timestamp("%C", 2, trace);
  assert(ok(trace));
  assert(strlen(timestamp) == 2);
  free(timestamp);

  timestamp = get_timestamp("%Y%m%d%H%M%S", 14, trace);
  assert(ok(trace));
  assert(strlen(timestamp) == 14);
  free(timestamp);

  timestamp = get_timestamp("%Y", 3, trace);
  assert(!ok(trace));
  assert(get_trace_message(trace) == messages.timestamp.overflow);
  clear(trace);
  free(trace);
  free(timestamp);
}
