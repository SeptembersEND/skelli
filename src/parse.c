#include "jsmn.h"
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define G_VARS_MAX 32
#define G_PD_MAX 32
size_t g_pdlen = 0;
ParsedData g_pd = {
    .pdn = NULL,
    .js = NULL,
    .tok = NULL,
    .vlen = 0,
    .vars = NULL,
};
int g_level = 0;

/* Function realloc_it() is a wrapper function for standard realloc()
 * with one difference - it frees old memory pointer in case of realloc
 * failure. Thus, DO NOT use old data pointer in anyway after call to
 * realloc_it(). If your code has some kind of fallback algorithm if
 * memory can't be re-allocated - use standard realloc() instead.
 */
static inline void *realloc_it(void *ptrmem, size_t size) {
  void *p = realloc(ptrmem, size);
  if (!p) {
    free(ptrmem);
    fprintf(stderr, "realloc(): errno=%d\n", errno);
  }
  return p;
}

// <https://stackoverflow.com/questions/5820810/case-insensitive-string-comparison-in-c>
int strcmp_i(char const *a, char const *b) {
  for (;; a++, b++) {
    int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
    if (d != 0 || !*a)
      return d;
  }
}


const char const* printIt(int type) {
  switch (type) {
    case JSMN_PRIMITIVE: {
      const char const* str = "Primitive";
			return str;
      break;
    }
    case JSMN_STRING: {
      const char const* str = "String";
			return str;
      break;
    }
    case JSMN_OBJECT: {
      const char const* str = "Object";
			return str;
      break;
    }
    case JSMN_ARRAY: {
      const char const* str = "Array";
			return str;
      break;
    }
  }
}

static char *printObj(const char const *js, jsmntok_t *t) {
  static char buf[1024] = {0};
  memset(buf, 0, 1024);
  snprintf(buf, 1023, "%.*s", t->end - t->start, js + t->start);
  return buf;
}
static char* printString(const char* js, jsmntok_t *t) {
  static char* none = "NONE";
  if (t->type != JSMN_STRING) {
    return none;
  }
  return printObj(js, t);
}
static char* indent(int d) {
  static char buf[128] = {0};
  memset(buf, 0, 128);
  memset(buf, '\t', d);
  return buf;
}

static jsmntok_t *printPairStrings(char *name, char *wname, const char *js,
                                   jsmntok_t *nt) {
  assert(nt->type == JSMN_STRING);
  assert(strcmp_i(name, printObj(js, nt)) == 0);
  nt++;
  assert(nt->type == JSMN_STRING);
  if (nt->end == nt->start) {
    printf("%sMenu %-*s:  NONE\n", indent(g_level), 20, wname);
  } else {
    printf("%sMenu %-*s: \"%s\"\n", indent(g_level), 20, wname, printObj(js, nt));
  }
  nt++;
  return nt;
}
static jsmntok_t *printStringArray(const char *js, jsmntok_t *nt) {
  int arrlen = nt->size;
  assert(nt->type == JSMN_ARRAY);
  nt++;
  assert(nt->type == JSMN_STRING);
  if (arrlen <= 0 && nt->end == nt->start) {
    printf(" NONE\n");
    return ++nt;
  }

  for (int x = 0; x < arrlen; x++) {
    assert(nt->type == JSMN_STRING);
    printf("\"%s\" ", printObj(js, nt));
    nt++;
  }

  return nt;
}
static jsmntok_t *printStringArrayPair(char *name, char *wname, const char *js,
                                   jsmntok_t *nt) {
  assert(nt->type == JSMN_STRING);
  assert(strcmp_i(name, printObj(js, nt)) == 0);
  nt++;
  printf("%sMenu %-*s: ", indent(g_level), 20, wname);

  nt = printStringArray(js, nt);

  printf("\n");
  return nt;
}

int checkVars(const char * str) {
  for (int x=0; x<g_pd.vlen && x<G_VARS_MAX; x++) {
    if (strcmp_i(str, g_pd.vars[x]) == 0)
      return x+1;
  }
  return 0;
}

static jsmntok_t* dump_menuitem_script(const char *js, jsmntok_t *nt) {
  g_pd.pdn[g_pdlen].type = PDT_SCRIPT;

  // Name && Description
  nt = printPairStrings("Name", "Script Name", js, nt);
  g_pd.pdn[g_pdlen].name = nt-1;
  nt = printPairStrings("Description", "Script Description", js, nt);
  g_pd.pdn[g_pdlen].description = nt-1;

  // Requirments && Disablers
  g_pd.pdn[g_pdlen].required = nt+1;
  nt = printStringArrayPair("Required", "Requirements", js, nt);
  g_pd.pdn[g_pdlen].disables = nt+1;
  nt = printStringArrayPair("Disables", "Disablers", js, nt);

  // File && Runner && Setter
  nt = printPairStrings("Run", "Commands", js, nt);
  g_pd.pdn[g_pdlen].run = nt-1;
  nt = printPairStrings("Runner", "Script Runner", js, nt);
  g_pd.pdn[g_pdlen].runner = nt-1;
  nt = printPairStrings("Set", "Script Setter", js, nt);
  g_pd.pdn[g_pdlen].set = nt-1;

  g_pd.pdn[g_pdlen].depth = g_level;
  return nt;
}

