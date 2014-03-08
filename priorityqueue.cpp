#include "priorityqueue.h"
#include<stdio.h>
#include<stdlib.h>

void enqueue(ListHeadPtr &p, void* input_data, int priority)
{
	ListNodePtr item_to_insert, node_p, prev_node;
	if(input_data == NULL){
		printf("\nInvalid input data!");
		return;
	}
	if(p == NULL){
		p = (ListHeadPtr)malloc(sizeof(ListHead));
		if(p == NULL){
			printf("\nError! Unable to allocate memory for head");
			return;
		}
		p->next = NULL;
		p->size_of_list = 0;
	}
	item_to_insert = (ListNodePtr)malloc(sizeof(ListNode));
	if(item_to_insert == NULL){
		printf("\nError! Unable to allocate memory for node");
		return;
	}
	if(p->next == NULL){
		p->next = item_to_insert;
		item_to_insert->next = NULL;
	}
	else{
		node_p = p->next;
		prev_node = NULL;
		while(node_p != NULL && priority > node_p->priority){
			prev_node = node_p;
			node_p = node_p->next;
		}
		if(prev_node == NULL){
			item_to_insert->next = p->next;
			p->next = item_to_insert;
		}
		else {
			item_to_insert->next = prev_node->next;
			prev_node->next = item_to_insert;
		}
	}
	item_to_insert->data = input_data;
	item_to_insert->priority = priority;
	p->size_of_list++;
}
ListNodePtr dequeue(ListHeadPtr p)
{
	ListNodePtr node_p = NULL;
	if(p == NULL){
		printf("\nList head is NULL");
		return NULL;
	}
	if(p->next == NULL || p->size_of_list == 0){
		printf("\nList is empty");
		return NULL;
	}
	node_p = p->next;
	p->next = node_p->next;
	node_p->next = NULL;
	p->size_of_list--;
	return node_p;
}
void purge(ListHeadPtr p){
	ListNodePtr node_p = NULL, tmp = NULL;
	if(p != NULL){
		ListNodePtr node_p = p->next;
		while(node_p != NULL){
			tmp = node_p;
			node_p = node_p->next;
			free(tmp);
		}
		free(p);
	}
}
