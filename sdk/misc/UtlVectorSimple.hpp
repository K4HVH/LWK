#pragma once

class CUtlVectorSimple {
public:
	unsigned memory;
	char pad[ 8 ];
	unsigned int count;
	inline void* Retrieve( int index, unsigned sizeofdata ) {
		return ( void* )( ( *( unsigned* )this ) + ( sizeofdata * index ) );
	}
	inline void* Base( ) {
		return ( void* )memory;
	}
	inline int Count( ) {
		return count;
	}
};

