#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


//token types
typedef enum {
    TOKEN_INT,
    TOKEN_PRINT,
    TOKEN_ID,
    TOKEN_NUMBER,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_SEMI,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF,
    TOKEN_INVALID
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[50];
} Token;

FILE *inputfile;

// Function prototypes 
void program();
void statement_list();
void statement();
void assign();
int expr();
int factor(int inherited);
int term();
void print_stmt();

void storeVariable(char *name, int value);
int getVariableValue(char *name);


//Tokeniser logic
Token getNextToken() {
    Token token;
    char ch;

    fscanf(inputfile, " %c", &ch);

    if (isalpha(ch)) {
        int i = 0;
        token.lexeme[i++] = ch;
        while (isalnum(ch = fgetc(inputfile)))
            token.lexeme[i++] = ch;
        ungetc(ch, inputfile);
        token.lexeme[i] = '\0';

        if (strcmp(token.lexeme, "int") == 0)
            token.type = TOKEN_INT;
        else if (strcmp(token.lexeme, "print") == 0)
            token.type = TOKEN_PRINT;
        else
            token.type = TOKEN_ID;
    }
    else if (isdigit(ch)) {
        int i = 0;
        token.lexeme[i++] = ch;
        while (isdigit(ch = fgetc(inputfile)))
            token.lexeme[i++] = ch;
        ungetc(ch, inputfile);
        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
    }
    else {
        switch (ch) {
            case '=': token.type = TOKEN_ASSIGN; break;
            case '+': token.type = TOKEN_PLUS; break;
            case ';': token.type = TOKEN_SEMI; break;
            case '(': token.type = TOKEN_LPAREN; break;
            case ')': token.type = TOKEN_RPAREN; break;
            case EOF: token.type = TOKEN_EOF; break;
            default: token.type = TOKEN_INVALID;
        }
    }
    return token;
}

//Parser logic
Token currentToken;

void match(TokenType expected) {
    if (currentToken.type == expected)
        currentToken = getNextToken();
    else {
        printf("Syntax Error\n");
        exit(1);
    }
}

//<program>
void program() {
    if (currentToken.type == TOKEN_EOF)
        return;

    if (currentToken.type != TOKEN_INT &&
        currentToken.type != TOKEN_PRINT) {
        printf("Syntax Error: Invalid start of program\n");
        exit(1);
    }

    statement_list();
}
//<statement_list>
void statement_list() {
    if (currentToken.type == TOKEN_INT ||
        currentToken.type == TOKEN_PRINT) {
        statement();
        statement_list();
    } 
}
//<statement>
void statement() {
    if (currentToken.type == TOKEN_INT)
        assign();
    else if (currentToken.type == TOKEN_PRINT)
        print_stmt();
    else {
        printf("Syntax Error: Invalid statement\n");
        exit(1);
    }
}
//<assign>
void assign() {
    match(TOKEN_INT);

    char varName[50];
    strcpy(varName, currentToken.lexeme);
    match(TOKEN_ID);

    match(TOKEN_ASSIGN);
    int value = expr();
    match(TOKEN_SEMI);

    storeVariable(varName, value);
}
//<expr>
int expr() {
    int val = term();
    return factor(val);
}
//<factor>
int factor(int inherited) {
    if (currentToken.type == TOKEN_PLUS) {
        match(TOKEN_PLUS);
        int t = term();
        return factor(inherited + t);
    }
    return inherited;
}
//<term>
int term() {
    int value;

    if (currentToken.type == TOKEN_ID) {
        value = getVariableValue(currentToken.lexeme);
        match(TOKEN_ID);
    }
    else if (currentToken.type == TOKEN_NUMBER) {
        value = atoi(currentToken.lexeme); //atio converts string to integer
        match(TOKEN_NUMBER);
    }
    else {
        printf("Syntax Error in expression\n");
        exit(1);
    }
    return value;
}

//Semantic Error Handlling
typedef struct {
    char name[50];
    int value;
} Symbol;

Symbol table[100];
int symbolCount = 0;

void storeVariable(char *name, int value) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(table[i].name, name) == 0) {
            printf("Semantic Error: Variable redeclared\n");
            exit(1);
        }
    }
    strcpy(table[symbolCount].name, name);
    table[symbolCount].value = value;
    symbolCount++;
}

int getVariableValue(char *name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(table[i].name, name) == 0)
            return table[i].value;
    }
    printf("Semantic Error: Variable not declared\n");
    exit(1);
}

//<print_stmt>
void print_stmt() {
    match(TOKEN_PRINT);
    match(TOKEN_LPAREN);

    char var[50];
    strcpy(var, currentToken.lexeme);
    match(TOKEN_ID);

    match(TOKEN_RPAREN);
    match(TOKEN_SEMI);

    printf("%d\n", getVariableValue(var));
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./parser inputfile\n");
        return 1;
    }

    inputfile = fopen(argv[1], "r");
    if (!inputfile) {
        printf("File error\n");
        return 1;
    }

    currentToken = getNextToken();
    program();

    fclose(inputfile);
    return 0;
}



