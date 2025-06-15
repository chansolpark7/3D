#pragma once

typedef struct _Node
{
    struct Node *next;
    struct Node *before;
    void *data;
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

void list_append(List *list, void *data);

void *list_pop(List *list);

void *list_index(List *list, int index);

void list_delete(List *list);