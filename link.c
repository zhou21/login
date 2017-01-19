#include <stdio.h>
#include <stdlib.h>
#include "link.h"


Config *insert_node(Config *head, Config node)
{
    Config *tmp = (Config *)malloc(sizeof(Config));
    *tmp = node;
    tmp->next = NULL;

    if (head == NULL){
        head = tmp;    
        return head;
    }

    tmp->next = head;
    head = tmp;
    return head;
}

int delete_link(Config *head)
{
    if (head == NULL){
        return -1;
    }
    Config *tmp = head;
    while(tmp == NULL){
        tmp = head;
        head = head->next;
        free(tmp);
    }
    return 0;
}

int print_link(Config *head)
{
    Config *tmp = head;
    while(tmp != NULL){
        printf("<feild>: %s | <value>: %s\n", tmp->feild, tmp->value);
        tmp = tmp->next;
    }
}