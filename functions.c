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
                        printf("Pas d'extends\n");
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
        if(strcmp(rule, rules->tab[found])) {
            strcpy(rules->tab[found], rule);
            rules->tab[found][strlen(rules->tab[found])] = '\0';
            printf("La regle %s a ete modifiee\n", rules->tab[found]);
        }
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
    char* temp = malloc(sizeof(char) * 300);
    printf("%s\n", currDirectory);
    DIR *dir = opendir(currDirectory);
    DIR *test;
    struct dirent *d_res;
    // DEBUG int i;
    recursive = 1; // DEBUG

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
StringTabs *initTabs(int size) {
    StringTabs *t = malloc(sizeof(StringTabs));
    int i;
    t->maxSize = size;
    t->tab = malloc(sizeof(char*) * t->maxSize);
    for(i = 0; i < t->maxSize; i++) {
        t->tab[i] = malloc(sizeof(char) * 60);
    }
    t->size = 0;
    return t;
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
    strcpy(t->tab[t->size], s);
    t->tab[t->size][strlen(t->tab[t->size])] = '\0';
    t->size += 1;
}

int isDir(char *s) {
    int i;
    for(i = 0; i < strlen(s); i++) {
        if(s[i] == '.') {
            return 0;
        }
    }
    return 1;
}

void getPath(char *path, char *fileName, char *target) {
    strcpy(target, path);
    strcat(target, "/");
    strcat(target, fileName);
    target[strlen(target)] = '\0';
}

void getDirToAnalyze(char *path) {
    int redo = 1;
    while(redo) {
        redo = 0;
        printf("Veuillez saisir le chemin d'acces vers le dossier a analyser : \n");
        fflush(stdin);
        fflush(stdin);
        scanf("%[^\n]s", path);
        redo = !isPathADir(path);
    }
    modifySpecialChar(path);
}

int isPathADir(char *p) {
    int i;
    int temp = 0;
    char *ret;
    if(strlen(p) <= 1) {
        return 1;
    }
    for(i = 0; i < strlen(p); i++) {
        if(p[i] == '\\' || p[i] == '/') {
            temp = i;
        }
    }
    temp += temp != 0 ? 1 : 0;
    ret = malloc(sizeof(char) * (strlen(p) - temp + 1));
    strncpy(ret, p + temp, strlen(p) - temp);
    ret[strlen(ret)] = '\0';

    temp = isDir(ret);
    free(ret);
    return temp;
}

void modifySpecialChar(char *path) {
    int i;
    for(i = 0; i < strlen(path); i++) {
        if(path[i] == -126) {
            path[i] = -23;
        }
    }
}

// Vérifie si la regle en parametre existe.
// Retourne 1 si elle existe et que sa valeur sera n|off,
// retourne 2 si elle existe et que sa valeur sera on|off,
// retourne 0 si elle n'existe pas ou si elle est mal ecrite.
int check_if_rules_exist(char *rule) {
    int i;
    int exist = -1;
    StringTabs *rulesTest = initTabs(16);
    rulesTest->tab[0] = "array-bracket-eol";
    rulesTest->tab[1] = "operators-spacing";
    rulesTest->tab[2] = "comma-spacing";
    rulesTest->tab[3] = "indent";
    rulesTest->tab[4] = "comments-header";
    rulesTest->tab[5] = "max-line-numbers";
    rulesTest->tab[6] = "max-file-line-numbers";
    rulesTest->tab[7] = "no-trailing-spaces";
    rulesTest->tab[8] = "no-multi-declaration";
    rulesTest->tab[9] = "unused-variable";
    rulesTest->tab[10] = "undeclared-variable";
    rulesTest->tab[11] = "no-prototype";
    rulesTest->tab[12] = "unused-function";
    rulesTest->tab[13] = "undeclared-function";
    rulesTest->tab[14] = "variable-assignment-type";
    rulesTest->tab[15] = "function-parameters-type";

    for (i = 0; i < 16; i++) {

        if (strstr(rule, rulesTest->tab[i]) != NULL) {

            exist = i;
            break;
        }
    }
    freeTabs(rulesTest);
    return exist;
}

// Recupere le chiffre en char*, retourne le chiffre converti en int
int recup_number_of_rule(int numDepart, char *rule) {
    int n = -1;
    int m = 0;
    int k;
//    int autre = 0;
    int chiffre = 0;
    char *num = malloc(sizeof(char)*4);

    for (k = numDepart; k < strlen(rule); k++) {

        if (((rule[k] > 47) && (rule[k] < 58)) && ((chiffre == 1) || (m == 0))) { // si c'est un chiffre
            num[m] = rule[k];
            m++;
            chiffre = 1;
        } else if (((rule[k] < 48) || (rule[k] > 57)) && (chiffre == 1)) { // si ce n'est pas un chiffre mais qu'il y en avait un avant
            chiffre = 0;
        } else if ((rule[k] > 32) && (chiffre == 0) && (m > 0)) { // si c'est n'importe quel caractere et qu'il y avait un chiffre avant mais pas directement
            m = 0;
            break;
        } else if (rule[k] > 32){ // si ce n'est pas un espace
            m = 0;
            break;
        }
    }
    if (m == 4) {
        n = ((int)num[0]-48)*1000;
        n += ((int)num[1]-48)*100;
        n += ((int)num[2]-48)*10;
        n += ((int)num[3]-48);
    } else if (m == 3) {
        n = ((int)num[0]-48)*100;
        n += ((int)num[1]-48)*10;
        n += ((int)num[2]-48);
    } else if (m == 2) {
        n = ((int)num[0]-48)*10;
        n += ((int)num[1]-48);
    } else if (m == 1) {
        n = ((int)num[0]-48);
    } else if (m == 0) {
        n = -1;
    }
    free(num);
    return n;
}


