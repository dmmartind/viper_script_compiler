//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
//
/////////////////////////////////////////////////////////////////////

#ifndef ARRAY2D_H
#define ARRAY2D_H


template <class Datatype>
class Array2D
{
public:

	
	Array2D(int p_width = 10, int p_height = 2)
	{
		m_array = new Datatype[p_width * p_height];

		if (Array2D<Datatype>::counter >= 0)
			Array2D<Datatype>::counter++;
		if (Array2D<Datatype>::counter < 0)
			return;
		
		m_width = p_width;
		m_height = p_height;

		Fill();
	}

	Array2D(Array2D<Datatype> &obj)
	{
		if (!Array2D<Datatype>::counter)
			return;
		if (Array2D<Datatype>::counter < 0)
			return;
		
		if (obj.m_array)
		{
			this->m_array = new Datatype[obj.Width() * obj.Height()];
		}

		if (!this->m_array)
			return;
		else
			Array2D<Datatype>::counter++;

		this->m_width = obj.m_width;
		this->m_height = obj.m_height;
		int datasize = obj.m_width*obj.m_height;

		for (int x = 0; x < datasize; x++)
		{
			this->m_array[x] = obj.m_array[x];

		}
	}

	Array2D<Datatype>& operator=(Array2D<Datatype> &obj)
	{
		if (this == &obj)
			return (*this);

		delete this->m_array;

		this->m_width = obj.m_width;
		this->m_height = obj.m_height;

		int datasize = obj.m_width*obj.m_height;
		this->m_array = new Datatype[obj.Width() * obj.Height()];


		for (int x = 0; x < datasize; x++)
		{
			this->m_array[x] = obj.m_array[x];
		}

		return *this;
	}


	~Array2D()
	{
		Array2D<Datatype>::counter--;
		if (!Array2D<Datatype>::counter)
			return;
		if (Array2D<Datatype>::counter < 0)
			return;

		if (m_array != 0)
			delete[] m_array;
		m_array = 0;
	}

	void Fill()
	{
		for (int x = 0; x < m_width; x++)
		{
			for (int y = 0; y < m_height; y++)
			{
				Get(x, y) = -1;
			}

		}


	}


	Datatype& Get(int p_x, int p_y)
	{
		return m_array[p_y * this->m_width + p_x];
	}

	void Resize(int p_width, int p_height)
	{
		Datatype* newarray = new Datatype[p_width * p_height];

		if (newarray == 0)
			return;

		int x, y, t1, t2;

		int minx = (p_width < m_width ? p_width : m_width);
		int miny = (p_height < m_height ? p_height : m_height);

		for (y = 0; y < miny; y++)
		{
			t1 = y * p_width;
			t2 = y * m_width;
			for (x = 0; x < minx; x++)
			{
				newarray[t1 + x] = m_array[t2 + x];
			}
		}
      
		if (m_array != 0)
			delete[] m_array;

		m_array = newarray;
		m_width = p_width;
		m_height = p_height;
	}


	int Size()
	{
		return m_width * m_height;
	}

	int Width()
	{
		return m_width;
	}

	int Height()
	{
		return m_height;
	}

	void setArr(Datatype* obj)
	{
		if (obj.m_array)
		{
			this->m_array = new Datatype[obj.Width() * obj.Height()];
		}

		this->m_width = obj.m_width;
		this->m_height = obj.m_height;
		int datasize = obj.m_width*obj.m_height;

		for (int x = 0; x < datasize; x++)
		{
			this->m_array[x] = obj.m_array[x];
		}
	}

private:
	static int counter;

	Datatype* m_array;
	int m_width;
	int m_height;
};

template<class T>
int Array2D<T>::counter = 0;

#endif