#ifndef LISTVISITOR_H
#define LISTVISITOR_H

template<class T>
class Node;

class ListVisitor 
{
public:
	Visitor() {}
	virtual ~Visitor() {}

	virtual void visit(Node<T>* node) = 0;
};


#endif