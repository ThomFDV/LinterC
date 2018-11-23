#include <stdio.h>
#include "struct.h"

// Valeur de retour : si la vérification de fichier par le programme est récursive (1) ou non (0)
int parseConf(StringTabs*, StringTabs*, char*);

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
void initTabs(StringTabs*, int);

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
