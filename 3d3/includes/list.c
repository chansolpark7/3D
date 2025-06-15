#include <stdio.h>
#include <stdlib.h>

#include "list.h"

List *new_list()
{
    List *list = malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->last_referenced_node = NULL;
    list->last_referenced_index = -1;
    list->length = 0;
    return list;
}

void list_append(List *list, void *data)
{
    Node *new_node = malloc(sizeof(Node));
    Node *tail_node;
    new_node->next = NULL;
    new_node->data = data;
    if (list->length == 0)
    {
        list->head = new_node;
        list->tail = new_node;
        new_node->before = NULL;
        list->length = 1;
    }
    else
    {
        tail_node = list->tail;
        tail_node->next = new_node;
        new_node->before = tail_node;
        list->tail = new_node;
        list->length++;
    }
}

void *list_pop(List *list)
{
    Node *tail_node;
    Node *new_tail_node;
    void *data;
    if (list->length == 0)
    {
        return NULL;
    }
    else if (list->length == 1)
    {
        tail_node = list->tail;
        data = tail_node->data;
        list->length = 0;
        list->head = NULL;
        list->tail = NULL;
        free(tail_node);

        return data;
    }
    else
    {
        tail_node = list->tail;
        new_tail_node = tail_node->before;
        list->tail = new_tail_node;
        new_tail_node->next = NULL;
        list->length -= 1;
        data = tail_node->data;
        free(tail_node);

        return data;
    }
}

void *list_index(List *list, int index)
{
    int i;
    Node *node;
    if (index < 0 || index > list->length)
    {
        return 0;
    }

    if (list->last_referenced_index == -1)
    {
        if (index < list->length-1-index)
        {
            node = list->head;
            for (i=0; i<index; i++)
            {
                node = node->next;
            }
        }
        else
        {
            node = list->tail;
            for (i=list->length-1; i>index; i--)
            {
                node = node->before;
            }
        }
    }
    else
    {
        if (index < list->length-1-index)
        {
            if (index < abs(list->last_referenced_index-index))
            {
                node = list->head;
                for (i=0; i<index; i++)
                {
                    node = node->next;
                }
            }
            else
            {
                node = list->last_referenced_node;
                if (list->last_referenced_index < index)
                {
                    for (i=list->last_referenced_index; i<index; i++)
                    {
                        node = node->next;
                    }
                }
                else
                {
                    for (i=list->last_referenced_index; i>index; i--)
                    {
                        node = node->before;
                    }
                }
            }
        }
        else
        {
            if (list->length-1-index < abs(list->last_referenced_index-index))
            {
                node = list->tail;
                for (i=list->length-1; i>index; i--)
                {
                    node = node->before;
                }
            }
            else
            {
                node = list->last_referenced_node;
                if (list->last_referenced_index < index)
                {
                    for (i=list->last_referenced_index; i<index; i++)
                    {
                        node = node->next;
                    }
                }
                else
                {
                    for (i=list->last_referenced_index; i>index; i--)
                    {
                        node = node->before;
                    }
                }
            }
        }
    }
    list->last_referenced_index = index;
    list->last_referenced_node = node;
    return node->data;
}

void list_delete(List *list)
{
    int i;
    Node *node = list->head;
    for (i=0; i<list->length-1; i++)
    {
        node = node->next;
        free(node->before);
    }
    free(list->tail);
    free(list);
}