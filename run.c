//bin/cc "$0" -o stage1 && (./stage1 $@ && rm ./stage1) ; exit
//TODO: move the current directory to script execution location

#include "lib/menu/menu.h"
#include "unistd.h"
#include <ctype.h>
#include <string.h>

#include <ftw.h>
#include <sys/stat.h>
#include <sys/wait.h>

// TODO: move to a lib file
// TODO: make OS agnostic
/**
 * get_fileModifyTime - Get the time a file was modified.
 * @path: String, holding the path to the desired file.
 *
 * return: -1 if file does not exist, else modified time.
 */
ssize_t get_fileModifyTime(const char *path) {
  struct stat statbuf = {0};
  if (stat(path, &statbuf) < 0) {
    fprintf(stderr, "[ERROR] Cannot find file (%s): %s\n", path,
            strerror(errno));
    return -1;
  }
  return statbuf.st_mtime;
}

int makedir(char* dir) {
  struct stat st = {0};
  if (stat(dir, &st) == -1) {
    mkdir(dir, 0755);
  }
}

// TODO: rewrite
int run_cmd(MenuConsole *con, char **args) {
  pid_t pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      // TODO: Make function
      char e1_buf[128] = {0};
      snprintf(e1_buf, 127, "Unable to execute child: %s", strerror(errno));
      menu_appendconsole(con, e1_buf);
      exit(1);
    }
    exit(0);
  } else if (pid < 0) {
    menu_appendconsole(con, "Unable to fork child.");
    return -2;
  } else {
    char buf[128] = {0};
    strncpy(buf, "+ ", 127);
    for (int x=0; args[x]!=NULL; x++) {
      strncat(buf, args[x], 127);
      strncat(buf, " ", 127);
    }
    menu_appendconsole(con, buf);
    int status;
    pid_t wpid;

    do {
      wpid = waitpid(pid, &status, 0);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    status = WEXITSTATUS(status);
    return status;
  }
}

ssize_t pathtime = -1;
int checktime(const char *fpath, const struct stat *sb, int typeflag) {
  if ((*sb).st_mtime >= pathtime) {
    return 1;
  }
  return 0;
}
/**
 * confileupdir - Is file older than all files in directory, report using console.
 * return:
 * 	-2	path does not exist
 * 	-1	FTW error
 * 	 0	all files in dir older than path
 * 	 1	a file in dir is older than path
 */
// <https://github.com/brechtsanders/libdirtrav>
// <https://linux.die.net/man/3/nftw>
int confileupdir(MenuConsole *con, const char* dir, const char* path) {
  pathtime = get_fileModifyTime(path);
  if (pathtime == -1) {
    menu_appendconsole(con, "[RUN] Unable to find file.");
    return -2;
  }
  int result = 0;
  result = ftw(dir, checktime, 0);
  return result;
}

// <https://stackoverflow.com/questions/5820810/case-insensitive-string-comparison-in-c>
int strcmp_i(char const *a, char const *b) {
  for (;; a++, b++) {
    int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
    if (d != 0 || !*a)
      return d;
  }
}

#define S2PATH	"./stage2"
char *const s2path = S2PATH;
/**
 * check - checks if stage2 needs updating or even exists
 *
 * return:
 * 	 0	no need to update
 * 	 1	confileupdir did not find updates or failed
 *	 2	library does not exist
 */
