// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "threadtools.h"

typedef BOOL( WINAPI*TryEnterCriticalSectionFunc_t )( LPCRITICAL_SECTION );
static CDynamicFunction<TryEnterCriticalSectionFunc_t> DynTryEnterCriticalSection( "Kernel32.dll", "TryEnterCriticalSection" );

bool CThreadMutex::TryLock( ) {

#if defined( _WIN32 )
#ifdef THREAD_MUTEX_TRACING_ENABLED
	uint thisThreadID = ThreadGetCurrentId( );
	if ( m_bTrace && m_currentOwnerID && ( m_currentOwnerID != thisThreadID ) )
		Msg( "Thread %u about to try-wait for lock %x owned by %u\n", ThreadGetCurrentId( ), ( CRITICAL_SECTION * )&m_CriticalSection, m_currentOwnerID );
#endif
	if ( DynTryEnterCriticalSection != NULL ) {
		if ( ( *DynTryEnterCriticalSection )( ( CRITICAL_SECTION * )&m_CriticalSection ) != FALSE ) {
#ifdef THREAD_MUTEX_TRACING_ENABLED
			if ( m_lockCount == 0 ) {
				// we now own it for the first time.  Set owner information
				m_currentOwnerID = thisThreadID;
				if ( m_bTrace )
					Msg( "Thread %u now owns lock 0x%x\n", m_currentOwnerID, ( CRITICAL_SECTION * )&m_CriticalSection );
			}
			m_lockCount++;
#endif
			return true;
		}
		return false;
	}
	Lock( );
	return true;
#elif defined( _LINUX )
	return pthread_mutex_trylock( &m_Mutex ) == 0;
#else
#error "Implement me!"
	return true;
#endif
}

void CThreadFastMutex::Lock( const UINT32 threadId, unsigned nSpinSleepTime ) volatile {
	int i;
	if ( nSpinSleepTime != TT_INFINITE ) {
		for ( i = 1000; i != 0; --i ) {
			if ( TryLock( threadId ) ) {
				return;
			}
			_mm_pause( );
		}

#ifdef _WIN32
		if ( !nSpinSleepTime && GetThreadPriority( GetCurrentThread( ) ) > THREAD_PRIORITY_NORMAL ) {
			nSpinSleepTime = 1;
		} else
#endif

			if ( nSpinSleepTime ) {
				for ( i = 4000; i != 0; --i ) {
					if ( TryLock( threadId ) ) {
						return;
					}

					_mm_pause( );
					Sleep( 0 );
				}

			}

		for ( ;; ) // coded as for instead of while to make easy to breakpoint success
		{
			if ( TryLock( threadId ) ) {
				return;
			}

			_mm_pause( );
			Sleep( nSpinSleepTime );
		}
	} else {
		for ( ;; ) // coded as for instead of while to make easy to breakpoint success
		{
			if ( TryLock( threadId ) ) {
				return;
			}

			_mm_pause( );
		}
	}
}







































