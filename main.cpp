
#ifdef  WIN32
  #include <windows.h>
  #include <winbase.h>
#else
  #include <unistd.h>
  #include <fcntl.h>
  #include "myconio.h"
#endif //WIN32


#include <string>
#include <iostream>


#include "UdpTransport.h"




/*
int request(char* data, int size, char* buffer)
{
  GError* err = NULL;

  g_socket_set_blocking(my_socket, false);
  int result = g_socket_send_to(my_socket, socketAddress, data, size, NULL, &err);
  
  g_socket_set_blocking(my_socket, true);
  result = g_socket_receive(my_socket, buffer, 255, NULL, NULL);

  return result;
}//end request


void enable_notification(char id1, char id2, char interval)
{
  int size = 7;
  //                id   action
  char data[7] = { 0x03, Pure::INSERT, 0x01, 0x00, id1, id2, interval };
  char buffer[255];

  int result = request(data, size, buffer);
  
  if(result > 0)
  {
    Pure::print((Pure::Result) buffer[4]);
  }
}//end enable_notification


void disable_notification(char id1, char id2)
{
  int size = 6;
  //                id   action
  char data[6] = { 0x04, Pure::DELETE, 0x01, 0x00, id1, id2 };
  char buffer[255];

  int result = request(data, size, buffer);
  
  if(result > 0)
  {
    Pure::print((Pure::Result) buffer[4]);
  }
}//end enable_notification


void test_drive()
{



  int size = 11;
  //
  char data[11] = { 0xFF, 0x03, 0x00,    0x00, 0x00, 0x00, 0x00,    0x00, 0x00, 0x80, 0x3F};
  char buffer[255];


  GError* err = NULL;
  g_socket_set_blocking(my_socket, false);
  int result = g_socket_send_to(my_socket, socketAddress, data, size, NULL, &err);

  //
  //enable_notification(0x03, 0x00, 0x05);
  //disable_notification(0x03, 0x00);

  int waitTime = 30;

  int t = 0;
  while(t < 100)
  {
    g_socket_send_to(my_socket, socketAddress, data, size, NULL, &err);

    t++;
    char notify_buffer[1024];
    g_socket_set_blocking(my_socket, true);
    int result = g_socket_receive(my_socket, notify_buffer, 1024, NULL, NULL);


    for(int i = 5; i < result; i+=4)
    {
      float value = *((float*)(&buffer[i])); 
      std::cout << value;
    }
    std::cout << std::endl;

    // wait some time
    #ifdef WIN32
    Sleep(waitTime);
    #else
    usleep(waitTime * 1000);
    #endif
  }//end while
}//end test_drive


void print_drive_data()
{
  int size = 4;
  //                id   action
  char data[4] = { 0x03, GET, 0x03, 0x00 };
  char buffer[255];

  int result = request(data, size, buffer);
  
  if(result > 0)
  {
    print_result((Result) buffer[4]);
  }

  int offset = 5;

  //float max_lin_speed = *((float*)(&buffer[offset])); offset += 4;
  //float min_lin_speed = *((float*)(&buffer[offset])); offset += 4;

  //float max_ang_speed = *((float*)(&buffer[offset])); offset += 4;
  //float min_ang_speed = *((float*)(&buffer[offset])); offset += 4;

  //float max_lin_acc = *((float*)(&buffer[offset])); offset += 4;
  //float min_lin_acc = *((float*)(&buffer[offset])); offset += 4;

  //float max_ang_acc = *((float*)(&buffer[offset])); offset += 4;
  //float min_ang_acc = *((float*)(&buffer[offset])); offset += 4;

  //float wheel_dist = *((float*)(&buffer[offset]));


  for(int i = 5; i < result; i+=4)
  {
    float value = *((float*)(&buffer[i])); 
    std::cout << i << " - " << value << std::endl;
  }
}//end print_drive_data


void print_services()
{
  int size = 4;
  //                id   action
  char data[4] = { 0x03, GET, 0x00, 0x00 };
  char buffer[255];

  int result = request(data, size, buffer);
  
  if(result > 0)
  {
    print_result((Result) buffer[4]);
  }

  int offset = 5;
  while(offset < result)
  {
    char name_data[6] = {0x01, QUERY, 0x00, 0x00, buffer[offset+2], buffer[offset+3]};
    char name_buffer[255];

    int name_result = request(name_data, 6, name_buffer);
    //print_result((Result) name_buffer[4]);

    std::cout << "[" << *((short*)(&(buffer[offset+2]))) << "]\t";

    for(int i = 5; i < name_result; i++)
    {
      std::cout << name_buffer[i];
    }
    std::cout << std::endl;

    offset += 4;
  }//end while
}//end print_services
*/


