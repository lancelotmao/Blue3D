#ifndef __Houyi__HouyiImage__
#define __Houyi__HouyiImage__

namespace Houyi
{
    class Image
    {
    public:
        enum ImageFormat
        {
            E8888
        };

    public:
        virtual ~Image(){}

        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;

        virtual void lock() = 0;
        virtual void unlock() = 0;

        virtual void* getData() const = 0;
    };
};

#endif
