#include <stdio.h>
#include <stdlib.h>

#include "includes/list.h"

void print_list(List *list)
{
    int i, *data, index;
    printf("length : %d\n", list->length);
    for (i=0; i<list->length; i++)
    {
        data = list_index(list, i);
        index = list->last_referenced_index;
        printf("data : %d, last referenced index : %d\n", *data, index);
    }
}

void index_value(List *list, int x)
{
    int *data;
    data = list_index(list, x);
    if (data == NULL)
    {
        printf("index out of range\n");
    }
    else
    {
        printf("data : %d, last referenced index : %d\n", *data, list->last_referenced_index);
    }
}

int main()
{
    int data1 = 6136;
    int data2 = 8648;
    int data3 = 1574;
    int data4 = 9572;
    int *data;
    List *list = new_list();
    print_list(list);
    list_append(list, &data1);
    print_list(list);
    list_append(list, &data2);
    print_list(list);
    list_append(list, &data3);
    print_list(list);
    list_append(list, &data4);
    print_list(list);

    index_value(list, 0);
    index_value(list, 5);
    index_value(list, -1);
    index_value(list, 2);
    index_value(list, 4);


    list_delete(list);

    printf("end\n");
    return 0;
}