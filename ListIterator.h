#ifndef LISTITERATOR_H
#define LISTITERATOR_H

#include "Iterator.h"

template<class T>
class ListIterator : public Iterator<T>
{
	public:
		ListIterator(List<T>& l) : list(l) { reset(); }
		virtual void reset() { previous = NULL; current = list.head; }
		virtual T operator()() { return current->data; }
		virtual void operator=(T t) { current->data = t; }
		
		virtual int operator!();
		virtual int operator++();
		
		void insert(T t);
		void preInsert(T t);
		void remove();
		
	protected:
		Node<T>* current;
		Node<T>* previous;
		List<T>& list;
};



template<class T>
int ListIterator<T>::operator!()
{
	if(current == NULL)
		if(previous == NULL)		    // list is empty, or has only one item
			current = list.head;
		else
			current = previous->next;
		
		return (current != NULL);		// allows us to say if (!iterator) ...
}

template<class T>
int ListIterator<T>::operator++()
{
	if(current == NULL) {
		if(previous == NULL)
			current = list.head;
		else
			current = previous->next;
	}
	else
	{
		previous = current;
		current = current->next;
	}
	
	return (current != NULL);
}

template<class T>
void ListIterator<T>::insert(T t)
{
	Node<T>* ptr = list.newNode(t ,0);
	if(list.isEmpty())
		list.head = ptr;
	else
	{
		ptr->next = current->next;
		current->next = ptr;
	}
}

template<class T>
void ListIterator<T>::preInsert(T t)
{
	Node<T>* ptr = list.newNode(t, current);
	if(current == list.head)
		list.head = previous = ptr;
	else
		previous->next = ptr;
}

template<class T>
void ListIterator<T>::remove()
{
	if(current == list.head)
		list.head = current->next;
	else
		previous->next = current->next;
	
	delete current;
	current = 0;
}

#endif
