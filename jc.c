/*
 * jc.c
 */

#include "token.h"
#include "stack.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	FILE *input_file = fopen(argv[1], "r");
	FILE *output_file;
	
	// Initialize token
	token *tk = malloc(sizeof(token));
	tk->type = BAD_TOKEN;
	tk->literal_value = 0;
	tk->arg_no = 0;
	
	// Create .asm file
	char *outfile_name = malloc(((strlen(argv[1]) + 3) * sizeof(char)));
	strcpy(outfile_name, argv[1]);
	outfile_name[strlen(argv[1]) - 1] = '\0'; // remove 'j'
	strcat(outfile_name, "asm");
	output_file = fopen(outfile_name, "w");
	
	int comp_counter = 0;
	int if_counter = 1;
	//int else_counter = 0;
	int while_counter = 0;
	bool within_function = false;
	
	stack *if_stk = malloc(sizeof(stack));
	construct_stack(if_stk);
	stack *else_stk = malloc(sizeof(stack));
	construct_stack(else_stk);
	stack *while_stk = malloc(sizeof(stack));
	construct_stack(while_stk);
	
	while (next_token(input_file, tk)) {
		// DEFUN
		if (tk->type == DEFUN) {
			next_token(input_file, tk);
			if (!within_function && tk->type == IDENT) {
				within_function = true;
				fprintf(output_file, ".CODE ; DEFUN/IDENT\n");
				fprintf(output_file, ".FALIGN ; DEFUN/IDENT\n");
				fprintf(output_file, "%s ; DEFUN/IDENT\n", tk->str);
				fprintf(output_file, "ADD R6, R6, #-3 ; DEFUN/IDENT\n");
				fprintf(output_file, "STR R7, R6, #1 ; DEFUN/IDENT\n");
				fprintf(output_file, "STR R5, R6, #0 ; DEFUN/IDENT\n");
				fprintf(output_file, "ADD R5, R6, #0 ; DEFUN/IDENT\n");
			}
		}
		
		// IDENT
		else if (tk->type == IDENT) {
			fprintf(output_file, "JSR %s ; IDENT\n", tk->str);
			fprintf(output_file, "ADD R6, R6, #-1 ; IDENT\n");
		}
		
		// RETURN
		else if (tk->type == RETURN) {
			if (within_function) {
				within_function = false;
				fprintf(output_file, "LDR R7, R6, #0 ; RETURN\n");
				fprintf(output_file, "STR R7, R5, #2 ; RETURN\n");
				fprintf(output_file, "ADD R6, R5, #0 ; RETURN\n");
				fprintf(output_file, "LDR R5, R6, #0 ; RETURN\n");
				fprintf(output_file, "LDR R7, R6, #1 ; RETURN\n");
				fprintf(output_file, "ADD R6, R6, #3 ; RETURN\n");
				fprintf(output_file, "RET ; RETURN\n");
			}
		}
		
		// ARG
		else if (tk->type == ARG) {
			fprintf(output_file, "LDR R0, R5, #%d ; ARG\n", tk->arg_no + 2);
			fprintf(output_file, "ADD R6, R6, #-1 ; ARG\n");
			fprintf(output_file, "STR R0, R6, #0 ; ARG\n");
		}
		
		// Literals
		else if (tk->type == LITERAL) {
			fprintf(output_file, "ADD R6 R6 #-1 ; LITERAL\n");
			fprintf(output_file, "CONST R0 #%d ; LITERAL\n", (unsigned short int) (tk->literal_value & 0x00FF));
			fprintf(output_file, "HICONST R0 #%d ; LITERAL\n", (unsigned short int) ((unsigned) (tk->literal_value & 0xFF00)) >> 8);
			fprintf(output_file, "STR R0 R6 #0 ; LITERAL\n");
		}

		// Arithmetic - PLUS, MINUS, MUL, DIV, MOD
		else if (tk->type == PLUS) {
			fprintf(output_file, "LDR R0, R6, #0 ; PLUS\n");
		    	fprintf(output_file, "LDR R1, R6, #1 ; PLUS\n");
			fprintf(output_file, "ADD R0, R0, R1 ; PLUS\n");
			fprintf(output_file, "ADD R6, R6, #1 ; PLUS\n");
		    	fprintf(output_file, "STR R0, R6, #0 ; PLUS\n");
		}
		else if (tk->type == MINUS) {
			fprintf(output_file, "LDR R0, R6, #0 ; MINUS\n");
		    	fprintf(output_file, "LDR R1, R6, #1 ; MINUS\n");
			fprintf(output_file, "SUB R0, R0, R1 ; MINUS\n");
			fprintf(output_file, "ADD R6, R6, #1 ; MINUS\n");
		    	fprintf(output_file, "STR R0, R6, #0 ; MINUS\n");
		}
		else if (tk->type == MUL) {
			fprintf(output_file, "LDR R0, R6, #0 ; MUL\n");
		    	fprintf(output_file, "LDR R1, R6, #1 ; MUL\n");
			fprintf(output_file, "MUL R0, R0, R1 ; MUL\n");
			fprintf(output_file, "ADD R6, R6, #1 ; MUL\n");
		    	fprintf(output_file, "STR R0, R6, #0 ; MUL\n");
		}
		else if (tk->type == DIV) {
			fprintf(output_file, "LDR R0, R6, #0 ; DIV\n");
		    	fprintf(output_file, "LDR R1, R6, #1 ; DIV\n");
			fprintf(output_file, "DIV R0, R0, R1 ; DIV\n");
			fprintf(output_file, "ADD R6, R6, #1 ; DIV\n");
		    	fprintf(output_file, "STR R0, R6, #0 ; DIV\n");
		}
		else if (tk->type == MOD) {
			fprintf(output_file, "LDR R0, R6, #0 ; MOD\n");
		    	fprintf(output_file, "LDR R1, R6, #1 ; MOD\n");
			fprintf(output_file, "MOD R0, R0, R1 ; MOD\n");
			fprintf(output_file, "ADD R6, R6, #1 ; MOD\n");
		    	fprintf(output_file, "STR R0, R6, #0 ; MOD\n");
		}
		
		// Stack operations - DROP, DUP, SWAP, ROT
		else if (tk->type == DROP) {
			fprintf(output_file, "ADD R6, R6, #1 ; DROP\n");
		}
		else if (tk->type == DUP) {
			fprintf(output_file, "LDR R0, R6, #0 ; DUP\n");
	            	fprintf(output_file, "ADD R6, R6, #-1 ; DUP\n");
	            	fprintf(output_file, "STR R0, R6, #0 ; DUP\n");
		}
		else if (tk->type == SWAP) {
			fprintf(output_file, "LDR R0, R6, #0 ; SWAP\n");
	            	fprintf(output_file, "LDR R1, R6, #1 ; SWAP\n");
	            	fprintf(output_file, "STR R0, R6, #1 ; SWAP\n");
	            	fprintf(output_file, "STR R1, R6, #0 ; SWAP\n");
		}
		else if (tk->type == ROT) {
			fprintf(output_file, "LDR R0, R6, #0 ; ROT\n");
	            	fprintf(output_file, "LDR R1, R6, #1 ; ROT\n");
	            	fprintf(output_file, "LDR R2, R6, #2 ; ROT\n");
	            	fprintf(output_file, "STR R0, R6, #1 ; ROT\n");
	            	fprintf(output_file, "STR R1, R6, #2 ; ROT\n");
	            	fprintf(output_file, "STR R2, R6, #0 ; ROT\n");
		}
		
		// Bitwise operations - AND, OR, NOT
		else if (tk->type == AND) {
			fprintf(output_file, "LDR R0, R6, #0 ; AND\n");
		    	fprintf(output_file, "LDR R1, R6, #1 ; AND\n");
			fprintf(output_file, "AND R0, R0, R1 ; AND\n");
			fprintf(output_file, "ADD R6, R6, #1 ; AND\n");
		    	fprintf(output_file, "STR R0, R6, #0 ; AND\n");
		}
		else if (tk->type == OR) {
			fprintf(output_file, "LDR R0, R6, #0 ; OR\n");
		    	fprintf(output_file, "LDR R1, R6, #1 ; OR\n");
			fprintf(output_file, "OR R0, R0, R1 ; OR\n");
			fprintf(output_file, "ADD R6, R6, #1 ; OR\n");
		    	fprintf(output_file, "STR R0, R6, #0 ; OR\n");
		}
		else if (tk->type == NOT) {
			//fprintf(output_file, "LDR R0, R6, #0 ; NOT\n");
		    	fprintf(output_file, "LDR R1, R6, #0 ; NOT\n");
			fprintf(output_file, "NOT R1, R1 ; NOT\n");
			//fprintf(output_file, "ADD R6, R6, #1 ; NOT\n");
		    	fprintf(output_file, "STR R1, R6, #0 ; NOT\n");
		}
		
		// Comparisons - LT, LE, EQ, GE, GT
		else if (tk->type == LT) {
			fprintf(output_file, "LDR R0, R6, #0 ; LT\n");
			fprintf(output_file, "LDR R1, R6, #1 ; LT\n");
			fprintf(output_file, "CMP R0, R1; LT\n");
			
			fprintf(output_file, "BRn TRUE_L_%d ; LT\n", comp_counter);
			fprintf(output_file, "BRzp FALSE_L_%d ; LT\n", comp_counter);
			
			// TRUE
			fprintf(output_file, "TRUE_L_%d ; LT\n", comp_counter);
			fprintf(output_file, "CONST R2, #1 ; LT\n");
			fprintf(output_file, "BRnzp TERMINATE_L_%d ; LT\n", comp_counter);
			
			// FALSE
			fprintf(output_file, "FALSE_L_%d ; LT\n", comp_counter);
			fprintf(output_file, "CONST R2, #0 ; LT\n");
			fprintf(output_file, "BRnzp TERMINATE_L_%d ; LT\n", comp_counter);
			
			// TERMINATE
			fprintf(output_file, "TERMINATE_L_%d ; LT\n", comp_counter);
			fprintf(output_file, "ADD R6, R6, #1; LT\n");
			fprintf(output_file, "STR R2, R6, #0 ; LT\n");
			
			comp_counter++;
		}
		else if (tk->type == LE) {
			fprintf(output_file, "LDR R0, R6, #0 ; LE\n");
			fprintf(output_file, "LDR R1, R6, #1 ; LE\n");
			fprintf(output_file, "CMP R0, R1; LE\n");
			
			fprintf(output_file, "BRnz TRUE_L_%d ; LE\n", comp_counter);
			fprintf(output_file, "BRp FALSE_L_%d ; LE\n", comp_counter);
			
			// TRUE
			fprintf(output_file, "TRUE_L_%d ; LE\n", comp_counter);
			fprintf(output_file, "CONST R2, #1 ; LE\n");
			fprintf(output_file, "BRnzp TERMINATE_L_%d ; LE\n", comp_counter);
			
			// FALSE
			fprintf(output_file, "FALSE_L_%d ; LE\n", comp_counter);
			fprintf(output_file, "CONST R2, #0 ; LE\n");
			fprintf(output_file, "BRnzp TERMINATE_L_%d ; LE\n", comp_counter);
			
			// TERMINATE
			fprintf(output_file, "TERMINATE_L_%d ; LE\n", comp_counter);
			fprintf(output_file, "ADD R6, R6, #1; LE\n");
			fprintf(output_file, "STR R2, R6, #0 ; LE\n");
			
			comp_counter++;
		}
		else if (tk->type == EQ) {
			fprintf(output_file, "LDR R0, R6, #0 ; EQ\n");
			fprintf(output_file, "LDR R1, R6, #1 ; EQ\n");
			fprintf(output_file, "CMP R0, R1; EQ\n");
			
			fprintf(output_file, "BRz TRUE_L_%d ; EQ\n", comp_counter);
			fprintf(output_file, "BRnp FALSE_L_%d ; EQ\n", comp_counter);
			
			// TRUE
			fprintf(output_file, "TRUE_L_%d ; EQ\n", comp_counter);
			fprintf(output_file, "CONST R2, #1 ; EQ\n");
			fprintf(output_file, "BRnzp TERMINATE_L_%d ; EQ\n", comp_counter);
			
			// FALSE
			fprintf(output_file, "FALSE_L_%d ; EQ\n", comp_counter);
			fprintf(output_file, "CONST R2, #0 ; EQ\n");
			fprintf(output_file, "BRnzp TERMINATE_L_%d ; EQ\n", comp_counter);
			
			// TERMINATE
			fprintf(output_file, "TERMINATE_L_%d ; EQ\n", comp_counter);
			fprintf(output_file, "ADD R6, R6, #1; EQ\n");
			fprintf(output_file, "STR R2, R6, #0 ; EQ\n");
			
			comp_counter++;
		}
		else if (tk->type == GE) {
			fprintf(output_file, "LDR R0, R6, #0 ; GE\n");
			fprintf(output_file, "LDR R1, R6, #1 ; GE\n");
			fprintf(output_file, "CMP R0, R1; GE\n");
			
			fprintf(output_file, "BRzp TRUE_L_%d ; GE\n", comp_counter);
			fprintf(output_file, "BRn FALSE_L_%d ; GE\n", comp_counter);
			
			// TRUE
			fprintf(output_file, "TRUE_L_%d ; GE\n", comp_counter);
			fprintf(output_file, "CONST R2, #1 ; GE\n");
			fprintf(output_file, "BRnzp TERMINATE_L_%d ; GE\n", comp_counter);
			
			// FALSE
			fprintf(output_file, "FALSE_L_%d ; GE\n", comp_counter);
			fprintf(output_file, "CONST R2, #0 ; GE\n");
			fprintf(output_file, "BRnzp TERMINATE_L_%d ; GE\n", comp_counter);
			
			// TERMINATE
			fprintf(output_file, "TERMINATE_L_%d ; GE\n", comp_counter);
			fprintf(output_file, "ADD R6, R6, #1; GE\n");
			fprintf(output_file, "STR R2, R6, #0 ; GE\n");
			
			comp_counter++;
		}
		else if (tk->type == GT) {
			fprintf(output_file, "LDR R0, R6, #0 ; GT\n");
			fprintf(output_file, "LDR R1, R6, #1 ; GT\n");
			fprintf(output_file, "CMP R0, R1; GT\n");
			
			fprintf(output_file, "BRp TRUE_L_%d ; GT\n", comp_counter);
			fprintf(output_file, "BRnz FALSE_L_%d ; GT\n", comp_counter);
			
			// TRUE
			fprintf(output_file, "TRUE_L_%d ; GT\n", comp_counter);
			fprintf(output_file, "CONST R2, #1 ; GT\n");
			fprintf(output_file, "BRnzp TERMINATE_L_%d ; GT\n", comp_counter);
			
			// FALSE
			fprintf(output_file, "FALSE_L_%d ; GT\n", comp_counter);
			fprintf(output_file, "CONST R2, #0 ; GT\n");
			fprintf(output_file, "BRnzp TERMINATE_L_%d ; GT\n", comp_counter);
			
			// TERMINATE
			fprintf(output_file, "TERMINATE_L_%d ; GT\n", comp_counter);
			fprintf(output_file, "ADD R6, R6, #1; GT\n");
			fprintf(output_file, "STR R2, R6, #0 ; GT\n");
			
			comp_counter++;
		}
		
		// IF statements - IF, ELSE, ENDIF
		else if (tk->type == IF) {
			fprintf(output_file, "ADD R6, R6, #1 ; IF\n");
	            	fprintf(output_file, "LDR R0, R6, #-1 ; IF\n");
	            	fprintf(output_file, "BRz ELSE_BRANCH_%d ; IF\n", if_counter);
	            	
	            	add_to_top(if_stk, if_counter);
	            	add_to_top(else_stk, if_counter);
	            	
	            	if_counter++;
		}
		else if (tk->type == ELSE) {
			int else_removed = remove_from_top(else_stk);
			
			fprintf(output_file, "JMP ENDIF_BRANCH_%d ; ELSE\n", else_removed);
			fprintf(output_file, "ELSE_BRANCH_%d ; ELSE\n", else_removed);
			
			//else_counter++;
		}
		else if (tk->type == ENDIF) {
			int else_element = return_from_top(else_stk);
			int if_removed = remove_from_top(if_stk);
			
			/*
			if (return_from_top(if_stk) == return_from_top(else_stk)) {
				fprintf(output_file, "ELSE_BRANCH_%d ; ENDIF\n", return_from_top(else_stk));
			}
			*/
			
			if (else_element != 0) {
				if (if_removed == else_element) {
					fprintf(output_file, "ELSE_BRANCH_%d ; ENDIF\n", remove_from_top(else_stk));
					fprintf(output_file, "ENDIF_BRANCH_%d ; ENDIF\n", if_removed);
				}
				else {
					fprintf(output_file, "ENDIF_BRANCH_%d ; ENDIF\n", if_removed);
				}
			}
			else {
				fprintf(output_file, "ENDIF_BRANCH_%d ; ENDIF\n", if_removed);
			}
		}
		
		// WHILE statements - WHILE, ENDWHILE
		else if (tk->type == WHILE) {
			fprintf(output_file, "WHILE_BRANCH_%d ; ENDIF\n", while_counter);
			fprintf(output_file, "ADD R6, R6, #1 ; WHILE\n");
	            	fprintf(output_file, "LDR R0, R6, #-1 ; WHILE\n");
	            	fprintf(output_file, "BRz ENDWHILE_BRANCH_%d ; WHILE\n", while_counter);
	            	
	            	add_to_top(while_stk, while_counter);
	            	
	            	while_counter++;
		}
		else if (tk->type == ENDWHILE) {
			fprintf(output_file, "JMP WHILE_BRANCH_%d ; ENDWHILE\n", return_from_top(while_stk));
			fprintf(output_file, "ENDWHILE_BRANCH_%d ; ENDWHILE\n", return_from_top(while_stk));
			
			remove_from_top(while_stk);
		}
		
	}
	
	fclose(input_file);
	fclose(output_file);
	
	free(outfile_name);
	free(tk);
	clear_stack(if_stk);
	free(if_stk);
	clear_stack(else_stk);
	free(else_stk);
	clear_stack(while_stk);
	free(while_stk);
}
