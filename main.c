#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "functions.h"
#include "parcours.h"

int main(int argc, char **argv) {
    int recursive; // Entier qui va indiquer si Linter va s'executer en recursif ou non
    char *filename = malloc(sizeof(char) * 40);
    char *path = malloc(sizeof(char) * 200);
    // int i;

    StringTabs *rules = initTabs(10); // Tableau de chaines de caract�res contenant les noms des r�gles et leur valeur
    StringTabs *excludedFiles = initTabs(10); // Tableau de chaines de caract�res contenant les noms des fichiers � ne pas v�rifier
    StringTabs *analyzedFiles = initTabs(10);

    strcpy(filename, "default.lconf");

    // Fonction servant � r�cup�rer les param�tres du fichier de configuration
    recursive = parseConf(rules, excludedFiles, filename);

    getDirToAnalyze(path);

    // Fonction r�cup�rant les fichiers � analyser
    browseDir(analyzedFiles, path, excludedFiles, recursive);
    //check_if_rules_exist(rules);
    system("pause");
    system("cls");
    exec_rules(rules, analyzedFiles);

    /*system("pause");
    system("cls");
    for(i = 0; i < analyzedFiles->size; i++) {
        printf("%s\n", analyzedFiles->tab[i]);
    }*/

    freeTabs(rules);
    freeTabs(excludedFiles);
    freeTabs(analyzedFiles);

    free(filename);

    system("pause");
    return 0;
}
