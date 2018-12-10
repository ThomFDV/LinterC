#include <stdio.h>

typedef struct StringTabs {
    char **tab;
    int maxSize;
    int size;
} StringTabs;

typedef struct Functions {
    char *type;
    char *name;
    int startLine;
    int endLine;
} Functions;

typedef struct Variables {
    char *type;
    char *name;
    Functions *declaredIn;
    int declaredLine;
} Variables;

typedef struct FuncTab {
    Functions **tab;
    int size;
    int maxSize;
} FuncTab;

typedef struct VarTab {
    Variables **tab;
    int size;
    int maxSize;
} VarTab;

typedef struct calledStuff {
    int size;
    int maxSize;
    char **name;
    int *line;
} calledStuff;
