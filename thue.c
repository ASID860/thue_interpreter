#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct rule {
    char lhs[64];
    char rhs[64];
};

void changeNul(char *p) {
    if (p) *p = '\0';
}

int main(int argc, char **argv) {
    int i, j, state = 0;
    struct rule rules[256];
    char *c, *minc, code[2048]={0}, buffer[256], tmp[2048];
    FILE *infile;

    if (argc > 1) {
        if ((infile=fopen(argv[argc-1], "r")) == NULL) {
            fprintf(stderr, "error: Can't open file [%s]\n", argv[argc-1]);
            exit(1);
        }
    } else {
        infile = stdin;
    }

    for (i=0; fgets(buffer, 256, infile) != NULL ;) {
        if (!state) {
            changeNul(strstr(buffer, "\r"));
            changeNul(strstr(buffer, "\n"));
            c = strstr(buffer, "::=");
            if (c == buffer) {
                state = 1;
            } else if (c) {
                *c = '\0';
                c += strlen("::=");
                strncpy(rules[i].lhs, buffer, 64);
                strncpy(rules[i].rhs, c, 64);
                i++;
            }
        } else {
            changeNul(strstr(buffer, "\r"));
            changeNul(strstr(buffer, "\n"));
            strncat(code, buffer, 2048);
        }
    }

    while (1) {
        for (j=0,minc=NULL; j < i ;j++) {
            c = strstr(code, rules[j].lhs);
            if (minc == NULL || (c != NULL && c < minc)) {
                minc = c;
                state = j;
            }
        }
        if (minc == NULL) break;
        *minc = '\0';

        if (rules[state].rhs[0] == '~') {
            printf("%s", rules[state].rhs+1);
            snprintf(tmp, 2048, "%s%s",
                code,
                minc+strlen(rules[state].lhs));
        } else if (!strcmp(rules[state].rhs, ":::")) {
            fgets(buffer, 2048, stdin);
            changeNul(strstr(buffer, "\r"));
            changeNul(strstr(buffer, "\n"));
            snprintf(tmp, 2048, "%s%s%s",
                code,
                buffer,
                minc+strlen(rules[state].lhs));
        } else {
            snprintf(tmp, 2048, "%s%s%s",
                code,
                rules[state].rhs,
                minc+strlen(rules[state].lhs));
#ifdef DEBUG
            fprintf(stderr, "-> %s\n", tmp);
#endif
        }
        strcpy(code, tmp);
    }
#ifdef DEBUG
    fprintf(stderr, "code: %s\n", code);
#endif
    return 0;
}
