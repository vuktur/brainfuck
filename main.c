#include <stdio.h>
#include <stdlib.h>

#define TAPE_LENGTH 1000
#define PROG_MAX_LENGTH 1000

int readFile(char *fileName, char* prog)
{
    FILE* file = fopen(fileName, "r");
    if (!file) return -1;

    int i, c;
    for (i = 0; i < PROG_MAX_LENGTH; ++i) {
        if ((c = getc(file)) == EOF)
            break;
        *(prog+i) = c;
    }
    *(prog+i) = 0;
    
    fclose(file);
    return i;
}

int interpret(char* tape, char** head, char* prog, int proglen, int* cmd) {
    int loopCounter = 0;
    switch (*(prog+*cmd)) {
        case '+':
            ++**head;
            break;

        case '-':
            --**head;
            break;

        case '>':
            if (*head >= tape+TAPE_LENGTH-1) {
                printf("error: the head moved out of the bounds of the tape (0 to %d)\n", TAPE_LENGTH);
                return 1;
            }
            ++*head;
            break;

        case '<':
            if (*head <= tape) {
                printf("error: the head moved out of the bounds of the tape (0 to %d)\n", TAPE_LENGTH);
                return 1;
            }
            --*head;
            break;

        case '[':
            if (**head != 0) break;
            ++*cmd;
            while (*(prog+*cmd) != ']' || loopCounter > 0) {
                if (*cmd > proglen) {
                    printf("error: there was an error during a loop, and the command pointer is out of bounds\n");
                    return 1;
                }
                if (*(prog+*cmd) == '[')
                    ++loopCounter;
                if (*(prog+*cmd) == ']')
                    --loopCounter;
                ++*cmd;
            }
            break;

        case ']':
            --*cmd;
            while (*(prog+*cmd) != '[' || loopCounter > 0) {
                if (*cmd < 0) {
                    printf("error: there was an error during a loop, and the command pointer is out of bounds\n");
                    return 1;
                }
                if (*(prog+*cmd) == ']')
                    ++loopCounter;
                if (*(prog+*cmd) == '[')
                    --loopCounter;
                --*cmd;
            }
            --*cmd;
            break;

        case '.': 
            printf("%c", **head);
//            printf("printing value: %c\n", **head);
            break;

        case ',':
            **head = getchar();
            break;

        case '\n': case '\r': case '\t': case ' ':
            break;

        default:
            printf("error: an unknown character has been encountered\n");
            return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("error: the file cant be read\n");
        return 1;
    }

    char* prog = malloc(PROG_MAX_LENGTH*sizeof(char));
    
    int proglen = readFile(argv[1], prog);

    if (proglen == -1) {
        free(prog);
        printf("error: the file cant be read\n");
        return 1;
    }

    char* tape = malloc(TAPE_LENGTH*sizeof(char));
    char* head = tape;
    
    int status = 0;
    for(int cmd = 0; cmd < proglen; ++cmd) {
        status = interpret(tape, &head, prog, proglen, &cmd);
        if (status != 0) break;
    }
    
    printf("\n");
    free(prog); 
    free(tape);
    return status;
}
