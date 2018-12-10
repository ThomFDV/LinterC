#include <stdio.h>
#include "struct.h"

// Valeur de retour : si la vérification de fichier par le programme est récursive (1) ou non (0)
int parseConf(StringTabs*, StringTabs*, char*);

void getDirToAnalyze(char*);

// Parse les règles dans le fichier de configuration
void parseRules(FILE*, StringTabs*);

// Charge la configuration du  fichier dont découle le fichier initial
// Retourne le nom du fichier dont il dépend si il y en a un, sinon NULL
char *parseExtends(FILE*);

// Vérifie si l'analyse des fichiers doit se faire de manière récursive
// Renvoie 1 si récursif, sinon 0
int parseRecursive(FILE*);

// Parse les fichiers exclus de l'analyse de LinterC
void parseExcludedFiles(FILE*, StringTabs*);

// Ajoute ou modifie une règle de la liste de règles. Aggrandit le tableau si besoin.
void modifyRules(char*, StringTabs*);

// Ajoute ou modifie un fichier à la liste des fichiers exclus. Aggrandit le tableau si besoin.
void addExcludedFile(char*, StringTabs*);

// Test de parcours de dossier
void browseDir(StringTabs*, char*, StringTabs*, int);

// Initialise les tableaux de chaines de caractères
StringTabs *initTabs(int);

// Libère l'espace alloué aux tableaux de chaines de caractères
void freeTabs(StringTabs*);

// Aggrandi le nombre de lignes possibles pour un tableau de chaines de caractères
void increaseTabSize(StringTabs*);

// Ajout d'une chaine de caractère dans le tableau
void addString(StringTabs*, char*);

// Vérifie que le fichier a bien une extension .c
int isValidCFile(char*, StringTabs*);

// Vérifie si la chaine est un répertoire
int isDir(char*);

// Concatène le chemin d'accès et le nom du fichier pour soit le stocker, soit ouvrir un nouveau dossier
void getPath(char*, char*, char*);

// Saisie du chemin pour le dossier à analyser
void getDirToAnalyze(char*);

int isPathADir(char*);

void modifySpecialChar(char*);

/*
    PT1
    Le prototype ça serait : void nom_fonction(StringTabs*, StringTabs*);
*/

// Vérifie si la regle en parametre existe.// Retourne 1 si elle existe et que sa valeur sera n|off,// retourne 2 si elle existe et que sa valeur sera on|off,// retourne 0 si elle n'existe pas ou si elle est mal ecrite.
int check_if_rules_exist(char*);

// Recupere le chiffre en char*, retourne le chiffre converti en int
int recup_number_of_rule(int, char*);

// Recupere la valeur (on ou off) en char*
int recup_value_of_rule(int, char*);

// Execute les regles si elles sont "on" ou si "n" > 0
void exec_rules(StringTabs*, StringTabs*);

// Avance le curseur de 1 si un commentaire est rencontré
void isComment(char* c, FILE* f, int* lines);

// Avance le curseur de 1 si des guillemets sont rencontrés
void isQuote(char* c, FILE* f);

// Retourne 1 si un saut a la ligne est rencontré
int isLine(char* c);

// Passe la ligne si c'est un include
void isSharp(char*, FILE*, int*);

/* ************************************************************************************************************************* */
/*                                                          RULES                                                            */
/* ************************************************************************************************************************* */

// L’accolade doit se trouver en fin de ligne pour les fonctions, if, boucles,
void array_bracket_eol(char*);

// Il doit avoir un espace à droite d’une virgule.
void comma_spacing(char*);

// Test la présence d’un commentaire multi-ligne en entête de fichier.
void comments_header(char*);

// Les fichiers ne doivent pas dépasser n lignes.
void max_file_line_numbers(char*, int);

// Vérifie si l'indentation est correcte
void checkIndent(int* n, char* filename);

// Vérifie si le nombre de char est bien respecté
void maxLineNumbers(char* filename, int* n);

