#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>

#include "functions.h"

// Fonction principale de recupération de configuration
int parseConf(StringTabs *rules, StringTabs *excludedFiles, char *filename) {
    FILE *f = fopen(filename, "r");
    char *key;
    int c;
    int i;
    int write = 0;
    char *ext;
    int rec;
    if(f == NULL) {
        printf("\n Rate, pas de fichier %s\n", filename);
        return -1;
    } else {
        printf("Le fichier %s a bien ete ouvert\n", filename);
    }

    key = malloc(sizeof(char) * 15);

    while((c = fgetc(f)) != EOF) {
        if(c == '=') {
            if((c = fgetc(f)) != ' ') {
                while(c != '\n') {
                    key[write++] = c;
                    c = fgetc(f);
                }
                key[write] = '\0';
                printf("------%s------\n", key);
                write = 0;
                if(!strcmp(key, "extends")) {
                    ext = parseExtends(f);
                    if(ext == NULL) {
                        printf("Pas d'extends\n", ext);
                    } else {
                        printf("Ouverture du fichier %s\n", ext);
                        rec = parseConf(rules, excludedFiles, ext);
                        system("pause");
                    }
                    printf("\n");
                } else if(!strcmp(key, "rules")) {
                    parseRules(f, rules);
                    for(i = 0; i < rules->size; i++) {
                        printf("%s\n", rules->tab[i]);
                    }
                } else if(!strcmp(key, "excludedFiles")) {
                    parseExcludedFiles(f, excludedFiles);
                    for(i = 0; i < excludedFiles->size; i++) {
                        printf("%s\n", excludedFiles->tab[i]);
                    }
                } else if(!strcmp(key, "recursive")) {
                    rec = parseRecursive(f);
                    printf("\n");
                } else {
                    printf("[ERR] Configuration file is not correctly implemented !\n");
                    return -1;
                }
            }
        }
    }

    free(key);
    if(fclose(f) == 0) {
        printf("Le fichier a bien ete ferme\n");
    } else {
        printf("Fichier encore ouvert, probleme\n");
    }

    system("pause");

    return rec;
}

// Parse les règles dans le fichier de configuration
void parseRules(FILE *f, StringTabs *rules) {
    char c;
    char *rule = malloc(sizeof(char) * 30);
    int endOfKey = 0; // Signale quand on arrive à la fin des règles
    int countKeyCol = 0; // Remplissage de la chaine de caractère contenant la règle

    while(endOfKey == 0) {
        if((c = fgetc(f)) == '-') {
            fseek(f, 1, SEEK_CUR);
            while((c = fgetc(f)) != ' ') {
                rule[countKeyCol++] = c;
            }
            fseek(f, 2, SEEK_CUR);
            rule[countKeyCol++] = ' ';
            while((c = fgetc(f)) != '\n') {
                rule[countKeyCol++] = c;
            }
            rule[countKeyCol] = '\0';
            modifyRules(rule, rules);
            countKeyCol = 0;
        } else if(c == '\n') {
            endOfKey = 1;
        }
    }
    free(rule);
}

// Parse les fichiers exclus de l'analyse de LinterC
void parseExcludedFiles(FILE *f, StringTabs *excludedFiles) {
    char c;
    char *excluded = malloc(sizeof(char) * 30);
    int endOfKey = 0; // Signale quand on arrive à la fin des règles
    int countKeyCol = 0; // Remplissage de la chaine de caractère contenant la règle

    while(endOfKey == 0) {
        if((c = fgetc(f)) == '-') {
            fseek(f, 1, SEEK_CUR);
            while((c = fgetc(f)) != '\n') {
                excluded[countKeyCol++] = c;
            }
            excluded[countKeyCol] = '\0';
            addExcludedFile(excluded, excludedFiles);
            countKeyCol = 0;
        } else if(c == '\n') {
            endOfKey = 1;
        }
    }
    free(excluded);
}

// Charge la configuration du  fichier dont découle le fichier initial
char *parseExtends(FILE* f) {
    char *res;
    char c;
    int i = 0;

    res = malloc(sizeof(char) * 30);

    while((c = fgetc(f)) != '\n') {
        res[i++] = c;
    }
    res[i] = '\0';
    if(!strcmp(res, "NONE")) {
        free(res);
        return NULL;
    }
    return res;
}

// renvoie 1 si recursif, 0 si ne l'est pas
int parseRecursive(FILE* f) {
    char* res;
    char c;
    int i = 0;

    res = malloc(sizeof(char) * 30);

    while((c = fgetc(f)) != '\n') {
        res[i++] = c;
    }
    res[i] = '\0';
    printf("%s", res);
    if(!strcmp(res, "false")) {
        free(res);
        return 0;
    }
    free(res);
    return 1;
}

