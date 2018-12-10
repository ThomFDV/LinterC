#include <stdio.h>
#include "struct.h"

// Valeur de retour : si la v�rification de fichier par le programme est r�cursive (1) ou non (0)
int parseConf(StringTabs*, StringTabs*, char*);

void getDirToAnalyze(char*);

// Parse les r�gles dans le fichier de configuration
void parseRules(FILE*, StringTabs*);

// Charge la configuration du  fichier dont d�coule le fichier initial
// Retourne le nom du fichier dont il d�pend si il y en a un, sinon NULL
char *parseExtends(FILE*);

// V�rifie si l'analyse des fichiers doit se faire de mani�re r�cursive
// Renvoie 1 si r�cursif, sinon 0
int parseRecursive(FILE*);

// Parse les fichiers exclus de l'analyse de LinterC
void parseExcludedFiles(FILE*, StringTabs*);

// Ajoute ou modifie une r�gle de la liste de r�gles. Aggrandit le tableau si besoin.
void modifyRules(char*, StringTabs*);

// Ajoute ou modifie un fichier � la liste des fichiers exclus. Aggrandit le tableau si besoin.
void addExcludedFile(char*, StringTabs*);

// Test de parcours de dossier
void browseDir(StringTabs*, char*, StringTabs*, int);

// Initialise les tableaux de chaines de caract�res
StringTabs *initTabs(int);

// Lib�re l'espace allou� aux tableaux de chaines de caract�res
void freeTabs(StringTabs*);

// Aggrandi le nombre de lignes possibles pour un tableau de chaines de caract�res
void increaseTabSize(StringTabs*);

// Ajout d'une chaine de caract�re dans le tableau
void addString(StringTabs*, char*);

// V�rifie que le fichier a bien une extension .c
int isValidCFile(char*, StringTabs*);

// V�rifie si la chaine est un r�pertoire
int isDir(char*);

// Concat�ne le chemin d'acc�s et le nom du fichier pour soit le stocker, soit ouvrir un nouveau dossier
void getPath(char*, char*, char*);

// Saisie du chemin pour le dossier � analyser
void getDirToAnalyze(char*);

int isPathADir(char*);

void modifySpecialChar(char*);

/*
    PT1
    Le prototype �a serait : void nom_fonction(StringTabs*, StringTabs*);
*/

// V�rifie si la regle en parametre existe.// Retourne 1 si elle existe et que sa valeur sera n|off,// retourne 2 si elle existe et que sa valeur sera on|off,// retourne 0 si elle n'existe pas ou si elle est mal ecrite.
int check_if_rules_exist(char*);

// Recupere le chiffre en char*, retourne le chiffre converti en int
int recup_number_of_rule(int, char*);

// Recupere la valeur (on ou off) en char*
int recup_value_of_rule(int, char*);

// Execute les regles si elles sont "on" ou si "n" > 0
void exec_rules(StringTabs*, StringTabs*);

// Avance le curseur de 1 si un commentaire est rencontr�
void isComment(char* c, FILE* f, int* lines);

// Avance le curseur de 1 si des guillemets sont rencontr�s
void isQuote(char* c, FILE* f);

// Retourne 1 si un saut a la ligne est rencontr�
int isLine(char* c);

// Passe la ligne si c'est un include
void isSharp(char*, FILE*, int*);

/* ************************************************************************************************************************* */
/*                                                          RULES                                                            */
/* ************************************************************************************************************************* */

// L�accolade doit se trouver en fin de ligne pour les fonctions, if, boucles,
void array_bracket_eol(char*);

// Il doit avoir un espace � droite d�une virgule.
void comma_spacing(char*);

// Test la pr�sence d�un commentaire multi-ligne en ent�te de fichier.
void comments_header(char*);

// Les fichiers ne doivent pas d�passer n lignes.
void max_file_line_numbers(char*, int);

// V�rifie si l'indentation est correcte
void checkIndent(int* n, char* filename);

// V�rifie si le nombre de char est bien respect�
void maxLineNumbers(char* filename, int* n);

