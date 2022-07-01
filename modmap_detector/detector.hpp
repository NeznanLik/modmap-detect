#pragma once
#include "includes.h"


namespace cc
{
	bool detect( structs::PPEB peb )
	{
		const auto list = ( peb->Ldr->InLoadOrderModuleList );

		for( PLIST_ENTRY i = list.Flink; i != &list ; i = i->Flink )
		{
			const auto dll = CONTAINING_RECORD( i, structs::LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );
			const auto dllbase = dll->DllBase;

			printf( "Inside %wZ\n", &dll->BaseDllName );

			const auto dos = reinterpret_cast< PIMAGE_DOS_HEADER >( dllbase );
			const auto nt = reinterpret_cast< PIMAGE_NT_HEADERS >( reinterpret_cast< uintptr_t >( dllbase ) + dos->e_lfanew );
			const auto nt_size = nt->OptionalHeader.SizeOfImage;

			auto section = IMAGE_FIRST_SECTION( nt );
			for( unsigned int sec = 0; sec < nt->FileHeader.NumberOfSections; )
			{
				if( ( section->Characteristics & IMAGE_SCN_MEM_EXECUTE ) == IMAGE_SCN_MEM_EXECUTE ) { // in executable section
					auto start = reinterpret_cast< void* >( reinterpret_cast< uintptr_t >( dllbase ) + section->VirtualAddress );
					auto end = reinterpret_cast< void* >( reinterpret_cast< uintptr_t >( start ) + section->Misc.VirtualSize );

					for( unsigned long page = 0; page <= nt_size; )
					{
						MEMORY_BASIC_INFORMATION info { 0 };
						VirtualQuery( reinterpret_cast< void* >( reinterpret_cast< uintptr_t >( dllbase ) + page ), &info, sizeof( MEMORY_BASIC_INFORMATION ) );

						if( info.Protect & PAGE_EXECUTE_READ ||
							info.Protect & PAGE_EXECUTE_READWRITE || // check if page is executable
							info.Protect & PAGE_EXECUTE_READ ||
							info.Protect & PAGE_EXECUTE_WRITECOPY
							)
						{
							auto page_start = reinterpret_cast< void* >( info.AllocationBase );
							auto page_end = reinterpret_cast< void* >( reinterpret_cast< uintptr_t >( page_start ) + info.RegionSize );

							if( page_start > start && page_end > end )
							{
								printf( "-> Found extended memory:[%p] in %wZ! \n", page_start, &dll->BaseDllName );
								return true;
							}
						}

						page += 0x1000;
					}
				}
				++sec, ++section;
			}
		}

		return false;
	}
}