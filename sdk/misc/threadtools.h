#pragma once
#pragma warning(push)
#pragma warning(disable:4251)

#include <Windows.h>

#define USE_INTRINSIC_INTERLOCKED
#define THREAD_MUTEX_TRACING_SUPPORTED

const unsigned TT_INFINITE = 0xffffffff;

extern "C"
{
	long __cdecl _InterlockedIncrement( volatile long* );
	long __cdecl _InterlockedDecrement( volatile long* );
	long __cdecl _InterlockedExchange( volatile long*, long );
	long __cdecl _InterlockedExchangeAdd( volatile long*, long );
	long __cdecl _InterlockedCompareExchange( volatile long*, long, long );
}

#pragma intrinsic( _InterlockedCompareExchange )
#pragma intrinsic( _InterlockedDecrement )
#pragma intrinsic( _InterlockedExchange )
#pragma intrinsic( _InterlockedExchangeAdd ) 
#pragma intrinsic( _InterlockedIncrement )

inline INT32 ThreadInterlockedIncrement( INT32 volatile *p ) { return _InterlockedIncrement( ( volatile long* )p ); }
inline INT32 ThreadInterlockedDecrement( INT32 volatile *p ) { return _InterlockedDecrement( ( volatile long* )p ); }
inline INT32 ThreadInterlockedExchange( INT32 volatile *p, INT32 value ) { return _InterlockedExchange( ( volatile long* )p, value ); }
inline INT32 ThreadInterlockedExchangeAdd( INT32 volatile *p, INT32 value ) { return _InterlockedExchangeAdd( ( volatile long* )p, value ); }
inline INT32 ThreadInterlockedCompareExchange( INT32 volatile *p, INT32 value, INT32 comperand ) { return _InterlockedCompareExchange( ( volatile long* )p, value, comperand ); }
inline bool ThreadInterlockedAssignIf( INT32 volatile *p, INT32 value, INT32 comperand ) { return ( _InterlockedCompareExchange( ( volatile long* )p, value, comperand ) == comperand ); }


inline unsigned ThreadInterlockedExchangeSubtract( UINT32 volatile *p, UINT32 value ) { return ThreadInterlockedExchangeAdd( ( INT32 volatile * )p, value ); }
inline unsigned ThreadInterlockedIncrement( UINT32 volatile *p ) { return ThreadInterlockedIncrement( ( INT32 volatile * )p ); }
inline unsigned ThreadInterlockedDecrement( UINT32 volatile *p ) { return ThreadInterlockedDecrement( ( INT32 volatile * )p ); }
inline unsigned ThreadInterlockedExchange( UINT32 volatile *p, UINT32 value ) { return ThreadInterlockedExchange( ( INT32 volatile * )p, value ); }
inline unsigned ThreadInterlockedExchangeAdd( UINT32 volatile *p, UINT32 value ) { return ThreadInterlockedExchangeAdd( ( INT32 volatile * )p, value ); } //-V524
inline unsigned ThreadInterlockedCompareExchange( UINT32 volatile *p, UINT32 value, UINT32 comperand ) { return ThreadInterlockedCompareExchange( ( INT32 volatile * )p, value, comperand ); }
inline bool ThreadInterlockedAssignIf( UINT32 volatile *p, UINT32 value, UINT32 comperand ) { return ThreadInterlockedAssignIf( ( INT32 volatile * )p, value, comperand ); }



class CThreadMutex {
public:
	CThreadMutex( );
	~CThreadMutex( );

	//------------------------------------------------------
	// Mutex acquisition/release. Const intentionally defeated.
	//------------------------------------------------------
	void Lock( );
	void Lock( ) const { ( const_cast<CThreadMutex *>( this ) )->Lock( ); }
	void Unlock( );
	void Unlock( ) const { ( const_cast<CThreadMutex *>( this ) )->Unlock( ); }

	bool TryLock( );
	bool TryLock( ) const { return ( const_cast<CThreadMutex *>( this ) )->TryLock( ); }

