// MUD Programming
// Ron Penton
// (C)2003
// ThreadLibFunctions.h - Contains all of the functions for use within
// the threading library

#ifndef THREADLIBFUNCTIONS_H
#define THREADLIBFUNCTIONS_H

// ============================================================================
//  Include Files for the threading libraries
// ============================================================================
#ifdef WIN32                // windows95 and above
    #include <windows.h>
    #include <map>
#else                       // linux
    #include <pthread.h>
    #include <unistd.h>
#endif

#ifdef _DEBUG //重载new运算符，把分配内存的信息保存下来，可以定位到那一行发生了内存泄露。用于检测 new 分配的内存
#define NEW_ND  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW_ND  new
#endif

#include "ThreadException.h"
#include <thread>
#include <chrono>
#include "../basic/GameLog.h"
namespace ThreadLib
{

    // ========================================================================
    // define the standard thread function format, which takes a void* as its
    // parameter, and returns nothing
    // ========================================================================
    typedef void (*ThreadFunc)(void*);

    // ========================================================================
    //  Define the standard ThreadID datatype, depending on the system
    // ========================================================================
    #ifdef WIN32                // windows95 and above
        typedef DWORD ThreadID;
        extern std::map< DWORD, HANDLE > g_handlemap;
    #else                       // linux
        typedef pthread_t ThreadID;
    #endif


    // ========================================================================
    // Description: This is a "dummy" class that will be used to
    //              transparently translate function pointers to whatever
    //              system the user is currently compiling on. It is meant to
    //              be passed into the DummyRun function.
    // ========================================================================
    class DummyData
    {
    public:
        ThreadFunc m_func;
        void* m_data;
    };

    // ========================================================================
    // Description: This is a "dummy" thread function that will be used to
    //              transparently translate function pointers to whatever
    //              system the user is currently compiling on.
    // ========================================================================
    #ifdef WIN32
        DWORD WINAPI DummyRun( void* p_data );
    #else
        void* DummyRun( void* p_data );
    #endif


 	//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
 	inline std::string GetLastErrorAsString()
 	{
 		//Get the error message, if any.
 		DWORD errorMessageID = GetLastError();
 		if (errorMessageID == 0)
 			return std::string(); //No error message has been recorded
 
 		LPSTR messageBuffer = nullptr;
 		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
 			NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
 
 		std::string message(messageBuffer, size);
 
 		//Free the buffer.
 		LocalFree(messageBuffer);
 
 		return message;
 	}

    // ========================================================================
    // Description: Creates a thread and returns its ID.
    // ========================================================================
    inline ThreadID Create( ThreadFunc p_func, void* p_param )
    {
        ThreadID t;
        // create a new dummy data block		
        DummyData* data = NEW_ND DummyData;
        data->m_func = p_func;
        data->m_data = p_param;

        #ifdef WIN32    // create a Win32 thread
            HANDLE h;
            h = CreateThread( NULL, 0, DummyRun, data, 0, &t );
            if( h != 0 )
            {
                // insert the handle into the handlemap
                g_handlemap[t] = h;
            }
			else
			{
				logger_error("创建线程失败: {}", GetLastErrorAsString());
				// delete the data
				delete data;
				data = NULL;

				// throw an error
				throw Exception(CreationFailure);
			}
        #else           // create a linux thread
            int err = pthread_create( &t, 0, DummyRun, data );
			// linux环境下返回0标识创建线程成功
			if (err != 0)
			{
				logger_error("创建线程失败: {}", strerror(err));
				// delete the data
				delete data;
				data = NULL;

				// throw an error
				throw Exception(CreationFailure);
			}
        #endif
		logger_trace("创建线程,线程号{}", t);
        
        return t;
    }

    // ========================================================================
    // Description: Get the ID of the current thread
    // ========================================================================
    inline ThreadID GetID()
    {
        #ifdef WIN32
            return GetCurrentThreadId();
        #else
            return pthread_self();
        #endif
    }


    // ========================================================================
    // Description: This waits for a thread to finish executing before 
    //              returning.
    // ========================================================================
    inline void WaitForFinish( ThreadID p_thread )
    {
        #ifdef WIN32
            // look up the handle and wait for the thread to finish
            WaitForSingleObject( g_handlemap[p_thread], INFINITE );

            // close the handle of the thread
            CloseHandle( g_handlemap[p_thread] );

            // remove the handle from the map
            g_handlemap.erase( p_thread );
        #else
            // "join" the thread. This essentially transfers control over to
            // the thread and waits for it to finish.
            pthread_join( p_thread, NULL );
        #endif
    }


    // ========================================================================
    // Description: This forcefully terminates a thread. Don't do this unless
    //              it is absolutely neccessary.
    // ========================================================================
    inline void Kill( ThreadID& p_thread )
    {
        #ifdef WIN32
            // terminate the thread
            TerminateThread( g_handlemap[p_thread], 0 );

            // close the handle of the thread
            CloseHandle( g_handlemap[p_thread] );

            // remove the handle from the map
            g_handlemap.erase( p_thread );
        #else
            // cancel the thread.
            pthread_cancel( p_thread );
        #endif
    }


    // ========================================================================
    // Description: This yields the thread to the operating system, and gives
    //              up its current timeslice.
    // ========================================================================
    inline void YieldThread( int p_milliseconds = 1 )
    {
        //#ifdef WIN32
        //    Sleep( p_milliseconds );
        //#else
        //    usleep( p_milliseconds * 1000 );
        //#endif

		std::this_thread::sleep_for(std::chrono::milliseconds(p_milliseconds));
    }


}   // end namespace ThreadLib


#endif
