#ifndef VARTABLE_H
#define VARTABLE_H

#include "tokenstack.h"

struct VarTable;
typedef struct VarTable VarTable;

extern VarTable *VarTable_new();
extern void VarTable_delete(VarTable *table);
extern Var *VarTable_insert(VarTable *table, char *name);
extern bool VarTable_remove(VarTable *table, Var *item);
extern Var *VarTable_search(VarTable *table, char *name);

#endif