// Ajoute la règle dans le tableau de règles, ou modifie la valeur si elle est déjà existante
void modifyRules(char *rule, StringTabs *rules) {
    int i;
    int j;
    int found = -1;
    for(i = 0; i < rules->size; i++) {
        j = 0;
        while(rules->tab[i][j] == rule[j]) {
            if(rule[j] == ' ') {
                found = i;
                break;
            }
            j += 1;
        }
    }
    if(found == -1) {
        addString(rules, rule);
        printf("La regle a ete ajoutee\n");
    } else {
        strcpy(rules->tab[found], rule);
        printf("La regle a ete modifiee\n");
    }
}

// Ajoute le fichier à la liste des fichiers à ne pas analyser
void addExcludedFile(char *excluded, StringTabs* excludedFiles) {
    int i;
    for(i = 0; i < excludedFiles->size; i++) {
        if(!strcmp(excluded, excludedFiles->tab[i])) {
            return;
        }
    }
    addString(excludedFiles, excluded);
}

// Parcours et récupère tous les fichiers .c à analyser
void browseDir(StringTabs *analyzedFiles, char *currDirectory, StringTabs *excludedFiles, int recursive) {
    char* temp = malloc(sizeof(char) * 250);
    DIR *dir = opendir(currDirectory);
    DIR *test;
    struct dirent *d_res;
    // DEBUG int i;
    recursive = 1;

    if(dir == NULL) {
        printf("Pas ouvert\n");
        system("pause");
        return;
    } else {
        printf("Le dossier est ouvert\n");
    }

    while((d_res = readdir(dir)) != NULL) {
        if(isValidCFile(d_res->d_name, excludedFiles)) {
            getPath(currDirectory, d_res->d_name, temp);
            addString(analyzedFiles, temp);
            printf("Fichier C : %s\n", d_res->d_name);
        } else if(recursive == 1 && isDir(d_res->d_name)) {
            if(d_res->d_name[0] != '.') {
                getPath(currDirectory, d_res->d_name, temp);
                test = opendir(temp);
                if(test == NULL) {
                    printf("%s is not a directory\n", d_res->d_name);
                } else {
                    closedir(test);
                    browseDir(analyzedFiles, temp, excludedFiles, recursive);
                }
            }
        }

    }

    free(temp);
    closedir(dir);

    // DEBUG
    /*printf("\n--- Analyzed files ---\n");
    for(i = 0; i < analyzedFiles->size; i++) {
        printf("%s\n", analyzedFiles->tab[i]);
    }*/
}

// Initialise une instance de StringTabs
void initTabs(StringTabs *t, int size) {
    int i;
    t->maxSize = size;
    t->tab = malloc(sizeof(char*) * t->maxSize);
    for(i = 0; i < t->maxSize; i++) {
        t->tab[i] = malloc(sizeof(char) * 60);
    }
    t->size = 0;
}

// Libère la place occupée en mémoire par une instance de StringTabs
void freeTabs(StringTabs *t) {
    int i;
    for(i = 0; i < t->maxSize; i++) {
        free(t->tab[i]);
    }
    free(t->tab);
    free(t);
}

// Augmente la taille du tableau contenu dans StringTabs
void increaseTabSize(StringTabs *t) {
    int i;
    t->tab = realloc(t->tab, t->maxSize += 5);
    for(i = t->size; i < t->maxSize; i++) {
        t->tab[i] = malloc(sizeof(char) * 60);
    }
}

// Vérifie si la chaine récupérée désigne un fichier .c
// et qu'il n'est pas dans la liste des fichiers exclus
int isValidCFile(char *name, StringTabs *excludedFiles) {
    int i;
    int size = strlen(name);
    if(name[size - 1] == 'c' && name[size - 2] == '.') {
        for(i = 0; i < excludedFiles->size; i++) {
            if(!strcmp(excludedFiles->tab[i], name)) {
                printf("Fichier exclu !\n");
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

// Ajoute une chaine de caractère à la fin du tableau d'une instance de StringTabs
void addString(StringTabs *t, char *s) {
    if(t->size == t->maxSize) {
        increaseTabSize(t);
    }
    strcpy(t->tab[t->size++], s);
}

int isDir(char *s) {
    int i;
    for(i = 0; i < strlen(s); i++) {
        if(s[i] == '.') {
            return 0;
        }
    }
    printf("%s est un dossier\n", s);
    return 1;
}

void getPath(char *path, char *fileName, char *target) {
    strcpy(target, path);
    strcat(target, "/");
    strcat(target, fileName);
    target[strlen(target)] = '\0';
}