class IRNotificationHandler: public NotificationHandler
{
public:
  virtual void handle_message(char* data, int size)
  {
    for(int i = 5; i < size; i++)
    {
      std::cout << data[i];
    }
    std::cout << std::endl;
  }//end handle_message
};


class SimpleNotificationHandler: public NotificationHandler
{
public:
  virtual void handle_message(char* data, int size)
  {
    for(int i = 5; i < size; i++)
    {
      std::cout << data[i];
    }
    std::cout << std::endl;
  }//end handle_message
};


class DifferentialDriveSender: public NotificationSender
{
public:
  DifferentialDriveSender()
    : linear_speed(0.0f),
      angular_speed(0.0f)
  {
  }

  virtual int get_message(char* data)
  {
    data[0] = 0xFF;
    // target
    data[1] = 0x03;
    data[2] = 0x00;

    // enable
    data[3] = 0x01;

    float* lin_speed = (float*)(&data[4]);
    float* ang_speed = (float*)(&data[8]);

    *(lin_speed) = linear_speed;
    *(ang_speed) = angular_speed;

    return 12;
  }//end get_message

  void setSpeed(float lin, float ang)
  {
    linear_speed = lin;
    angular_speed = ang;
  }

private:
  float linear_speed;
  float angular_speed;
};


class NotificationService
{
private:
  UdpTransport& server;
public:
  NotificationService(UdpTransport& server) : server(server) {}
  ~NotificationService(){}

  virtual void print_directory()
  {
    int size = 4;
    //                id   action
    char data[4] = { 0x03, Pure::GET, 0x01, 0x00 };
    char buffer[255];

    
    int result = server.request(data, size, buffer);
  
    if(result > 0)
    {
      //print_result((Result) buffer[4]);
    }


    std::cout << " -- registered notifications -- " << std::endl;
    int offset = 5;
    while(offset < result)
    {
      char name_data[6] = {0x01, Pure::QUERY, 0x00, 0x00, buffer[offset], buffer[offset+1]};
      char name_buffer[255];

      int name_result = server.request(name_data, 6, name_buffer);
      //print_result((Result) name_buffer[4]);

      char number[2] = {buffer[offset], buffer[offset+1]};
      char period[2] = {buffer[offset+2], 0x00};

      std::cout << "[" << *((short*)(number)) << ", " << *((short*)(period)) << "]\t";

      for(int i = 5; i < name_result; i++)
      {
        std::cout << name_buffer[i];
      }
      std::cout << std::endl;

      offset += 3;
    }//end while
  }//end print_directory

};//end class NotificationService



class DirectoryService
{
private:
  UdpTransport& server;
public:
  DirectoryService(UdpTransport& server) : server(server) {}
  ~DirectoryService(){}

  virtual void print_directory()
  {
    int size = 4;
    //                id   action
    char data[4] = { 0x03, Pure::GET, 0x00, 0x00 };
    char buffer[255];

    
    int result = server.request(data, size, buffer);
  
    if(result > 0)
    {
      //print_result((Result) buffer[4]);
    }

    std::cout << " -- registered services -- " << std::endl;

    int offset = 5;
    while(offset < result)
    {
      char name_data[6] = {0x01, Pure::QUERY, 0x00, 0x00, buffer[offset+2], buffer[offset+3]};
      char name_buffer[255];

      int name_result = server.request(name_data, 6, name_buffer);
      //print_result((Result) name_buffer[4]);

      std::cout << "[" << *((short*)(&(buffer[offset+2]))) << "]\t";

      for(int i = 5; i < name_result; i++)
      {
        std::cout << name_buffer[i];
      }
      std::cout << std::endl;

      offset += 4;
    }//end while
  }//end print_directory

};//end class DirectoryService




