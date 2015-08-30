#ifndef houyi_Vector_h
#define houyi_Vector_h
#include "HouyiLog.h"

namespace Houyi
{
    const long DEFAULT_SIZE = 2000;
    const float SIZE_RATIO = 1.2;
    
	template <class T> class WeakVector
    {
    public:
		WeakVector(long capacity = DEFAULT_SIZE) : mCapacity(capacity) , mSize(0) {
			mData = new T[capacity];
			LOGE("new WeakVector: %d, %d", (int)mCapacity, (int)mSize);
		}

		~WeakVector() {
			LOGE("delete WeakVector");
			delete []mData;
		}

        T* getData() {
        	return mData;
        }

        inline void add(T t) {
			if (mSize >= mCapacity) {
				increateSize();
			}

			mData[mSize++] = t;
        }

        inline void clear() {
    		memset(mData, 0, sizeof(T) * mSize);
        	mSize = 0;
        }

		inline T operator [] ( const size_t i ) const
        {
            return *(mData+i);
        }

		inline T& operator [] ( const size_t i )
        {
            return *(mData+i);
        }

		inline long size() {
			return mSize;
		}

		inline long capaity() {
			return mCapacity;
		}
    private:
        void increateSize() {
    		int oldCapacity = mCapacity;
    		T* oldData = mData;
    		mCapacity = (long)(mCapacity * SIZE_RATIO);
    		mData = new T[mCapacity];
    		memcpy(mData, oldData, sizeof(T) * oldCapacity);
    		delete[] oldData;
        }
        
    protected:
        long mCapacity;
        long mSize;
        T* mData;
    };
}
#endif
