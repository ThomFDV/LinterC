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
    strcpy(t->tab[t->size++], s);
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
                            break;
                        case 5  : // 5  max-line-numbers
                            n = recup_number_of_rule(17, rules->tab[i]);
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
                                // regle
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
                                // regle
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
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 10  : // 10 undeclared-variable
                            isValid = recup_value_of_rule(20, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 11  : // 11 no-prototype
                            isValid = recup_value_of_rule(13, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 12  : // 12 unused-function
                            isValid = recup_value_of_rule(16, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
                            }
                            else {
                                printf(" --- Valeur saisie non valide\n");
                            }
                            break;
                        case 13  : // 13 undeclared-function
                            isValid = recup_value_of_rule(20, rules->tab[i]);
                            if (isValid > -1) {
                                // regle
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
        system("pause");
        system("cls");
    }
}

/*
    PT1
    Première étape : vérifier que les règles existent.
    Pour ça, ça serait bien de faire une sous-fonction "check_si_regle_existe"
    Ensuite il faut regarder si la valeur affectée à la règle est valide :
        - Si elle est à off on la lance pas
        - Si elle est à on, on la lance seulement si la fonction a pas besoin d'argument
        - Si c'est autre chose, on vérifie que le type de la valeur correspond bien au type de paramètre attendu,
          et donc si c'est bon on la lance (en convertissant si besoin la valeur qui sera une chaine de caractère
          en int par exemple), sinon on lance pas
    Du coup ça donnerait quelque chose comme ça :
    Pour chaque règle :
    if(check_si_regle_existe(nom_regle)) {
        if(check_si_valeur_ok(nom_regle, valeur_regle) {
            for(i = 0; i < strlen(analyzedFiles->size); i++) {

            }
        }
    }
*/

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
