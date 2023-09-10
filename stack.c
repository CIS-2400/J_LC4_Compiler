/*
 * stack.c
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "stack.h"

///////////////////////////////////////////////////////////////////////////////

void construct_stack(stack *sk) {
	sk->front = NULL;
	sk->numElements = 0;
}

void add_to_top(stack *sk, int a) {
	node *n = malloc(sizeof(*n));
	
	n->payload = a;
	n->next = sk->front;
	sk->front = n;
	
	sk->numElements++;
}

int return_from_top(stack *sk) {
	int top_element;
	node *n;
	
	if (sk->front != NULL) {
		n = sk->front;
		top_element = n->payload;
	}
	
	return top_element;
}

int remove_from_top(stack *sk) {
	int top_element;
	node *n;
	
	if (sk->front != NULL) {
		n = sk->front;
		top_element = n->payload;
		
		sk->numElements--;
		
		if (sk->numElements != 0) {
			sk->front = n->next;
		}
		else {
			sk->front = NULL;
		}
		
		free(n);
	}
	
	return top_element;
}

void clear_stack(stack *sk) {
	while (sk->front) {
		remove_from_top(sk);
	}
}