	void LockSilent( ); // A Lock() operation which never spews.  Required by the logging system to prevent badness.
	void UnlockSilent( ); // An Unlock() operation which never spews.  Required by the logging system to prevent badness.

						  //------------------------------------------------------
						  // Use this to make deadlocks easier to track by asserting
						  // when it is expected that the current thread owns the mutex
						  //------------------------------------------------------
	bool AssertOwnedByCurrentThread( );

	//------------------------------------------------------
	// Enable tracing to track deadlock problems
	//------------------------------------------------------
	void SetTrace( bool );

private:
	// Disallow copying
	CThreadMutex( const CThreadMutex & );
	CThreadMutex &operator=( const CThreadMutex & );

#if defined( _WIN32 )
	// Efficient solution to breaking the windows.h dependency, invariant is tested.
#ifdef _WIN64
#define TT_SIZEOF_CRITICALSECTION 40	
#else
#ifndef _X360
#define TT_SIZEOF_CRITICALSECTION 24
#else
#define TT_SIZEOF_CRITICALSECTION 28
#endif // !_X360
#endif // _WIN64
	byte m_CriticalSection[ TT_SIZEOF_CRITICALSECTION ];
#elif defined(POSIX)
	pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_Attr;
#else
#error
#endif

#ifdef THREAD_MUTEX_TRACING_SUPPORTED
	// Debugging (always herge to allow mixed debug/release builds w/o changing size)
	UINT	m_currentOwnerID;
	UINT16	m_lockCount;
	bool	m_bTrace;
#endif
};

//-----------------------------------------------------------------------------
//
// An alternative mutex that is useful for cases when thread contention is 
// rare, but a mutex is required. Instances should be declared volatile.
// Sleep of 0 may not be sufficient to keep high priority threads from starving 
// lesser threads. This class is not a suitable replacement for a critical
// section if the resource contention is high.
//
//-----------------------------------------------------------------------------

class CThreadFastMutex {
public:
	CThreadFastMutex( )
		: m_ownerID( 0 ),
		m_depth( 0 ) { }

private:
	FORCEINLINE bool TryLockInline( const UINT32 threadId ) volatile {
		if ( threadId != m_ownerID && !ThreadInterlockedAssignIf( ( volatile INT32 * )&m_ownerID, ( INT32 )threadId, 0 ) )
			return false;

		++m_depth;
		return true;
	}

	bool TryLock( const UINT32 threadId ) volatile {
		return TryLockInline( threadId );
	}

	void Lock( const UINT32 threadId, unsigned nSpinSleepTime ) volatile;

public:
	bool TryLock( ) volatile {
#ifdef _DEBUG
		if ( m_depth == INT_MAX )
			DebugBreak( );

		if ( m_depth < 0 )
			DebugBreak( );
#endif
		return TryLockInline( GetCurrentThreadId( ) );
	}

#ifndef _DEBUG 
	FORCEINLINE
#endif
		void Lock( unsigned int nSpinSleepTime = 0 ) volatile {
		const UINT32 threadId = GetCurrentThreadId( );

		if ( !TryLockInline( threadId ) ) {
			_mm_pause( );
			Lock( threadId, nSpinSleepTime );
		}
#ifdef _DEBUG
		if ( m_ownerID != GetCurrentThreadId( ) )
			DebugBreak( );

		if ( m_depth == INT_MAX )
			DebugBreak( );

		if ( m_depth < 0 )
			DebugBreak( );
#endif
	}

#ifndef _DEBUG
	FORCEINLINE
#endif
		void Unlock( ) volatile {
#ifdef _DEBUG
		if ( m_ownerID != GetCurrentThreadId( ) )
			DebugBreak( );

		if ( m_depth <= 0 )
			DebugBreak( );
#endif

		--m_depth;
		if ( !m_depth ) {
			ThreadInterlockedExchange( &m_ownerID, 0 );
		}
	}

