#include "LockFreeLinkedList.h"
#include <stdio.h>

int main()
{
	LockFreeList<int> list;
	list.add(5);
	if (list.add(5) == false)
		printf("no add\n");
	if (list.contains(5))
		printf("YOLO\n");
	list.remove(5);
	if (!list.contains(5))
		printf("YOLO\n");


	getchar();
	return 0;
}