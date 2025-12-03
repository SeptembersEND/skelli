#ifndef MENU_H
#define MENU_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__linux__)
#define MENU_OS LINUX
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#elif defined(_WIN32) || defined(_WIN64)
#error "Windows not supported"
#else
#error "Linux support only currently"
#endif


/**
 * enum Menu_Status - The menu's status enumerated
 */
enum Menu_Status {
  MENU_END = -1,	///< Exit Menu Entirely
  MENU_CONTINUE = 0,	///< Continue
  MENU_NEXT = 1,	///< Got to next menu / Select current item
  MENU_PREV = 2,	///< Got to previous menu / Back
};

static const int KEY_LEN = 5;
typedef char *MenuKeys[5];
/**
 * enum Menu_Inputs - The menu control inputs enumerated
 */
enum Menu_Inputs {
  MENU_NONE = -1,
  MENU_SELECT = 0,
  MENU_DOWN = 1,
  MENU_UP = 2,
  MENU_BACK = 3,
  // TODO: add flag disable quit
  MENU_QUIT = 4,
};

#define MFDEC(F,MF)	(F & MF)
enum Menu_Flags {
	MF_BKEND = 0b0001,
};

#define MENU_LIST_NULL {0,0,0,NULL,NULL}
typedef struct {
  size_t len;
  size_t cap;
  size_t titlen;
  char *title; // TODO: have appending to make title a path
  char **list;
} MenuList;

/**
 * typedef MenuState - Variable that holds the state of the menu.
 */
typedef struct {
  size_t depth;  ///< Menu Depth
  size_t i;      ///< Index of menu
  unsigned int flags;
  MenuKeys keys; ///< Keys for menu functions
  MenuList* list;
} MenuState;

typedef struct {
  size_t len;
  size_t cap;
  char *data;
} MenuConsole;

#include "menu.c"

#endif
