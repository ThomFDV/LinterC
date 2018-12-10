#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parcours.h"

void goThroughSpaces(FILE *f) {
    char c;
    while((c = fgetc(f)) == ' '){}
    fseek(f, -1, SEEK_CUR);
}

void goThroughParams(FILE *f) {
    char c;
    int countParentheses = 1;
    while(countParentheses){
        c = fgetc(f);
        if(c == '"') {
            goThroughStrings(f);
        }
        if(c == '(') {
            countParentheses++;
        }
        if(c == ')') {
            countParentheses--;
        }
    }
}

void goThroughStrings(FILE *f) {
    char c;
    int countQuote = 1;
    while(countQuote) {
        c = fgetc(f);
        if(c == '\\') {
            fseek(f, 1, SEEK_CUR);
            continue;
        }
        if(c == '"') {
            countQuote--;
        }
    }
}

void goThroughLine(FILE* f) {
    //char c;
    while(fgetc(f) != '\n') {}
}

void goThroughComment(FILE *f, int *line) {
    char c;
    int out = 1;
    while(out) {
        c = fgetc(f);
        if(c == '*') {
            if((c = fgetc(f)) == '/') {
                out = 0;
            } else {
                fseek(f, -1, SEEK_CUR);
            }
        } else if(c == '\n') {
            *line += 1;
        }
    }
}

