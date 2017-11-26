#ifndef LOCKFREEHASHSET_H
#define LOCKFREEHASHSET_H

#include <atomic>
#include <cmath>
#include "BucketList.h"

#define THRESHOLD 4
#define uint size_t

template <typename T>
class LockFreeHashSet
{
protected:
	BucketList<T>** bucket;
	std::atomic<uint> bucketSize;
	std::atomic<uint> setSize;

public:
	LockFreeHashSet(int capacity)
	{
		bucket = new BucketList<T>*[capacity];
		std::fill_n(bucket, capacity, nullptr);
		bucket[0] = new BucketList<T>();
		bucketSize = 2;
		setSize = 0;
	}
	~LockFreeHashSet()
	{
		for (int i = 0; i < bucketSize; i++)
		{
			if (bucket[i] != nullptr)
				delete bucket[i];
		}
		delete[] bucket;
	}

	bool add(T item)
	{
		uint myBucket = std::abs((long)(BucketList<T>::hashCode(item) % bucketSize));
		BucketList<T>* bucketList = getBuckList(myBucket);
		if (!bucketList->add(item))
			return false;
		uint setSizeNow = setSize.fetch_add(1);
		uint bucketSizeNow = bucketSize;
		if (setSizeNow / (double)bucketSizeNow > THRESHOLD)
			bucketSize.compare_exchange_strong(bucketSizeNow, 2 * bucketSizeNow);
		return true;
	}

	bool remove(T item)
	{
		uint myBucket = std::abs((long)(BucketList<T>::hashCode(item) % bucketSize));
		BucketList<T>* bucketList = getBuckList(myBucket);
		if (!bucketList->remove(item))
			return false;
		return true;
	}

	bool contains(T item)
	{
		uint myBucket = std::abs((long)(BucketList<T>::hashCode(item) % bucketSize));
		BucketList<T>* bucketList = getBuckList(myBucket);
		return bucketList->contains(item);
	}

private:
	BucketList<T>* getBuckList(uint myBucket)
	{
		if (bucket[myBucket] == nullptr)
			initializeBucket(myBucket);
		return bucket[myBucket];
	}

	void initializeBucket(uint myBucket)
	{
		uint parent = getParent(myBucket);
		if (bucket[parent] == nullptr)
			initializeBucket(parent);
		BucketList<T>* bucketList = bucket[parent]->getSentinel(myBucket);
		if (bucketList != nullptr)
			bucket[myBucket] = bucketList;
	}

	uint getParent(uint myBucket)
	{
		uint parent = bucketSize;
		do {
			parent = parent >> 1;
		} while (parent > myBucket);
		parent = myBucket - parent;
		return parent;
	}

};


#endif