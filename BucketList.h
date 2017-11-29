#ifndef BUCKETLIST_H
#define BUCKETLIST_H

#include <cstdint>
#if SIZE_MAX == UINT32_MAX
#define WORD_SIZE 24
#define HI_MASK 0x00800000
#define MASK 0x00FFFFFF
#elif SIZE_MAX == UINT64_MAX
#define WORD_SIZE 56
#define HI_MASK 0x008000000000
#define MASK 0x00FFFFFFFFFFFFFF
#endif

#define LO_MASK 0x1

#include "StampedMarkableReference.h"
#define hash_t size_t
#include <atomic>
#include <functional>
#include <memory>

#ifndef SMR
#define SMR StampedMarkableReference<Node<T>*>
#endif

thread_local void* freeList = nullptr;

template <typename T>
class BucketList;

template <typename T>
class Window;

template <typename T>
class Node
{
public:
	T item;
	hash_t key;
	std::atomic<SMR> next = { nullptr };
	Node<T>* _next = nullptr;
	Node(hash_t key, bool setSentinelNode)
	{
		if (setSentinelNode)
		{
			this->key = key;
			this->next = SMR();
			this->item = (T)NULL;
		}

	}

	Node(hash_t key, T item)
	{
		this->key = key;
		this->item = item;
		this->next = SMR();
	}

	Node()
	{
		this->next = SMR();
	}


	void set(hash_t key, T item)
	{
		this->key = key;
		this->item = item;
		//if (this->next.load() != nullptr && this->next.load()->key != SIZE_MAX)
		//	this->next = SMR();
		//this->_next = nullptr;
	}

	SMR getNext()
	{
		bool cMarked = false;
		bool sMarked = false;
		Node<T>* entry = this->next.load().getMark(&cMarked);
		//SMR entry = this->next.load().getMark(&cMarked);
		bool isStamped = false;
		size_t entryStamp = 0;
		this->next.load().getStamp(&entryStamp);
		bool flag = false;
		while (cMarked)
		{
			Node<T>* succ = entry->next.load().getMark(&sMarked);
			this->next.load().getStamp(&entryStamp);
			SMR expected = SMR(entry, entryStamp, true);
			flag = this->next.compare_exchange_strong(expected, SMR(succ, entryStamp + 1, sMarked));
			isStamped = true;
			Node<T>* temp = entry;
			entry = this->next.load().getMark(&cMarked);
			if (flag)
				BucketList<T>::free(temp);
		}
		return SMR(entry, entryStamp + (int)isStamped, cMarked);
	}
};


template<typename T>
class BucketList
{
private:
	
	SMR head;
	BucketList(SMR e)
	{
		this->head = e;
	}

	hash_t makeSentinelKey(hash_t key)
	{
		return reverse(key & MASK);
	}
public:
	BucketList() {
		this->head = new Node<T>(0, true);
		this->head->next = SMR(new Node<T>(SIZE_MAX, true), false);
	}

	~BucketList()
	{
		if (head.getRef() == nullptr)
			return;
		Node<T>* temp = nullptr;
		Node<T>* curr = head.getRef();
		while (curr != nullptr)
		{
			temp = curr;
			curr = curr->next.load().getRef();
			delete temp;
		}
	}

	Node<T>* allocate(hash_t key)
	{
		return allocate(key, (T)NULL);
	}


	Node<T>* allocate(hash_t key, T item)
	{
		Node<T>* node = (Node<T>*)freeList;
		if (node == nullptr)
			node = new Node<T>();
		else
			freeList = (void*)(node->_next);
		node->set(key, item);
		return node;
	}

	static void free(Node<T>* node)
	{
		//node->next = SMR((Node<T>*)nullptr);
		node->_next = (Node<T>*)freeList;
		freeList = (void*)node;
	}


	static hash_t hashCode(T item) {
		return std::hash<T>()(item) & MASK;
	}

	bool add(T item)
	{
		hash_t key = makeRegularKey(item);
		SMR entry = allocate(key, item);
		bool splice;
		while (true)
		{
			Window<T>* window = find(head, key);
			SMR pred = window->pred;
			SMR curr = window->curr;
			delete window;
			if (curr->key == key)
			{
				//if (entry.getRef() != (T)nullptr)
				//{
				free(entry.getRef());
				//}
				return false;
			}
			else
			{
				size_t currStamp = 0;
				pred->next.load().getStamp(&currStamp);
				//SMR entry = new Node<T>(key, item);
				entry->next = SMR(curr);
				SMR expected = SMR(curr, currStamp, false);
				splice = pred->next.compare_exchange_strong(expected, SMR(entry, 0, false));
				if (splice)
					return true;
				else
					continue;
			}
		}
	}

	bool remove(T item)
	{
		hash_t key = makeRegularKey(item);
		bool snip;
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
				//Node<T>* temp = pred->next.load().getRef();
				//SMR currTemp = pred->next.load();
				snip = pred->next.compare_exchange_strong(curr, SMR(curr, true));
				if (snip)
					return true;
				else
					continue;
			}

		}
	}

	bool contains(T item)
	{
		//hash_t key = makeRegularKey(item);
		//Window<T>* window = find(head, key);
		//SMR pred = window->pred;
		//SMR curr = window->curr;
		//delete window;
		//return (curr->key == key);
		hash_t key = makeRegularKey(item);
		bool marked = false;
		SMR curr = head;
		while (curr->key < key)
		{
			curr = curr->next.load();
			curr->next.load().getMark(&marked);
		}
		return (curr->key == key && marked);

	}

	BucketList<T>* getSentinel(unsigned int index)
	{
		hash_t key = makeSentinelKey(index);
		SMR entry = allocate(key);
		bool splice;
		while (true)
		{
			Window<T>* window = find(head, key);
			SMR pred = window->pred;
			SMR curr = window->curr;
			delete window;
			if (curr->key == key)
			{
				//if (entry.getRef() != (T)nullptr)
				free(entry.getRef());
				return new BucketList<T>(curr);
			}
			else
			{
				size_t currStamp = 0;
				pred->next.load().getStamp(&currStamp);
				//SMR entry = new Node<T>(key, true);
				entry->next = SMR(curr);
				SMR expected = SMR(curr, currStamp, false);
				splice = pred->next.compare_exchange_strong(expected, SMR(entry, 0, false));
				if (splice)
				{
					return new BucketList<T>(entry);
				}
				else
				{
					continue;
				}
			}
		}
	}


	hash_t reverse(hash_t key)
	{
		hash_t loMask = LO_MASK;
		hash_t hiMask = HI_MASK;
		hash_t result = 0;
		for (int i = 0; i < WORD_SIZE; i++)
		{
			if ((key & loMask) != 0)
			{
				result |= hiMask;
			}
			loMask <<= 1;
			hiMask >>= 1;
		}
		return result;
	}




	hash_t makeRegularKey(T item)
	{
		hash_t code = std::hash<T>()(item) & MASK;
		return reverse(code | HI_MASK);
	}


};

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
Window<T>* find(SMR head, hash_t key)
{
	SMR pred = head;
	SMR curr = head->getNext();
	while (curr->key < key)
	{
		pred = curr;
		curr = pred->getNext();
	}
	return new Window<T>(pred, curr);
}


#endif