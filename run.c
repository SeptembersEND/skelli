//usr/bin/env cc "$0" -o stage1 && (./stage1 $@ ; rm ./stage1); exit
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define STAGE2_SRC "src/main.c"

#define NAME "stage1"
#define NAME_LEN 6

#define ASSERT(expr)                                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Failed: %s, file %s, line %d\n", #expr, __FILE__,       \
              __LINE__);                                                       \
      end();                                                                   \
      exit(1);                                                                 \
    }                                                                          \
  } while (0);

enum Type {
  T_NONE,
  T_RUN,
  T_MAKE,
  T_DOWN,
  T_CLEAN,
};

void (*end_ptr)(void) = NULL;
static inline void end(void) {
  if (end_ptr != NULL) {
    end_ptr();
  }
  return;
}

#include "lib/platform/platform.h"

// <https://stackoverflow.com/questions/5820810/case-insensitive-string-comparison-in-c>
int arg_strcmp_i(char const *a, char const *b) {
  if (*a == *b && a[1] == '\0') {
    return 0;
  }
  for (;; a++, b++) {
    int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
    if (d != 0 || !*a)
      return d;
  }
}

enum Type find_type(char *arg) {
  if (arg_strcmp_i(arg, "run") == 0) {
    return T_RUN;
  } else if (arg_strcmp_i(arg, "make") == 0) {
    return T_MAKE;
  } else if (arg_strcmp_i(arg, "download") == 0) {
    return T_DOWN;
  } else if (arg_strcmp_i(arg, "clean") == 0) {
    return T_CLEAN;
  }
  return T_NONE;
}

void exec_type(enum Type type) {
  switch (type) {
  case T_RUN:
    exec_type(T_DOWN);
    exec_type(T_MAKE);
    return;
  case T_MAKE:
    CHECK(STAGE2_SRC);
    EXEC(CC, STAGE2_SRC, "-o", "stage2");
    return;
  case T_DOWN:
    CHECK(".git");
    //DOWNLOAD("", "");
    EXEC("git", "submodule", "update", "--init", "--recursive");
    return;
  case T_CLEAN:
    (void)remove("stage1");
    (void)remove("stage2");
    return;
  default:
    ASSERT(1);
    return;
  }
}

void help(void) {
  fputs("Usage: " NAME " MODE\n\n"
        "Modes:\n"
        "all are case insensitive.\n"
        "  run, r\tDownload, Make, and Run Stage2 binary\n"
        "  make, m\tCompile Stage2 binary\n"
        "  download, down, d\n"
        "\t\tDownload dependencies for Stage2 binary\n"
        "  clean, c\tRemove all downloads and binaries\n",
        stdout);
}

int main(int argc, char *argv[]) {
  end_ptr = help;
  ASSERT(argc > 1);
  ASSERT(argc < 3);

  enum Type type = find_type(argv[1]);
  ASSERT(type != T_NONE);

  exec_type(type);

  return 0;
}
