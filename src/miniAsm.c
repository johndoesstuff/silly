#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

typedef enum {
	TOKEN_KEYWORD,
	TOKEN_NUMBER,
	TOKEN_IDENTIFIER,
	TOKEN_VARIABLE_VALUE,
} TokenType;

typedef enum {
	NODE_PROGRAM,
	NODE_STATEMENT,
	NODE_ARGUMENT,
} NodeType;

#define TOKEN_RULE_COUNT (sizeof(tokenRules) / sizeof(TokenRule))

Token getNextToken(const char **input) {
        Token token = { .type = TOKEN_UNKNOWN, .value = "" };
        while (**input == ' ' || **input == '\t') {
                (*input)++;
        }

        if (**input == '\0') {
                token.type = TOKEN_END;
                return token;
        }

        for (int i = 0; i < TOKEN_RULE_COUNT; i++) {
                regmatch_t match;
                if (regexec(&tokenRules[i].regex, *input, 1, &match, 0) == 0) {
                        size_t matchLength = match.rm_eo - match.rm_so;
                        strncpy(token.value, *input, matchLength);
                        token.value[matchLength] = '\0';
                        token.type = tokenRules[i].type;
                        *input += matchLength;
                        return token;
                }
        }

        token.value[0] = **input;
        token.value[1] = '\0';
        (*input)++;
        return token;
}

Token* getTokensFromFile(FILE *file, size_t *tokenCount) {
	
	size_t capacity = 16;
	Token *tokens = malloc(capacity * sizeof(Token));
	if (!tokens) {
                perror("Failed to allocate memory for tokens");
                exit(1);
        }

        *tokenCount = 0;

        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file)) {
                const char *input = buffer;
                Token token;
                while ((token = getNextToken(&input)).type != TOKEN_END) {
                        if (*tokenCount >= capacity) {
                                capacity *= 2;
                                Token *newTokens = realloc(tokens, capacity * sizeof(Token));
                                if (!newTokens) {
                                        perror("Failed to reallocate memory for tokens");
                                        free(tokens);
                                        exit(1);
                                }
                                tokens = newTokens;
                        }

                        tokens[*tokenCount] = token;
                        (*tokenCount)++;
                }
        }

        for (int i = 0; i < TOKEN_RULE_COUNT; i++) {
                regfree(&tokenRules[i].regex);
        }

        return tokens;
}

int main(int argc, char *argv[]) {
        if (argc != 2) {
                fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
                return 1;
        }

        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
                perror("Failed to open file");
                return 1;
        }

        size_t tokenCount = 0;
        Token *tokens = getTokensFromFile(file, &tokenCount);

        for (int i = 0; i < tokenCount; i++) {
                Token token = tokens[i];
                printf("Token: Type = %d, Value = '%s'\n", token.type, token.value);
        }

        fclose(file);
        return 0;
}
