#ifndef Houyi_HouyiMemoryPool_h
#define Houyi_HouyiMemoryPool_h

#include <queue>
#include "HouyiAllocator.h"

namespace Houyi
{
    template<typename T>
    class MemoryPool : public Allocator
    {
    public:
        static MemoryPool* getInstance()
        {
            if (!mInstance)
            {
                mInstance = HouyiNew MemoryPool<T>();
            }
            return mInstance;
        }

        static T* allocate()
        {
            MemoryPool* pool = MemoryPool::getInstance();
            return pool->_allocate();
        }

        static T* deallocate(T* p)
        {
            MemoryPool* pool = MemoryPool::getInstance();
            return pool->_deallocate(p);
        }

        ~MemoryPool()
        {
            delete[] (char*)mData;
            mInstance = 0;
        }

    private:
        MemoryPool()
        {
            int alignment = 16;
            size_t offset = alignment - (sizeof(T) & (alignment-1));
            size_t alignedSize = sizeof(T) + offset;
            mCapacity = POOL_SIZE / alignedSize;
            LOGD("creating memory pool T size = %d, aligned size = %d, capacity = %d",
                    (int)sizeof(T), (int)alignedSize, mCapacity);
            mData = (T*)(new char[POOL_SIZE]);
            for (int i = 0;i < mCapacity;++i)
            {
                mAvailableQueue.push(mData + i);
            }
        }

        T* _allocate()
        {
            T* data = mAvailableQueue.front();
            mAvailableQueue.pop();
            return data;
        }

        T* _deallocate(T* p)
        {
            p->~T();
            mAvailableQueue.push(p);
            return p;
        }

        static MemoryPool* mInstance;
        static const int POOL_SIZE;

        T* mData; // allocate on heap to avoid calling constructor twice
        std::queue<T*> mAvailableQueue;
        int mCapacity;
    };

    template<typename T> MemoryPool<T>* MemoryPool<T>::mInstance = 0;
    template<typename T> const int MemoryPool<T>::POOL_SIZE = 1024 * 1024 * 4;
}
#endif
