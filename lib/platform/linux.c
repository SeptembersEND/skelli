int g_platform_linux_error_errno = 0;
char *g_platform_linux_error_msg = NULL;
void platform_linux_error(void) {
  if (g_platform_linux_error_msg == NULL || g_platform_linux_error_errno == 0) {
    fprintf(stderr, _ERROR_MSG
            ": An unknown error has occured in `lib/platform/linux`\n");
    return;
  }

  if (g_platform_linux_error_errno == 1) { /* 01: Means string does not exist */
    fprintf(stderr, _ERROR_MSG ": Does not exist: `%s`\n",
            g_platform_linux_error_msg);
  } else if (g_platform_linux_error_errno == 2) { /* 02: Errno error */
    fprintf(stderr, _ERROR_MSG ": %s: %s\n", g_platform_linux_error_msg,
            strerror(errno));
  }

  g_platform_linux_error_errno = 0;
  g_platform_linux_error_msg = NULL;
}

void set_error(char *msg, int num) {
  g_platform_linux_error_msg = msg;
  g_platform_linux_error_errno = num;
}
void end_error(void) {
  end_ptr = NULL;
  g_platform_linux_error_errno = 0;
  g_platform_linux_error_msg = NULL;
}

#define ERROR platform_linux_error
#define SET_ERROR(STR, NUM)                                                    \
  ERROR;                                                                       \
  set_error(STR, NUM);

int is_file(const char *path) {
  struct stat buf;
  stat(path, &buf);
  return S_ISREG(buf.st_mode);
}

// <https://stackoverflow.com/questions/12765010/c-test-for-file-existence-before-calling-execvp>
int is_executable_in_path(char *name) {
  if (is_file(name) && !(access(name, F_OK) || access(name, X_OK))) {
    return -1;
  }

  char *path = getenv("PATH");
  char *item = NULL;
  int found = 0;

  if (!path)
    return 0;
  path = strdup(path);

  char real_path[4096]; // or PATH_MAX or something smarter
  for (item = strtok(path, ":"); (!found) && item; item = strtok(NULL, ":")) {
    sprintf(real_path, "%s/%s", item, name);
    if (is_file(real_path) &&
        !(access(real_path, F_OK) ||
          access(real_path,
                 X_OK))) // check if the file exists and is executable
    {
      found = 1;
    }
  }

  free(path);
  return found;
}

#define EXISTS(NAME) is_executable_in_path(NAME)

int execute(char **args) {
  end_ptr = SET_ERROR(args[0], 1);
  int exist = EXISTS(args[0]);
  ASSERT(exist != 0);
  end_error();

  do {
    for (int x = 0; args[x] != NULL; x++) {
      printf("`%s` ", args[x]);
    }
    printf("\n");
  } while (0);

  pid_t pid = fork();
  if (pid == 0) {
    int ret = execvp(args[0], args);
    ASSERT(ret == -1);
    exit(0);
  } else if (pid < 0) {
    fputs("`execvp()` failed to run\n", stderr);
    return -1;
  } else {
    int status;
    pid_t wpid = waitpid(pid, &status, 0);
    if (!WIFEXITED(status)) {
      fprintf(stderr, "`execvp()`: pid %d exited abnormally (%d)\n", pid,
              WEXITSTATUS(status));
    }
  }
  return 0;
}

#define EXEC(...) execute((char *[]){__VA_ARGS__, NULL})

#define DOWNLOAD(SAVE, SOURCE)                                                 \
  execute((char *[]){CMD_DOWNLOAD, SAVE, SOURCE, NULL})

void must_exist(const char* path) {
  end_ptr = SET_ERROR((char*)path, 1);
  int exist = access(path, F_OK);
  ASSERT(exist == 0);
  end_error();
}

#define CHECK(PATH) must_exist(PATH);
#undef ERROR
