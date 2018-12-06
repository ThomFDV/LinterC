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

