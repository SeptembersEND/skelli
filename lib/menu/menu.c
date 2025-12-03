// TODO: move to utility file
// TODO: exit-error print function and file location
#define EXIT_ERROR(...)                                                        \
  fprintf(stderr, __VA_ARGS__);                                                \
  exit(1);
/**
 * ptr_create - Creates a pointer, checking for errors, and returns it.
 * @size:	Size of pointer required.
 *
 * Takes a size. Using a void pointer and using malloc, creates a pointer the
 * size specified by size variable. Checks if malloc failed, printing an error,
 * and errno, then escaping. Else it returns the new pointer.
 *
 * return: freeable pointer.
 */
void *ptr_create(size_t size) {
  // DEBUG: ("Malloc'd variable of size %lu", size);
  void *ptr = malloc(size);
  if (ptr == NULL) {
    EXIT_ERROR("Unable to allocate variable");
  }

  return ptr;
}
/**
 * ptr_increase - Increase the size of a pointer, by reallocing.
 * @len:	Length in type, for the array.
 * @ptr:	Pointer for the array.
 *
 * Takes the length then increase it by a specified ammount. Takes the pointer,
 * and reallocates the memory used by it, by the new length number. It takes the
 * new pointer from realloc, and test if it worked. Then returns the new length
 * of the array.
 *
 * context:	Use a (void**)&ptr to cast any pointer for this fucntion, and to
 * 		avoid warnings.
 * return: new size of length.
 */
size_t ptr_increase(size_t len, size_t s, void** ptr) {
	// DEBUG: ("Realloc'd variable from %lu to %lu", len, len+len);
	len += (len < 1<<12) ? len : 1<<12;

	void* nptr = realloc(*ptr, (len+1) * s);
	if (nptr == NULL) {
		free(*ptr);
		EXIT_ERROR("realloc failure");
	}
	*ptr = nptr;
	return len;
}

void ptr_free(void** ptr) {
  if ((*ptr) == NULL) {
    // TODO: error
    return;
  }
  free(*ptr);
  (*ptr) = NULL;
}


struct termios orig_termios_structure = {0};
struct termios* orig_termios = NULL;
void menu_ba(char *str, size_t len) {
  // TODO: add to buffer
  fwrite(str, sizeof(char), len, stdout);
}

#if MENU_OS == LINUX
int menu_ossetup_termios() {
  if (tcgetattr(STDIN_FILENO, orig_termios) == 01)
    // TODO: error
    return 1;

  // <https://github.com/snaptoken/kilo-src/blob/master/kilo.c#L144>
  // <https://www.man7.org/linux/man-pages/man3/termios.3.html>
  struct termios new_termios = *orig_termios;

  // ICRNL:	Carriage return to newline
  // INPCK:	Input parity checking
  // ISTRIP:	Strip eighth charactor bit
  // IXON:	XON/XOFF flow control (special charactors)
  new_termios.c_iflag &= ~(INPCK | ISTRIP);
  // CS8:	Charactor Size 8bits
  new_termios.c_cflag |= CS8;
  // ECHO:	echo typed charactors
  // ICANON:	cannon mode
  // ISIG:	interupt signals
  new_termios.c_lflag &= ~(ECHO | ICANON | ISIG);
  // VMIN:	index to minimum number of charactors for noncanonical read
  new_termios.c_cc[VMIN] = 0;
  // VTIME:	time in decaseconds for non-cononical read
  new_termios.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios) == -1)
    // TODO: Error
    return 1;
  return 0;
}

void menu_ossetup() {
  // TODO: append atexit operation
  menu_ossetup_termios();
  menu_ba("\x1b[?25l", 6); // Hide Cursor
  menu_ba("\x1b[H", 3);    // Move Cursor Home
  return;
}

void menu_osend() {
	if (orig_termios == NULL)
		return;
  menu_ba("\x1b[?25h", 6); // Show Cursor
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios) == -1)
    // TODO: do error
    return;
  return;
}

char menu_osgetkey() {
  int nread = 0;
  char c = '\0';
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      return '\0';
    // TODO: Error failed to read
  }

  // TODO: process escape sequences
  return c;
}
#endif

void menu_setup(MenuState *m, unsigned int flags) {
  menu_ossetup();
  if (m != NULL) {
    (*m).flags = flags;
  }
}
void menu_end() { menu_osend(); }

