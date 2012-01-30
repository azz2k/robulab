/**

*/

#ifndef _BasicTypes_h_
#define _BasicTypes_h_

#ifdef DELETE
#undef DELETE
#endif

#include <vector>
#include <iostream>

namespace Pure
{
  typedef unsigned char Byte;

  typedef short Int16;
  typedef long Int32;
  typedef long long Int64;

  typedef unsigned short UInt16;
  typedef unsigned long UInt32;
  typedef unsigned long long UInt64;
  
  typedef float Float32;
  typedef double Float64;

  typedef std::vector<Byte> Bytes;
};


#endif // _BasicTypes_h_