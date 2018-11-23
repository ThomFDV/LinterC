#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "functions.h"

int main(int argc, char **argv) {
    int recursive; // Entier qui va indiquer si Linter va s'executer en recursif ou non
    char *filename = malloc(sizeof(char) * 100);

    StringTabs *rules = malloc(sizeof(StringTabs)); // Tableau de chaines de caract�res contenant les noms des r�gles et leur valeur
    StringTabs *excludedFiles = malloc(sizeof(StringTabs)); // Tableau de chaines de caract�res contenant les noms des fichiers � ne pas v�rifier
    StringTabs *analyzedFiles = malloc(sizeof(StringTabs));

    initTabs(rules, 10);
    initTabs(excludedFiles, 10);
    initTabs(analyzedFiles, 10);

    strcpy(filename, "default.lconf");

    // Fonction servant � r�cup�rer les param�tres du fichier de configuration
    recursive = parseConf(rules, excludedFiles, filename);
    // Fonction r�cup�rant les fichiers � analyser
    browseDir(analyzedFiles, "analyse", excludedFiles, recursive);
    // Pour chaque fichier, lancer les r�gles

    freeTabs(rules);
    freeTabs(excludedFiles);
    freeTabs(analyzedFiles);

    free(filename);

    system("pause");
    return 0;
}
