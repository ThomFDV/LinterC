#include <stdlib.h>
#include <string.h>
#include "functions.h"

FuncTab *initFuncTab(int size) {
    FuncTab *ft = malloc(sizeof(FuncTab*));
    ft->size = 0;
    ft->maxSize = size;
    ft->tab = malloc(sizeof(Functions*) * ft->maxSize);
    return ft;
}

void addFunction(FuncTab *ft, char *type, char *name, int startLine, int endLine) {
    if(ft->size == ft->maxSize) {
        ft->maxSize += 10;
        ft->tab = realloc(ft->tab, sizeof(Functions*) * ft->maxSize);
    }
    ft->tab[ft->size] = malloc(sizeof(Functions));
    if(type != NULL) {
        ft->tab[ft->size]->type = malloc(sizeof(char) * strlen(type));
        strcpy(ft->tab[ft->size]->type, type);
    } else {
        ft->tab[ft->size]->type = NULL;
    }
    ft->tab[ft->size]->name = malloc(sizeof(char) * strlen(name));
    strcpy(ft->tab[ft->size]->name, name);
    ft->tab[ft->size]->startLine = startLine;
    ft->tab[ft->size++]->endLine = endLine;
}

void freeFuncTab(FuncTab *ft) {
    int i;
    for(i = 0; i < ft->size; i++) {
        free(ft->tab[i]->name);
        free(ft->tab[i]->type);
        free(ft->tab[i]);
    }
    free(ft->tab);
    free(ft);
}

VarTab *initVarTab(int size) {
    VarTab *var = malloc(sizeof(VarTab*));
    var->size = 0;
    var->maxSize = size;
    var->tab = malloc(sizeof(Variables*) * var->maxSize);
    return var;
}

void addVariable(VarTab *var, char *type, char *name, Functions *func, int declaredLine) {
    if(var->size == var->maxSize) {
        var->maxSize += 10;
        var->tab = realloc(var->tab, sizeof(Variables*) * var->maxSize);
    }

    var->tab[var->size] = malloc(sizeof(Variables));
    var->tab[var->size]->name = malloc(sizeof(char) * strlen(name));

    if(type != NULL) {
        var->tab[var->size]->type = malloc(sizeof(char) * strlen(type));
        strcpy(var->tab[var->size]->type, type);
    } else {
        var->tab[var->size]->type = NULL;
    }
    strcpy(var->tab[var->size]->name, name);
    var->tab[var->size]->declaredLine = declaredLine;
    var->tab[var->size++]->declaredIn = func;
}

void freeVarTab(VarTab *var) {
    int i;
    printf("Bonjour\n");
    for(i = 0; i < var->size; i++) {
        printf("%d - Step one\n", i + 1);
        free(var->tab[i]->name);
        printf("%d - Step two\n", i + 1);
        if(var->tab[i]->type != NULL) {
            free(var->tab[i]->type);
        }
        printf("%d - Step three\n", i + 1);
        free(var->tab[i]);
        printf("[ %d ] Free is done\n", i + 1);
    }
    free(var->tab);
    free(var);
}

calledStuff *initCalledStuff(int maxSize) {
    calledStuff *cs = malloc(sizeof(calledStuff));
    cs->size = 0;
    cs->maxSize = maxSize;
    cs->name = malloc(sizeof(char*) * cs->maxSize);
    cs->line = malloc(sizeof(int) * cs->maxSize);
    return cs;
}

void addCalledStuff(calledStuff *cs, char* name, int line) {
    if(cs->size == cs->maxSize) {
        cs->maxSize += 10;
        cs->name = realloc(cs->name, sizeof(char*) * cs->maxSize);
        cs->line = realloc(cs->line, sizeof(int) * cs->maxSize);
    }
    cs->name[cs->size] = malloc(sizeof(char) * strlen(name));
    strcpy(cs->name[cs->size], name);
    cs->line[cs->size++] = line;
}

void freeCalledStuff(calledStuff *cs) {
    int i;
    for(i = 0; i < cs->size; i++) {
        free(cs->name[cs->size]);
    }
    free(cs->line);
    free(cs->name);
    free(cs);
}

