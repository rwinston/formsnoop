#ifndef ITERATOR_H
#define ITERATOR_H

template<class T>
class Iterator
{
	public:
		virtual void reset()= 0;
		virtual T operator()() = 0;
		virtual void operator=(T t) = 0;
		virtual int operator!()= 0;
		virtual int operator++() = 0;
};

#endif
