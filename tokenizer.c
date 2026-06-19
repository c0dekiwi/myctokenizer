#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "utils.h"

#define \
	sam \
	10

/* Tokens list
 * (  : L_PARENTHESES
 * )  : R_PARENTHESES
 * {  : L_CURLY_BRACE
 * }  : R_CURLY_BRACE
 * [  : L_BRACKET
 * ]  : R_BRACKET
 * ;  : SEMI_COLON
 * =  : ASSIGN
 * .  : DOT
 * *  : ASTERISK
 * /  : FORWARD_SLASH
 * %  : MODULUS
 * +  : PLUS
 * -  : MINUS
 * !  : NOT
 * "  : DOUBLE_QUOTE
 * '  : SINGLE_QUOTE
 * :  : COLON
 * ?  : MARK
 * ~  : BITWISE_NOT
 * |  : BITWISE_OR
 * &  : BITWISE_AND
 * ^  : BITWISE_XOR
 * >> : BITWISE_LEFT_SHIFT
 * << : BITWISE_RIGHT_SHIFT
 * <  : LESS_THAN
 * >  : GREATER_THAN
 * == : EQUAL
 * != : NOT_EQUAL
 * <= : LESS_EQUAL
 * >= : GREATER_EQUAL
 *
 * STRING_LITTERAL
 * INTEGER
 * FLOAT
 *
 * More generaly, a token can be composed of either [letter & numbers] or
 * [special characters].
 */

typedef struct string_view {
	size_t len;
	char *str;
} string_view_s;

typedef enum token_type {
	TOKEN_TYPE_EMPTY,
	TOKEN_TYPE_NOT_YET_IMPLEMENTED,
	TOKEN_TYPE_WORD,
	TOKEN_TYPE_PREPROCESSOR,
	TOKEN_TYPE_IDENTIFIER,
	TOKEN_TYPE_INT_LITTERAL,
	TOKEN_TYPE_CHAR_LITTERAL,
	TOKEN_TYPE_FLOAT_LITTERAL,
	TOKEN_TYPE_STRING_LITTERAL,
	TOKEN_TYPE_R_BRACKET,
	TOKEN_TYPE_L_BRACKET,
	TOKEN_TYPE_R_CURLY_BRACE,
	TOKEN_TYPE_L_CURLY_BRACE,
	TOKEN_TYPE_R_PARENTHESES,
	TOKEN_TYPE_L_PARENTHESES,
	TOKEN_TYPE_DOT,
	TOKEN_TYPE_COLON,
	TOKEN_TYPE_SEMI_COLON,
	TOKEN_TYPE_DOUBLE_QUOTE,
	TOKEN_TYPE_SINGLE_QUOTE,
	TOKEN_TYPE_PLUS,
	TOKEN_TYPE_MINUS,
	TOKEN_TYPE_ASTERISK,
	TOKEN_TYPE_FORWARD_SLASH,
	TOKEN_TYPE_BLOCK_COMMENT_START,
	TOKEN_TYPE_BLOCK_COMMENT_END,
	TOKEN_TYPE_LINE_COMMENT,
	TOKEN_TYPE_ASSIGN,
	TOKEN_TYPE_COMMA,
	TOKEN_TYPE_NOT,
	TOKEN_TYPE_OR,
	TOKEN_TYPE_AND,
	TOKEN_TYPE_BITWISE_NOT,
	TOKEN_TYPE_BITWISE_OR,
	TOKEN_TYPE_BITWISE_XOR,
	TOKEN_TYPE_BITWISE_AND,
	TOKEN_TYPE_EQUAL,
	TOKEN_TYPE_NOT_EQUAL,
	TOKEN_TYPE_LESS_THAN,
	TOKEN_TYPE_LESS_THAN_EQUAL,
	TOKEN_TYPE_GREATER_THAN,
	TOKEN_TYPE_GREATER_THAN_EQUAL,
	TOKEN_TYPE_KEYWORD_IF,
	TOKEN_TYPE_KEYWORD_INT,
	TOKEN_TYPE_KEYWORD_FOR,
	TOKEN_TYPE_KEYWORD_LONG,
	TOKEN_TYPE_KEYWORD_CHAR,
	TOKEN_TYPE_KEYWORD_ELSE,
	TOKEN_TYPE_KEYWORD_VOID,
	TOKEN_TYPE_KEYWORD_CASE,
	TOKEN_TYPE_KEYWORD_ENUM,
	TOKEN_TYPE_KEYWORD_FLOAT,
	TOKEN_TYPE_KEYWORD_WHILE,
	TOKEN_TYPE_KEYWORD_BREAK,
	TOKEN_TYPE_KEYWORD_STRUCT,
	TOKEN_TYPE_KEYWORD_DOUBLE,
	TOKEN_TYPE_KEYWORD_RETURN,
	TOKEN_TYPE_KEYWORD_SWITCH,
	TOKEN_TYPE_KEYWORD_TYPEDEF,
} token_type_e;

