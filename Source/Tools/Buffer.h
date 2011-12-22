/**

*/

#ifndef _Buffer_h_
#define _Buffer_h_

#ifdef DELETE
#undef DELETE
#endif

#include <assert.h>
#include "BasicTypes.h"

namespace Pure
{

class Buffer
{
public:

    static const Int32 MaxSize = 4096;

    Buffer()
    {
        _start = 0;
        _end = 0;
    }

    void Clear()
    {
        _start = 0;
        _end = 0;
    }

    Int32 Size() const
    {
        return _end - _start;
    }

    Byte& operator[](Int32 i)
    {
        if(i > _end - _start - 1)
        {
            throw std::runtime_error("Out of bounds.");
        }

        return _buffer[_start + i];
    }

    template<class T>
    Buffer& operator<<(const T& val)
    {
        if(sizeof(T) > MaxSize - _end)
        {
            throw std::runtime_error("Not enough space.");
        }

        memcpy(&_buffer[_end], &val, sizeof(T));

        _end += sizeof(T);

        return *this;
    }

    template<class T>
    Buffer& operator>>(T& val)
    {

        if(sizeof(T) > _end - _start)
        {
            throw std::runtime_error("Not enough data.");
        }

        memcpy(&val, &_buffer[_start], sizeof(val));

        _start += sizeof(T);

        return *this;
    }

    Buffer& operator<<(const Buffer& val)
    {
        if(val.Size() > MaxSize - _end)
        {
            throw std::runtime_error("Not enough space.");
        }

        memcpy(&_buffer[_end], &(val._buffer[_start]), val.Size());

        _end += val.Size();

        return *this;
    }

    Buffer& operator>>(Buffer& val)
    {
        return val << *this;
    }

    const Byte* operator*() const
    {
        return _buffer;
    }

private:

    Int32 _start;
    Int32 _end;
    Byte _buffer[MaxSize];



public:
  class Modifier
  {
  private:
    Buffer& buffer;
  public:
      Modifier(Buffer& buffer)
        : buffer(buffer)
      {
      }

      Byte* operator*()
      {
          return buffer._buffer;
      }

      const Byte* operator*() const
      {
          return buffer._buffer;
      }

      void Reset(Int32 length)
      {
          buffer._start = 0;
          buffer._end = length;
      }
  };//end class Modifier
};


}// Pure


#endif // _Buffer_h_