#include <stdio.h>

// Parcours de tous les éventuels espaces
void goThroughSpaces(FILE*);

// Parcours le contenu entre parenthèses
void goThroughParams(FILE*);

// Parcours la chaine de caractère
void goThroughStrings(FILE*);

// Parcours le reste ou la ligne entière
void goThroughLine(FILE*);

// Parcours un commentaire multiligne. Incrémente le nombre de lignes si besoin
void goThroughComment(FILE*, int*);
