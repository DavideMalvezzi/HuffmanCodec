#include "PriorityList.h"

struct PriorityListItem{
	int priority;
	void* data;
};

struct PriorityList{
	int last, size;
	PriorityListItem* items;
};

PriorityList* createNewPriorityList(const int size){
	PriorityList* list = new PriorityList;
	list->last = 0;
	list->size = size;
	list->items = new PriorityListItem[size];
	return list;
}

void deletePriorityList(PriorityList* list, GDestroyNotify deleteFun){
		if(deleteFun != NULL){
			for(int i = 0; i < list->last; i++){
				deleteFun(list->items[i].data);
			}
		}
		delete[] list->items;
		delete list;
}

void insertListItem(PriorityList* list, void* data, const int priority){
	int i, parent;
	PriorityListItem temp;

	i = list->last;

	if(i + 1 <= list->size){
		list->items[i].priority = priority;
		list->items[i].data = data;
		list->last++;

		parent = getParent(list, i);

		while(i > 0 && list->items[parent].priority > priority){
			temp = list->items[i];
			list->items[i] = list->items[parent];
			list->items[parent] = temp;

			i = parent;
			parent = getParent(list, i);
		}
	}
}

void* getListMinData(PriorityList* list){
	void* data = NULL;
	if(list->last > 0){
		data = list->items[0].data;
		list->items[0] = list->items[list->last - 1];
		list->last--;
		heapify(list);
	}
	return data;
}

int getListSize(PriorityList* list){
	return list->last;
}

void heapify(const PriorityList* list){
	int curr = 0, minChild;
	PriorityListItem temp;

	minChild = getMinChild(list, curr);

	while(minChild > 0 && list->items[curr].priority > list->items[minChild].priority){
		temp = list->items[curr];
		list->items[curr] = list->items[minChild];
		list->items[minChild] = temp;

		curr = minChild;
		minChild = getMinChild(list, curr);
	}
}

int getParent(const PriorityList* list, int index){
	return ceil((float)index / 2) - 1;
}

int getLeftChild(const PriorityList* list, int index){
	int child = 2 * index + 1;
	return child >= list->last ? -1 : child;
}

int getRightChild(const PriorityList* list, int index){
	int child = 2 * index + 2;
	return child >= list->last ? -1 : child;
}

int getMinChild(const PriorityList* list, int index){
	int left = getLeftChild(list, index);
	int right = getRightChild(list, index);

	if(left == -1 && right == -1){
		return -1;
	}
	else if (right == -1){
		return left;
	}
	else if(left == -1){
		return right;
	}

	return list->items[left].priority <= list->items[right].priority ? left : right;
}
