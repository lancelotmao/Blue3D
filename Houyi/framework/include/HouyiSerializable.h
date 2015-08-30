#ifndef __Houyi__HouyiSerializable__
#define __Houyi__HouyiSerializable__

#include "HouyiDataOutputStream.h"
#include "HouyiDataInputStream.h"

namespace Houyi
{
    class Serializable
    {
    public:
        virtual ~Serializable(){}
        virtual void writeObject(DataOutputStream& dos) = 0;
        virtual void readObject(DataInputStream& dis) = 0;
    };
}
#endif
