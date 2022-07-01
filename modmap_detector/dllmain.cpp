#include "includes.h"

void onDllMain( void )
{
	AllocConsole( );
	freopen( ( "CONOUT$" ), ( "w" ), stdout );

	const auto peb = reinterpret_cast< structs::PPEB >( __readgsqword( 0x60 ) );

	cc::detect( peb );
}

bool __stdcall dll_entry( std::uintptr_t module, std::uint32_t reason, void* reserved )
{
	if( reason == DLL_PROCESS_ATTACH ) {
		onDllMain( );
	}

	return true;
}
