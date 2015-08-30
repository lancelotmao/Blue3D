#pragma once

#include "RefCount.h"

//
// SmartPtr safely manages the reference count on the stack
//

template<typename T>
class SmartPtr
{
    // Allow access to other template's internal pointer
    template<typename U>
    friend class SmartPtr;

public:
    SmartPtr() :
            m_pointer(0)
    {

    }

    inline SmartPtr(const T *p) :
            m_pointer(const_cast<T*>(p))
    {
        if (m_pointer)
        {
            m_pointer->incr_ref_count();
        }
    }

    inline SmartPtr(const SmartPtr &r) :
            m_pointer(0)
    {
        m_pointer = r.m_pointer;

        if (m_pointer)
        {
            m_pointer->incr_ref_count();
        }
    }

    template<typename U>
    inline SmartPtr(const SmartPtr<U> &r) :
            m_pointer(0)
    {
        m_pointer = r.m_pointer;

        if (m_pointer)
        {
            m_pointer->incr_ref_count();
        }
    }

    ~SmartPtr()
    {
        if (m_pointer != 0)
        {
            m_pointer->decr_ref_count();
            m_pointer = 0;
        }
    }

    inline SmartPtr& operator=(const T* p)
    {
        // just in case, we do the increment before the decrement, and prior to the assignment
        T* new_pointer = const_cast<T*>(p);

        // notice the validity check
        if (new_pointer != 0)
        {
            // and increase a reference on the new guy we look at
            new_pointer->incr_ref_count();
        }

        if (m_pointer != 0)
        {
            // forget one reference on who we look at...
            m_pointer->decr_ref_count();
        }

        // do the assignment
        m_pointer = new_pointer;

        return *this;
    }

    inline SmartPtr& operator=(const SmartPtr& p)
    {
        return operator=(p.m_pointer);
    }

    template<typename U>
    inline SmartPtr& operator=(const SmartPtr<U> &r)
    {
        return operator=(r.m_pointer);
    }

    inline bool operator==(const T* p) const
    {
        return m_pointer == p;
    }

    inline bool operator==(const SmartPtr& p) const
    {
        return operator==(p.m_pointer);
    }

    template<typename U>
    inline bool operator==(const SmartPtr<U>& r)
    {
        return operator==(r.m_pointer);
    }

    inline bool operator!=(const T* p) const
    {
        return !operator==(p);
    }

    inline bool operator!=(const SmartPtr& p) const
    {
        return operator!=(p.m_pointer);
    }

    template<typename U>
    inline bool operator!=(const SmartPtr<U> &r)
    {
        return operator!=(r.m_pointer);
    }

    inline T* operator->() const
    {
        return m_pointer;
    }

    inline operator T*() const
    {
        return m_pointer;
    }

    inline bool references_object() const
    {
        return m_pointer != 0;
    }

    inline T* ptr() const
    {
        return m_pointer;
    }

    template<typename U>
    inline SmartPtr<U> cast() const
    {
        return SmartPtr<U>((U*) m_pointer);
    }

protected:
    T* m_pointer;
};
