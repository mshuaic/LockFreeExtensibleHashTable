#ifndef LOCKFREELIST_H
#define LOCKFREELIST_H

#include <atomic>
#include <functional>
#include <limits>
#include <cstdint>
#include <thread>
#include "StampedMarkableReference.h"

#define SMR StampedMarkableReference<Node<T>*>

template <typename T>
class Node
{
public:
	T item;
	size_t key;
	std::atomic<SMR> next = nullptr;// = SMR();
	Node<T>* _next = nullptr;
	Node(size_t key, bool setSentinelNode)
	{
		if (setSentinelNode)
			this->key = key;
		this->next = SMR();
	}

	Node(T item)
	{
		this->item = item;
		this->key = std::hash<T>()(item);
		this->next = SMR();
	}

	Node()
	{
		this->next = SMR();
	}

	~Node()
	{
		delete this;
	}

	void set(T item)
	{
		this->item = item;
		this->key = std::hash<T>()(item);
	}
};

//template<typename T>
thread_local void* freeList = nullptr;

template<typename T>
class LockFreeList
{
private:

	SMR head;

public:
	LockFreeList();
	bool add(T item);
	bool remove(T item);
	bool contains(T item);

	Node<T>* allocate(T item)
	{
		Node<T>* node = (Node<T>*)freeList;
		if (node == nullptr)
			node = new Node<T>();
		else
			freeList = (void*)(node->_next);
		node->set(item);
		return node;
	}

	static void free(Node<T>* node)
	{
		//node->next = SMR(node->next.load().getRef());
		node->_next = (Node<T>*)freeList;
		freeList = (void*)node;
	}

	~LockFreeList()
	{

		Node<T>* temp = nullptr;
		Node<T>* curr = head.getRef();
		while (curr->next.load().getRef() != nullptr)
		{
			temp = curr;
			curr = curr->next.load().getRef();
			delete temp;
		}
	}
};

template<typename T>
LockFreeList<T>::LockFreeList()
{
	this->head = new Node<T>(0, true);
	SMR tail = new Node<T>(SIZE_MAX, true);
	while (!head->next.compare_exchange_strong(SMR(), SMR(tail, false)));
}

//template<typename T>
//LockFreeList<T>::~LockFreeList()
//{
//
//	SMR temp = nullptr;
//	SMR curr = head;
//	while (curr->next != nullptr)
//	{
//		temp = curr;
//		curr = curr->next;
//		delete temp;
//	}
//}


template<typename T>
bool LockFreeList<T>::add(T item)
{
	size_t key = std::hash<T>()(item);
	Node<T>* node = allocate(item);
	while (true)
	{
		Window<T>* window = find(head, key);
		SMR pred = window->pred;
		SMR curr = window->curr;
		delete window;
		if (curr->key == key)
		{
			if (node != nullptr)
				//delete node;
				free(node);
			return false;
		}
		else
		{
			uint currStamp = 0;
			pred->next.load().getStamp(&currStamp);	
			node->next = SMR(curr);
			if (pred->next.compare_exchange_strong(SMR(curr, currStamp, false), SMR(node, 0, false)))
			{
				return true;
			}
		}
	}
}


template<typename T>
bool LockFreeList<T>::remove(T item)
{
	size_t key = std::hash<T>()(item);
	uint currStamp = 0;
	uint succStamp = 0;
	bool snip = false;
	while (true)
	{
		Window<T>* window = find(head, key);
		SMR pred = window->pred;
		SMR curr = window->curr;
		delete window;
		if (curr->key != key)
		{
			return false;
		}
		else
		{
			SMR succ = curr->next.load();
			snip = curr->next.compare_exchange_strong(succ, SMR(curr->next.load(), true));
			if (!snip)
			{
				continue;
			}
			/*pred->next.load().getStamp(&currStamp);
			curr->next.load().getStamp(&succStamp);
			pred->next.compare_exchange_strong(SMR(curr, currStamp, false), SMR(succ, succStamp, false));*/
			return true;
		}
	}
}

template<typename T>
bool LockFreeList<T>::contains(T item)
{
	size_t key = std::hash<T>()(item);
	Window<T>* window = find(head, key);
	SMR pred = window->pred;
	SMR curr = window->curr;
	delete window;
	return (curr.getRef()->key == key);
}

template <typename T>
class Window
{
public:
	SMR pred;
	SMR curr;
	Window(SMR pred, SMR curr)
	{
		this->curr = curr;
		this->pred = pred;
	}
};

template <typename T>
Window<T>* find(SMR head, size_t key)
{
	SMR pred;
	SMR curr;
	SMR succ;
	uint predStamp = 0;
	uint currStamp = 0;
	uint succStamp = 0;
	bool marked = false;
	bool snip;
retry:
	while (true) {
		pred = head.getStamp(&predStamp);
		curr = pred->next.load().getStamp(&currStamp);
		while (true)
		{
			succ = curr->next.load().get(&succStamp, &marked);
			while (marked)
			{
				pred->next.load().getStamp(&currStamp);
				snip = pred->next.compare_exchange_strong(SMR(curr.getRef(), currStamp, false), SMR(succ.getRef(), currStamp+1, false));
				if (!snip) goto retry;
				Node<T>* temp = curr.getRef();
				curr = pred->next.load();
				succ = curr->next.load().getMark(&marked);
				LockFreeList<T>::free(temp);
				//delete temp;
			}
			if (curr.getRef()->key >= key)
				return new Window<T>(pred, curr);
			pred = curr;
			curr = succ;
		}
	}
}

#endif