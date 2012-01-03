#ifndef ELEMENTLIST_H
#define ELEMENTLIST_H

#include "List.h"

// specialization of List class to hold form elements
template<class T>
class ElementList : public List<T>
{
public:
	ElementList() : List<T>()
	{
	}

	~ElementList()
	{
	}

	int print();
};



template<class T>
int ElementList<T>::print()
{
	dprintf("ElementList<T>::print()\n");

	if(isEmpty())
		return -1;
	
	for (Node<T>* p = head; p; p = p->next)
 	  dprintf("%ls->", (const wchar_t*)p->data);

	return 0;

}



#endif