	bool TryLock( ) const volatile { return ( const_cast<CThreadFastMutex *>( this ) )->TryLock( ); }
	void Lock( unsigned nSpinSleepTime = 0 ) const volatile { ( const_cast<CThreadFastMutex *>( this ) )->Lock( nSpinSleepTime ); }
	void Unlock( ) const	volatile { ( const_cast<CThreadFastMutex *>( this ) )->Unlock( ); }

	// To match regular CThreadMutex:
	bool AssertOwnedByCurrentThread( ) { return true; }
	void SetTrace( bool ) { }

	UINT32 GetOwnerId( ) const { return m_ownerID; }
	int	GetDepth( ) const { return m_depth; }
private:
	volatile UINT32 m_ownerID;
	int				m_depth;
};

typedef struct _RTL_CRITICAL_SECTION RTL_CRITICAL_SECTION;
typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;

inline void CThreadMutex::Lock( ) {
#ifdef THREAD_MUTEX_TRACING_ENABLED
	uint thisThreadID = ThreadGetCurrentId( );
	if ( m_bTrace && m_currentOwnerID && ( m_currentOwnerID != thisThreadID ) )
		Msg( _T( "Thread %u about to wait for lock %x owned by %u\n" ), ThreadGetCurrentId( ), ( CRITICAL_SECTION * )&m_CriticalSection, m_currentOwnerID );
#endif

	LockSilent( );

#ifdef THREAD_MUTEX_TRACING_ENABLED
	if ( m_lockCount == 0 ) {
		// we now own it for the first time.  Set owner information
		m_currentOwnerID = thisThreadID;
		if ( m_bTrace )
			Msg( _T( "Thread %u now owns lock 0x%x\n" ), m_currentOwnerID, ( CRITICAL_SECTION * )&m_CriticalSection );
	}
	m_lockCount++;
#endif
}

//---------------------------------------------------------

inline void CThreadMutex::Unlock( ) {
#ifdef THREAD_MUTEX_TRACING_ENABLED
	AssertMsg( m_lockCount >= 1, "Invalid unlock of thread lock" );
	m_lockCount--;
	if ( m_lockCount == 0 ) {
		if ( m_bTrace )
			Msg( _T( "Thread %u releasing lock 0x%x\n" ), m_currentOwnerID, ( CRITICAL_SECTION * )&m_CriticalSection );
		m_currentOwnerID = 0;
	}
#endif
	UnlockSilent( );
}

//---------------------------------------------------------

inline void CThreadMutex::LockSilent( ) {
	EnterCriticalSection( ( CRITICAL_SECTION * )&m_CriticalSection );
}

//---------------------------------------------------------

inline void CThreadMutex::UnlockSilent( ) {
	LeaveCriticalSection( ( CRITICAL_SECTION * )&m_CriticalSection );
}

//---------------------------------------------------------

inline bool CThreadMutex::AssertOwnedByCurrentThread( ) {
#ifdef THREAD_MUTEX_TRACING_ENABLED
	if ( ThreadGetCurrentId( ) == m_currentOwnerID )
		return true;
	AssertMsg3( 0, "Expected thread %u as owner of lock 0x%x, but %u owns", ThreadGetCurrentId( ), ( CRITICAL_SECTION * )&m_CriticalSection, m_currentOwnerID );
	return false;
#else
	return true;
#endif
}

//---------------------------------------------------------

inline void CThreadMutex::SetTrace( bool bTrace ) {
#ifdef THREAD_MUTEX_TRACING_ENABLED
	m_bTrace = bTrace;
#endif
}

template <typename FUNCPTR_TYPE>
class CDynamicFunction {
public:
	CDynamicFunction( const char *pszModule, const char *pszName, FUNCPTR_TYPE pfnFallback = NULL ) {
		m_pfn = pfnFallback;

		HMODULE hModule = ::LoadLibrary( pszModule );
		if ( hModule )
			m_pfn = ( FUNCPTR_TYPE )::GetProcAddress( hModule, pszName );
	}

	operator bool( ) { return m_pfn != NULL; }
	bool operator !( ) { return !m_pfn; }
	operator FUNCPTR_TYPE( ) { return m_pfn; }

private:
	FUNCPTR_TYPE m_pfn;
};