#include <vector>

template<class T>
class ElementProcessor
{
public:
	ElementProcessor()
	{
		fillVector();
	}

	~ElementProcessor()
	{
	}

private:
	CreateElement(T Interface);	
	vector<IDispatch> vecInterfaces;
	fillVector(void);
};


// CreateElement _com_ptr_t<IID>