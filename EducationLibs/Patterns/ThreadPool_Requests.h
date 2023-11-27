#pragma once

#include <iostream> 
#include <atomic> 
#include <mutex> 
#include <queue> 
#include <thread> 

//todo: ������� ����� - CoutLogger

// ������� ��� ������ ��������� � ������� (std::cout �� ����������������)
std::mutex cout_mx;

/**
 * \brief ������
 */
class Request
{
public:
  Request() = default;
  Request( size_t request_id ) : id( request_id )
  {
    std::cout << " - Called constructor. Request task " << id << ".\n";    
  }
  ~Request()
  {
    std::cout << " - Called destructor. Request task " << id << ".\n";    
  }

  /**
   * \brief ��������� �������
   */
  void handling()
  {
    // ���������� ��������� ������� 
    std::this_thread::sleep_for( std::chrono::microseconds( 200 ) );
  }

  // ��� �������� ������� ������ ������
  void * operator new( size_t size )
  {
    auto p = malloc( size );
    std::cout << " - Malloc: " << p << "\n";
    return p;
  }
  // ��� �������� ������� ������ ������
  void operator delete( void * p )
  {
    {
      std::lock_guard<std::mutex> cout_lock( cout_mx );
      std::cout << " - Free: " << p << "\n";
    }
    return free( p );
  }

  size_t id = 0;  // id �������
};

//todo: ������� ����� - RequstQueue

// ���������������� ������� �������
static std::queue<Request *> requestQueue;

/**
 * \brief �������� ������
 * \return ���������� nullptr ���� ����� ��������� �������, ���� ��������� �� ������, ������� � ���������� ��������� ������� 
 */
Request * GetRequest() noexcept
{
  // ���������� ��������� ������� 
  std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
  Request *request = nullptr;
  try
  {
    // � ������� ���� ������� � ��� �� ������
    if ( !requestQueue.empty() )
    {
      request = requestQueue.front();
      requestQueue.pop();
    }
    else
      return nullptr;
  }
  catch ( ... )
  {
  }
  return request;
}

//todo: ������� ����� - ProcessRequest

/**
 * \brief ������������ ������, �� ������ �� ������� 
 * \param request - ������
 */
void ProcessRequest( Request * request ) noexcept
{
  try
  {
    request->handling();
  }
  catch ( ... )
  {
  }
}

// todo: ��������� ����� - WorkThreads

// ���������� ������� ������� 
const int NumberOfThreads = 2;

// ������� ������� ��� ������� ������� 
std::queue<Request *> workRequestQueue;

// ������� ��� ������������ �������� �� ������� ������� ��� ������� ������� 
std::mutex workMutex;

// ���� ������������� ���������� ������ ������, ����� ������ �����������, ��� ������ �������� ����� ��� ��� ���������� 
static std::atomic<bool> needWork = true;

// ����������� ������� ��������. ��������� ���������� ������ �
//  ������������� ������� ����� �������� �������� � ������� �������
std::condition_variable work_req_queue_notifier;

/**
 * \brief ������� �������� ������.
 * ������ GetRequest() � ProcessRequest() ����� �������� �����. 
 * \param thread_id - id ������
 */
void workThread( size_t thread_id )
{
  try
  {
    // �����������, ��� ������ �������� ����� ��� ��� ����������. 
    while ( needWork )
    {
      Request * workRequest = nullptr;
      {
        std::unique_lock<std::mutex> unique_lock( workMutex );
        if ( !work_req_queue_notifier.wait_for( unique_lock, std::chrono::milliseconds( 100 ),
          [] { return !workRequestQueue.empty() || needWork == true; } ) )
          continue;
        if ( workRequestQueue.empty() )
          continue;
        workRequest = workRequestQueue.front();
        workRequestQueue.pop();
      }
      {
        std::lock_guard<std::mutex> cout_lock( cout_mx );
        std::cout << " - Work thread #" << thread_id << " took task: " << workRequest->id << "\n";
      }
      // ������������ ����������� ����� ������� ������� � ������� ProcessRequest. 
      ProcessRequest( workRequest );
      {
        std::lock_guard<std::mutex> cout_lock( cout_mx );
        std::cout << " - Work thread #" << thread_id << " task: " << workRequest->id << " done!\n";
      }

      // ������� ������ ������������ �������� (�� ������������ ������� ����� �������� ������� �������!
      delete workRequest;
    }
  }
  catch ( ... )
  {
  }
  std::lock_guard<std::mutex> cout_lock( cout_mx );
  std::cout << " - Work thread #" << thread_id << " finished job\n";
}