class Server
{
public:
  Server(){};

  void run()
  {
    GError* err = NULL;
    GSocket* socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
    g_socket_set_blocking(socket, true);
    g_socket_set_timeout(socket, 1);

    char buffer[1024];

    while(true)
    {
      int size = g_socket_receive(socket, buffer, 1024, NULL, NULL);

      if(size > 0)
      {
        for(int i = 5; i < size; i++)
        {
          std::cout << buffer[i];
        }
        std::cout << std::endl;
      }
      else
      {
        std::cout << "time out" << std::endl;
      }
    }//end while
  }
};


int main(int argc, char** argv)
{
 
  int port = 60000;
  std::string ip_string = "192.168.1.2";
  
  UdpTransport server(ip_string, port);

  SimpleNotificationHandler a;
  DifferentialDriveSender drive;
  
  server.unregister_notificationHandler(0x03, 0x00, 0x05);
  server.register_notificationHandler(a, 0x03, 0x00, 0x05);
  
  //server.register_notificationSender(drive);
  

  DirectoryService directoryService(server);
  directoryService.print_directory();

  NotificationService notificytionService(server);
  notificytionService.print_directory();

  //server.loop(10);

  int cycle = 11;
  float t = 0;

  int cycles = 0;
  while(cycles < 30000)
  {
    cycles++;
    server.send_receive();

    drive.setSpeed(0.1f, std::sin(t*3.14f*2.0f*4.0f));

    t += cycle/1000.0f;

    // wait some time
  }//end while

  

   /*
  print_drive_data();
  print_services();
  test_drive();

 
  {
  {
  int size = 7;
  //                id   action
  char data[7] = { 0x03, INSERT, 0x01, 0x00, 0x06, 0x00, 0x05 };
  char buffer[255];

  int result = request(data, size, buffer);
  
  if(result > 0)
  {
    print_result((Result) buffer[4]);
  }
  }
    
  int waitTime = 10;

  int t = 0;
  while(t < 30)
  {
    t++;
    char notify_buffer[1024];
    g_socket_set_blocking(my_socket, true);
    int result = g_socket_receive(my_socket, notify_buffer, 1024, NULL, NULL);


    for(int i = 5; i < result; i++)
    {
      std::cout << notify_buffer[i];
    }
    std::cout << std::endl;

    // wait some time
    #ifdef WIN32
    Sleep(waitTime);
    #else
    usleep(waitTime * 1000);
    #endif
  }//end while

  {
  int size = 6;
  //                id   action
  char data[6] = { 0x03, 0x05, 0x01, 0x00, 0x06, 0x00 };
  char buffer[255];

  int result = request(data, size, buffer);
  
  if(result > 0)
  {
    print_result((Result) buffer[4]);
  }
  }

  }
  */

  /*
  {

    int waitTime = 10;

    int t = 0;
    while(t < 30)
    {
      int size = 12;
      char data[12] = { 0xFF, 0x03, 0x00, 0x01,   
        0x00, 0x00, 0x00, 0x00,    
        0x00, 0x00, 0x00, 0x00 };
      char buffer[255];

      float* linear = (float*)(&(data[4]));
      *linear = 0.01f;

      int result = request(data, size, buffer);
  
      if(result > 0)
      {
        print_result((Result) buffer[4]);
      }

      // wait some time
      #ifdef WIN32
      Sleep(waitTime);
      #else
      usleep(waitTime * 1000);
      #endif
    }

    

    {
    int size = 12;
    char data[12] = { 0xFF, 0x03, 0x00, 0x00,   
      0x00, 0x00, 0x00, 0x00,    
      0x00, 0x00, 0x00, 0x00 };
    char buffer[255];

    int result = request(data, size, buffer);
  
    if(result > 0)
    {
      print_result((Result) buffer[4]);
    }
    }
  }
  */
  
  int x = 0;
}//end main