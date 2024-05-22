//////////////////////////////////////////////////////////////////////
//2017 Copyright David Martin. All Rights Reserved. 
/////////////////////////////////////////////////////////////////////

#ifndef SLinkList_
#define SLinkList_
#pragma once


template<class T> class DNode;
template<class T> class DList;
template<class T> class DLItr;

template<class T>
class DNode
{
public:
	//friend DLItr<T>;
	//friend DList<T>;

	DNode()
	{

		DNode<T>* next = 0;
		DNode<T>* prev = 0;
	}
	void DLink()
	{
		if (prev != 0)
			prev->next = next;
		if (next != 0)
			next->prev = prev;
	}
	void InsertBefore(T data)
	{
		DNode<T>* temp = new DNode<T>;
		temp->data = data;
		temp->next = this;
		temp->prev = prev;
		if (prev != 0)
			prev->next = temp;
		prev = temp;

	}

	void InsertAfter(T data)
	{
		DNode<T>* temp = new DNode<T>;
		temp->data = data;
		temp->next = next;
		temp->prev = this;
		if (next != 0)
			next->prev = temp;
		next = temp;
	}


	T data;
	DNode<T>* next;
	DNode<T>* prev;
};

template <class T>
class DLItr
{
public:
	//friend DList<T>;
	DLItr()
	{
		list = 0;
		node = 0;
		count = 0;
	}

	DLItr(DList<T>* plist, DNode<T>* pnode)
	{
		list = plist;
		node = pnode;
		count = plist->getCount();
	}
	void Start()
	{
		if (list != 0)
			node = list->head;
	}
	void End()
	{
		if (list != 0)
			node = list->tail;
	}
	void Forth()
	{
		if (node != 0)
			node = node->next;

	}
	void Back()
	{
		if (node != 0)
			node = node->prev;
	}
	T& Item()
	{
		return node->data;
	}
	bool Valid()
	{
		return (node != 0);
	}
	int Size()
	{
		return count;
	}
	bool operator==(DLItr<T>& rhs)
	{
		if (node == rhs.node && list == rhs.list)
		{
			return true;
		}
		return false;

	}
private:
	DNode<T>* node;
	DList<T>* list;
	int count;
};

template <class T>
class DList
{
public:
	friend DLItr<T>;
	DList()
	{
		head = 0;
		tail = 0;
		count = 0;
	}
	~DList()
	{
		DNode<T>* node = head;
		DNode<T>* temp;

		while (node)
		{
			temp = node->next;
			delete node;
			node = temp;
		}
	}

	void TrapAppend(T data)
	{
		int length = strlen(data);
		char* newString = (char*)malloc(length * sizeof(char) + 1);
		int x = 0;
		for (x = 0; x < length; x++)
		{
			newString[x] = data[x];
		}

		newString[x] = '\0';
		this->Append(newString);
	}
	void Append(T data)
	{
		if (head == 0)
		{
			head = tail = new DNode<T>;
			head->data = data;
			head->next = 0;
			head->prev = 0;
		}
		else
		{
			tail->InsertAfter(data);
			tail = tail->next;
		}
		count++;
	}


	void Prepend(T data)
	{
		if (!head)
		{
			head = tail = new DNode<T>;
			head->data = data;
			head->next = 0;
			head->prev = 0;
		}
		else
		{
			head->InsertBefore(data);
			head = head->prev;
		}
		count++;
	}
	void RemoveHead()
	{
		if (!head)
			return;
		DNode<T>* temp = head->next;
		delete head;
		head = temp;
		if (head == 0)
		{
			tail = 0;
		}
		else
			head->prev = 0;
		count--;
	}
	void RemoveTail()
	{
		if (tail)
			return;
		DNode<T>* temp = tail->prev;
		delete tail;
		tail = temp;
		if (tail == 0)
			head = 0;
		else
			tail->next = 0;
		count--;
	}
	void InsertBefore(DLItr<T>& node, T data)
	{
		if (node.node != 0)
		{
			node.InsertBefore(data);
			if (node.node == head)
				head = head->prev;
			count++;
		}
		else
		{
			Prepend(data);
		}
	}
	void InsertAfter(DLItr<T>& pnode, T data)
	{
		if (pnode.node)
		{
			pnode.node->InsertAfter(data);
			if (pnode.node == tail)
				tail = tail->next;
			count++;
		}
		else
		{
			Append(data);
		}

	}

	void Remove(DLItr<T>& data)
	{
		DNode<T>* temp;
		if (!data.node)
			return;
		temp = data.node;
		if (temp == head)
			head = head->next;
		else if (temp == tail)
			tail = tail->prev;
		data.Forth();
		temp->DLink();
		delete temp;
		if (head == 0)
			tail = 0;
		count--;
	}

	void Append(T input, DLItr<T>& data)
	{
		data.node->InsertAfter(input);

	}

	void Prepend(T input, DLItr<T>& data)
	{
		data.node->InsertBefore(input);

	}


	DLItr<T> GetIt()
	{
		return DLItr<T>(this, head);
	}
	void setCount(int data)
	{
		this->count = data;
	}
	int getCount()
	{
		return this->count;
	}
	DNode<T>* returnHead()
	{
		return head;
	}

	char* GetStringByIndex(int iIndex)
	{
		char* nil = 0;
		int _count = 0;
		DLItr<T> obj = this->GetIt();

		for (obj.Start(); obj.Valid(); obj.Forth())
		{
			if (iIndex == _count)
				return (char*)obj.Item();
			_count++;
		}
		return nil;
	}
	protected:
	int count;

	DNode<T>* head;
	DNode<T>* tail;


};

#endif