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