typedef struct token {
	enum token_type type;
	struct string_view value;
} token_s;

int isalnum_(int c);
inline int isalnum_(int c) {
	return isalnum(c) || c == '_';
}

void print_token_type(token_type_e tt) {
	switch (tt) {
	case TOKEN_TYPE_EMPTY:               printf("EMPTY");               break;
	case TOKEN_TYPE_NOT_YET_IMPLEMENTED: printf("NOT_YET_IMPLEMENTED"); break;
	case TOKEN_TYPE_WORD:                printf("WORD");                break;
	case TOKEN_TYPE_PREPROCESSOR:        printf("PREPROCESSOR");        break;
	case TOKEN_TYPE_IDENTIFIER:          printf("IDENTIFIER");          break;
	case TOKEN_TYPE_INT_LITTERAL:        printf("INT_LITTERAL");        break;
	case TOKEN_TYPE_CHAR_LITTERAL:       printf("CHAR_LITTERAL");       break;
	case TOKEN_TYPE_FLOAT_LITTERAL:      printf("FLOAT_LITTERAL");      break;
	case TOKEN_TYPE_STRING_LITTERAL:     printf("STRING_LITTERAL");     break;
	case TOKEN_TYPE_R_BRACKET:           printf("R_BRACKET");           break;
	case TOKEN_TYPE_L_BRACKET:           printf("L_BRACKET");           break;
	case TOKEN_TYPE_R_CURLY_BRACE:       printf("R_CURLY_BRACE");       break;
	case TOKEN_TYPE_L_CURLY_BRACE:       printf("L_CURLY_BRACE");       break;
	case TOKEN_TYPE_R_PARENTHESES:       printf("R_PARENTHESES");       break;
	case TOKEN_TYPE_L_PARENTHESES:       printf("L_PARENTHESES");       break;
	case TOKEN_TYPE_DOT:                 printf("DOT");                 break;
	case TOKEN_TYPE_COLON:               printf("COLON");               break;
	case TOKEN_TYPE_SEMI_COLON:          printf("SEMI_COLON");          break;
	case TOKEN_TYPE_DOUBLE_QUOTE:        printf("DOUBLE_QUOTE");        break;
	case TOKEN_TYPE_SINGLE_QUOTE:        printf("SINGLE_QUOTE");        break;
	case TOKEN_TYPE_PLUS:                printf("PLUS");                break;
	case TOKEN_TYPE_MINUS:               printf("MINUS");               break;
	case TOKEN_TYPE_ASTERISK:            printf("ASTERISK");            break;
	case TOKEN_TYPE_FORWARD_SLASH:       printf("FORWARD_SLASH");       break;
	case TOKEN_TYPE_BLOCK_COMMENT_START: printf("BLOCK_COMMENT_START"); break;
	case TOKEN_TYPE_BLOCK_COMMENT_END:   printf("BLOCK_COMMENT_END");   break;
	case TOKEN_TYPE_LINE_COMMENT:        printf("LINE_COMMENT");        break;
	case TOKEN_TYPE_ASSIGN:              printf("ASSIGN");              break;
	case TOKEN_TYPE_COMMA:               printf("COMMA");               break;
	case TOKEN_TYPE_NOT:                 printf("NOT");                 break;
	case TOKEN_TYPE_OR:                  printf("OR");                  break;
	case TOKEN_TYPE_AND:                 printf("AND");                 break;
	case TOKEN_TYPE_BITWISE_NOT:         printf("BITWISE_NOT");         break;
	case TOKEN_TYPE_BITWISE_OR:          printf("BITWISE_OR");          break;
	case TOKEN_TYPE_BITWISE_XOR:         printf("BITWISE_XOR");         break;
	case TOKEN_TYPE_BITWISE_AND:         printf("BITWISE_AND");         break;
	case TOKEN_TYPE_EQUAL:               printf("EQUAL");               break;
	case TOKEN_TYPE_NOT_EQUAL:           printf("NOT_EQUAL");           break;
	case TOKEN_TYPE_LESS_THAN:           printf("LESS_THAN");           break;
	case TOKEN_TYPE_LESS_THAN_EQUAL:     printf("LESS_THAN_EQUAL");     break;
	case TOKEN_TYPE_GREATER_THAN:        printf("GREATER_THAN");        break;
	case TOKEN_TYPE_GREATER_THAN_EQUAL:  printf("GREATER_THAN_EQUAL");  break;
	case TOKEN_TYPE_KEYWORD_IF:          printf("KEYWORD_IF");          break;
	case TOKEN_TYPE_KEYWORD_INT:         printf("KEYWORD_INT");         break;
	case TOKEN_TYPE_KEYWORD_FOR:         printf("KEYWORD_FOR");         break;
	case TOKEN_TYPE_KEYWORD_LONG:        printf("KEYWORD_LONG");        break;
	case TOKEN_TYPE_KEYWORD_CHAR:        printf("KEYWORD_CHAR");        break;
	case TOKEN_TYPE_KEYWORD_ELSE:        printf("KEYWORD_ELSE");        break;
	case TOKEN_TYPE_KEYWORD_VOID:        printf("KEYWORD_VOID");        break;
	case TOKEN_TYPE_KEYWORD_CASE:        printf("KEYWORD_CASE");        break;
	case TOKEN_TYPE_KEYWORD_ENUM:        printf("KEYWORD_ENUM");        break;
	case TOKEN_TYPE_KEYWORD_FLOAT:       printf("KEYWORD_FLOAT");       break;
	case TOKEN_TYPE_KEYWORD_WHILE:       printf("KEYWORD_WHILE");       break;
	case TOKEN_TYPE_KEYWORD_BREAK:       printf("KEYWORD_BREAK");       break;
	case TOKEN_TYPE_KEYWORD_STRUCT:      printf("KEYWORD_STRUCT");      break;
	case TOKEN_TYPE_KEYWORD_DOUBLE:      printf("KEYWORD_DOUBLE");      break;
	case TOKEN_TYPE_KEYWORD_RETURN:      printf("KEYWORD_RETURN");      break;
	case TOKEN_TYPE_KEYWORD_SWITCH:      printf("KEYWORD_SWITCH");      break;
	case TOKEN_TYPE_KEYWORD_TYPEDEF:     printf("KEYWORD_TYPEDEF");     break;
	}
}

