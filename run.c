//bin/cc "$0" && (./a.out self && rm ./a.out) ; exit

#include "src/menu.h"
#include "unistd.h"

int main(void) {
  MenuList main_menu = MENU_LIST_NULL;
  menu_makelist(&main_menu, "Main");
  menu_appendlist(&main_menu, "Make");
  menu_appendlist(&main_menu, "Something");
  int exit = menu_appendlistn(&main_menu, "Exit");

  MenuState menu_state = {0};
  menu_setup(&menu_state, MF_BKEND);
  menu_print(&menu_state, &main_menu);
  menu_keys(&menu_state, " l", "j", "k", "hq");

  MenuConsole console = {0};
  int status = MENU_CONTINUE;
  while (status != MENU_END) {
    status = menu_input(&menu_state, &main_menu);
    if (status == MENU_NEXT && menu_state.i == exit) {
      static int hitme = 0;
      if (hitme != 0) {
	status = MENU_END;
        break;
      } else {
        // TODO: console add new line tracking and use
        // `menu_appendconnl()` append console new line.
        menu_appendconsole(&console, "you wanna leave?\nHit me again\n");
        hitme++;
      }
    }

    menu_print(&menu_state, &main_menu);
    menu_console(&console);
    usleep(10000);
  }

  menu_freelist(&main_menu);
  menu_freeconsole(&console);
  menu_end();

}
