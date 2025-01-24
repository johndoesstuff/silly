#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

typedef enum {
	TOKEN_KEYWORD,
	TOKEN_NUMBER,
	TOKEN_STRING,
	TOKEN_IDENTIFIER,
	TOKEN_UNKNOWN,
	TOKEN_END,
} TokenType;

typedef struct {
	TokenType type;
	char value[100];
} Token;

typedef struct {
	regex_t regex;
	TokenType type;
} TokenRule;

TokenRule tokenRules[] = {
	{ .type = TOKEN_KEYWORD, .regex = {0} },
	{ .type = TOKEN_NUMBER, .regex = {0} },
	{ .type = TOKEN_STRING, .regex = {0} },
	{ .type = TOKEN_IDENTIFIER, .regex = {0} },
};

#define TOKEN_RULE_COUNT (sizeof(tokenRules) / sizeof(TokenRule))

void compileTokenRules() {
	if (regcomp(&tokenRules[0].regex, "int|char|pointer", REG_EXTENDED)) {
		fprintf(stderr, "Failed to compile regex for KEYWORD\n");
		exit(1);
	}
	if (regcomp(&tokenRules[1].regex, "[0-9]+", REG_EXTENDED)) {
		fprintf(stderr, "Failed to compile regex for NUMBER\n");
		exit(1);
	}
	if (regcomp(&tokenRules[0].regex, "\".*?\"", REG_EXTENDED)) {
		fprintf(stderr, "Failed to compile regex for STRING\n");
		exit(1);
	}
	if (regcomp(&tokenRules[0].regex, "[a-zA-Z]+", REG_EXTENDED)) {
		fprintf(stderr, "Failed to compile regex for IDENTIFIER\n");
		exit(1);
	}
}

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

	char buffer[256];
	while (fgets(buffer, sizeof(buffer), file)) {
		printf("%s", buffer);
	}

	

	fclose(file);
	return 0;
}
