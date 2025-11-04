#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    T_ID,
    T_KEYWORD,
    T_NUM,
    T_PLUS, T_MINUS, T_STAR, T_SLASH,
    T_LT, T_LE, T_GT, T_GE,
    T_EQ, T_EQEQ,
    T_NEQ,
    T_ASSIGN, /* = */
    T_SEMI, T_COMMA,
    T_LPAREN, T_RPAREN, T_LBRACK, T_RBRACK, T_LBRACE, T_RBRACE,
    T_ERROR,
    T_EOF
} TokenType;

const char *token_type_str[] = {
    "ID", "KEYWORD", "NUM",
    "PLUS", "MINUS", "STAR", "SLASH",
    "LT", "LE", "GT", "GE",
    "EQ", "EQEQ",
    "NEQ",
    "ASSIGN",
    "SEMI", "COMMA",
    "LPAREN", "RPAREN", "LBRACK", "RBRACK", "LBRACE", "RBRACE",
    "ERROR",
    "EOF"
};

const char *keywords[] = {
    "else", "if", "int", "return", "void", "while", NULL
};


int is_letter(int c) { return isalpha((unsigned char)c); }
int is_digit(int c) { return isdigit((unsigned char)c); }
int is_alnum_custom(int c) { return isalnum((unsigned char)c); }

/* Verifica se o lexema é uma palavra chave */
int is_keyword(const char *s) {
    for (const char **k = keywords; *k != NULL; ++k) {
        if (strcmp(s, *k) == 0) return 1;
    }
    return 0;
}

int peek_char(FILE *f) {
    int c = fgetc(f);
    if (c == EOF) return EOF;
    ungetc(c, f);
    return c;
}

