#include "../lib/menu/menu.h"
#include "jsmn.h"

void test(void) {
	jsmn_parser parser;
	jsmntok_t tokens[10];
	jsmn_init(&parser);
}

int main(void) {
  MenuList main_menu = MENU_LIST_NULL;
  menu_makelist(&main_menu, "Main");
  menu_appendlist(&main_menu, "Test");
  menu_appendlist(&main_menu, "Second");
  menu_appendlist(&main_menu, "End");

  MenuState menu_state = {0};
  menu_setup(&menu_state, 0);
  menu_print(&menu_state, &main_menu);
  menu_keys(&menu_state, " l", "j", "k", "h", "q\e");

  int status = MENU_CONTINUE;
  while (status != MENU_END) {
    status = menu_input(&menu_state, &main_menu);
    menu_print(&menu_state, &main_menu);
    usleep(50000);
  }

  menu_freelist(&main_menu);
  menu_end();
  return 0;
}