// Recupere la valeur (on ou off) en char*
int recup_value_of_rule(int numDepart, char *rule) {
    int n = -1;
    int lettreO = 0;
    int k;
//    char* value = malloc(sizeof(char)*4);
    for (k = numDepart; k < strlen(rule); k++) {

            if ((rule[k] == 'o') && (k == numDepart)) {
                lettreO = 1;
            } else if ((rule[k] == 'n') && (lettreO == 1)) {
                lettreO = 0;
                n = 1;
                //break;
            } else if (rule[k] > 32){
                n = -1;
                lettreO = 0;
                break;
            }
    }
    return n;
}

// Execute les regles si elles sont "on" ou si "n" > 0
void exec_rules(StringTabs *rules, StringTabs *analyzedFiles) {
    int k;
    int i;
    int exist;
    int n;
    int isValid;
    for (k = 0; k < analyzedFiles->size; k++) {
        FuncTab *prototypes = initFuncTab(5);
        FuncTab *declaredFunc = initFuncTab(5);
        VarTab *declaredVar = initVarTab(5);
        calledStuff *calledFunc = initCalledStuff(5);
        calledStuff *usedVars = initCalledStuff(5);
        printf("Fichier %d/%d --- %s ---\n", k+1, analyzedFiles->size, analyzedFiles->tab[k]);
        for(i = 0; i < rules->size; i++) {
            exist = check_if_rules_exist(rules->tab[i]);

            if ((exist == 3) || (exist == 5) || (exist == 6)) {
                if (strstr(rules->tab[i], "off") != NULL) {
                    //printf("\n%s\n", rules->tab[i]);
                    //printf("------------------ 1");
                } else {
                    printf("\n%s\n", rules->tab[i]);
                    printf("------------------\n");
                    n = 0;
                    switch(exist) {

                        case 3  : // 3  indent
                            n = recup_number_of_rule(7, rules->tab[i]);
                            if (n > -1) {
                                checkIndent(&n, analyzedFiles->tab[k]);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 5  : // 5  max-line-numbers
                            n = recup_number_of_rule(17, rules->tab[i]);
                            if (n > -1) {
                                maxLineNumbers(analyzedFiles->tab[k], &n);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 6  : // 6  max-file-line-numbers

                            n = recup_number_of_rule(21, rules->tab[i]);
                            if (n > -1) {
                                max_file_line_numbers(analyzedFiles->tab[k], n);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                    }
                }
            } else if (exist != -1) {
                if (strstr(rules->tab[i], "on") != NULL) {
                    printf("\n%s\n", rules->tab[i]);
                    printf("------------------\n");
                    isValid = 0;
                    switch(exist) {
                        case 0  : // 0  array-bracket-eol
                            isValid = recup_value_of_rule(18, rules->tab[i]);
                            if (isValid > -1) {
                                array_bracket_eol(analyzedFiles->tab[k]);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 1  : // 1  operators-spacing
                            isValid = recup_value_of_rule(18, rules->tab[i]);
                            if (isValid > -1) {
                                operatorsSpacing(analyzedFiles->tab[k]);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 2  : // 2  comma-spacing                              !!!!!! ',' -> ne doit pas être une erreur
                            isValid = recup_value_of_rule(14, rules->tab[i]);
                            if (isValid > -1) {
                                comma_spacing(analyzedFiles->tab[k]);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 4  : // 4  comments-header
                            isValid = recup_value_of_rule(16, rules->tab[i]);
                            if (isValid > -1) {
                                comments_header(analyzedFiles->tab[k]);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 7  : // 7  no-trailing-spaces
                            isValid = recup_value_of_rule(19, rules->tab[i]);
                            if (isValid > -1) {
                                trailingSpaces(analyzedFiles->tab[k]);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 8  : // 8  no-multi-declaration
                            isValid = recup_value_of_rule(21, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 9  : // 9  unused-variable
                            isValid = recup_value_of_rule(16, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                                unused_variable(declaredVar, usedVars);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 10  : // 10 undeclared-variable
                            isValid = recup_value_of_rule(20, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                                undeclared_variable(declaredVar, usedVars);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 11  : // 11 no-prototype
                            isValid = recup_value_of_rule(13, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                                no_prototype(prototypes, declaredFunc);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 12  : // 12 unused-function
                            isValid = recup_value_of_rule(16, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                                unused_function(declaredFunc, calledFunc);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 13  : // 13 undeclared-function
                            isValid = recup_value_of_rule(20, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                                undeclared_function(declaredFunc, calledFunc);
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 14  : // 14 variable-assignment-type
                            isValid = recup_value_of_rule(25, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 15  : // 15 function-parameters-type
                            isValid = recup_value_of_rule(26, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                    }
                } else if (strstr(rules->tab[i], "off") != NULL) {

                } else {
                    printf("\n%s\n", rules->tab[i]);
                    printf("------------------\n");
                    printf(" --- Valeur saisie non valide\n");
                }
            }
        }
        freeCalledStuff(calledFunc);
        freeCalledStuff(usedVars);
        freeFuncTab(prototypes);
        printf("Free 1 done\n");
        freeFuncTab(declaredFunc);
        printf("Free 2 done\n");
        freeVarTab(declaredVar);
        system("pause");
        system("cls");
    }
}

void isComment(char* c, FILE* f, int* lines){
    if(*c == '/'){
        *c = fgetc(f);
        if(*c == '/'){
            while(*c != '\n'){
                *c = fgetc(f);
            }
        } else if(*c == '*'){
            do{
                *c = fgetc(f);
                *lines += isLine(c);
            } while((*c != '*') && ((*c = fgetc(f)) != '/'));
            *c = fgetc(f);
            *c = fgetc(f);
            *lines += isLine(c);
        }
    }
}

void isQuote(char* c, FILE* f){
    if(*c == '"'){
        do{
            *c = fgetc(f);
        } while(*c != '"');
    }
}

int isLine(char* c){
    if (*c == '\n') {
        return 1;
    } else{
        return 0;
    }
}

/* ************************************************************************************************************************* */
/*                                                          RULES            a deplacer                                      */
/* ************************************************************************************************************************* */

// L’accolade doit se trouver en fin de ligne pour les fonctions, if, boucles, …
void array_bracket_eol(char *analyzedFiles) {

    FILE* f = fopen(analyzedFiles, "r+");

    if (f != NULL) {

        //printf("Ouverture du fichier reussie\n");

        char c;
        int numberLine = 1;
        int sautDeLigne = 0;
        int espace = 0;

        // Parcours du fichier f caractere par caractere
        while ((c = fgetc(f)) != EOF) {

            if (c == '\n') {
                espace = 0;
                sautDeLigne = 1;
                numberLine++;
            }
            // Si après un saut de ligne il y a une accolade ouvrante OU si apres un saut de ligne et apres plusieurs espaces il y a une accolade ouvrante
            else if (((c == '{') && (sautDeLigne == 1)) || ((sautDeLigne == 1) && (espace > 0) && (c == '{'))) {

                printf(" --- Erreur de la regle array-bracket-eol, a la ligne :");
                printf(" %d\n",  numberLine);
                sautDeLigne = 0;
                espace = 0;
            }
            // si c'est un espace ou une tabulation ET qu'il suit un saut de ligne
            else if (((c == ' ') || (c == '	'))  && (sautDeLigne == 1)) {
                espace++;
            }
            // si ce n'est ni un espace ni une tabulation ET qu'il suit un saut de ligne
            else if (((c != ' ') || ((c == '	'))) && (sautDeLigne == 1)) {
                espace = 0;
                sautDeLigne = 0;
            }
        }
        fclose(f);
    }
    else {
        printf("Echec de l'ouverture du fichier");
    }

}

// Il doit avoir un espace à droite d’une virgule.
void comma_spacing(char *analyzedFiles) {

    FILE* f = fopen(analyzedFiles, "r+");

    if (f != NULL) {

        //printf("Ouverture du fichier reussie\n");
        char c;
        int numberLine = 1;
        int virgule = 0;
        int guillemets = 0;

        // Parcours du fichier f caractere par caractere
        while ((c = fgetc(f)) != EOF) {

            if (c == '\n') {
                virgule = 0;
                numberLine++;
            }
            else if (c == ',') {
                virgule = 1;
            }
            // ne prends pas en compte les parties entre guillemets
            else if (c == '"') {
                guillemets = 1;
                while ((c = fgetc(f)) != '"') {
                    if (c == '\n') {
                        numberLine++;
                    }
                    else if ((c == '\'') && (guillemets == 1)) {
                        break;
                    }
                    else if ((c != '\'') && (guillemets == 1)) {
                        guillemets = 0;
                    }
                }
            }
            else if (c == '\'') { // ne prends pas en compte ce qui est entre ''
                while ((c = fgetc(f)) != '\'') {
                }
            }
            // ne prend pas en compte les commentaires
            else if (c == '/') {
                c = fgetc(f);
                if (c == '/') {
                    while ((c = fgetc(f)) != '\n') {
                    }
                    numberLine++;
                }
                else if (c == '*') {
                    int etoile = 0;
                    while (((c = fgetc(f)) != '/') || (etoile == 0)) {
                        etoile = 0;
                        if (c == '\n') {
                            numberLine++;
                        }
                        else if (c == '*') {
                            etoile = 1;
                        }
                    }
                }
            }
            else if ((c != ' ') && (virgule == 1)) {

                virgule = 0;
                printf(" --- Erreur de la regle comma-spacing, a la ligne :");
                printf(" %d\n",  numberLine);
            }
            else {
                virgule = 0;
            }
        }
        fclose(f);
    }
}

// Test la présence d’un commentaire multi-ligne en entête de fichier.
void comments_header(char *analyzedFiles) {
    FILE* f = fopen(analyzedFiles, "r+");

    if (f != NULL) {

        //printf("Ouverture du fichier reussie\n");

        char c;
        int numberLine = 1;

        // Parcours du fichier f caractere par caractere
        while ((c = fgetc(f)) != EOF) {
            // test la presence d'un commentaire et s'il a plusieurs lignes ou non
            if (c == '/') {
                c = fgetc(f);
                if (c == '*') {
                    int etoile = 0;
                    while (((c = fgetc(f)) != '/') || (etoile == 0)) {
                        etoile = 0;
                        if (c == '\n') {
                            numberLine++;
                        }
                        if (c == '*') {
                            etoile = 1;
                        }
                    }
                    if (numberLine > 1) {
                        printf(" --- Regle comments-header : OUI\n");
                        break;
                    }
                    else {
                        printf(" --- Regle comments-header : NON\n");
                        break;
                    }
                }
                else {
                    printf(" --- Regle comments-header : NON\n");
                    break;
                }
            }
            else {
                printf(" --- Regle comments-header : NON\n");
                break;
            }

        }
        fclose(f);
    }
}

// Les fichiers ne doivent pas dépasser n lignes.
void max_file_line_numbers(char *analyzedFiles, int maxLineNumber) {
    FILE* f = fopen(analyzedFiles, "r+");

    if (f != NULL) {
        //printf("Ouverture du fichier reussie\n");
        char c;
        int numberLine = 1;

        // Parcours du fichier f caractere par caractere
        while ((c = fgetc(f)) != EOF) {
            if (c == '\n') {
                numberLine++;
            }
            // si le nombre de lignes depasse le nombre max defini,
            if (numberLine > maxLineNumber)  {
                printf(" --- Erreur de la regle max-file-line-numbers: le fichier depasse le nombre de ligne:");
                printf(" %d\n",  maxLineNumber);
                break;
            }
        }
    }
    fclose(f);
}

void operatorsSpacing(char* filename){
    FILE* f = fopen(filename,"r");
    char c;
    int i;
    int lines = 1;
    char operators[11] = {'+', '-', '/', '*', '%', '!', '^', '<', '>', '|', '&'};
    int pbmBefore;
    int pbmAfter;
    int pointeur;
    pointeur = 0;
    pbmAfter = 0;
    pbmBefore = 0;
    if(f != NULL){
        while((c = fgetc(f)) != EOF ){
            isComment(&c, f, &lines);
            isQuote(&c, f);
            isSharp(&c, f, &lines);
            lines += isLine(&c);
            //Verifie si le char rencontré est un operateur
            for(i = 0; i < 11; i++){
                if(c == operators[i]){
                    if(operators[i] == '&' || operators[i] == '*'){
                        //ajoute le cas de pointeur en exception
                        pointeur = 1;
                    }
                    fseek(f, -2, SEEK_CUR);
                    c = fgetc(f);
                    //Verifie si le char precedent l'operateur est un espace sinon stock l'information
                    if(c != ' '){
                        pbmBefore = 1;
                    }
                    c = fgetc(f);
                    c = fgetc(f);
                    //Verifie si le char apres l'operateur est un espace
                    if(c != ' '){
                        // ou un egal
                        if(c != '='){
                            //ou si c'est un cas d'incrementation ++ ou --
                            if((c == operators[i]) && i < 2){
                                c = fgetc(f);
                                // Mais il faut qu'il y ait forcement un ; apres sinon erreur
                                if(c == ';'){
                                    //Si c'est le cas affiche pas d'erreur s'il n'y a pas eu d'espace avant et/ou apres
                                    pbmAfter = 0;
                                    pbmBefore = 0;
                                } else {
                                    pbmAfter = 1;
                                }
                            } else if((c == operators[i]) && i > 1){
                                //Verifie si le char et le meme operateur que le char precedant
                                c = fgetc(f);
                                if(c != ' '){
                                    pbmAfter = 1;
                                } else {
                                    pbmAfter = 0;
                                }
                            } else if(pointeur == 1){
                                pbmAfter = 0;
                            } else {
                                pbmAfter = 1;
                            }
                        } else {
                            c = fgetc(f);
                            if(c != ' '){
                                pbmAfter = 1;
                            }
                        }
                    }
                }
                if(pbmBefore == 1){
                    printf("\n---Erreur de la regle operator-spacing a la ligne : %d", lines);
                } else if(pbmAfter == 1){
                    printf("\n---Erreur de la regle operator-spacing a la ligne : %d", lines);
                }
                pbmBefore = 0;
                pbmAfter = 0;
                pointeur = 0;
            }
        }
    }
    fclose(f);
}

void trailingSpaces(char* filename){
    FILE* f = fopen(filename,"r+");
    char c;
    int lines = 1;
    if(f != NULL){
        while((c = fgetc(f)) != EOF ){
            isComment(&c, f, &lines);
            isQuote(&c, f);
            lines += isLine(&c);
            //Verifie s'il y a un espace avant un retour a la ligne, si oui erreur sinon ok
            if(c == ' '){
                c = fgetc(f);
                if(c == '\n'){
                    printf("Erreur de la regle no-trailing-spaces a la ligne %d\n", lines);
                }
                fseek(f, -1, SEEK_CUR);
            }
        }
    }
    fclose(f);
}

void maxLineNumbers(char* filename, int* n){
    FILE* f = fopen(filename,"r+");
    char c;
    int lines = 0;
    int charCounter;
    charCounter = 0;
    int charMax = *n;
    if(f != NULL){
        while((c = fgetc(f)) != EOF ){
            lines += isLine(&c);
            if(c == '\n'){
                //Verifie le nombre de char avant un retour a la ligne et test s'il est pas superieur au nombre indiqué
                if(charCounter > charMax){
                    printf("\Erreur de la regle maxLineNumbers a la ligne %d\n", lines);
                }
                charCounter = 0;
            } else {
                charCounter += 1;
            }
        }
    }
    fclose(f);
}

void checkIndent(int* n, char* filename){
    FILE* f = fopen(filename,"r");
    char c;
    int m;
    int newN;
    int lines = 1;
    int charNbr = 0;
    int spaceNbr = 0;
    int spaceCounter = 1;
    int isChanged = 0;
    int spaceBefore = 0;
    int newSpaceCounter = 0;
    int wentBack = 0;
    newN = *n;
    m = *n;

    if(f != NULL){
        while((c = fgetc(f)) != EOF ){
            isComment(&c, f, &lines);
            isQuote(&c, f);
            lines += isLine(&c);
            charNbr +=1;
            //Si c'est un { on verifie si il y en a eu un de recontré avant deja
            if(c == '{' && wentBack == 0){
                if(isChanged > 0){
                    //Si un { a été rencontré avant on augmente l'indentation de n fois
                    newN += m;
                }
                //sinon c'est que c'est le premier est dans ce cas on revient en arriere pour verifier son nombre d'espace
                fseek(f, -charNbr, SEEK_CUR);
                isChanged += 1;
                charNbr = 0;
                wentBack = 1;
            }
            if(c == ' ' && isChanged > 0){
                if(c == ' ' && spaceBefore == 1){
                    spaceCounter +=1;
                } else{
                    spaceBefore = 1;
                }
            } else{
                spaceNbr = spaceCounter;
                spaceCounter = 1;
                spaceBefore = 0;
            }

            if(c == '\n'){
                charNbr = 0;
            }
            if(c == '\n' && isChanged > 0){
                c = fgetc(f);
                wentBack = 0;
                while(c == ' '){
                    c = fgetc(f);
                    newSpaceCounter +=1;
                }
                //Verifie si le nombre d'espace rencontré
                if(c == '}' && (newSpaceCounter != (spaceCounter + newN - 1 - m))){
                    printf("Erreur d'indentation a la ligne : %d", lines);
                } else if((newSpaceCounter != (spaceCounter + newN - 1)) && isChanged > 0 && c != '}'){
                    printf("Erreur d'indentation a la ligne : %d", lines);
                }
                spaceBefore = 0;
                newSpaceCounter = 0;

            }
            if(c == '}'){
                newN -= m;
                isChanged -= 1;
            }


        }
    }
    fclose(f);
}

void isSharp(char* c, FILE* f, int* lines){
    if(*c == '#'){
        do{
            *c = fgetc(f);
        } while(*c != '\n');
    }
}

/* ---------------- CORENTIN ------------------------- */

int isAnIncludedFunc(char *func) {
    char tab[89][40] =  {"printf",
                        "scanf",
                        "getc",
                        "gets",
                        "getchar",
                        "puts",
                        "putchar",
                        "clearerr",
                        "fopen",
                        "fclose",
                        "getw",
                        "putw",
                        "fgetc",
                        "putc",
                        "fputc",
                        "fgets",
                        "fputs",
                        "feof",
                        "fgetchar",
                        "fprintf",
                        "fscanf",
                        "fputchar",
                        "fseek",
                        "ftell",
                        "sizeof",
                        "rewind",
                        "sprint",
                        "sscanf",
                        "remove",
                        "fflush",
                        "malloc",
                        "calloc",
                        "realloc",
                        "free",
                        "abs",
                        "div",
                        "abort",
                        "exit",
                        "system",
                        "atoi",
                        "atol",
                        "atof",
                        "strtod",
                        "strtol",
                        "getenv",
                        "setenv",
                        "perror",
                        "rand",
                        "delay",
                        "strcat",
                        "strncat",
                        "strcpy",
                        "strncpy",
                        "strlen",
                        "strcmp",
                        "strcmpi",
                        "strchr",
                        "strrchr",
                        "strstr",
                        "strrstr",
                        "strdup",
                        "strlwr",
                        "strupr",
                        "strrev",
                        "strset",
                        "strnset",
                        "strtok",
                        "memset",
                        "memcpy",
                        "memmove",
                        "memcmp",
                        "memicmp",
                        "memchr",
                        "floor",
                        "round",
                        "ceil",
                        "sin",
                        "cos",
                        "cosh",
                        "exp",
                        "tan",
                        "tanh",
                        "sinh",
                        "log",
                        "log10",
                        "sqrt",
                        "pow",
                        "trunc",
                        "return"};
    int i;
    for(i = 0; i < 89; i++) {
        if(!strcmp(func, tab[i])) {
            printf("%s matched with %s\n", func, tab[i]);
            return 1;
        }
    }
    return 0;
}

int isJustANumber(char *s) {
    int i;
    for(i = 0; i < strlen(s); i++) {
        if(s[i] < '0' || s[i] > '9') {
            return 0;
        }
    }
    return 1;
}

int isAnInstruction(char *s) {
    char instructions[7][10] = {"while", "for", "if", "do", "else", "switch", "return"};
    int i;
    for(i = 0; i < 7; i++) {
        if(!strcmp(s, instructions[i])) {
            return 1;
        }
    }
    return 0;
}

int isAConstant(char *s) {
    char tab[47][20] = {"BUFSIZ",
                      "_IOFBF",
                      "_IOLBF",
                      "_IOLBF",
                      "L_ctermid",
                      "L_tmpnam",
                      "SEEK_CUR",
                      "SEEK_END",
                      "SEEK_SET",
                      "FILENAME_MAX",
                      "FOPEN_MAX",
                      "TMP_MAX",
                      "EOF",
                      "NULL",
                      "P_tmpdir",
                      "EXIT_FAILURE",
                      "EXIT_SUCCESS",
                      "RAND_MAX",
                      "MB_CUR_MAX",
                      "M_E",
                      "M_LOG2E",
                      "M_LOG10E",
                      "M_LN2",
                      "M_LN10",
                      "M_PI",
                      "M_PI_2",
                      "M_PI_4",
                      "M_1_PI",
                      "M_2_PI",
                      "M_2_SQRTPI",
                      "M_SQRT2",
                      "M_SQRT1_2",
                      "MAXFLOAT",
                      "HUGE_VAL",
                      "HUGE_VALF",
                      "HUGE_VALL",
                      "INFINITY",
                      "NAN",
                      "return",
                      "char",
                      "int",
                      "float",
                      "double",
                      "FILE",
                      "void",
                      "break",
                      "continue"};
    int i;
    for(i = 0; i < 47; i++) {
        if(!strcmp(s, tab[i])) {
            return 1;
        }
    }
    return 0;
}

int isFollowedByRightOps(char c) {
    char tab[12] = {'=', '+', '-', '/', '*', ',', ')', '<', '>', '[', ']', ';'};
    int i;
    for(i = 0; i < 12; i++) {
        if(c == tab[i]) {
            return 1;
        }
    }
    return 0;
}

int isADir(char *p) {
    int i;
    int temp = 0;
    char *ret;
    for(i = 0; i < strlen(p); i++) {
        if(p[i] == '\\' || p[i] == '/') {
            temp = i;
        }
    }
    temp += temp != 0 ? 1 : 0;
    ret = malloc(sizeof(char) * (strlen(p) - temp + 1));
    strncpy(ret, p + temp, strlen(p) - temp);
    ret[strlen(ret)] = '\0';

    for(i = 0; i < strlen(ret); i++) {
        if(ret[i] == '.') {
            free(ret);
            return 0;
        }
    }

    free(ret);
    return 1;
}

void getFuncAndVariables(FILE *f, FuncTab *prototypes, FuncTab *declaredFunc, VarTab *declaredVar, calledStuff *calledFunc, calledStuff *usedVars) {
    char *buffer = malloc(sizeof(char) * 10);
    char *name = malloc(sizeof(char) * 50);
    long startParams = 0;
    int nameWrite = 0;
    char c;
    int line = 1;
    int countBraces = 0;
    int i; //DEBUG
    while((c = fgetc(f)) != EOF) {
        printf("Ligne %d\n", line);
        if(c == '#') {
            goThroughLine(f, &line, &countBraces);
            continue;
        }
        if(c == ' ') {
            goThroughSpaces(f);
            continue;
        }
        if(c == '/') {
            if((c = fgetc(f)) == '/') {
                goThroughLine(f, &line, &countBraces);
                continue;
            } else if(c == '*') {
                goThroughComment(f, &line);
            }
        }
        if(c == '\n') {
            line++;
            continue;
        }
        /*if(line % 30 == 0) {
            system("pause");
        }*/
        if(c == '}') {
            if(countBraces == 1) {
                declaredFunc->tab[declaredFunc->size - 1]->endLine = line;
                printf("FIN DE LA FONCTION ATTENTIOOOOOOOOOOOON %d\n", declaredFunc->tab[declaredFunc->size - 1]->endLine);
            }
            countBraces--;
            continue;
        }
        if(((c >= 'a' && c <= 'z') || c == 'F') && (startWithType(f, buffer))) {
            printf("It does start with type !\n");
            c = fgetc(f);
            if((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_') {
                goThroughLine(f, &line, &countBraces);
                continue;
            } else {
                fseek(f, -1, SEEK_CUR);
            }
            nameWrite = 0;
            while(isValidChar(c = fgetc(f))) {
                name[nameWrite++] = c;
            }
            name[nameWrite] = '\0';
            fseek(f, -1, SEEK_CUR);
            goThroughSpaces(f);
            c = fgetc(f);
            if(c == '(') {
                printf("[!]");
                startParams = ftell(f);
                goThroughParams(f);
            } else if(c == '=' || c == ';'){
                if(countBraces != 0 && declaredFunc->size > 0) {
                    printf("%s %s at line %d, in %s\n", buffer, name, line, declaredFunc->tab[(declaredFunc->size) - 1]->name);
                    addVariable(declaredVar, buffer, name, declaredFunc->tab[(declaredFunc->size) - 1], line);
                } else {
                    addVariable(declaredVar, buffer, name, NULL, line);
                }
                printf("%s %s is a variable, line %d\n", buffer, name, line);
                goThroughLine(f, &line, &countBraces);
                nameWrite = 0;
                continue;
            } else {
                nameWrite = 0;
                continue;
            }
            goThroughSpaces(f);
            if((c = fgetc(f)) == '{') {
                printf("%d : this is a function !\n", line);
                addFunction(declaredFunc, buffer, name, line, 0);
                getParams(f, startParams, declaredFunc->tab[(declaredFunc->size) - 1], declaredVar, line);
                //countBraces++;
                //goThroughSpaces(f);
            } else if(c == ';') {
                printf("%d : this is a prototype !\n", line);
                addFunction(prototypes, buffer, name, line, -1);
            }
        } else {
            analyzeLine(f, line, &countBraces, declaredFunc, calledFunc, usedVars);
            line++;
        }
    }

    system("pause");
    system("cls");
    for(i = 0; i < calledFunc->size; i++) {
        printf("La fonction %s est appelee ligne %d\n", calledFunc->name[i], calledFunc->line[i]);
    }

    system("pause");
    system("cls");

    for(i = 0; i < usedVars->size; i++) {
        printf("La variable %s est utilisee ligne %d\n", usedVars->name[i], usedVars->line[i]);
    }

    free(name);
    free(buffer);
}

int startWithType(FILE *f, char *type) {
    char c;
    int write;
    int count = 1;
    int isType = 0;
    fseek(f, -1, SEEK_CUR);
    c = fgetc(f);
    switch(c) {
        case 'i' :
            if((c = fgetc(f)) == 'n') {
                count++;
                if((c = fgetc(f)) == 't') {
                    strcpy(type, "int");
                    write = 3;
                    goThroughSpaces(f);
                    while((c = fgetc(f)) == '*') {
                        type[write++] = '*';
                    }
                    type[write] = '\0';
                    count = 1;
                    isType = 1;
                }
            }
            fseek(f, -count, SEEK_CUR);
            break;
        case 'c' :
            if((c = fgetc(f)) == 'h') {
                count++;
                if((c = fgetc(f)) == 'a') {
                    count++;
                    if((c = fgetc(f)) == 'r') {
                        strcpy(type, "char");
                        write = 4;
                        goThroughSpaces(f);
                        while((c = fgetc(f)) == '*') {
                            type[write++] = '*';
                        }
                        type[write] = '\0';
                        count = 1;
                        isType = 1;
                    }
                }
            }
            fseek(f, -count, SEEK_CUR);
            break;
        case 'F' :
            if((c = fgetc(f)) == 'I') {
                count++;
                if((c = fgetc(f)) == 'L') {
                    count++;
                    if((c = fgetc(f)) == 'E') {
                        strcpy(type, "FILE");
                        write = 4;
                        goThroughSpaces(f);
                        while((c = fgetc(f)) == '*') {
                            type[write++] = '*';
                        }
                        type[write] = '\0';
                        count = 1;
                        isType = 1;
                    }
                }
            }
            fseek(f, -count, SEEK_CUR);
            break;
        case 'v' :
            if((c = fgetc(f)) == 'o') {
                count++;
                if((c = fgetc(f)) == 'i') {
                    count++;
                    if((c = fgetc(f)) == 'd') {
                        strcpy(type, "void");
                        write = 4;
                        goThroughSpaces(f);
                        while((c = fgetc(f)) == '*') {
                            type[write++] = '*';
                        }
                        type[write] = '\0';
                        count = 1;
                        isType = 1;
                    }
                }
            }
            fseek(f, -count, SEEK_CUR);
            break;
        case 'f' :
            if((c = fgetc(f)) == 'l') {
                count++;
                if((c = fgetc(f)) == 'o') {
                    count++;
                    if((c = fgetc(f)) == 'a') {
                        count++;
                        if((c = fgetc(f)) == 't') {
                            strcpy(type, "float");
                            write = 5;
                            goThroughSpaces(f);
                            while((c = fgetc(f)) == '*') {
                                type[write++] = '*';
                            }
                            type[write] = '\0';
                            count = 1;
                            isType = 1;
                        }
                    }
                }
            }
            fseek(f, -count, SEEK_CUR);
            break;
        case 'd' :
            if((c = fgetc(f)) == 'o') {
                count++;
                if((c = fgetc(f)) == 'u') {
                    count++;
                    if((c = fgetc(f)) == 'b') {
                        count++;
                        if((c = fgetc(f)) == 'l') {
                            count++;
                            if((c = fgetc(f)) == 'e') {
                                strcpy(type, "double");
                                write = 6;
                                goThroughSpaces(f);
                                while((c = fgetc(f)) == '*') {
                                    type[write++] = '*';
                                }
                                type[write] = '\0';
                                count = 1;
                                isType = 1;
                            }
                        }
                    }
                }
            }
            fseek(f, -count, SEEK_CUR);
            break;
    }
    //goThroughSpaces(f);
    return isType;
}

void goThroughSpaces(FILE *f) {
    char c;
    while((c = fgetc(f)) == ' '){}
    fseek(f, -1, SEEK_CUR);
}

int isValidChar(char c) {
    if((c >= 'a' && c <= 'z') ||
       (c >= 'A' && c <= 'Z') ||
       (c >= '0' && c <= '9') ||
       (c == '_')) {
        return 1;
       }
    return 0;
}

void goThroughParams(FILE *f) {
    char c;
    int countParentheses = 1;
    while(countParentheses){
        c = fgetc(f);
        if(c == '"') {
            goThroughStrings(f);
        }
        if(c == '(') {
            countParentheses++;
        }
        if(c == ')') {
            countParentheses--;
        }
    }
}

void goThroughStrings(FILE *f) {
    char c;
    int countQuote = 1;
    while(countQuote) {
        c = fgetc(f);
        if(c == '\\') {
            fseek(f, 1, SEEK_CUR);
            continue;
        }
        if(c == '"') {
            countQuote--;
        }
    }
}

void goThroughLine(FILE* f, int *line, int *countBraces) {
    char c;
    while((c = fgetc(f)) != '\n') {
        if(c == '{') {
            *countBraces += 1;
        } else if(c == '}') {
            *countBraces -= 1;
            if(*countBraces == 0) {
                printf("Fin de la fonction ligne %d\n", *line);
            }
        }
    }
    *line += 1;
}

void goThroughComment(FILE *f, int *line) {
    char c;
    int out = 1;
    while(out) {
        c = fgetc(f);
        if(c == '*') {
            if((c = fgetc(f)) == '/') {
                out = 0;
            } else {
                fseek(f, -1, SEEK_CUR);
            }
        } else if(c == '\n') {
            *line += 1;
        }
    }
}

void getParams(FILE *f, long start, Functions *func, VarTab *declaredVar, int line) {
    char c;
    char *type = malloc(sizeof(char) * 10);
    char *name;
    int write = 0;
    rewind(f);
    fseek(f, start, SEEK_CUR);
    goThroughSpaces(f);
    printf("GetParams\n");

    // Si pas de paramètres, on arrête de parcourir la fonction
    if((c = fgetc(f)) == ')') {
        return;
    }
    rewind(f);
    fseek(f, start - 1, SEEK_CUR); // start-1 pour être bien positioné dans la boucle qui suit
    name = malloc(sizeof(char) * 100);

    while((c = fgetc(f)) != ')') {
        if(!startWithType(f, type)) {
            type = getUnknownType(f);
        }
        c = fgetc(f);
        if((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_') {
            do {
                c = fgetc(f);
            } while(c != ',' && c != ')');
            if(goToNextParam(f)) {
                fseek(f, -1, SEEK_CUR); // On se positionne pour la prochaine occurence de la boucle
                continue;
            } else {
                break;
            }
        }
        name[write++] = c;
        while(isValidChar(c = fgetc(f))) {
            name[write++] = c;
        }
        name[write] = '\0';
        addVariable(declaredVar, type, name, func, line);
        write = 0;
        //printf("\n--- Ceci est un parametre : ------\ntype %s nom %s\n\n", type, name); // DEBUG
        if(c == ',') {
            continue;
        } else if(c == ')') {
            break;
        }
        if(goToNextParam(f)) {
            fseek(f, -1, SEEK_CUR);
            continue;
        } else {
            break;
        }
    }
    free(name);
    free(type);
}

int goToNextParam(FILE *f) {
    char c;
    do {
        c = fgetc(f);
        if(c == '"') {
            goThroughStrings(f);
        }
    } while(c != ',' && c != ')');
    if(c == ')') {
        return 0;
    } else {
        return 1;
    }
}

char *getUnknownType(FILE *f) {
    char *res = malloc(sizeof(char) * 50);
    char c;
    int write = 0;
    while(isValidChar(c = fgetc(f))) {
        res[write++] = c;
    }
    if(c == '*') {
        res[write++] = '*';
    }
    goThroughSpaces(f);
    while((c = fgetc(f)) == '*') {
        res[write++] = '*';
    }
    res[write] = '\0';
    fseek(f, -1, SEEK_CUR);
    goThroughSpaces(f);
    return res;
}

void analyzeLine(FILE *f, int line, int *countBraces, FuncTab *declaredFunc, calledStuff *calledFunc, calledStuff *usedVars) {
    printf("---analyzeLine\n");
    char c;
    char *buffer = malloc(sizeof(char) * 30);
    int write = 0;
    fseek(f, -1, SEEK_CUR);
    while((c = fgetc(f)) != '\n') {
        if(isValidChar(c)) {
            write = 0;
            buffer[write++] = c;
            while(isValidChar(c = fgetc(f))) {
                buffer[write++] = c;
            }
            // Gère les structures. On considère ici qu'une structure est utilisée peu importe quel variable qui la contient est utilisée
            if(c == '-') {
                if((c = fgetc(f)) == '>') {
                    while(isValidChar(c = fgetc(f))){}
                }
            } else if(c == '.') {
                while(isValidChar(c = fgetc(f))){}
            }
            buffer[write] = '\0';
            if(c == ' ') {
                goThroughSpaces(f);
                c = fgetc(f);
            }
            if(c == '(') {
                printf("[%d] %s should be a function\n", line, buffer);
                if(!isAnIncludedFunc(buffer)) {
                    if(!isJustANumber(buffer)) {
                        if(!isAnInstruction(buffer)) {
                            addCalledStuff(calledFunc, buffer, line);
                            printf("This is a call of %s\n", buffer);
                        }
                    }
                } else {
                    printf("Fonction dans un include, pas la peine de mettre d'erreur\n"); //DEBUG
                }
            } else if(isFollowedByRightOps(c)) {
                if(!isJustANumber(buffer)) {
                    if(!isAConstant(buffer)) {
                        addCalledStuff(usedVars, buffer, line);
                    }
                }
                printf("[%d] %s should be a variable\n", line, buffer);
            } else if(c == '{') {
                *countBraces += 1;
            } else if(c == '}') {
                *countBraces -= 1;
                if(*countBraces == 0) {
                    declaredFunc->tab[declaredFunc->size - 1]->endLine = line;
                }
            } else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                fseek(f, -1, SEEK_CUR);
            }
        } else if(c == '"') {
            goThroughStrings(f);
        } else if(c == '\n') {
            break;
        } else if(c == '{') {
            *countBraces += 1;
        } else if(c == '}') {
            *countBraces -= 1;
            if(*countBraces == 0) {
                printf("FIN DE LA FONCTION OUAIS OUAIS LIGNE %d", line); // CHANGE
            }
        }
    }
    free(buffer);
    printf("--- end analyzeLine\n");
}

void no_prototype(FuncTab *prototypes, FuncTab *declaredFunc) {
    int i;
    int j;
    int hasPrototype;
    for(i = 0; i < declaredFunc->size; i++) {
        hasPrototype = 0;
        for(j = 0; j < prototypes->size; j++) {
            if(isAPrototypeOf(declaredFunc->tab[i], prototypes->tab[j])) {
                if(declaredFunc->tab[i]->startLine > prototypes->tab[j]->startLine) {
                    hasPrototype = 1;
                } else {
                    hasPrototype = 2;
                }
                break;
            }
        }
        if(!hasPrototype) {
            printf("Ligne %d : the function %s %s has no prototype\n",
                   declaredFunc->tab[i]->startLine, declaredFunc->tab[i]->type, declaredFunc->tab[i]->name);
        } else if(hasPrototype == 2) {
            printf("Ligne %d : the function %s %s has a prototype but is declared after the function\n",
                   declaredFunc->tab[i]->startLine, declaredFunc->tab[i]->type, declaredFunc->tab[i]->name);
        }
    }
}

int isAPrototypeOf(Functions *funcOne, Functions *funcTwo) {
    if(!strcmp(funcOne->name, funcTwo->name)) {
        if(!strcmp(funcOne->type, funcTwo->type)) {
            return 1;
        }
    }
    return 0;
}

void unused_function(FuncTab *declaredFunc, calledStuff *calledFunc) {
    int i;
    int j;
    int isDeclared;
    for(i = 0; i < declaredFunc->size; i++) {
        isDeclared = 0;
        for(j = 0; j < calledFunc->size; j++) {
            if(!strcmp(declaredFunc->tab[i]->name, calledFunc->name[j])) {
                isDeclared = 1;
                break;
            }
        }
        if(!isDeclared) {
            printf("Line %d : function %s declared but not used", declaredFunc->tab[i]->startLine, declaredFunc->tab[i]->name);
        }
    }
}

void undeclared_function(FuncTab *declaredFunc, calledStuff *calledFunc) {
    int i;
    int j;
    int isUsed;
    for(i = 0; i < calledFunc->size; i++) {
        isUsed = 0;
        for(j = 0; j < declaredFunc->size; j++) {
            if(!strcmp(calledFunc->name[i], declaredFunc->tab[j]->name)) {
                isUsed = 1;
                break;
            }
        }
        if(!isUsed) {
            printf("Line %d : function %s is used but not declared\n", calledFunc->line[i], calledFunc->name[i]);
        }
    }
}

void unused_variable(VarTab *declaredVar, calledStuff *usedVars) {
    int i;
    int j;
    int isUsed;
    for(i = 0; i < declaredVar->size; i++) {
        isUsed = 0;
        if(declaredVar->tab[i]->declaredIn == NULL) {
            for(j = 0; j < usedVars->size; j++) {
                if(!strcmp(declaredVar->tab[i]->name, usedVars->name[j])) {
                    if(declaredVar->tab[i]->declaredLine < usedVars->line[j]) {
                        isUsed = 1;
                        break;
                    }
                }
            }
        } else {
            for(j = 0; j < usedVars->size; j++) {
                if(!isInFunc(declaredVar->tab[i]->declaredIn, usedVars->line[j])) {
                    continue;
                }
                if(!strcmp(declaredVar->tab[i]->name, usedVars->name[j])) {
                    if(declaredVar->tab[i]->declaredLine < usedVars->line[j]) {
                        isUsed = 1;
                        break;
                    }
                }
            }
        }
        if(!isUsed) {
            printf("Line %d : variable %s is instancied but not used\n", declaredVar->tab[i]->declaredLine, declaredVar->tab[i]->name);
        }
    }
}

int isInFunc(Functions *f, int line) {
    if(line >= f->startLine && line <= f->endLine) {
        return 1;
    }
    return 0;
}

void undeclared_variable(VarTab *declaredVar, calledStuff *usedVars) {
    int i;
    int j;
    int isDeclared;
    for(i = 0; i < usedVars->size; i++) {
        isDeclared = 0;
        for(j = 0; j < declaredVar->size; j++) {
            if(!strcmp(usedVars->name[i], declaredVar->tab[j]->name)) {
                if(declaredVar->tab[j]->declaredIn == NULL) {
                    if(usedVars->line[i] > declaredVar->tab[j]->declaredLine) {
                        isDeclared = 1;
                        break;
                    }
                } else {
                    if(!isInFunc(declaredVar->tab[j]->declaredIn, usedVars->line[i])) {
                        continue;
                    } else {
                        if(usedVars->line[i] > declaredVar->tab[j]->declaredLine) {
                            isDeclared = 1;
                            break;
                        }
                    }
                }
            }
        }
        if(!isDeclared) {
            printf("Line %d : variable %s is used but not declared\n", usedVars->line[i], usedVars->name[i]);
        }
    }
}
