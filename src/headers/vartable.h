#ifndef VARTABLE_H
#define VARTABLE_H

#include "arena.h"
#include "tokenstack.h"

struct VarTable;
typedef struct VarTable VarTable;

extern VarTable *VarTable_new();
extern void VarTable_delete(VarTable *table);
extern Var *VarTable_insert(VarTable *table, char *name);
extern Var *VarTable_search(VarTable *table, char *name);
extern Arena *VarTable_getArena(VarTable *table);
extern void Var_delete(Var *item);

#endif
