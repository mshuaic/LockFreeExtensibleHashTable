//#include "LockFreeLinkedList.h"
//
////template <typename T>
////class Node
////{
////public:
////	T item;
////	int key;
////	std::atomic<MarkableReference<Node<T>*>> next = MarkableReference<Node<T>*>();
////	Node(int key, bool setSentinelNode)
////	{
////		if(setSentinelNode)
////			this->key = key;
////	}
////	
////	Node(T item)
////	{
////		this->item = item;
////		this->key = std::hash<T>()(item);
////	}
////};
//
//template<typename T>
//LockFreeList<T>::LockFreeList()
//{
//	this->head = new Node<T>(INT_MIN, true);
//	MarkableReference<Node<T>*> tail = new Node<T>(INT_MAX, true);
//	while (!head->next.compare_exchange_strong(MarkableReference<Node<T>*>(), MarkableReference<Node<T>*>(tail, false)));
//}
//
//
//
//template<typename T>
//bool LockFreeList<T>::add(T item)
//{
//	int key = std::hash<T>()(item);
//	while (true)
//	{
//		Window<T>* window = find(head, key);
//		MarkableReference<Node<T>*> pred = window->pred;
//		MarkableReference<Node<T>*> curr = window->curr;
//		if (curr->key == key)
//		{
//			return false;
//		}
//		else
//		{
//			Node<T>* node = new Node<T>(item);
//			node->next = MarkableReference<Node<T>*>(curr, false);
//			if (pred->next.compare_exchange_strong(MarkableReference<Node<T>*>(curr, false), MarkableReference<Node<T>*>(node, false)))
//			{
//				return true;
//			}
//		}
//	}
//}
//
//
//template<typename T>
//bool LockFreeList<T>::remove(T item)
//{
//	int key = std::hash<T>()(item);
//	bool snip = false;
//	while (true)
//	{
//		Window<T>* window = find(head, key);
//		MarkableReference<Node<T>*> pred = window->pred;
//		MarkableReference<Node<T>*> curr = window->curr;
//		if (curr->key != key)
//		{
//			return false;
//		}
//		else
//		{
//			MarkableReference<Node<T>*> succ = curr->next.load();
//			snip = curr->next.compare_exchange_strong(succ, MarkableReference<Node<T>*>(curr->next.load(), true));
//			if (!snip)
//			{
//				continue;
//			}
//			pred->next.compare_exchange_strong(MarkableReference<Node<T>*>(curr, false), MarkableReference<Node<T>*>(succ, false));
//			return true;
//		}
//	}
//}
//
//template<typename T>
//bool LockFreeList<T>::contains(T item)
//{
//	int key = std::hash<T>()(item);
//	Window<T>* window = find(head, key);
//	MarkableReference<Node<T>*> pred = window->pred;
//	MarkableReference<Node<T>*> curr = window->curr;
//	return (curr.getRef()->key == key);
//}
//
//template <typename T>
//class Window
//{
//public:
//	MarkableReference<Node<T>*> pred;
//	MarkableReference<Node<T>*> curr;
//	Window(MarkableReference<Node<T>*> pred, MarkableReference<Node<T>*> curr)
//	{
//		this->curr = curr;
//		this->pred = pred;
//	}
//};
//
//template <typename T>
//Window<T>* find(MarkableReference<Node<T>*> head, int key)
//{
//	MarkableReference<Node<T>*> pred;
//	MarkableReference<Node<T>*> curr;
//	MarkableReference<Node<T>*> succ;
//	bool marked = false;
//	bool snip;
//retry: 
//	while (true) {
//		pred = head;
//		curr = pred->next.load();
//		while (true)
//		{
//			succ = curr->next.load().get(&marked);
//			while (marked)
//			{
//				snip = pred->next.compare_exchange_strong(MarkableReference<Node<T>*>(curr,false), MarkableReference<Node<T>*>(succ, false));
//				if (!snip) goto retry;
//				curr = pred->next.load();
//				succ = curr->next.load().get(&marked);
//			}
//			if (curr.getRef()->key >= key)
//				return new Window<T>(pred, curr);
//			pred = curr;
//			curr = succ;
//		}
//	}
//}
//
//
//
////template<typename T>
////class LockFreeList
////{
////	MarkableReference<Node<T>*> head;
////
////public:
////	LockFreeList()
////	{
////		this->head = new Node<T>(INT_MIN,true);
////		MarkableReference<Node<T>*> tail = new Node<T>(INT_MAX,true);
////		while (!head->next.compare_exchange_strong(MarkableReference<Node<T>*>(), MarkableReference<Node<T>*>(tail, false)));
////	}
////
////	bool add(T item)
////	{
////		int key = std::hash<T>()(item);
////		while (true)
////		{
////			Window<T>* window = find(head, key);
////			MarkableReference<Node<T>*> pred = window->pred;
////			MarkableReference<Node<T>*> curr = window->curr;
////			if (curr->key == key)
////			{
////				return false;
////			}
////			else
////			{
////				Node<T>* node = new Node<T>(item);
////				node->next = MarkableReference<Node<T>*>(curr, false);
////				if (pred->next.compare_exchange_strong(MarkableReference<Node<T>*>(curr, false), MarkableReference<Node<T>*>(node, false)))
////				{
////					return true;
////				}
////			}
////		}
////	}
////
////	bool remove(T item)
////	{
////		int key = std::hash<T>()(item);
////		bool snip = false;
////		while (true)
////		{
////			Window<T>* window = find(head, key);
////			MarkableReference<Node<T>*> pred = window->pred;
////			MarkableReference<Node<T>*> curr = window->curr;
////			if (curr->key != key)
////			{
////				return false;
////			}
////			else
////			{
////				MarkableReference<Node<T>*> succ = curr->next.load();
////				snip = curr->next.compare_exchange_strong(succ, MarkableReference<Node<T>*>(curr->next.load(), true));
////				if (!snip)
////				{
////					continue;
////				}
////				pred->next.compare_exchange_strong(MarkableReference<Node<T>*>(curr, false), MarkableReference<Node<T>*>(succ, false));
////				return true;
////			}
////		}
////	}
////
////	bool contains(T item) 
////	{
////		int key = std::hash<T>()(item);
////		Window<T>* window = find(head, key);
////		MarkableReference<Node<T>*> pred = window->pred;
////		MarkableReference<Node<T>*> curr = window->curr;
////		return (curr.getRef()->key == key);
////	}
////};
//
////bool get_flag(void* p)
////{
////	return (uintptr_t)p & 1;
////}
////
////void *get_pointer(uintptr_t p)
////{
////	return (void *)(p & (UINTPTR_MAX ^ 1));
////}
////
////void* set_flag(void* p, bool value)
////{
////	return (void*)(((uintptr_t)p & (UINTPTR_MAX ^ 1)) | (int)value);
////}
////
////const int i = 1;
////#define is_bigendian() ( (*(char*)&i) == 0 )
//
////Node<int>& foo()
////{
////	Node<int> node(6);
////	printf("%d\n", node.item);
////	return *(new Node<int>(7));
////}
//
//
////#define NUM_THREAD 4
////
////int main(void)
////{
////	//Node<int> node(6);
////	//MarkableReference<Node<int>*> curr(&Node<int>(5));
////	//MarkableReference<Node<int>*> aaa(&Node<int>(111));
////	//Node<int>* p;
////	//p = &foo();
////	//curr = aaa;
////	//
////	//printf("%d\n", curr->item);
////	//printf("%d\n", p->item);
////
////	LockFreeList<int> list;
////	list.add(5);
////	if (list.add(5) == false)
////		printf("no add\n");
////	if (list.contains(5))
////		printf("YOLO\n");
////	list.remove(5);
////	if (!list.contains(5))
////		printf("YOLO\n");
////	//std::vector<std::thread> threads;
////	//for (int i = 0; i < NUM_THREAD; i++)
////	//{
////	//	threads.push_back(std::thread(list.add(5)));
////	//}
////	//for (auto & t : threads)
////	//{
////	//	t.join();
////	//}
////
////	getchar();
////	return 0;
////}
//