// V�rifie s'il n'y a pas d'espaces inutiles
void trailingSpaces(char* filename);

// V�rifie s'il y a bien un espace entre chaque op�rateurs
void operatorsSpacing(char* filename);

/* ------------------------------------ CORENTIN ------------------------------------- */

int isADir(char*);

// R�cup�re le type, le nom et les param�tres de toutes les fonctions du fichier
void getFuncAndVariables(FILE*, FuncTab*, FuncTab*, VarTab*, calledStuff*, calledStuff*);

// Si la ligne commence avec un type, le renvoie, sinon renvoie NULL
int startWithType(FILE*, char*);

// Parcours de tous les �ventuels espaces
void goThroughSpaces(FILE*);

// Renvoie 0 si le caract�re n'est pas valide pour un nom de fonction
int isValidChar(char);

// Parcours le contenu entre parenth�ses
void goThroughParams(FILE*);

// Parcours la chaine de caract�re
void goThroughStrings(FILE*);

// Parcours le reste ou la ligne enti�re
void goThroughLine(FILE*, int*, int*);

// Parcours un commentaire multiligne. Incr�mente le nombre de lignes si besoin
void goThroughComment(FILE*, int*);

// Initialisation du tableau de fonctions
FuncTab *initFuncTab(int);

// Ajout d'une fonction au tableau
void addFunction(FuncTab*, char*, char*, int, int);

// Lib�re la m�moire allou�e au tableau de fonctions
void freeFuncTab(FuncTab*);

// Initialisation d'un tableau de variables
VarTab *initVarTab(int);

// Ajout d'une fonction au tableau
void addVariable(VarTab*, char*, char*, Functions*, int);

// Lib�re la m�moire allou�e au tableau de fonctions
void freeVarTab(VarTab*);

// R�cup�re les param�tres d'une fonction
void getParams(FILE*, long, Functions*, VarTab*, int);

// Parcours jusqu'au prochain param�tre ou la fin de la d�claration de fonction si il n'y a plus de param�tres
int goToNextParam(FILE*);

// Retourne le nom du type que l'on ne connait pas, pour ne pas bloquer l'analyse de la fonction et des parametres
char *getUnknownType(FILE*);

// parse une ligne pour voir si il y a des variables utilis�es ou des fonctions appel�es
void analyzeLine(FILE*, int, int*, FuncTab*, calledStuff*, calledStuff*);

// V�rifie que chaque fonction a bien un prototype
void no_prototype(FuncTab*, FuncTab*);

// V�rifie que la fonction n'est pas dans les includes de base
int isAnIncludedFunc(char*);

// Si c'est un nombre, pas la peine de l'analysze avec les autres variables ou fonctions d�clar�es
int isJustANumber(char*);

// v�rifie que c'est pas un while, for, if etc...
int isAnInstruction(char*);

// Initialise une liste de fonctions ou variables appel�es
calledStuff *initCalledStuff(int);

// Ajoute une ligne � la liste
void addCalledStuff(calledStuff*, char*, int);

// Lib�re la place de la liste en m�moire
void freeCalledStuff(calledStuff*);

// V�rifie que la premi�re variable Functions* est un prototype de la deuxi�me
int isAPrototypeOf(Functions*, Functions*);

// V�rifie si la variable est une constante connue ou non
int isAConstant(char*);

// Renvoie 1 si c'est un carct�re qui peut potentiellement suivre une variable, sinon 0
int isFollowedByRightOps(char);

// Fonction du sujet
void no_prototype(FuncTab*, FuncTab*);

// Fonction du sujet
void unused_function(FuncTab*, calledStuff*);

// Fonction du sujet
void undeclared_function(FuncTab*, calledStuff*);

// Fonction du sujet
void unused_variable(VarTab*, calledStuff*);

// V�rifie que la variable est utilis�e dans la fonction o� est instanci�e la variable
int isInFunc(Functions*, int);

// Fonction du sujet
void undeclared_variable(VarTab*, calledStuff*);

