/*
 * stack.h
 */
 
#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdbool.h>

typedef struct stack_Node {
  int         payload;     // customer-supplied payload
  struct stack_Node *next;     // next node in stack, or NULL
} node;

typedef struct stack {
  int		numElements;
  node   *front;  // beginning of stack, or NULL if empty
} stack;

void construct_stack(stack *sk);

void add_to_top(stack *sk, int a);

int return_from_top(stack *sk);

int remove_from_top(stack *sk);

void clear_stack(stack *sk);

#endif