int menu_print(MenuState *state, MenuList *m) {
  menu_ba("\x1b[2J", 4);
  menu_ba("\x1b[2;0H", 7);
  menu_ba("\t", 1);
  menu_ba((*m).title, (*m).titlen);
  menu_ba("\n\n", 2);

  for (int x = 0; x < (*m).len; x++) {
    // TODO: get and use terminal size, store in state.
    // Update state every key input
    char *str = (*m).list[x];
    size_t len = strlen(str);
    menu_ba("\t\t", 2);
    if (x == (*state).i) {
      menu_ba("\x1b[1m", 4);
      menu_ba(str, len);
      menu_ba("\x1b[0m", 4);
    } else {
      menu_ba(str, len);
    }
    menu_ba("\n", 1);
  }

  return 0;
}
void menu_console(MenuConsole *mc) {
	if (mc == NULL) return;
  menu_ba("\n", 1);
  menu_ba((*mc).data, (*mc).len);
}

void menu_freeconsole(MenuConsole *mc) { ptr_free((void **)&((*mc).data)); }
void menu_appendconsole(MenuConsole *mc, char* str) {
  if (mc == NULL) {
    // TODO: error
    return;
  }

  int length = strlen(str);
  if ((*mc).data == NULL) {
    int cap = 8;
    for (; cap < (length + 1);)
      cap *= 2;
    (*mc).cap = cap;
    (*mc).data = ptr_create(sizeof(MenuList) * (*mc).cap + 2);
    memset((*mc).data, 0, (*mc).cap);
  }
  if ((*mc).len >= (*mc).cap + length) {
    (*mc).cap = ptr_increase((*mc).cap, sizeof(MenuList), (void **)&(*mc).data);
  }

  strncat((*mc).data, str, (*mc).cap);
  strncat((*mc).data, "\n", (*mc).cap);
  (*mc).len += length+1;
}

/**
 * menu_freelist - free the base element of a list.
 * m:	Menu variable
 *
 * NOTE: This does not free object within the list, just the memory holding the
 * list.
 */
void menu_freelist(MenuList *m) {
	ptr_free((void**)&((*m).list));
}
void menu_makelist(MenuList *m, char *title) {
  (*m).title = title;
  (*m).titlen = strlen(title);
}
// TODO: make `menu_appendlistn` has condition to check if assumed passed
// index in the list. Dies if isnt.
int menu_nappendlist(MenuList *m, char *str) {
  if (m == NULL) {
    // TODO: error
    return -1;
  }

  if ((*m).list == NULL) {
    (*m).cap = 8;
    (*m).list = ptr_create(sizeof(MenuList) * (*m).cap);
  } else if ((*m).len >= (*m).cap) {
    (*m).cap = ptr_increase((*m).cap, sizeof(MenuList), (void **)&(*m).list);
  }

  (*m).list[(*m).len] = str;
  return (*m).len++;
}
int menu_appendlist(MenuList *m, char *str) {
  return (menu_nappendlist(m, str) == -1) ? 1 : 0;
}

int menu_in2map(char *f[KEY_LEN], char ch) {
  for (int x = 0; x < KEY_LEN; x++) {
    char *k = f[x];
    for (int y = 0; k[y] != '\0'; y++) {
      if (k[y] == ch) {
        return x;
      }
    }
  }
  return MENU_NONE;
}

int menu_input(MenuState *state, MenuList* m) {
  char ch = menu_osgetkey();
  // TODO: return int with enum list of key codes
  int ret = menu_in2map((*state).keys, ch);

  switch (ret) {
  case MENU_SELECT:
    return MENU_NEXT;
    break;
  case MENU_DOWN:
    if ((*state).i < (*m).len-1) {
      (*state).i++;
    }
    break;
  case MENU_UP:
    if ((*state).i > 0) {
      (*state).i--;
    }
    break;
  case MENU_BACK:
    if ((*state).depth == 0 && MFDEC((*state).flags, MF_BKEND) == 0) {
      return MENU_END;
    }
    return MENU_PREV;
  case MENU_QUIT:
    return MENU_END;
  default:
    // TODO: error
  }

  return MENU_CONTINUE;
}

void menu_keys(MenuState *state, char *s, char *d, char *u, char *b, char *q) {
  (*state).keys[0] = s;
  (*state).keys[1] = d;
  (*state).keys[2] = u;
  (*state).keys[3] = b;
  (*state).keys[4] = q;
}
