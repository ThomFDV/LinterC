#include <stdio.h>
#include "struct.h"

// Valeur de retour : si la v�rification de fichier par le programme est r�cursive (1) ou non (0)
int parseConf(StringTabs*, StringTabs*, char*);

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
void initTabs(StringTabs*, int);

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
