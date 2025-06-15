#pragma once

typedef struct _Node
{
    struct Node *next;
    struct Node *before;
    int data;
} Node;

typedef struct _List
{
    Node *head;
    Node *tail;
    Node *last_referenced_node;
    int last_referenced_index;
    int length;
} List;

List *new_list();

void list_append(List *list, int data);

int list_pop(List *list);

int list_index(List *list, int index);

void list_reset(List *list);

void list_delete(List *list);

void list_print(List *list);