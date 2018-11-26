#include <stdio.h>

// Parcours de tous les �ventuels espaces
void goThroughSpaces(FILE*);

// Parcours le contenu entre parenth�ses
void goThroughParams(FILE*);

// Parcours la chaine de caract�re
void goThroughStrings(FILE*);

// Parcours le reste ou la ligne enti�re
void goThroughLine(FILE*);

// Parcours un commentaire multiligne. Incr�mente le nombre de lignes si besoin
void goThroughComment(FILE*, int*);
