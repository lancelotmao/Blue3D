#pragma once

//
// The base class
// Derive from this to enable tracking and cleanup of objects automatically
// Sub class should override clone if the data might be changed after
// copied or assigned from an existing object otherwise changing the data
// of the assignee object will change the data of the original object
// 

class RefCountBase
{
private:
    int m_ref_count;

public:
    RefCountBase() :
            m_ref_count(0)
    {

    }

    RefCountBase(const RefCountBase& rhs) :
            m_ref_count(0)
    {

    }

    RefCountBase& operator=(const RefCountBase& rhs)
    {
        return *this;
    }

    virtual ~RefCountBase() // pure virtual to prevent instanciation?
    {

    }

    virtual int get_ref_count() const
    {
        return m_ref_count;
    }

    virtual void incr_ref_count()
    {
        m_ref_count++;
    }

    virtual void decr_ref_count()
    {
        if (--m_ref_count == 0)
        {
            delete this;
        }
    }

    virtual RefCountBase* clone()
    {
        return new RefCountBase();
    }
};

