#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "functions.h"
#include "parcours.h"

int main(int argc, char **argv) {
    int recursive; // Entier qui va indiquer si Linter va s'executer en recursif ou non
    char *filename = malloc(sizeof(char) * 100);
    int i;

    StringTabs *rules = initTabs(10); // Tableau de chaines de caractères contenant les noms des règles et leur valeur
    StringTabs *excludedFiles = initTabs(10); // Tableau de chaines de caractères contenant les noms des fichiers à ne pas vérifier
    StringTabs *analyzedFiles = initTabs(10);

    strcpy(filename, "default.lconf");

    // Fonction servant à récupérer les paramètres du fichier de configuration
    recursive = parseConf(rules, excludedFiles, filename);
    // Fonction récupérant les fichiers à analyser
    browseDir(analyzedFiles, "analyse", excludedFiles, recursive);
    /*
        PT1
        Ici tu vas appeler ta fonction qui va lancer les règles
        Vu que dans rules y aura les règles que j'ai écrites
        dans le fichier lconf qui est à la racine, hésite pas à le modifier
        pour qu'il corresponde
        La fonction ressemblera à un truc du genre du coup :
        nom_fonction(rules, analyzedFiles);
    */

    system("pause");
    system("cls");
    for(i = 0; i < analyzedFiles->size; i++) {
        printf("%s\n", analyzedFiles->tab[i]);
    }

    freeTabs(rules);
    freeTabs(excludedFiles);
    freeTabs(analyzedFiles);

    free(filename);

    system("pause");
    return 0;
}
