#include "../lib/menu/menu.h"
#include "parse.h"

void runMenu(ParsedData *data, MenuState *ms, MenuList (*ml)[64],
             unsigned int *mi, MenuList** mml, int *iaddr) {
  ParsedData_Node* pdn = &(data->pdn[ms->i]);
  jsmntok_t *run = pdn->run;

  if (pdn->type == PDT_FOLDER) {
    (*mi)++;
    (*mml) = &((*ml)[(*mi)]);
    (*iaddr) += ms->i+1;
    ms->i = 0;
    return;
  }
  assert(0);
  if (pdn->type != PDT_COMMAND) {
    return;
  }
  if (strcmp_i("bash", printObj(data->js, pdn->runner)) != 0) {
    return;
  }

  int arrlen = run->size;
  assert(run->type == JSMN_ARRAY);
  run++;
  assert(run->type == JSMN_STRING);
  if (arrlen <= 0 && run->end == run->start) {
    return;
  }

  assert(run->type == JSMN_STRING);
  char* cmd = printObj(data->js, run);
  printf("\"%s\" ", cmd);

  system(cmd);
}

void menuDescription(ParsedData *data, MenuState *ms, MenuConsole *mc,
                     int *iaddr) {
  static size_t cur = ~0;
  int index = ms->i + (*iaddr);
  if (index == cur)
    return;
  if (data->pdn[index].description == NULL || data->pdn[index].name == NULL ||
      data->pdn[index].runner == NULL || data->pdn[index].required == NULL) {
    menu_clearconsole(mc);
    return;
  }

  char buf[128 * 4] = {0};
  char buf1[128] = {0};
  char buf2[128] = {0};
  char buf3[128] = {0};
  memset(buf, 0, 128 * 4);
  memset(buf1, 0, 128);
  memset(buf2, 0, 128);
  memset(buf3, 0, 128);

  snprintf(buf1, 127, "(%2d) Name:   %s", data->pdn[index].depth,
           printObj(data->js, data->pdn[index].name));
  snprintf(buf2, 127, "Runner:      %s",
           printObj(data->js, data->pdn[index].runner));
  snprintf(buf3, 127, "Required:    %s",
           printObj(data->js, data->pdn[index].required));
  snprintf(buf, (128*4)-1, "%s\nDescription: %s\n%s\n%s\n", buf1,
           printObj(data->js, data->pdn[index].description), buf2, buf3);

  menu_clearconsole(mc);
  menu_appendconsole(mc, buf);
  cur = index;
}

void menu(ParsedData* data) {
  int menu_list_len = 0;
  // NOTE: the size of this array is fixed.
  //MenuList **menu_list = malloc(sizeof(MenuList) * 64);
  MenuList menu_list[64] = {0};
  MenuList* main_menu = &(menu_list[menu_list_len++]);

  int ml = 1;
  menu_makelist(main_menu, "Commands");
  // NOTE: the size of this array is fixed.
  int menu_status[64] = {0};

  int actual_len = 0;
  int len = 0;
  for (; (data->pdn[len].name) != NULL && len<G_PD_MAX; len++) {
    assert(data->vars != NULL);
    int dis = checkArrVars_disablers(data->pdn[len].required);

    if (menu_list[ml-1].title == NULL) {
      menu_makelist(&(menu_list[ml-1]), "Commands");
    }
    ml = data->pdn[len].depth;

    char *bn = printObj(data->js, data->pdn[len].name);
    char bnn[64] = {0};
    char *unsup = "\x1b[33mUNSUPPORTED\x1b[0m ";
    char *folder = "> ";
    char *en[2] = {"", ""};

    if (dis == 0) {
      en[0] = unsup;
      menu_status[len] = -1;
    }

    if (data->pdn[len].type == PDT_FOLDER) {
      en[1] = folder;
    }

    snprintf(bnn, 64, "%s%s%s", en[0], en[1], bn);

    // TODO: make sure to be able to free this!
    char* name = strdup(bnn);
    menu_appendlist(&(menu_list[ml-1]), name);
  }

  MenuState menu_state = {0};
  menu_setup(&menu_state, 0);
  menu_print(&menu_state, main_menu);
  menu_keys(&menu_state, " l", "j", "k", "h", "");

  MenuConsole console = {0};

  // TODO: move this to another function that recurres
  int mi = 0, iaddr = 0;
  int status = MENU_CONTINUE;
  while (status != MENU_END) {
    status = menu_input(&menu_state, main_menu);

    if (status == MENU_NEXT && menu_status[menu_state.i + iaddr] == 0) {
      runMenu(data, &menu_state, &menu_list, &mi, &main_menu, &iaddr);
    }

    menuDescription(data, &menu_state, &console, &iaddr);
    menu_print(&menu_state, main_menu);
    menu_console(&console);
    usleep(50000);
  }

  for (int x=0; x<menu_list_len; x++) {
    menu_freelist(&(menu_list[x]));
  }
  //free(menu_list);
  menu_end();
}

void menuParse() {
	FILE* file = fopen("config.json", "r");
	ParsedData* g_pd = parseConfig(file);
	fclose(file);

  menu(g_pd);
  return;

  printf("%2d Vars: ", g_pd->vlen);
  for (int x=0; x<g_pd->vlen; x++) {
    printf("\"%s\" ", g_pd->vars[x]);
  }
  printf("\n");

  for (int x=0; (g_pd->pdn[x].name) != NULL && x<G_PD_MAX; x++) {
    // Check required and disables
    // TODO: add better solution than manual lookup/check of each.
    assert(g_pd->vars != NULL);
    int dis = checkArrVars_disablers(g_pd->pdn[x].required);

    //if (dis == 0) continue; // remove unsupported

    printf("\n");
    printf("%2d: `%s`\t", g_pd->pdn[x].type,
           printObj(g_pd->js, g_pd->pdn[x].name));
    if (dis == 0) {
      printf("\x1b[33mUNSUPPORTED\x1b[0m ");
      continue;
    }

    printStringArray(g_pd->js, g_pd->pdn[x].required);
    printf("%s\t", printObj(g_pd->js, g_pd->pdn[x].description));

    if (g_pd->pdn[x].type == PDT_FOLDER) {
      continue;
    }
  }
  printf("\n");
}

void startMenu() {
  MenuList main_menu = MENU_LIST_NULL;
  menu_makelist(&main_menu, "Main");
  menu_appendlist(&main_menu, "Start");
  menu_appendlist(&main_menu, "End");

  MenuState menu_state = {0};
  menu_setup(&menu_state, 0);
  menu_print(&menu_state, &main_menu);
  menu_keys(&menu_state, " l", "j", "k", "h", "q\e");

  int status = MENU_CONTINUE;
  while (status != MENU_END) {
    status = menu_input(&menu_state, &main_menu);

    if (status == MENU_NEXT && menu_state.i == 0) {
      menuParse();
      break;
    }

    menu_print(&menu_state, &main_menu);
    usleep(50000);
  }

  menu_freelist(&main_menu);
  menu_end();
}

int main(void) {
  menuParse();
	//startMenu();
  return 0;
}
// vim: tabstop=2 shiftwidth=2 expandtab
