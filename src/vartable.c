#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokenstack.h"
#include "vartable.h"

#define VAR_CAP 1024

struct VarTable {
	Var data[VAR_CAP];
	short size;
};

VarTable *VarTable_new() {
	VarTable *newTable = malloc(sizeof(VarTable));
	memset(newTable->data, 0, VAR_CAP * sizeof(Var));
	newTable->size = 0;

	return newTable;
}

void VarTable_delete(VarTable *table) {
	if (table != NULL) {
		for (short i = 0; i < VAR_CAP; i++) free(table->data[i].name);
		free(table);
	}
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

	table->data[index] = (Var){
		.name = name,
		.data = 0
	};

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

void Var_delete(Var *item) {
	if (item != NULL) {
		free(item->name);
		*item = (Var){0};
	}
}
