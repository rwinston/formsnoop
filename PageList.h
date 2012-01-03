#ifndef PAGELIST_H
#define PAGELIST_H

#include "List.h" 
#include "ListIterator.h"

template<class T>
class ListIterator;

template<class T>
class PageList : public List<T>
{
	friend class ListIterator<T>;

public:
	PageList() : List<T>() {}
};



#endif