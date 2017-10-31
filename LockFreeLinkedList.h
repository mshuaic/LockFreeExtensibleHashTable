#ifndef LOCKFREELIST_H
#define LOCKFREELIST_H

#include <cstdint>
#include <atomic>
#include <functional>
#include <climits>
#include "MarkableReference.h"


template <typename T>
class Node
{
public:
	T item;
	int key;
	std::atomic<MarkableReference<Node<T>*>> next = MarkableReference<Node<T>*>();
	Node(int key, bool setSentinelNode)
	{
		if (setSentinelNode)
			this->key = key;
	}

	Node(T item)
	{
		this->item = item;
		this->key = std::hash<T>()(item);
	}
};


template<typename T>
class LockFreeList 
{
private:

	MarkableReference<Node<T>*> head;

public:
	LockFreeList();
	bool add(T item);
	bool remove(T item);
	bool contains(T item);
};

template<typename T>
LockFreeList<T>::LockFreeList()
{
	this->head = new Node<T>(INT_MIN, true);
	MarkableReference<Node<T>*> tail = new Node<T>(INT_MAX, true);
	while (!head->next.compare_exchange_strong(MarkableReference<Node<T>*>(), MarkableReference<Node<T>*>(tail, false)));
}



template<typename T>
bool LockFreeList<T>::add(T item)
{
	int key = std::hash<T>()(item);
	while (true)
	{
		Window<T>* window = find(head, key);
		MarkableReference<Node<T>*> pred = window->pred;
		MarkableReference<Node<T>*> curr = window->curr;
		if (curr->key == key)
		{
			return false;
		}
		else
		{
			Node<T>* node = new Node<T>(item);
			node->next = MarkableReference<Node<T>*>(curr, false);
			if (pred->next.compare_exchange_strong(MarkableReference<Node<T>*>(curr, false), MarkableReference<Node<T>*>(node, false)))
			{
				return true;
			}
		}
	}
}


template<typename T>
bool LockFreeList<T>::remove(T item)
{
	int key = std::hash<T>()(item);
	bool snip = false;
	while (true)
	{
		Window<T>* window = find(head, key);
		MarkableReference<Node<T>*> pred = window->pred;
		MarkableReference<Node<T>*> curr = window->curr;
		if (curr->key != key)
		{
			return false;
		}
		else
		{
			MarkableReference<Node<T>*> succ = curr->next.load();
			snip = curr->next.compare_exchange_strong(succ, MarkableReference<Node<T>*>(curr->next.load(), true));
			if (!snip)
			{
				continue;
			}
			pred->next.compare_exchange_strong(MarkableReference<Node<T>*>(curr, false), MarkableReference<Node<T>*>(succ, false));
			return true;
		}
	}
}

template<typename T>
bool LockFreeList<T>::contains(T item)
{
	int key = std::hash<T>()(item);
	Window<T>* window = find(head, key);
	MarkableReference<Node<T>*> pred = window->pred;
	MarkableReference<Node<T>*> curr = window->curr;
	return (curr.getRef()->key == key);
}

template <typename T>
class Window
{
public:
	MarkableReference<Node<T>*> pred;
	MarkableReference<Node<T>*> curr;
	Window(MarkableReference<Node<T>*> pred, MarkableReference<Node<T>*> curr)
	{
		this->curr = curr;
		this->pred = pred;
	}
};

template <typename T>
Window<T>* find(MarkableReference<Node<T>*> head, int key)
{
	MarkableReference<Node<T>*> pred;
	MarkableReference<Node<T>*> curr;
	MarkableReference<Node<T>*> succ;
	bool marked = false;
	bool snip;
retry:
	while (true) {
		pred = head;
		curr = pred->next.load();
		while (true)
		{
			succ = curr->next.load().get(&marked);
			while (marked)
			{
				snip = pred->next.compare_exchange_strong(MarkableReference<Node<T>*>(curr, false), MarkableReference<Node<T>*>(succ, false));
				if (!snip) goto retry;
				curr = pred->next.load();
				succ = curr->next.load().get(&marked);
			}
			if (curr.getRef()->key >= key)
				return new Window<T>(pred, curr);
			pred = curr;
			curr = succ;
		}
	}
}

#endif