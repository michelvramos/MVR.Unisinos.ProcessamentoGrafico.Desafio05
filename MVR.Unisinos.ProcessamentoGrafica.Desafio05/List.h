#pragma once
template <class T> class List
{
private:
	T* _array;
	unsigned int _capacity;
	unsigned int _length;

public:
	List();
	List(unsigned int capacity);
	~List();
	void Add(T);
	void AddRange(List<T> list);
	unsigned int Length();
	void Clear();
	T Get(unsigned int index);
	T RemoveAt(unsigned int index);
};

template<class T>
void List<T>::Add(T obj)
{
	if (_length < _capacity)
	{
		_array[_length] = obj;
		_length++;
	}
	else
	{
		unsigned int _newCapacity = _capacity * 2;
		T* temp = new T[_newCapacity];

		for (int i = 0; i < _capacity; i++)
		{
			temp[i] = _array[i];
		}

		delete[] _array;
		_array = temp;
		_capacity = _newCapacity;

		_array[_length] = obj;
		_length++;
	}
}

template<class T>
void List<T>::AddRange(List<T> list)
{
	throw "Not implemented";
}

template<class T>
unsigned int List<T>::Length()
{
	return _length;
}

template<class T>
void List<T>::Clear()
{
	delete[] _array;
	_length = 0;
	_capacity = 3;
	_array = new T[_capacity];
}

template<class T>
T List<T>::Get(unsigned int index)
{
	if (index < _length)
	{
		return _array[index];
	}

	throw "Index out of range";
}

template<class T>
T List<T>::RemoveAt(unsigned int index)
{
	throw "Not implemented";
}


template<class T>
List<T>::List()
{
	_capacity = 3;
	_array = new T[_capacity];
}

template<class T>
List<T>::List(unsigned int capacity)
{
	_array = new T[capacity];
	_capacity = capacity;
	_length = 0;
}

template<class T>
inline List<T>::~List()
{
	delete[] _array;
}


