template<typename T>
struct Array
{
private:
	int _Capacity = 0;
	int _Count = 0;
	T* _Values = NULL;

	void __ctor(int capacity)
	{
		_Capacity = capacity;
		_Values = (T*)malloc(sizeof(T) * _Capacity);
	}
public:
	Array()
	{
		__ctor(0);
	}
	Array(int capacity)
	{
		__ctor(capacity);
	}

	int Count()
	{
		return _Count;
	}
	void Add(const T &value)
	{
		if (_Count == _Capacity)
		{
			_Capacity = (_Capacity & ~0xff) + 0x100;

			T *newValues = (T*)malloc(sizeof(T) * _Capacity);
			memcpy(newValues, _Values, sizeof(T) * _Count);

			free(_Values);
			_Values = newValues;
		}

		_Values[_Count++] = value;
	}

	void Clear() {
		_Count = 0;
	}

	T& operator [](int index)
	{
		if (index < 0 || index >= _Count) 
		{
			throw;
		}
		return _Values[index];
	}
};