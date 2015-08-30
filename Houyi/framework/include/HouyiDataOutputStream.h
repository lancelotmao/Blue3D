#ifndef __Houyi__HouyiDataOutputStream__
#define __Houyi__HouyiDataOutputStream__

#include <fstream>

namespace Houyi
{
    class DataOutputStream
    {
    public:
        DataOutputStream(ofstream& stream) : mFileStream(stream)
        {

        }

        inline void write(const void* p, int size)
        {
            mFileStream.write(reinterpret_cast<const char*>(p), size);
        }

        inline void writeBool(bool b)
        {
            char i = b ? 1 : 0;
            mFileStream.write(reinterpret_cast<char*>(&i), sizeof(char));
        }
        
        inline void writeByte(int b)
        {
            mFileStream.write(reinterpret_cast<char*>(&b), sizeof(char));
        }

        inline void writeInt(int i)
        {
            mFileStream.write(reinterpret_cast<char*>(&i), sizeof(int));
        }

        inline void writeIntArray(const int* ia, int offset, int length)
        {
            mFileStream.write(reinterpret_cast<const char*>(ia + offset), length * sizeof(int));
        }

        inline void writeUIntArray(const unsigned int* ia, int offset, int length)
        {
            mFileStream.write(reinterpret_cast<const char*>(ia + offset), length * sizeof(unsigned int));
        }

        inline void writeFloat(float f)
        {
            mFileStream.write(reinterpret_cast<char*>(&f), sizeof(float));
        }

        inline void writeFloatArray(const float* fa, int offset, int length)
        {
            mFileStream.write(reinterpret_cast<const char*>(fa + offset), length * sizeof(float));
        }

        inline void writeShort(int i)
        {
            mFileStream.write(reinterpret_cast<char*>(&i), sizeof(short));
        }

        inline void writeShortArray(const short* ia, int offset, int length)
        {
            mFileStream.write(reinterpret_cast<const char*>(&ia + offset), length * sizeof(short));
        }

        inline void writeUShortArray(const unsigned short* ia, int offset, int length)
        {
            mFileStream.write(reinterpret_cast<const char*>(&ia + offset), length * sizeof(unsigned short));
        }
        
        inline void writeUTFString(const string& str)
        {
            int len = (int)str.length();
            writeInt(len + 1);
            write(str.c_str(), len);
            writeByte(0);
        }

    private:
        ofstream& mFileStream;
    };
}
#endif
