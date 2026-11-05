/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)
 
 Implantation du TAD List vu en cours.
 */
/*-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

typedef struct s_LinkedElement {
	int value;
	struct s_LinkedElement* previous;
	struct s_LinkedElement* next;
} LinkedElement;

/* Use of a sentinel for implementing the list :
 The sentinel is a LinkedElement* whose next pointer refer always to the head of the list and previous pointer to the tail of the list
 */
struct s_List {
	LinkedElement* sentinel;
	int size;
};

typedef struct s_SubList {
	LinkedElement* head;
	LinkedElement* tail;
} SubList;

/*-----------------------------------------------------------------*/

List* list_create(void) {
	List* l = NULL;
	l = malloc(sizeof(struct s_List)+sizeof(struct s_LinkedElement));
	l->sentinel = (LinkedElement*)(l+1);
	l->size = 0;
	l->sentinel->next = l->sentinel;
	l->sentinel->previous = l->sentinel;
	l->sentinel->value = l->size;
	return l;
}

/*-----------------------------------------------------------------*/

List* list_push_back(List* l, int v) {
	LinkedElement* e = malloc(sizeof(struct s_LinkedElement));
	e->value = v;
	e->next = l->sentinel;
	e->previous = l->sentinel->previous;
	l->sentinel->previous->next = e;
	l->sentinel->previous = e;
	l->size++;
	return l;
}

/*-----------------------------------------------------------------*/

void list_delete(ptrList* l) {
	List* lst = *l;
	LinkedElement* p = lst->sentinel->next;
	LinkedElement* temp;
	while (p!=lst->sentinel) {
		p->next->previous = p->previous;
		p->previous->next = p->next;
		temp = p->next;
		free(p);
		lst->size--;
		p = temp;
	}
	free(*l);
	l=NULL;
}

/*-----------------------------------------------------------------*/

List* list_push_front(List* l, int v) {
	LinkedElement* e = malloc(sizeof(struct s_LinkedElement));
	e->value = v;
	e->previous = l->sentinel;
	e->next = l->sentinel->next;
	l->sentinel->next->previous = e;
	l->sentinel->next = e;
	l->size++;
	return l;
}

/*-----------------------------------------------------------------*/

int list_front(const List* l) {
	assert(!list_is_empty(l));
	return l->sentinel->next->value;
}

/*-----------------------------------------------------------------*/

int list_back(const List* l) {
	assert(!list_is_empty(l));
	return l->sentinel->previous->value;
}

/*-----------------------------------------------------------------*/

List* list_pop_front(List* l) {
	assert(!list_is_empty(l));
	l->sentinel->next->next->previous = l->sentinel;
	LinkedElement* temp = l->sentinel->next->next;
	free(l->sentinel->next);
	l->sentinel->next = temp;
	l->size--;
	return l;
}

/*-----------------------------------------------------------------*/

List* list_pop_back(List* l){
	assert(!list_is_empty(l));
	l->sentinel->previous->previous->next = l->sentinel;
	LinkedElement* temp = l->sentinel->previous->previous;
	free(l->sentinel->previous);
	l->sentinel->previous = temp;
	l->size--;
	return l;
}

/*-----------------------------------------------------------------*/

List* list_insert_at(List* l, int p, int v) {
	LinkedElement* pp = l->sentinel->next;
	int i = 0;
	while (i<p) {
		pp = pp->next;
		i++;
	}
	LinkedElement* n = malloc(sizeof(struct s_LinkedElement));
	n->value = v;
	n->next = pp;
	n->previous = pp->previous;
	pp->previous = n;
	n->previous->next = n;
	l->size++;
	return l;
}

/*-----------------------------------------------------------------*/

List* list_remove_at(List* l, int p) {
	if(list_is_empty(l) || p>=list_size(l)) return l;
	LinkedElement* pp = l->sentinel->next;
	int i = 0;
	while (i<p) {
		pp = pp->next;
		i++;
	}
	pp->next->previous = pp->previous;
	pp->previous->next = pp->next;
	l->size--;
	free(pp);
	return l;
}

/*-----------------------------------------------------------------*/

int list_at(const List* l, int p) {
	assert(!list_is_empty(l));
	LinkedElement* pp = l->sentinel->next;
	int i = 0;
	while (i<p) {
		pp = pp->next;
		i++;
	}
	return pp->value;
}

/*-----------------------------------------------------------------*/

bool list_is_empty(const List* l) {
	return l->size==0;
}

/*-----------------------------------------------------------------*/

int list_size(const List* l) {
	return l->size;
}

/*-----------------------------------------------------------------*/

List* list_map(List* l, ListFunctor f, void* environment) {
	for (LinkedElement* p=l->sentinel->next; p!=l->sentinel; p = p->next) {
		p->value = f(p->value, environment);
	}
	return l;
}

/*-----------------------------------------------------------------*/

SubList list_split(SubList l) {
	LinkedElement* p = l.head;
	LinkedElement* pp = l.head;
	int i = 0;
	bool isOdd = true;
	while (p!=l.tail) {
		i++;
		p = p->next;
		if(!isOdd) {
			pp = pp->next;
			isOdd = true;
		} else {
			isOdd = false;
		}
	}
	l.head = pp;
	l.tail = pp->next;
	return l;
}

/*-----------------------------------------------------------------*/

SubList list_merge(SubList leftlist, SubList rightlist, OrderFunctor f) {
	LinkedElement* p1 = leftlist.head;
	LinkedElement* p2 = rightlist.head;
	bool isleftfirst;
	int i = 0;
	while (p1!=leftlist.tail && p2!=rightlist.tail) {
		LinkedElement* temp1 = p1->next;
		LinkedElement* temp2 = p2->next;
		if(f(p1->value, p2->value)) {
			if(i==0) isleftfirst=true;
			p1->next = p2;
			p2->next = temp1;
			temp2->previous = p2->previous;
			p2->previous = p1;
			temp1->previous = p2;
		} else {
			if(i==0) isleftfirst=false;
			LinkedElement* temp = p1->previous;
			p1->previous = p2;
			p2->next = p1;
			temp2->previous = p2->previous;
			p2->previous = temp;
			temp1->previous = p2;
		}
		p1 = temp1;
		p2 = temp2;
		i++;
	}
	LinkedElement* p = isleftfirst?leftlist.head:rightlist.head;
	SubList l;
	l.head = p;
	int j = 0;
	while (j<i) {
		p = p->next;
		j++;
	}
	l.tail = p;
	return l;
}
/*-----------------------------------------------------------------*/

SubList list_mergesort(SubList l, OrderFunctor f) {
	if(l.head == l.tail) return l;
	SubList split = list_split(l);
	SubList left, right;
	left.head = l.head;
	left.tail = split.head;
	right.head = split.tail;
	right.tail = l.tail;
	return list_merge(list_mergesort(left,f), list_mergesort(right, f), f);
}

/*-----------------------------------------------------------------*/

List* list_sort(List* l, OrderFunctor f) {
	SubList sl;
	sl.head = l->sentinel->next;
	sl.tail = l->sentinel->previous;
	list_mergesort(sl, f);
	return l;
}