token_s extract_token(string_view_s *sv) {
	token_s token;
	size_t i;

	token.type = TOKEN_TYPE_EMPTY;
	if (!(*sv).len) return token;
	i = 0;
	if (isalnum_((*sv).str[0])) {
		while (i < (*sv).len && isalnum_((*sv).str[i])) {
			i = i + 1;
		}
		switch (i) {
		case 2:
			if (!strncmp((*sv).str, "if", 2)) {
				token.type = TOKEN_TYPE_KEYWORD_IF;
			}
			break;
		case 3:
			if (!strncmp((*sv).str, "int", 3)) {
				token.type = TOKEN_TYPE_KEYWORD_INT;
			}
			else if (!strncmp((*sv).str, "for", 3)) {
				token.type = TOKEN_TYPE_KEYWORD_FOR;
			}
			break;
		case 4:
			if (!strncmp((*sv).str, "else", 4)) {
				token.type = TOKEN_TYPE_KEYWORD_ELSE;
			}
			else if (!strncmp((*sv).str, "void", 4)) {
				token.type = TOKEN_TYPE_KEYWORD_VOID;
			}
			else if (!strncmp((*sv).str, "char", 4)) {
				token.type = TOKEN_TYPE_KEYWORD_CHAR;
			}
			else if (!strncmp((*sv).str, "long", 4)) {
				token.type = TOKEN_TYPE_KEYWORD_LONG;
			}
			else if (!strncmp((*sv).str, "case", 4)) {
				token.type = TOKEN_TYPE_KEYWORD_CASE;
			}
			else if (!strncmp((*sv).str, "enum", 4)) {
				token.type = TOKEN_TYPE_KEYWORD_ENUM;
			}
			break;
		case 5:
			if (!strncmp((*sv).str, "while", 5)) {
				token.type = TOKEN_TYPE_KEYWORD_WHILE;
			}
			else if (!strncmp((*sv).str, "float", 5)) {
				token.type = TOKEN_TYPE_KEYWORD_FLOAT;
			}
			else if (!strncmp((*sv).str, "break", 5)) {
				token.type = TOKEN_TYPE_KEYWORD_BREAK;
			}
			break;
		case 6:
			if (!strncmp((*sv).str, "struct", 6)) {
				token.type = TOKEN_TYPE_KEYWORD_STRUCT;
			}
			else if (!strncmp((*sv).str, "double", 6)) {
				token.type = TOKEN_TYPE_KEYWORD_DOUBLE;
			}
			else if (!strncmp((*sv).str, "return", 6)) {
				token.type = TOKEN_TYPE_KEYWORD_RETURN;
			}
			else if (!strncmp((*sv).str, "switch", 6)) {
				token.type = TOKEN_TYPE_KEYWORD_SWITCH;
			}
			break;
		case 7:
			if (!strncmp((*sv).str, "typedef", 7)) {
				token.type = TOKEN_TYPE_KEYWORD_TYPEDEF;
			}
			break;
		default:
			break;
		}
		if (token.type == TOKEN_TYPE_EMPTY) {
			if (isdigit((*sv).str[0])) {
				size_t j = 0;
				while (j < i && isdigit((*sv).str[j])) {
					j = j + 1;
				}
				if (j < i && (*sv).str[j] == '.') {
					j = j + 1;
					while (j < i && isdigit((*sv).str[j])) {
						j = j + 1;
					}
					if (j == i) {
						token.type = TOKEN_TYPE_FLOAT_LITTERAL;
					}
					else {
						token.type = TOKEN_TYPE_NOT_YET_IMPLEMENTED;
					}
				}
				else if (j == i) {
					token.type = TOKEN_TYPE_INT_LITTERAL;
				}
				else {
					token.type = TOKEN_TYPE_NOT_YET_IMPLEMENTED;
				}
			}
			else if (isalpha((*sv).str[0])) {
				token.type = TOKEN_TYPE_IDENTIFIER;
			}
		}
	}
	else {
		while (i < (*sv).len && !isalnum_((*sv).str[i]) && !isspace((*sv).str[i])) {
			i = i + 1;
		}
		token.type = TOKEN_TYPE_NOT_YET_IMPLEMENTED;
		switch ((*sv).str[0]) {
		case '"':
			token.type = TOKEN_TYPE_STRING_LITTERAL;
			i = 1;
			while (i < (*sv).len && (*sv).str[i] != '"') {
				if ((*sv).str[i] == '\\') {
					i = i + 1;
				}
				i = i + 1;
			}
			if (i < (*sv).len) {
				i = i + 1;
			}
			break;
		case '\'':
			token.type = TOKEN_TYPE_CHAR_LITTERAL;
			i = 1;
			while (i < (*sv).len && (*sv).str[i] != '\'') {
				if ((*sv).str[i] == '\\') {
					i = i + 1;
				}
				i = i + 1;
			}
			if (i < (*sv).len) {
				i = i + 1;
			}
			break;
		case '!':
			if (i == 1 || (i > 1 && (*sv).str[1] != '=')) {
				token.type = TOKEN_TYPE_NOT;
				i = 1;
			}
			else if (i > 1 && (*sv).str[1] == '=') {
				token.type = TOKEN_TYPE_NOT_EQUAL;
				i = 2;
			}
			break;
		case '~':
			token.type = TOKEN_TYPE_BITWISE_NOT;
			i = 1;
			break;
		case '^':
			token.type = TOKEN_TYPE_BITWISE_XOR;
			i = 1;
		case '<':
			if (i == 1 || (i > 1 && (*sv).str[1] != '=')) {
				token.type = TOKEN_TYPE_LESS_THAN;
				i = 1;
			}
			else if (i > 1 && (*sv).str[1] == '=') {
				token.type = TOKEN_TYPE_LESS_THAN_EQUAL;
				i = 2;
			}
			break;
		case '>':
			if (i == 1 || (i > 1 && (*sv).str[1] != '=')) {
				token.type = TOKEN_TYPE_GREATER_THAN;
				i = 1;
			}
			else if (i > 1 && (*sv).str[1] == '=') {
				token.type = TOKEN_TYPE_GREATER_THAN_EQUAL;
				i = 2;
			}
			break;
		case '#':
			token.type = TOKEN_TYPE_PREPROCESSOR;
			i = 0;
			while (i < (*sv).len && (*sv).str[i] != '\n') {
				while (i < (*sv).len && (*sv).str[i] == '\\') {
					i = i + 1;
				}
				i = i + 1;
			}
			break;
		case '|':
			if (i == 1 || (i > 1 && (*sv).str[1] != '|')) {
				token.type = TOKEN_TYPE_BITWISE_OR;
				i = 1;
			}
			else if (i > 1 && (*sv).str[1] == '|') {
				token.type = TOKEN_TYPE_OR;
				i = 2;
			}
			break;
		case '&':
			if (i == 1 || (i > 1 && (*sv).str[1] != '&')) {
				token.type = TOKEN_TYPE_BITWISE_AND;
				i = 1;
			}
			else if (i > 1 && (*sv).str[1] == '&') {
				token.type = TOKEN_TYPE_AND;
				i = 2;
			}
			break;
		case '.':
			token.type = TOKEN_TYPE_DOT;
			i = 1;
			break;
		case ',':
			token.type = TOKEN_TYPE_COMMA;
			i = 1;
			break;
		case '(':
			token.type = TOKEN_TYPE_L_PARENTHESES;
			i = 1;
			break;
		case ')':
			token.type = TOKEN_TYPE_R_PARENTHESES;
			i = 1;
			break;
		case '[':
			token.type = TOKEN_TYPE_L_BRACKET;
			i = 1;
			break;
		case ']':
			token.type = TOKEN_TYPE_R_BRACKET;
			i = 1;
			break;
		case '{':
			token.type = TOKEN_TYPE_L_CURLY_BRACE;
			i = 1;
			break;
		case '}':
			token.type = TOKEN_TYPE_R_CURLY_BRACE;
			i = 1;
			break;
		case '=':
			if (i == 1 || (i > 1 && (*sv).str[1] != '=')) {
				token.type = TOKEN_TYPE_ASSIGN;
				i = 1;
			}
			else if (i > 1 && (*sv).str[1] == '=') {
				token.type = TOKEN_TYPE_EQUAL;
				i = 2;
			}
			break;
		case ';':
			token.type = TOKEN_TYPE_SEMI_COLON;
			i = 1;
			break;
		case ':':
			token.type = TOKEN_TYPE_COLON;
			i = 1;
			break;
		case '+':
			token.type = TOKEN_TYPE_PLUS;
			i = 1;
			break;
		case '-':
			token.type = TOKEN_TYPE_MINUS;
			i = 1;
			break;
		case '*':
			if (i == 1 || (i > 1 && (*sv).str[1] != '/')) {
				token.type = TOKEN_TYPE_ASTERISK;
				i = 1;
			}
			else if (i > 1 && (*sv).str[1] == '/') {
				token.type = TOKEN_TYPE_BLOCK_COMMENT_END;
				i = 2;
			}
			break;
		case '/':
			if (i == 1 || (i > 1 && (*sv).str[1] != '/' && (*sv).str[1] != '*')) {
				token.type = TOKEN_TYPE_FORWARD_SLASH;
				i = 1;
			}
			else if (i > 1 && (*sv).str[1] == '/') {
				i = 0;
				while (i < (*sv).len && (*sv).str[i] != '\n') {
					i = i + 1;
				}
				token.type = TOKEN_TYPE_LINE_COMMENT;
			}
			else if (i > 1 && (*sv).str[1] == '*') {
				i = 0;
				while (i < (*sv).len-1 && strncmp(&(*sv).str[i], "*/", 2)) {
					i = i + 1;
				}
				if (i < (*sv).len-1) {
					i = i + 2;
				}
				token.type = TOKEN_TYPE_BLOCK_COMMENT_START;
			}
			break;
		default:
			break;
		}
	}
	token.value.str = (*sv).str;
	token.value.len = i;
	(*sv).len = (*sv).len - i;
	(*sv).str = (*sv).str + i;

	return token;
}

