#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"
#include "tokenstack.h"
#include "vartable.h"

#define VAR_CAP 1024

struct VarTable {
	Var data[VAR_CAP];
	Arena *arena;
	short size;
};

VarTable *VarTable_new() {
	Arena *newArena = Arena_new_dynamic(2 * sizeof(VarTable));
	VarTable *newTable = Arena_alloc(newArena, sizeof(VarTable));

	memset(newTable->data, 0, VAR_CAP * sizeof(Var));
	newTable->arena = newArena;
	newTable->size = 0;

	return newTable;
}

void VarTable_delete(VarTable *table) {
	if (table != NULL) Arena_delete(table->arena);
}

static short VarTable_hash(char *name) {
	size_t hash = 0;
	for (size_t i = 1; *name != '\0'; i++) hash += *name++ * i;

	return hash % VAR_CAP;
}

Var *VarTable_insert(VarTable *table, char *name) {
	if (table->size == VAR_CAP) {
		fprintf(stderr, "You've allocated too many variables!\n");
		exit(1);
	}

	short index = VarTable_hash(name);
	while (table->data[index].name != NULL) {
		index++;
		index %= VAR_CAP;
	}

	table->data[index].name = name;
	table->data[index].data = 0;

	table->size++;
	return &table->data[index];
}

Var *VarTable_search(VarTable *table, char *name) {
	short index = VarTable_hash(name);
	for (short i = 0; i < VAR_CAP && table->data[index].name != NULL; i++) {
		if (strcmp(table->data[index].name, name) == 0) return &table->data[index];
		else {
			index++;
			index %= VAR_CAP;
		}
	}

	return NULL;
}

Arena *VarTable_getArena(VarTable *table) {
	return table->arena;
}