// TODO: make into function and expand/initalize p_pd.pdn
#define ADD_2PD(A, C, B)                                                       \
  do {                                                                         \
    assert((B)##->type == JSMN_##C);                                           \
    g_pd->pdn.##A = B;                                                         \
  } while (0);

static jsmntok_t* dump_menuitem_command(const char *js, jsmntok_t *nt) {
  g_pd.pdn[g_pdlen].type = PDT_COMMAND;

  // Name && Description
  nt = printPairStrings("Name", "Menu Name", js, nt);
  g_pd.pdn[g_pdlen].name = nt-1;
  nt = printPairStrings("Description", "Menu Description", js, nt);
  g_pd.pdn[g_pdlen].description = nt-1;

  // Requirments && Disablers && Commands
  g_pd.pdn[g_pdlen].required = nt+1;
  nt = printStringArrayPair("Required", "Requirements", js, nt);
  g_pd.pdn[g_pdlen].disables = nt+1;
  nt = printStringArrayPair("Disables", "Disablers", js, nt);
  g_pd.pdn[g_pdlen].run = nt+1;
  nt = printStringArrayPair("Run", "Commands", js, nt);

  // Runner && Setter
  nt = printPairStrings("Runner", "Menu Runner", js, nt);
  g_pd.pdn[g_pdlen].runner = nt-1;
  nt = printPairStrings("Set", "Menu Setter", js, nt);
  g_pd.pdn[g_pdlen].set = nt-1;

  g_pd.pdn[g_pdlen].depth = g_level;
  g_pdlen++;
  return nt;
}
static jsmntok_t* dump_menu(const char *, jsmntok_t *);
static jsmntok_t *dump_menuitem_folder(const char* js, jsmntok_t *nt) {
  g_pd.pdn[g_pdlen].type = PDT_FOLDER;

  nt = printPairStrings("Name", "Folder Name", js, nt);
  g_pd.pdn[g_pdlen].name = nt-1;
  nt = printPairStrings("Description", "Folder Description", js, nt);
  g_pd.pdn[g_pdlen].description = nt-1;

  g_pd.pdn[g_pdlen].required = nt+1;
  nt = printStringArrayPair("Required", "Requirements", js, nt);
  g_pd.pdn[g_pdlen].disables = nt+1;
  nt = printStringArrayPair("Disables", "Disablers", js, nt);

  g_pd.pdn[g_pdlen].depth = g_level;
  g_pdlen++;
  nt = dump_menu(js, nt);
  return nt;
}

static jsmntok_t* dump_menuitem(const char *js, jsmntok_t *nt) {
  assert(nt->type == JSMN_OBJECT);
  int objlen = nt->size;
  nt++;

  // Type
  assert(nt->type == JSMN_STRING);
  assert(strcmp_i("Type", printObj(js, nt)) == 0);
  nt++;

  printf("%s====\n", indent(g_level));
  if (strcmp_i("Command", printObj(js, nt)) == 0) {
    nt++;
    nt = dump_menuitem_command(js, nt);
  } else if (strcmp_i("Script", printObj(js, nt)) == 0) {
    nt++;
    nt = dump_menuitem_script(js, nt);
  } else if (strcmp_i("Folder", printObj(js, nt)) == 0) {
    nt++;
    nt = dump_menuitem_folder(js, nt);
  } else {
    assert("unable to identify menu entry type" && 0);
  }

  return nt;
}
static jsmntok_t* dump_menu(const char *js, jsmntok_t *nt) {
  g_level++;
  assert(nt->type == JSMN_STRING);
  assert(strcmp_i("Menu", printObj(js, nt)) == 0);
  nt++;
  assert(nt->type == JSMN_ARRAY);
  int arrlen = nt->size;
  printf("[DEBUG]: Level %d has %d Menu Items\n", g_level, arrlen);
  nt++;

  for (int x=0; x<arrlen; x++) {
    nt = dump_menuitem(js, nt);
  }
  g_level--;
  return nt;
}

static int json2Data(const char *js, jsmntok_t *t, size_t count, int indent) {
  int i, j, k;
  jsmntok_t *key;
  if (count == 0) {
    return 0;
  }



  // Get 'Format Version'
  jsmntok_t *nt = t;
  assert(nt->type == JSMN_STRING);
  assert(strcmp_i("Format Version", printObj(js, nt)) == 0);
  nt++;
  assert(nt->type == JSMN_STRING);
  char *fvstr = printObj(js, nt);
  float fv = atof(fvstr);
  printf("Format Version: %.2f\n", fv);
  nt++;

  // To menu
  nt = dump_menu(js, nt);

  return 0;
}

static int dump(const char *js, jsmntok_t *t, size_t count, int indent) {
  int i, j, k;
  jsmntok_t *key;
  if (count == 0) {
    return 0;
  }
  if (t->type == JSMN_PRIMITIVE) {
    printf("PRIMITIVE:%.*s", t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_STRING) {
    printf("STRING:'%.*s'", t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_OBJECT) {
    printf("\n");
    j = 0;
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent; k++) {
        printf("  ");
      }
      key = t + 1 + j;
      j += dump(js, key, count - j, indent + 1);
      if (key->size > 0) {
        printf(": ");
        j += dump(js, t + 1 + j, count - j, indent + 1);
      }
      printf("\n");
    }
    return j + 1;
  } else if (t->type == JSMN_ARRAY) {
    j = 0;
    printf("\n");
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent - 1; k++) {
        printf("  ");
      }
      printf("   - ");
      j += dump(js, t + 1 + j, count - j, indent + 1);
      printf("\n");
    }
    return j + 1;
  }
  return 0;
}

