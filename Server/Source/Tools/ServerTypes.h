/**

*/

#ifndef _ServerTypes_h_
#define _ServerTypes_h_

#ifdef DELETE
#undef DELETE
#endif


#include "BasicTypes.h"
#include "Buffer.h"

#include <iostream>

namespace Pure
{
  enum Action
  {
    GET = 0x00,     // retrieve full content of a service state
    QUERY = 0x01,   // query information from a service
    REPLACE = 0x02, // replace the full state of a service
    UPDATE = 0x03,  // replace a part of the state
    INSERT = 0x04,  // insert items in a state
    _DELETE = 0x05,  // delete things in a state
  };


  /*
  enum Result
  {
    SUCCESS = 0x00,
    TARGET_DOES_NOT_EXIST = 0x01,
    ACTION_IS_NOT_SUPPORTED = 0x02,
    UNKNOWN_ACTION = 0x03,
    INVALID_LENGTH = 0x04,
    INVALID_DATA = 0x05,
    UNKNOWN_RESULT = 0xff
  };

  void print(Result result)
  {
    using namespace std;
    switch(result)
    {
      case SUCCESS: cout << "Success" << endl; break;
      case TARGET_DOES_NOT_EXIST: cout << "UnknownTarget" << endl; break;
      case ACTION_IS_NOT_SUPPORTED: cout << "ActionNotSupported" << endl; break;
      case UNKNOWN_ACTION: cout << "UnknownAction" << endl; break;
      case INVALID_LENGTH: cout << "InvalidLength" << endl; break;
      case INVALID_DATA: cout << "InvalidData" << endl; break;
      default: cout << "Unknown result." << endl;
    }//end switch
  }//end print_result
  */

  enum ServiceType
  {
    DIRECTORY_SERVICE = 0x0000,
    NOTIFICATION_MANAGER = 0x0001,
    DIFFERENTIAL_DRIVE = 0x4005,
    IO = 0x4001,
    LASER = 0x4004,
    LOCALIZATION = 0x8002,
    TELEMETERS = 0x4008,
    UNKNOWN_SERVICE_TYPE = 0x9999
  };


  
  // Describes a response from a PURE Request.
  struct Response
  {
    // Result of the request.
    Byte Result;

    // Optional data.
    Buffer Data;

    // Predefined Result codes.
    static const Byte OK = 0x00;
    static const Byte ServiceNotFound = 0x01;
    static const Byte ActionNotSupported = 0x02;
    static const Byte UnknownAction = 0x03;
    static const Byte InvalidLength = 0x04;
    static const Byte InvalidData = 0x05;
    static const Byte ServiceSpecificBase = 0x10;

    void from_message(Buffer& data)
    {
      data >> Result;
      data >> Data;
    }//end from_message
  };

  // Describes a request to a PURE controller.
  struct Request
  {    
    // Message id.
    Byte Id;

    // Action code.
    Byte Action;

    // Target service instance.
    UInt16 Target;    

    // Request data.
    Buffer Data;
 
    // Action codes defined by the protocol.
    static const Byte Get = 0x00;
    static const Byte Query = 0x01;
    static const Byte Replace = 0x02;
    static const Byte Update = 0x03;
    static const Byte Insert = 0x04;    
    static const Byte Delete = 0x05;    
    static const Byte Submit = 0x06;

    // Response from PURE.
    Response response;


    void to_message(Buffer& data)
    {
      data << Id;
      data << Action;
      data << Target;
      data << Data;
    }//end get_message
  };

  // Describes a notification message to or from PURE.
  struct Notification
  {
    // Target service instance.
    UInt16 Target;

    // Notification data.
    Buffer Data;

    void from_message(Buffer& buffer)
    {
      buffer >> Target;
      buffer >> Data;
    }//end from_message

    void to_message(Buffer& buffer)
    {
      buffer << Target;
      buffer << Data;
    }//end from_message
  };

};


#endif // _ServerTypes_h_