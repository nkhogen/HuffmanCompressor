#ifndef __PRIORITYQUEUE__
#define __PRIORITYQUEUE__
typedef struct TheListNode
{
	void* data;
	int priority;
	struct TheListNode *next;
}  ListNode, *ListNodePtr;

typedef struct TheListHead
{
	int size_of_list;
	//student_type data;
	struct TheListNode *next;
}  ListHead, *ListHeadPtr;

void enqueue(ListHeadPtr &p, void* input_data, int priority);
ListNodePtr dequeue(ListHeadPtr p);
void purge(ListHeadPtr p);
#endif