token_s* tokenize(char *buf) {
	if (!buf) return nul;

	size_t i;
	size_t it;
	string_view_s file;
	token_s *tokens;

	file.str = buf;
	file.len = strlen(buf);

	tokens = malloc(sizeof(*tokens) * file.len);
	if (!tokens) return nul;

	it = 0;
	while (file.len) {
		while (file.len && isspace(*file.str)) {
			file.len = file.len - 1;
			file.str = file.str + 1;
		}
		tokens[it] = extract_token(&file);
		it = it + 1;
	}
	return tokens;
}

int main(void) {
	// Lexer

	char *file;
	token_s *tokens;
	size_t i;

	file = readentirefile("tokenizer.c");
//	printf("%s\n", file);

	tokens = tokenize(file);

	if (!tokens) {
		printf("?\n");
	}
	else {
		i = 0;
		while (tokens[i].type != TOKEN_TYPE_EMPTY) {
			printf("TOKEN{");
			print_token_type(tokens[i].type);
			printf("}");
			if (tokens[i].type == TOKEN_TYPE_INT_LITTERAL
			||  tokens[i].type == TOKEN_TYPE_CHAR_LITTERAL
			||  tokens[i].type == TOKEN_TYPE_FLOAT_LITTERAL
			||  tokens[i].type == TOKEN_TYPE_STRING_LITTERAL
			||  tokens[i].type == TOKEN_TYPE_IDENTIFIER
			||  tokens[i].type == TOKEN_TYPE_EMPTY) {
				printf(":[%zu][%.*s]", tokens[i].value.len, tokens[i].value.len, tokens[i].value.str);
			}
			printf("\n");
			i = i + 1;
		}
	}

	// words

	return 0;
}
