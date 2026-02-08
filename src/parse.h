#ifndef SRC_PARSE__H
#define SRC_PARSE__H

#include "jsmn.h"

enum ParsedData_Type {
  PDT_COMMAND, PDT_SCRIPT, PDT_FOLDER
};

/**
 *  [ A ] Applies to all
 *  [CSF] From left to right, if present, indicates that it applies to Commands,
 *    Scripts, or Folders. If not present, needs to be NULL.
 *
 *  (STR) Requires a string
 *  (ARR) Requires a array
 */
typedef struct {
  enum ParsedData_Type type; ///< [ A ]: The type of Node
  jsmntok_t *name;           ///< [ A ]: (STR) Name of the node
  jsmntok_t *description;    ///< [ A ]: (STR) Description of the node
  jsmntok_t *required;       ///< [ A ]: (ARR) Required variables to run
  jsmntok_t *disables;       ///< [ A ]: (ARR) Varibles that if present disable.
  jsmntok_t *run;            ///< [CS ]: (STR|ARR) Holds the body of the node.
  // TODO: implement runner as an enum
  jsmntok_t *runner;         ///< [CS ]: (STR) What is used to process the run
  jsmntok_t *set;            ///< [CS ]: (STR) Sets a variable by name
  // TODO: implement "menu" for folders
  // Maybe add pointer to the next ParsedData_Node to bypass the need to walk
  // its child nodes.
  unsigned int depth;
} ParsedData_Node;

typedef struct {
  ParsedData_Node *pdn;
  char const *js;
  jsmntok_t *tok;

  size_t vlen;
  char **vars;
} ParsedData;
// TODO: add way to make a hierarchy

#include "parse.c"

#endif
// vim: tabstop=2 shiftwidth=2 expandtab
