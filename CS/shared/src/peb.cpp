#include "peb.h"
#include "process.h"
#include <iostream>
#include "address.h"
#include "asmfactory.h"
#include "redirection-helpers.h"

static inline uintptr_t PEBAddr() { return Process::CurrentProcess().GetPEBAddress(); }

Peb::Peb()
{
#if defined(ENVY_EXTERNAL)
	//Do nothing
	MemoryAddress addr;
#elif true
	MemoryAddress addr(
		Process::CurrentProcess().GetPEBAddress()
	);
#endif
	PEB peb = addr->Read<PEB>();
	addr = (uintptr_t)peb.Ldr;
	auto ldr = addr->Read<PEB_LDR_DATA>();
	m_peb = addr->As<PEB>();
	m_pebldr = &ldr;
	auto entry = (PLDR_DATA_TABLE_ENTRY)m_pebldr->InMemoryInitailizationOrder.Flink;
	while (entry->BaseAddress)
	{
		char buffer[1024];
		sprintf_s(buffer, 1024, "%ws", entry->BaseDllName.Buffer);
		m_modules.insert(
			std::make_pair(
				std::string(buffer),
				Module(reinterpret_cast<Address>(entry->BaseAddress))
			)
		);
		entry = (PLDR_DATA_TABLE_ENTRY)entry->InMemoryOrderLinks.Flink;
	}
}
