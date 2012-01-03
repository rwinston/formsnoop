#ifndef LIST_H
#define LIST_H

#include <tchar.h>

//forward declarations
template<class T>
class List;

template<class T>
class ListIterator;

template<class T>
class ReverseListIterator;

// Node class
template<class T>
class Node {
public:
	friend class List<T>;
	friend class ListIterator<T>;	
	friend class ReverseListIterator<T>;

	Node()  : data(0), next(NULL), prev(NULL) {}
	
	Node(T& data, Node<T>* ptr) : data(data), next(ptr), prev(NULL) { 
		
		if(ptr)
			ptr->prev = this;
	}

	~Node() {}
	
private:
	T  data;
	Node<T>* next;
	Node<T>* prev;
};

// Singly-linked List class
template<class T>
class List
{ 	
	friend class ListIterator<T>;
	friend class ReverseListIterator<T>;

	public:
	List() : head(0), nElems(0) { }
	virtual ~List();
	virtual void insert(T t);    
	virtual int remove (T& t);   
	inline virtual bool isEmpty() { return (head == 0); }
	virtual int print();
	virtual void empty();
	int getCount() { return nElems; }
	
protected:
	Node<T>* head;
	
	inline Node<T>* newNode(T& t, Node<T>* ptr)
	{ 
		Node<T>* nextNode = new Node<T>(t,ptr); 
		return nextNode; 
	}
	
private:
	int nElems;
	
	// disallow assignment operator/copy constructor
	List(const List<T>& rhs) {}
	List<T>& operator=(const List<T>& rhs) {}
};


template<class T>
List<T>::~List()
{
	Node<T>* temp;
	
	for (Node<T>* p = head; p; )   // traverses list
    {
		temp = p;
		p = p->next;
		delete temp;
    }
}

template<class T>
void List<T>::insert(T t)
{
	Node<T>* p = newNode(t, head);
	head = p; 
	nElems++;
}

template<class T>
int List<T>::remove(T& t)
{
	if (isEmpty()) return 0;   // flag to signal no removal
	
	t = head->data;           // data value returned by reference
	Node<T>* p = head;
	head = head->next;       // advance first pointer to remove node
	delete p;
	p = 0;
	
	nElems--;
	
	return 1;                  // flag to signal successful removal
}


template<class T>
void List<T>::empty()
{
	if(isEmpty())
		return;
	
	Node<T>* temp;
	Node<T>* p;
	int deletedElements = 0;
	
	p = head;
	
	while(p) {
		temp = p;
		p=p->next;
		delete temp;
		temp = 0;
		deletedElements++;
	}
	

	nElems -= deletedElements;
	
}


template<class T>
int List<T>::print()
{
	// do nothing
	return 0;
}


#endif