int check(MenuConsole *con) {
	// Check if directory is updated, or if s2path exists
	int ret = 0;
  if ((ret = confileupdir(con, "src", s2path)) == 1) {
    return 0;
  }
  return 1;
}
#define CC	"gcc"
#define BUILD_DIR	"./build"
#define SRC	"./src"
#define LIB	"./lib"
#define	CMD(...)	(char*[]){__VA_ARGS__, NULL}
void make(MenuConsole *con) {
  // TODO: seperate return for missing library
	if (get_fileModifyTime(LIB"/jsmn/jsmn.h") == -1) {
		menu_appendconsole(con, "[MAKE] Missing JSMN library");
		return;
	}

	// Compile stage2
  makedir(BUILD_DIR);
  run_cmd(con, CMD(CC, "-I", "lib/jsmn/", "-c", "-o", BUILD_DIR"/stage2.o", SRC"/main.c"));
  run_cmd(con, CMD(CC, "-c", "-o", BUILD_DIR"/jsmn.o", LIB"/jsmn/jsmn.h"));
  run_cmd(con, CMD(CC, BUILD_DIR"/stage2.o", "-L", BUILD_DIR"/jsmn.o", "-o", S2PATH));
}
void run(MenuConsole *con) {
  int ch = check(con);
  if (ch == 0) {
    make(con);
  }

  char *const argv[] = {s2path, NULL};
  menu_appendconsole(con, "[RUN] Starting stage2");
  menu_console(con);
  fflush(stdout);
  sleep(1);
  menu_end();
  if (execvp(argv[0], argv) != 0) {
    printf("[RUN] Failed to run `execvp()`: %s", strerror(errno));
  }
  exit(0);
}
void download(MenuConsole *con) {
#if MENU_OS == LINUX
	menu_appendconsole(con, "[DOWN] Retreiving submodules");
	run_cmd(con, CMD("git", "submodule", "update", "--init", "--recursive"));
#else
	menu_appendconsole(con, "[DOWN] Retreiving submodules for this OS is not supported");
#endif
}

void checkanrun(char* str) {
  if (strcmp_i(str, "run") == 0) {
	  printf("keyword `%s` used\n", str);
	  run(NULL);
	  exit(0);
  }

  if (strcmp_i(str, "make") == 0) {
	  printf("keyword `%s` used\n", str);
	  make(NULL);
	  exit(0);
  }
}

enum {
  MB_RUN = 0,
  MB_MAKE = 1,
  MB_DOWN = 2,
  MB_CLEAN = 3,
  MB_EXIT = 4,
};
void menuL1input(int index, MenuConsole *con, int *status) {
  static int hitme = 0;

  switch (index) {
  case MB_RUN:
	  run(con);
	  break;
  case MB_MAKE:
	  make(con);
	  break;
	case MB_DOWN:
		download(con);
		break;
	case MB_CLEAN:
		break;
  case MB_EXIT:
    if (hitme != 0) {
      (*status) = MENU_END;
    } else {
      // TODO: console add new line tracking and use
      // `menu_appendconnl()` append console new line.
      menu_appendconsole(con, "you wanna leave?\nHit me again\n");
      hitme++;
    }
    return;
  }

  hitme = 0;
}



int main(int argc, char* argv[]) {
  if (argc >= 2) {
    checkanrun(argv[1]);
  }

  MenuList main_menu = MENU_LIST_NULL;
  menu_makelist(&main_menu, "Main");
  menu_appendlist(&main_menu, "Run\tStart Second Stage. Compile if not.");
  menu_appendlist(&main_menu, "Make\tCompile Second Stage.");
  menu_appendlist(&main_menu, "Download");
  menu_appendlist(&main_menu, "Clean");
  menu_appendlist(&main_menu, "Exit");

  MenuState menu_state = {0};
  menu_setup(&menu_state, MF_BKEND);
  menu_print(&menu_state, &main_menu);
  menu_keys(&menu_state, " l\n", "j", "k", "h", "q\e");

  MenuConsole console = {0};

  // TODO: make utils add print all args
  int status = MENU_CONTINUE;
  while (status != MENU_END) {
    status = menu_input(&menu_state, &main_menu);
    if (status == MENU_NEXT) {
      menuL1input(menu_state.i, &console, &status);
    }

    menu_print(&menu_state, &main_menu);
    menu_console(&console);
    usleep(50000);
  }

  menu_freelist(&main_menu);
  menu_freeconsole(&console);
  menu_end();
  return 0;
}
// set shiftwidth=2 tabstop=2