int _parseConfig(FILE* file) {
  assert(file != NULL);

  int r;
  int eof_expected = 0;
  char *js = NULL;
  size_t jslen = 0;
  char buf[BUFSIZ];

  jsmn_parser p;
  jsmntok_t *tok;
  size_t tokcount = 2;

  /* Prepare parser */
  jsmn_init(&p);

  /* Allocate some tokens as a start */
  tok = malloc(sizeof(*tok) * tokcount);
  if (tok == NULL) {
    fprintf(stderr, "malloc(): errno=%d\n", errno);
    return 3;
  }

  for (;;) {
    /* Read another chunk */
    r = fread(buf, 1, sizeof(buf), file);
    if (r < 0) {
      fprintf(stderr, "fread(): %d, errno=%d\n", r, errno);
      return 1;
    }
    if (r == 0) {
      if (eof_expected != 0) {
        return 0;
      } else {
        fprintf(stderr, "fread(): unexpected EOF\n");
        return 2;
      }
    }

    js = realloc_it(js, jslen + r + 1);
    if (js == NULL) {
      return 3;
    }
    strncpy(js + jslen, buf, r);
    jslen = jslen + r;

  again:
    r = jsmn_parse(&p, js, jslen, tok, tokcount);
    if (r < 0) {
      if (r == JSMN_ERROR_NOMEM) {
        tokcount = tokcount * 2;
        tok = realloc_it(tok, sizeof(*tok) * tokcount);
        if (tok == NULL) {
          return 3;
        }
        goto again;
      }
    } else {
      //dump(js, tok, p.toknext, 0);
      g_pd.js = js;
      g_pd.tok = tok;
      json2Data(js, tok, p.toknext, 0);
      eof_expected = 1;
    }
  }

  return EXIT_SUCCESS;
}

int checkArrVars_disablers(jsmntok_t *nt) {
  int arrlen = nt->size;
  assert(nt->type == JSMN_ARRAY);
  nt++;
  assert(nt->type == JSMN_STRING);
  if (arrlen <= 0 && nt->end == nt->start) {
    return 0;
  }

  int x=0;
  for (; x < arrlen; x++) {
    assert(nt->type == JSMN_STRING);
    if (checkVars(printObj(g_pd.js, nt)) == 0) {
      return 0;
    }
    nt++;
  }
  return x+1;
}
void add2Vars(char *str) {
  if (checkVars(str) > 0)
    return;
  printf("%s\n", str);
  g_pd.vars[g_pd.vlen++] = strdup(str);
}
void addJS2Vars(jsmntok_t *nt) {
  char* tmp = printObj(g_pd.js, nt);
  checkVars(tmp);
}

ParsedData* parseConfig(FILE* file) {
  g_pd.pdn = malloc(sizeof(ParsedData_Node) * G_PD_MAX);
  memset(g_pd.pdn, 0, sizeof(ParsedData_Node) * G_PD_MAX);
  g_pd.vars = malloc(sizeof(jsmntok_t) * G_VARS_MAX);
  memset(g_pd.vars, 0, sizeof(jsmntok_t) * G_VARS_MAX);


  // TODO: add check for these vars
  add2Vars("BASH");
  add2Vars("PROGRAM_top");

  int r = _parseConfig(file);
  if (r != EXIT_SUCCESS) {
    // TODO: add better error
    fprintf(stderr,
            "parseConfig(): ERROR: unable to parse file correctly. Exited with "
            "(%d)\n", r);
    assert(0 && "parseConfig(): ERROR: unable to parse file correctly.");
    return NULL;
  }

  assert(g_pd.js != NULL);
  assert(g_pd.tok != NULL);

  return &g_pd;
}

// vim: tabstop=2 shiftwidth=2 expandtab
