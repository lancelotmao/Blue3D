#include "HouyiAllocator.h"
#include "HouyiLog.h"

namespace Houyi
{
#ifdef DEBUG_MEM
    bool is_lock_inited = false;
    list<void*> m_list;
    pthread_mutex_t m_lock;

    void initLock()
    {
        if (!is_lock_inited)
        {
            pthread_mutex_init(&m_lock, 0);
            is_lock_inited = true;
        }
    }

    void lock()
    {
        initLock();
        pthread_mutex_lock (&m_lock);
    }

    void unlock()
    {
        initLock();
        pthread_mutex_unlock (&m_lock);
    }

    void* Allocator::operator new(size_t size, const char* file, int line) throw()
    {
        Block* p = (Block*)malloc(sizeof(Block) + size);
        if (!p)
        {
            LOGD("Out of memory file = %s, line = %d, size = %d\n", file, line, (int)size);
            return 0;
        }
        p->m_size = size;
        p->m_line = line;
        p->m_file = file;
        //LOGD("HouyiNew: size = %d, line = %d, file = %s\n", (int)size, line, file);
        lock();
        m_list.push_back(p);
        unlock();
        return p+1;
    }

    void* Allocator::operator new(size_t size, const char* file, int line, Block* p) throw()
    {
        p->m_size = size;
        p->m_line = line;
        p->m_file = file;
        lock();
        m_list.push_back(p);
        unlock();
        return p+1;
    }

    void Allocator::operator delete(void* p) throw()
    {
        if (!p)
        {
            return;
        }
        Block* block = (Block*)p-1;
        lock();
        m_list.remove(block);
        unlock();
        free(block);
    }

    void Allocator::pool_deallocate(void* p)
    {
        if (!p)
        {
            return;
        }
        Block* block = (Block*)p;
        lock();
        m_list.remove(block);
        unlock();
    }

    void* Allocator::operator new[](size_t size, const char* file, int line) throw()
    {
        return operator new(size, file, line);
    }

    void Allocator::operator delete[](void* p) throw()
    {
        operator delete(p);
    }

    void dump_allocator_list()
    {
        lock();
        if (m_list.size() == 0)
        {
            LOGD("congratulations! no memory leak\n");
        }
        else
        {
            std::list<void*>::iterator it;
            for (it=m_list.begin(); it!=m_list.end(); ++it)
            {
                Block* block = (Block*)(*it);
                const char* file = block->m_file;
                int line = block->m_line;
                size_t size = block->m_size;
                LOGE("leak file = %s, line = %d, size = %d\n", file, line, (int)size);
            }
        }
        unlock();
    }
#endif
}

