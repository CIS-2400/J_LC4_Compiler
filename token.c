/*
 * token.c
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "token.h"

#define MAX_TOKEN_LENGTH 250

///////////////////////////////////////////////////////////////////////////////

bool function_name_check (char *c) {
	if ((isalpha(c[0])) == 0) {
		return false;
	}
	
	int i;
	for (i = 0; i < strlen(c); i++) {
		if ((isalnum(c[i])) == 0 && c[i] != '_') {
			return false;
		}
	}
	
	return true;
}

bool next_token(FILE *j_file, token *output) {
	char token[MAX_TOKEN_LENGTH];
	char nextChar;
	
	int arg_number = 0;
	int literal_val = 0;
	
	if (fscanf(j_file, "%s", token) == EOF) {
		return false;
	}
	else if (token[0] == ';') {
		nextChar = getc(j_file);
		while (nextChar != '\n') {
			nextChar = getc(j_file);
		}
		
		return next_token(j_file, output); // recursion
	}
	
	// CONDITIONALS
	else if (strcmp(token, "defun") == 0) {
		output->type = DEFUN;
		return true;
	}
	else if (strcmp(token, "return") == 0) {
		output->type = RETURN;
		return true;
	}
	else if (strcmp(token, "+") == 0) {
		output->type = PLUS;
		return true;
	}
	else if (strcmp(token, "-") == 0) {
		output->type = MINUS;
		return true;
	}
	else if (strcmp(token, "*") == 0) {
		output->type = MUL;
		return true;
	}
	else if (strcmp(token, "/") == 0) {
		output->type = DIV;
		return true;
	}
	else if (strcmp(token, "%") == 0) {
		output->type = MOD;
		return true;
	}
	else if (strcmp(token, "and") == 0) {
		output->type = AND;
		return true;
	}
	else if (strcmp(token, "or") == 0) {
		output->type = OR;
		return true;
	}
	else if (strcmp(token, "not") == 0) {
		output->type = NOT;
		return true;
	}
	else if (strcmp(token, "lt") == 0) {
		output->type = LT;
		return true;
	}
	else if (strcmp(token, "le") == 0) {
		output->type = LE;
		return true;
	}
	else if (strcmp(token, "eq") == 0) {
		output->type = EQ;
		return true;
	}
	else if (strcmp(token, "ge") == 0) {
		output->type = GE;
		return true;
	}
	else if (strcmp(token, "gt") == 0) {
		output->type = GT;
		return true;
	}
	else if (strcmp(token, "if") == 0) {
		output->type = IF;
		return true;
	}
	else if (strcmp(token, "else") == 0) {
		output->type = ELSE;
		return true;
	}
	else if (strcmp(token, "endif") == 0) {
		output->type = ENDIF;
		return true;
	}
	else if (strcmp(token, "while") == 0) {
		output->type = WHILE;
		return true;
	}
	else if (strcmp(token, "endwhile") == 0) {
		output->type = ENDWHILE;
		return true;
	}
	else if (strcmp(token, "drop") == 0) {
		output->type = DROP;
		return true;
	}
	else if (strcmp(token, "dup") == 0) {
		output->type = DUP;
		return true;
	}
	else if (strcmp(token, "swap") == 0) {
		output->type = SWAP;
		return true;
	}
	else if (strcmp(token, "rot") == 0) {
		output->type = ROT;
		return true;
	}
	else if (sscanf(token, "arg%d", &arg_number) == 1) {
		if (arg_number >= 1 && arg_number <= 20) {
			output->type = ARG;
			output->arg_no = arg_number;
			return true;
		}
	}
	else if (function_name_check(token)) {
		output->type = IDENT;
		strcpy(output->str, token);
		return true;
	}
	else if (sscanf(token, "0x%X", &literal_val) == 1) {
		output->type = LITERAL;
		output->literal_value = literal_val;
		return true;
	}
	else if (sscanf(token, "%d", &literal_val) == 1) {
		output->type = LITERAL;
		output->literal_value = literal_val;
		return true;
	}
	
	output->type = BAD_TOKEN;
	return true;
}
