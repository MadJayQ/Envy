#pragma once
#include "process.h"
template<typename Addr>
class MemoryReadWriteBase
{
public:
	MemoryReadWriteBase() {}
	explicit MemoryReadWriteBase(Addr addr, Process p) : m_process(p), m_addr(addr){}

	virtual void* Read(const size_t size) = 0;
	virtual void Write(void* buf, const size_t size) = 0;

	template<typename T>
	T* As() { return reinterpret_cast<T*>(m_addr); }

	Process GetProcess() const { return m_process; }

	Addr m_addr;

protected:
	Process m_process;
};


class Internal : public MemoryReadWriteBase<uintptr_t>
{
public:
	Internal() {}
	Internal(uintptr_t addr) : MemoryReadWriteBase<uintptr_t>(addr, Process::CurrentProcess())
	{

	}
	template<typename Type>
	void Write(Type val)
	{
		Write((void*)&val, sizeof(Type));
	}

	template<typename Type>
	Type Read()
	{
		void* buf = Read(sizeof(Type));
		Type ret = *reinterpret_cast<Type*>(buf);
		free(buf);
		return ret;
	}

	virtual void* Read(const size_t size) override
	{
		void* buf = malloc(size);
		memcpy(
			buf,
			(void*)m_addr,
			size
		);
		return buf;
	}

	virtual void Write(void* buf, const size_t size) override
	{
		memmove(
			(void*)m_addr,
			buf,
			size
		);
	}
};

class External : public MemoryReadWriteBase<uintptr_t>
{
public:
	External() {}
	External(uintptr_t addr, Process p) : MemoryReadWriteBase<uintptr_t>(addr, p) { }

	template<typename Type>
	void Write(Type buf)
	{
		Write(
			(void&)buf,
			sizeof(Type)
		);
	}

	template<typename Type>
	Type Read()
	{
		Type* ret = reinterpret_cast<Type*>(Read(sizeof(Type)));
		return *ret;
	}

	virtual void* Read(const size_t size) override
	{
		void* buf;
		m_process.ReadMemory(buf, m_addr, size);
		return buf;
	}

	virtual void Write(void* buf, const size_t size) override
	{
		m_process.WriteMemory(buf, m_addr, size);
	}
};