// Vérifie s'il n'y a pas d'espaces inutiles
void trailingSpaces(char* filename);

// Vérifie s'il y a bien un espace entre chaque opérateurs
void operatorsSpacing(char* filename);

/* ------------------------------------ CORENTIN ------------------------------------- */

int isADir(char*);

// Récupère le type, le nom et les paramètres de toutes les fonctions du fichier
void getFuncAndVariables(FILE*, FuncTab*, FuncTab*, VarTab*, calledStuff*, calledStuff*);

// Si la ligne commence avec un type, le renvoie, sinon renvoie NULL
int startWithType(FILE*, char*);

// Parcours de tous les éventuels espaces
void goThroughSpaces(FILE*);

// Renvoie 0 si le caractère n'est pas valide pour un nom de fonction
int isValidChar(char);

// Parcours le contenu entre parenthèses
void goThroughParams(FILE*);

// Parcours la chaine de caractère
void goThroughStrings(FILE*);

// Parcours le reste ou la ligne entière
void goThroughLine(FILE*, int*, int*);

// Parcours un commentaire multiligne. Incrémente le nombre de lignes si besoin
void goThroughComment(FILE*, int*);

// Initialisation du tableau de fonctions
FuncTab *initFuncTab(int);

// Ajout d'une fonction au tableau
void addFunction(FuncTab*, char*, char*, int, int);

// Libère la mémoire allouée au tableau de fonctions
void freeFuncTab(FuncTab*);

// Initialisation d'un tableau de variables
VarTab *initVarTab(int);

// Ajout d'une fonction au tableau
void addVariable(VarTab*, char*, char*, Functions*, int);

// Libère la mémoire allouée au tableau de fonctions
void freeVarTab(VarTab*);

// Récupère les paramètres d'une fonction
void getParams(FILE*, long, Functions*, VarTab*, int);

// Parcours jusqu'au prochain paramètre ou la fin de la déclaration de fonction si il n'y a plus de paramètres
int goToNextParam(FILE*);

// Retourne le nom du type que l'on ne connait pas, pour ne pas bloquer l'analyse de la fonction et des parametres
char *getUnknownType(FILE*);

// parse une ligne pour voir si il y a des variables utilisées ou des fonctions appelées
void analyzeLine(FILE*, int, int*, FuncTab*, calledStuff*, calledStuff*);

// Vérifie que chaque fonction a bien un prototype
void no_prototype(FuncTab*, FuncTab*);

// Vérifie que la fonction n'est pas dans les includes de base
int isAnIncludedFunc(char*);

// Si c'est un nombre, pas la peine de l'analysze avec les autres variables ou fonctions déclarées
int isJustANumber(char*);

// vérifie que c'est pas un while, for, if etc...
int isAnInstruction(char*);

// Initialise une liste de fonctions ou variables appelées
calledStuff *initCalledStuff(int);

// Ajoute une ligne à la liste
void addCalledStuff(calledStuff*, char*, int);

// Libère la place de la liste en mémoire
void freeCalledStuff(calledStuff*);

// Vérifie que la première variable Functions* est un prototype de la deuxième
int isAPrototypeOf(Functions*, Functions*);

// Vérifie si la variable est une constante connue ou non
int isAConstant(char*);

// Renvoie 1 si c'est un carctère qui peut potentiellement suivre une variable, sinon 0
int isFollowedByRightOps(char);

// Fonction du sujet
void no_prototype(FuncTab*, FuncTab*);

// Fonction du sujet
void unused_function(FuncTab*, calledStuff*);

// Fonction du sujet
void undeclared_function(FuncTab*, calledStuff*);

// Fonction du sujet
void unused_variable(VarTab*, calledStuff*);

// Vérifie que la variable est utilisée dans la fonction où est instanciée la variable
int isInFunc(Functions*, int);

// Fonction du sujet
void undeclared_variable(VarTab*, calledStuff*);

