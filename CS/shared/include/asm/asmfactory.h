#pragma once

#include <memory>
#include <vector>
#include <cstddef>
#include <type_traits>
#include <stdint.h>

#include <Windows.h>
#include "ntops.h"
#include "process.h"

typedef enum
{
	CCDECL,
	CSTDCALL,
	CTHISCALL,
	CFASTCALL,
	CWIN64CALL
} CallingConvention64;

typedef enum
{
	PARAM_INT,
	PARAM_INT64,
	PARAM_BOOL,
	PARAM_SHORT,
	PARAM_FLOAT,
	PARAM_DOUBLE,
	PARAM_BYTE,
	PARAM_PTR,
	PARAM_STRING,
	PARAM_WSTRING,
	PARAM_UNISTRUCT
} ParameterType64;

typedef enum
{
	RCX,
	RDX,
	R8,
	R9
} ParameterRegister64;

#define PARAM_REG_MAX 4

typedef enum
{
	PUSH_LEFT_TO_RIGHT,
	PUSH_RIGHT_TO_LEFT
} PushOrder64;

typedef struct
{
	ParameterType64 type;
	void* parameter_data;
} ParameterContext64;

typedef struct
{
	unsigned long size;
	void* ptr;
} StringContext64;

typedef struct
{
	unsigned long size;
	void* data;
} StructContext64;

typedef struct
{
	CallingConvention64 callingconvention;
	std::vector<ParameterContext64> params;
	std::vector<StringContext64> strings;
	std::vector<StructContext64> structs;
	uintptr_t invoke_addr;

} InvokeContext64;



#define DWORD(paramType) paramType == PARAM_INT || paramType == PARAM_FLOAT || paramType == PARAM_SHORT
#define QWORD(paramType) paramType == PARAM_INT64 || paramType == PARAM_DOUBLE || paramType == POINTER || paramType == PARAM_STRING || paramType == PARAM_WSTRING
#define STRING(paramType) paramType == PARAM_STRING || paramType == PARAM_STRING || paramType == PARAM_UNISTRUCT

class ASMFactory
{
public:
	ASMFactory(ASMFactory const&) = delete;
	ASMFactory& operator=(ASMFactory const&) = delete;

	static ASMFactory* GetAssembler()
	{
		static std::shared_ptr<ASMFactory> s_pInstance{ new ASMFactory };
		return s_pInstance.get();
	}

	static inline size_t Align(size_t val, size_t alignment)
	{
		return (val % alignment == 0) ? val : (val / alignment + 1) * alignment;
	}

	void PushParameter(ParameterContext64 ctx);
	void PushInt(int i);
	void PushUInt(unsigned int i);
	void PushInt64(__int64 i);
	void PushUInt64(unsigned __int64 i);
	void PushBool(bool b);
	void PushShort(short s);
	void PushFloat(float f);
	void PushDouble(double d);
	void PushByte(unsigned char uc);
	void PushPointer(void *ptr);
	void PushPointer64(void *ptr);
	void PushANSIString(const char* szString);
	void PushUNICODEString(const wchar_t* szString);
	void PushUNICODEStringStructure(UNICODE_STRING* ptrUnicodeString);

	void PushCall(CallingConvention64 cconv, FARPROC CallAddress);
	void PushAllParameters(PushOrder64 po = PUSH_LEFT_TO_RIGHT);


	void AddBytes(const std::initializer_list<unsigned char>& bytes);
	void AddByte(unsigned char b);
	void AddLong(unsigned long l);
	void AddLong64(uint64_t ull);


	void GeneratePrologue();
	void GenerateEpilogue(int ret_size = -1);

	void CreateRPCEnvironment();
	void SetAssemblerTargetProcess(Process p) { target_process = p; }

	void SaveReturn();
	void SignalOSEvent();

	void FlushAssembler();

	void ExecuteASMInWorker(std::vector<byte> buffer, uintptr_t& call_result);
	void ExecuteASMInWorker(uintptr_t& call_result);

	void ExecuteASM(std::vector<byte> buffer);
	void ExecuteASM() { ExecuteASM(remote_buffer); }

	void* CommitBlock(void* chunk, const size_t size);

	uintptr_t GetWorkerThreadID() { return GetThreadId(worker_thread); }

	void LoadStringParameter(ParameterContext64 pctx, ParameterRegister64 reg);
	void LoadParameter(uint64_t param, ParameterRegister64 reg);

	void TerminateWorkerThread();
	void FreeAllocatedBlocks();

private:
	ASMFactory() {}

	NTSTATUS CreateWorkerThread();
	NTSTATUS CreateAPCEvent(DWORD thid);
	void TerminateThreadSaveStatus(uintptr_t result);

	std::vector<uint8_t> remote_buffer;
	InvokeContext64 invoke_ctx;
	Process target_process;

	std::unordered_map<uintptr_t, const size_t> blocks;

private:
	HANDLE apc_sync_wait_event = NULL;
	HANDLE worker_thread = NULL;
	uint32_t worker_thread_id = 0;
	uintptr_t worker_thread_code_cave = 0;
	uintptr_t worker_code_thread = 0;
	uintptr_t user_code = 0;
	uintptr_t user_data = 0;
};