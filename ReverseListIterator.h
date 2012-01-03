#ifndef REVERSELISTITERATOR_H
#define REVERSELISTITERATOR_H

template<class T>
class ReverseListIterator : public Iterator<T>
{
public:
	ReverseListIterator(List<T>& l) : list(l) { reset(); }
	void reset();
	virtual T operator()() { return current->data; }  // returning reference saves copy ctor call
	void operator=(T t) { current->data = t; }
	
	int operator!();
	int operator++();

private:
	
	Node<T>* current;
	// We only need prev nodes if we are doing (pre)inserts
	//Node<T>* previous;    
	List<T>& list;
};


template<class T>
int ReverseListIterator<T>::operator++()
{
	if(current != NULL) //&& current->prev != NULL) 
	{
		// work our way back the list, towards the head
		current = current->prev;
	}
	
	return (current != NULL);
}

template<class T>
void ReverseListIterator<T>::reset()
{
	// find the last element
	// start at the head
	Node<T>* temp = list.head;

	while(temp->next) {
		temp = temp->next;
	}

	current = temp;
}

template<class T>
int ReverseListIterator<T>::operator!()
{
	return (current != NULL);
}

#endif