void append_char(char **buf, size_t *len, size_t *cap, char c) {
    if (*cap == 0) {
        *cap = 32;
        *buf = malloc(*cap);
        if (!*buf) { perror("malloc"); exit(1); }
        *len = 0;
        (*buf)[0] = '\0';
    }
    if (*len + 2 > *cap) { /* +1 for new char, +1 for '\0' */
        *cap *= 2;
        *buf = realloc(*buf, *cap);
        if (!*buf) { perror("realloc"); exit(1); }
    }
    (*buf)[(*len)++] = c;
    (*buf)[(*len)] = '\0';
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_entrada.cm>\n", argv[0]);
        return 1;
    }

    const char *nome = argv[1];

    const char *ext = strrchr(nome, '.');
    if (!ext || strcmp(ext, ".cm") != 0) {
        fprintf(stderr, "Aviso: o arquivo '%s' nao termina com .cm (continuando)\n", nome);
    }

    FILE *f = fopen(nome, "r");
    if (!f) { perror("Erro ao abrir arquivo"); return 1; }

    printf("🔍 Lendo código-fonte: %s\n", nome);
    printf("-----------------------------\n");

    int c;


    char *lex = NULL;
    size_t lex_len = 0, lex_cap = 0;

    while (1) {
        c = fgetc(f);
        if (c == EOF) {
            printf("%s : %s\n", token_type_str[T_EOF], "EOF");
            break;
        }

        
        if (isspace((unsigned char)c)) continue;

        
        lex_len = 0;
        if (!lex) {
            lex_cap = 32;
            lex = malloc(lex_cap);
            if (!lex) { perror("malloc"); fclose(f); return 1; }
        }
        lex[0] = '\0';

        
        if (is_letter(c)) {
            append_char(&lex, &lex_len, &lex_cap, (char)c);
            while (1) {
                int nx = peek_char(f);
                if (nx != EOF && is_alnum_custom(nx)) {
                    append_char(&lex, &lex_len, &lex_cap, (char)fgetc(f));
                } else break;
            }
            if (is_keyword(lex)) {
                printf("%s : %s\n", token_type_str[T_KEYWORD], lex);
            } else {
                printf("%s : %s\n", token_type_str[T_ID], lex);
            }
        } else if (is_digit(c)) {
            append_char(&lex, &lex_len, &lex_cap, (char)c);
            while (1) {
                int nx = peek_char(f);
                if (nx != EOF && is_digit(nx)) {
                    append_char(&lex, &lex_len, &lex_cap, (char)fgetc(f));
                } else break;
            }
            printf("%s : %s\n", token_type_str[T_NUM], lex);
        } else {
            switch (c) {
                case '+':
                    printf("%s : %s\n", token_type_str[T_PLUS], "+");
                    break;
                case '-':
                    printf("%s : %s\n", token_type_str[T_MINUS], "-");
                    break;
                case '*':
                    printf("%s : %s\n", token_type_str[T_STAR], "*");
                    break;
                case '/': {
                    int nx = fgetc(f);
                    if (nx == '*') {
                        /* enter comment: consume until end of comment (star-slash) */
                        int prev = 0;
                        int ended = 0;
                        while ((c = fgetc(f)) != EOF) {
                            if (prev == '*' && c == '/') { ended = 1; break; }
                            prev = c;
                        }
                        if (!ended) {
                            printf("%s : %s\n", token_type_str[T_ERROR], "Unterminated comment");
                            /* keep scanning after reporting error */
                        } else {
                            /* comment closed; do not emit token, continue */
                        }
                    } else {
                        if (nx != EOF) ungetc(nx, f);
                        printf("%s : %s\n", token_type_str[T_SLASH], "/");
                    }
                    break;
                }
                case '<': {
                    int nx = fgetc(f);
                    if (nx == '=') {
                        printf("%s : %s\n", token_type_str[T_LE], "<=");
                    } else {
                        if (nx != EOF) ungetc(nx, f);
                        printf("%s : %s\n", token_type_str[T_LT], "<");
                    }
                    break;
                }
                case '>': {
                    int nx = fgetc(f);
                    if (nx == '=') {
                        printf("%s : %s\n", token_type_str[T_GE], ">=");
                    } else {
                        if (nx != EOF) ungetc(nx, f);
                        printf("%s : %s\n", token_type_str[T_GT], ">");
                    }
                    break;
                }
                case '=': {
                    int nx = fgetc(f);
                    if (nx == '=') {
                        printf("%s : %s\n", token_type_str[T_EQEQ], "==");
                    } else {
                        if (nx != EOF) ungetc(nx, f);
                        printf("%s : %s\n", token_type_str[T_ASSIGN], "=");
                    }
                    break;
                }
                case '!': {
                    int nx = fgetc(f);
                    if (nx == '=') {
                        printf("%s : %s\n", token_type_str[T_NEQ], "!=");
                    } else {
                        if (nx != EOF) ungetc(nx, f);
                        printf("%s : %s\n", token_type_str[T_ERROR], "!");
                    }
                    break;
                }
                case ';':
                    printf("%s : %s\n", token_type_str[T_SEMI], ";");
                    break;
                case ',':
                    printf("%s : %s\n", token_type_str[T_COMMA], ",");
                    break;
                case '(':
                    printf("%s : %s\n", token_type_str[T_LPAREN], "(");
                    break;
                case ')':
                    printf("%s : %s\n", token_type_str[T_RPAREN], ")");
                    break;
                case '[':
                    printf("%s : %s\n", token_type_str[T_LBRACK], "[");
                    break;
                case ']':
                    printf("%s : %s\n", token_type_str[T_RBRACK], "]");
                    break;
                case '{':
                    printf("%s : %s\n", token_type_str[T_LBRACE], "{");
                    break;
                case '}':
                    printf("%s : %s\n", token_type_str[T_RBRACE], "}");
                    break;
                default: {
                    char msg[64];
                    if (isprint((unsigned char)c)) {
                        snprintf(msg, sizeof(msg), "Invalid char '%c'", c);
                    } else {
                        snprintf(msg, sizeof(msg), "Invalid char (code %d)", c);
                    }
                    printf("%s : %s\n", token_type_str[T_ERROR], msg);
                    break;
                }
            }
        }
    }

    free(lex);
    fclose(f);
    printf("-----------------------------\n");
    printf("Varredura finalizada.\n");
    return 0;
}
