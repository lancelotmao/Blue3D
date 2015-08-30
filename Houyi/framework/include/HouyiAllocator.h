#ifndef HouyiAllocator_H
#define HouyiAllocator_H

#include <list>

#ifdef DEBUG_MEM
#define HouyiNew new(__FILE__, __LINE__)
#define HouyiPoolNew(T) new(__FILE__, __LINE__, (Block*)(MemoryPool< PoolBlock<T> >::allocate()))
#define HouyiDelete(A) delete (A)
#define HouyiPoolDelete(T, A) pool_deallocate(MemoryPool< PoolBlock<T> >::deallocate((PoolBlock<T>*)((Block*)(A)-1)))
#define HouyiDestroyPool(T) HouyiDelete(MemoryPool< PoolBlock<T> >::getInstance())
#else
#define HouyiNew new
#define HouyiPoolNew(T) new(MemoryPool<T>::allocate())
#define HouyiDelete delete
#define HouyiPoolDelete(T, A) (MemoryPool<T>::deallocate(A))
#define HouyiDestroyPool(T) HouyiDelete(MemoryPool<T>::getInstance())
#define dump_allocator_list()
#endif

namespace Houyi
{
#ifdef DEBUG_MEM
    extern std::list<void*> m_list;
    void dump_allocator_list();

    struct Block
    {
        size_t m_size;
        int m_line;
        const char* m_file;
    };

    template<typename T>
    struct PoolBlock
    {
        Block m_block;
        T m_data;
    };
#endif

    class Allocator
    {
    public:
        virtual ~Allocator(){}
#ifdef DEBUG_MEM
        void* operator new(size_t size, const char* file, int line) throw();
        void operator delete(void* p) throw();

        void* operator new(size_t size, const char* file, int line, Block* p) throw();
        void pool_deallocate(void* p);

        void* operator new[](size_t size, const char* file, int line) throw();
        void operator delete[](void* p) throw();
#endif
    };
}

#endif
