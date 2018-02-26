#include "..\..\include\asm\asmfactory.h"

#include <sddl.h>
#include <iostream>
template<typename T>
inline std::vector<T> ReverseVector(const std::vector<T>& in)
{
	std::vector<T> ret;
	auto itr = in.rbegin();
	while (itr != in.rend())
	{
		ret.push_back(*itr);
		itr++;
	}
	return ret;
}

void ASMFactory::PushParameter(ParameterContext64 ctx)
{
	invoke_ctx.params.push_back(ctx);
}

void ASMFactory::PushInt(int i)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_INT;
	int* data = new int;
	*data = i;
	ctx.parameter_data = data;
	PushParameter(ctx);
}

void ASMFactory::PushUInt(unsigned int i)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_INT;
	unsigned int* data = new unsigned int;
	*data = i;
	ctx.parameter_data = data;
	PushParameter(ctx);
}

void ASMFactory::PushInt64(__int64 i)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_INT64;
	__int64* data = new __int64;
	*data = i;
	ctx.parameter_data = data;
	PushParameter(ctx);
}

void ASMFactory::PushUInt64(unsigned __int64 i)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_INT64;
	unsigned __int64* data = new unsigned __int64;
	*data = i;
	ctx.parameter_data = data;
	PushParameter(ctx);
}

void ASMFactory::PushBool(bool b)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_BOOL;
	bool* data = new bool;
	*data = b;
	ctx.parameter_data = data;
	PushParameter(ctx);
}

void ASMFactory::PushShort(short s)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_SHORT;
	short* data = new short;
	*data = s;
	ctx.parameter_data = data;
	PushParameter(ctx);
}

void ASMFactory::PushFloat(float f)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_FLOAT;
	float* data = new float;
	*data = f;
	ctx.parameter_data = data;
	PushParameter(ctx);
}

void ASMFactory::PushDouble(double d)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_DOUBLE;
	double* data = new double;
	*data = d;
	ctx.parameter_data = data;
	PushParameter(ctx);
}

void ASMFactory::PushByte(unsigned char uc)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_BYTE;
	unsigned char* data = new unsigned char;
	*data = uc;
	ctx.parameter_data = data;
	PushParameter(ctx);
}

void ASMFactory::PushPointer(void * ptr)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_PTR;
	ctx.parameter_data = ptr;
	PushParameter(ctx);
}

void ASMFactory::PushPointer64(void * ptr)
{

}

void ASMFactory::PushANSIString(const char * szString)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_STRING;
	ctx.parameter_data = (void*)szString;
	PushParameter(ctx);
}

void ASMFactory::PushUNICODEString(const wchar_t * szString)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_WSTRING;
	ctx.parameter_data = (void*)szString;
	PushParameter(ctx);
}

void ASMFactory::PushUNICODEStringStructure(UNICODE_STRING * ptrUnicodeString)
{
	ParameterContext64 ctx;
	ctx.type = PARAM_UNISTRUCT;
	ctx.parameter_data = (void*)ptrUnicodeString;
	PushParameter(ctx);
}

void ASMFactory::PushCall(CallingConvention64 cconv, FARPROC addr)
{
	auto begin = invoke_ctx.params.size();
	invoke_ctx.invoke_addr = (uintptr_t)addr;
	invoke_ctx.callingconvention = cconv;

	switch (cconv)
	{
	case CWIN64CALL:
	case CFASTCALL:
	{
		size_t rsp_delta = invoke_ctx.params.size() > 4 ? invoke_ctx.params.size() * sizeof(size_t) : 0x28;
		rsp_delta = Align(rsp_delta, 0x10);
		AddBytes({ 0x48, 0x83, 0xEC, (unsigned char)(rsp_delta + 8) });
		if (begin > 0)
		{
			for (int i = 0; i < PARAM_REG_MAX; i++)
			{
				if (invoke_ctx.params.empty()) break;
				if (STRING(invoke_ctx.params[0].type))
				{
					LoadStringParameter(invoke_ctx.params[0], (ParameterRegister64)i);
				}
				else
				{
					uint64_t param = *(uint64_t*)invoke_ctx.params[0].parameter_data;
					LoadParameter(param, (ParameterRegister64)i);
				}
				invoke_ctx.params.erase(invoke_ctx.params.begin());
			}
		}
		PushAllParameters(PUSH_RIGHT_TO_LEFT);
		AddBytes({ 0x49, 0xBD });
		AddLong64(invoke_ctx.invoke_addr);
		AddBytes({ 0x41, 0xFF, 0xD5 }); //Call
		AddBytes({ 0x48, 0x83, 0xC4, (unsigned char)(rsp_delta + 8) }); //Cleanup stack
		break;
	}
	case CCDECL:
	{
		int add_esp = begin * 4;
		PushAllParameters(PUSH_RIGHT_TO_LEFT);

		AddByte(0xB8);
		AddLong((unsigned long)invoke_ctx.invoke_addr);
		AddBytes({ 0xFF, 0xD0 });
		if (add_esp != 0)
		{
			bool use_byte = (add_esp <= 0xFF);
			if (use_byte)
			{
				AddBytes({ 0x83, 0xC4, (unsigned char)add_esp });
			}
			else
			{
				AddBytes({ 0x81, 0xC4 });
				AddLong(add_esp);
			}
		}
		break;
	}
	case CSTDCALL:
	{
		PushAllParameters(PUSH_RIGHT_TO_LEFT);
		AddByte(0xB8);
		AddLong((unsigned long)invoke_ctx.invoke_addr);
		AddBytes({ 0xFF, 0xD0 });
		break;
	}
	case CTHISCALL:
	{
		AddBytes({ 0xB8, 0x0D });
		AddLong((unsigned long)invoke_ctx.params[0].parameter_data);
		invoke_ctx.params.erase(invoke_ctx.params.begin());
		PushAllParameters(PUSH_RIGHT_TO_LEFT);
		AddByte(0xB8);
		AddLong((unsigned long)invoke_ctx.invoke_addr);
		AddBytes({ 0xFF, 0xD0 });
		break;
	}
	}
	invoke_ctx.params.clear();
	invoke_ctx.invoke_addr = NULL;
}

void ASMFactory::PushAllParameters(PushOrder64 po)
{
	if (invoke_ctx.params.empty()) return; //No parameters to push

	auto current_params = invoke_ctx.params;
	auto pushorder = (po == PUSH_LEFT_TO_RIGHT) ? current_params : ReverseVector(current_params);
	for (auto&& paramctx : pushorder)
	{
		if (paramctx.parameter_data == NULL)
		{
			AddByte(0x68);
			AddLong(0x00);
		}
		switch (paramctx.type)
		{
		case PARAM_DOUBLE:
		case PARAM_INT64:
		{
			if (paramctx.parameter_data)
			{
				uint64_t ulparam = *(uint64_t*)paramctx.parameter_data;
				AddByte(0x48);
				AddByte(0xB8);
				AddLong64(ulparam);
				AddByte(0x50);
			}
			else
			{
				AddByte(0x68);
				AddLong(0x00);
			}
			break;
		}
		case PARAM_PTR:
		{
			if (paramctx.parameter_data)
			{
				uintptr_t ulparam = *(uintptr_t*)(paramctx.parameter_data);
				AddByte(0x48);
				AddByte(0xB8);
				AddLong64(ulparam);
				AddByte(0x50);
			}
			else
			{
				AddByte(0x68);
				AddByte(0x00);
			}
			break;
		}
		case PARAM_SHORT:
		case PARAM_INT:
		case PARAM_FLOAT:
		{
			if (paramctx.parameter_data)
			{
				unsigned long ulparam = *(unsigned long*)paramctx.parameter_data;
				AddByte(0x68);
				AddLong(ulparam);
			}
			else
			{
				AddByte(0x68);
				AddLong(0x00);
			}
			break;
		}
		case PARAM_BYTE:
		{
			uint8_t byteparam = *(uint8_t*)paramctx.parameter_data;
			AddByte(0x6A);
			AddByte(byteparam);
			break;
		}
		case PARAM_BOOL:
		{
			bool boolparam = *(bool*)paramctx.parameter_data;
			AddByte(0x6A);
			AddByte((boolparam) ? 1 : 0);
			break;
		}
		case PARAM_STRING:
		{
			char* stringparam = (char*)paramctx.parameter_data;
			StringContext64 sctx;
			sctx.size = (ULONG)(strlen(stringparam) + 1);
			sctx.ptr = CommitBlock(stringparam, sctx.size);
			if (sctx.ptr == NULL)
				continue;
			invoke_ctx.strings.push_back(sctx);
			/*
			AddByte(0x48);
			AddByte(0xB8);
			AddLong64((uintptr_t)sctx.ptr);
			AddByte(0x50);
			*/
			AddByte({ 0x68 });
			AddLong((uintptr_t)sctx.ptr);
			break;
		}
		case PARAM_WSTRING:
		{
			wchar_t* stringparam = (wchar_t*)paramctx.parameter_data;
			StringContext64 sctx;
			sctx.size = (ULONG)(wcslen(stringparam) * 2) + 1;
			sctx.ptr = CommitBlock(stringparam, sctx.size);
			if (sctx.ptr == NULL)
				continue;
			invoke_ctx.strings.push_back(sctx);
			AddByte(0x48);
			AddByte(0xB8);
			AddLong64((uintptr_t)sctx.ptr);
			AddByte(0x50);
			break;
		}
		case PARAM_UNISTRUCT:
		{
			UNICODE_STRING ustr = *(UNICODE_STRING*)paramctx.parameter_data;

			StringContext64 sctx;
			sctx.size = (ULONG)(ustr.MaximumLength * 2) + 1;
			sctx.ptr = CommitBlock(ustr.Buffer, sctx.size);
			if (sctx.ptr == NULL)
				continue;
			invoke_ctx.strings.push_back(sctx);

			UNICODE_STRING alloc;
			alloc.Buffer = (wchar_t*)sctx.ptr;
			alloc.Length = ustr.Length;
			alloc.MaximumLength = ustr.MaximumLength;

			StructContext64 structctx;
			structctx.size = (ULONG)sizeof(UNICODE_STRING);
			structctx.data = CommitBlock(&alloc, structctx.size);

			invoke_ctx.structs.push_back(structctx);
			AddByte(0x68);
			AddLong((uintptr_t)structctx.data);
			break;
		}
		default:
		{
			break;
		}
		}
	}
}

void ASMFactory::AddBytes(const std::initializer_list<unsigned char>& bytes)
{
	for (auto& b : bytes)
		AddByte(b);
}

void ASMFactory::AddByte(unsigned char b)
{
	remote_buffer.push_back(b);
}

void ASMFactory::AddLong(unsigned long l)
{
	WORD lo = LOWORD(l);
	WORD hi = HIWORD(l);

	AddBytes({ LOBYTE(lo), HIBYTE(lo) });
	AddBytes({ LOBYTE(hi), HIBYTE(hi) });
}

void ASMFactory::AddLong64(uint64_t ull)
{
	unsigned long lo = (unsigned long)ull;
	unsigned long hi = (unsigned long)(ull >> 32);
	AddLong(lo);
	AddLong(hi);
}

void ASMFactory::GeneratePrologue()
{
	//Save parameter Registers
	/*
	mov QWORD PTR[rsp+0x8], rcx
	mov QWORD PTR[rsp+0x10], rdx
	mov QWORD PTR[rsp+0x18], r8
	mov QWORD PTR{rsp+0x20], rcx
	*/

	AddBytes({ 0x48, 0x89, 0x4C, 0x24, 1 * sizeof(size_t) });
	AddBytes({ 0x48, 0x89, 0x54, 0x24, 2 * sizeof(size_t) });
	AddBytes({ 0x4C, 0x89, 0x44, 0x24, 3 * sizeof(size_t) });
	AddBytes({ 0x4C, 0x89, 0x4C, 0x24, 4 * sizeof(size_t) });
}
void ASMFactory::GenerateEpilogue(int ret_size)
{
	//Restore registers
	/*
	mov rcx, QWORD PTR[rsp+0x8]
	mov rdx, QWORD PTR[rsp+0x10]
	mov r8,  QWORD PTR[rsp+0x18]
	mov r9,  QWORD PTR[rsp+0x20]
	ret
	*/
	AddBytes({ 0x48, 0x8B, 0x4C, 0x24, 1 * sizeof(size_t) });
	AddBytes({ 0x48, 0x8B, 0x54, 0x24, 2 * sizeof(size_t) });
	AddBytes({ 0x4C, 0x8B, 0x44, 0x24, 3 * sizeof(size_t) });
	AddBytes({ 0x4C, 0x8B, 0x4C, 0x24, 4 * sizeof(size_t) });
	AddByte(0xC3);

}

void ASMFactory::CreateRPCEnvironment()
{
	NTSTATUS status = ERROR_SUCCESS;
	uint32_t thid = 0xDEADBEEF;
	if (worker_thread_code_cave == 0)
		worker_thread_code_cave = target_process.AllocateMemory(0x1000);
	user_data = target_process.AllocateMemory(0x4000, 0, PAGE_READWRITE);
	status = CreateWorkerThread();
	if (!NT_SUCCESS(status))
	{
		return;
	}
	thid = GetThreadId(worker_thread);
	status = CreateAPCEvent(thid);
}

void ASMFactory::SaveReturn()
{
	/*
	mov rdx, [rsp + 0x8]
	mov [rdx], rax
	*/
	AddBytes({ 0x48, 0x8B, 0x54, 0x24, 0x08 });
	AddBytes({ 0x48, 0x89, 0x02 });
}

void ASMFactory::SignalOSEvent()
{
	/*
	mov rcx, [rdx + 0x8]
	mov r13, SetEvent
	call r13

	*/
	PushInt64((uintptr_t)apc_sync_wait_event);
	PushCall(CWIN64CALL, (FARPROC)SetEvent);
}

void ASMFactory::FlushAssembler()
{
	for (auto& str : invoke_ctx.strings)
	{
		target_process.FreeMemory((uintptr_t)str.ptr);
	}
	for (auto& rstruct : invoke_ctx.structs)
	{
		target_process.FreeMemory((uintptr_t)rstruct.data);
	}
	invoke_ctx.invoke_addr = 0;
	invoke_ctx.params.clear();
	remote_buffer.clear();
}

void ASMFactory::ExecuteASMInWorker(std::vector<byte> buffer, uintptr_t& call_result)
{
	byte* code_buffer = new byte[buffer.size()];
	int idx = 0;
	for (auto&& bytecode : buffer)
	{
		code_buffer[idx++] = bytecode;
	}
	auto remote_code_cave = CommitBlock(code_buffer, buffer.size());
	delete[] code_buffer;

	if (!worker_thread)
		CreateRPCEnvironment();

	ResetEvent(apc_sync_wait_event);

	if (QueueUserAPC((PAPCFUNC)remote_code_cave, worker_thread, (ULONG_PTR)worker_thread_code_cave))
	{
		auto result = WaitForSingleObject(apc_sync_wait_event, INFINITE);
		target_process.ReadMemory(&call_result, worker_thread_code_cave, sizeof(uintptr_t));
	}
	Sleep(5);

	FlushAssembler();
}

void ASMFactory::ExecuteASMInWorker(uintptr_t& call_result)
{
	ExecuteASMInWorker(remote_buffer, call_result);
}

void ASMFactory::ExecuteASM(std::vector<byte> code_buffer)
{
	void* remote_buffer = nullptr;
	byte* buffer = new byte[code_buffer.size()];

	int idx = 0;
	for (auto&& bytecode : code_buffer)
	{
		buffer[idx++] = bytecode;
	}

	remote_buffer = CommitBlock(buffer, code_buffer.size());
	delete[] buffer;

	auto handle = target_process.CreateThread((uintptr_t)remote_buffer, NULL);
	if (!handle || handle == INVALID_HANDLE_VALUE)
	{
		FlushAssembler();
		return;
	}

	WaitForSingleObject(handle, INFINITE);
	FlushAssembler();
}

void* ASMFactory::CommitBlock(void * chunk, const size_t size)
{
	auto remote_block = target_process.AllocateMemory(size);
	if (remote_block == NULL)
		return nullptr;

	blocks.insert(std::make_pair((uintptr_t)remote_block, size));
	return (target_process.WriteMemory(chunk, remote_block, size) ? (void*)remote_block : nullptr);
}

void ASMFactory::LoadStringParameter(ParameterContext64 pctx, ParameterRegister64 reg)
{
	switch (pctx.type)
	{
	case PARAM_STRING:
	{
		char* param = (char*)pctx.parameter_data;
		StringContext64 sctx;
		sctx.size = (ULONG)(strlen(param) + 1);
		sctx.ptr = CommitBlock(param, sctx.size);
		if (sctx.ptr == NULL)
			return;
		invoke_ctx.strings.push_back(sctx);
		LoadParameter((uintptr_t)sctx.ptr, reg);
		break;
	}
	case PARAM_WSTRING:
	{
		wchar_t* param = (wchar_t*)pctx.parameter_data;
		StringContext64 sctx;
		sctx.size = (ULONG)(wcslen(param) * 2) + 1;
		sctx.ptr = CommitBlock(param, sctx.size);
		if (sctx.ptr == NULL)
			return;
		invoke_ctx.strings.push_back(sctx);
		LoadParameter((uintptr_t)sctx.ptr, reg);
		break;
	}
	case PARAM_UNISTRUCT:
	{
		UNICODE_STRING ustr = *(UNICODE_STRING*)pctx.parameter_data;
		StringContext64 sctx;
		sctx.size = (ULONG)(ustr.MaximumLength * 2) + 1;
		sctx.ptr = CommitBlock(ustr.Buffer, sctx.size);
		if (sctx.ptr == NULL)
			return;
		invoke_ctx.strings.push_back(sctx);

		UNICODE_STRING alloc;
		alloc.Buffer = (wchar_t*)sctx.ptr;
		alloc.Length = ustr.Length;
		alloc.MaximumLength = ustr.MaximumLength;

		StructContext64 allocctx;
		allocctx.size = (ULONG)sizeof(UNICODE_STRING);
		allocctx.data = CommitBlock(&alloc, allocctx.size);

		invoke_ctx.structs.push_back(allocctx);
		LoadParameter((uintptr_t)allocctx.data, reg);
		break;
	}
	}
}

void ASMFactory::LoadParameter(uint64_t param, ParameterRegister64 reg)
{
	switch (reg)
	{
	case RCX:
	{
		AddBytes({ 0x48, 0xB9 });
		AddLong64(param);
		break;
	}
	case RDX:
	{
		AddBytes({ 0x48, 0xBA });
		AddLong64(param);
		break;
	}
	case R8:
	{
		AddBytes({ 0x49, 0xB8 });
		AddLong64(param);
		break;
	}
	case R9:
	{
		AddBytes({ 0x49, 0xB9 });
		AddLong64(param);
	}
	}
}

void ASMFactory::TerminateWorkerThread()
{
	if (apc_sync_wait_event)
	{
		CloseHandle(apc_sync_wait_event);
		apc_sync_wait_event = NULL;
	}
	if (worker_thread)
	{
		TerminateThread(worker_thread, 0);
		worker_thread = NULL;
		if (worker_code_thread)
		{
			target_process.FreeMemory(worker_code_thread, 0x1000);
			worker_code_thread = NULL;
		}
	}
}

void ASMFactory::FreeAllocatedBlocks()
{
	for (auto&& block : blocks)
	{
		if (!target_process.FreeMemory(block.first))
		{
			continue;
		}
	}
}

NTSTATUS ASMFactory::CreateWorkerThread()
{
	NTSTATUS status = ERROR_SUCCESS;
	if (worker_thread == NULL || worker_thread == INVALID_HANDLE_VALUE)
	{
		/*
		GeneratePrologue();
		PushUInt(5);
		PushUInt(TRUE);
		PushCall(CWIN64CALL, (FARPROC)SleepEx);
		AddBytes({ 0xEB, 0xD5 });
		TerminateThreadSaveStatus();
		GenerateEpilogue();
		byte* buffer = (byte*)malloc(remote_buffer.size());
		int idx = 0;
		for (auto& bytecode : remote_buffer)
		{
		buffer[idx++] = bytecode;
		}
		auto code_size = remote_buffer.size();
		auto worker_thread_addr = (worker_thread_code_cave + thread_stack);
		user_code = worker_thread_addr + code_size;
		target_process.WriteMemory(buffer, worker_thread_addr, code_size);
		worker_thread = target_process.CreateThread(worker_thread_addr, user_data);
		free(buffer);
		FlushAssembler();
		*/
		uintptr_t NtDelayExecution = target_process.GetModuleExport("ntdll.dll", "NtDelayExecution");
		GeneratePrologue();
		PushBool(TRUE);
		PushInt64(user_data);
		PushCall(CWIN64CALL, (FARPROC)NtDelayExecution);
		AddBytes({ 0xEB, 0xD5 });
		TerminateThreadSaveStatus(user_data);
		GenerateEpilogue();
		LARGE_INTEGER delay = { { 0 } };
		delay.QuadPart = -10 * 1000 * 5;
		target_process.WriteMemory((void*)&delay, worker_thread_code_cave, sizeof(LARGE_INTEGER));
		worker_thread_code_cave += Align(sizeof(LARGE_INTEGER), 0x10);

		byte* buffer = (byte*)malloc(remote_buffer.size());
		int idx = 0;
		for (auto& bytecode : remote_buffer)
		{
			buffer[idx++] = bytecode;
		}
		auto code_size = remote_buffer.size();
		target_process.WriteMemory(buffer, worker_thread_code_cave, code_size);
		worker_thread = target_process.CreateThread(worker_thread_code_cave, user_data);
		free(buffer);
		FlushAssembler();
	}
	return status;
}
NTSTATUS ASMFactory::CreateAPCEvent(DWORD thid)
{
	NTSTATUS status = ERROR_SUCCESS;
	if (apc_sync_wait_event == INVALID_HANDLE_VALUE || apc_sync_wait_event == NULL)
	{
		wchar_t event_name[MAX_PATH] = { 0 };
		uintptr_t res = NULL;
		USHORT len = sizeof(event_name);
		OBJECT_ATTRIBUTES attributes{ 0 };
		OBJECT_ATTRIBUTES remote_attributes{ 0 };
		UNICODE_STRING str = { 0 };
		UNICODE_STRING remote_str = { 0 };

		swprintf_s(event_name, ARRAYSIZE(event_name), L"\\BaseNamedObjects\\_MMapEvent_0x%x_0x%x", thid, GetTickCount());
		wchar_t* ss_desc = L"S:(ML;;NW;;;LW)D:(A;;GA;;;S-1-15-2-1)(A;;GA;;;WD)";
		PSECURITY_DESCRIPTOR desc = nullptr;
		ConvertStringSecurityDescriptorToSecurityDescriptorW(ss_desc, SDDL_REVISION_1, &desc, NULL);

		str.Length = (wcslen(event_name) * sizeof(wchar_t));
		str.MaximumLength = (len);
		str.Buffer = event_name;

		attributes.ObjectName = &str;
		attributes.Length = sizeof(attributes);
		attributes.SecurityDescriptor = desc;
		auto addr = user_data;
		status = NtCreateEvent(&apc_sync_wait_event, EVENT_ALL_ACCESS, &attributes, EVENT_TYPE::NotificationEvent, FALSE);
		if (!NT_SUCCESS(status))
		{
			return status;
		}
		wchar_t* _event_name = event_name;
		StringContext64 sctx;
		sctx.size = len;
		sctx.ptr = CommitBlock(_event_name, sctx.size);

		UNICODE_STRING alloc = { 0 };
		alloc.Buffer = (wchar_t*)sctx.ptr;
		alloc.Length = wcslen(_event_name) * sizeof(wchar_t);
		alloc.MaximumLength = (USHORT)len;

		StructContext64 structctx;
		structctx.size = sizeof(UNICODE_STRING);
		structctx.data = CommitBlock(&alloc, structctx.size);

		OBJECT_ATTRIBUTES attributes_alloc = { 0 };
		attributes_alloc.ObjectName = (PUNICODE_STRING)structctx.data;
		attributes_alloc.Length = sizeof(attributes_alloc);

		StructContext64 attributes_ctx;
		attributes_ctx.size = sizeof(OBJECT_ATTRIBUTES);
		attributes_ctx.data = CommitBlock(&attributes_alloc, attributes_ctx.size);

		/*
		if (desc)
		{
		delete desc;
		desc = NULL;
		}
		*/
		GeneratePrologue();
		PushInt64(user_data);
		PushInt64(EVENT_MODIFY_STATE | SYNCHRONIZE);
		PushInt64((uintptr_t)attributes_ctx.data);
		PushCall(CWIN64CALL, (FARPROC)target_process.GetModuleExport("ntdll.dll", "NtOpenEvent"));
		TerminateThreadSaveStatus(user_data + sizeof(HANDLE));
		GenerateEpilogue();

		byte* buffer = (byte*)malloc(remote_buffer.size());
		int idx = 0;
		for (auto& bytecode : remote_buffer)
		{
			buffer[idx++] = bytecode;
		}
		uintptr_t code_cave = (uintptr_t)CommitBlock(buffer, remote_buffer.size());
		free(buffer);
		HANDLE hThread = target_process.CreateThread(code_cave, worker_thread_code_cave);
		DWORD thread_exit_code;
		if (hThread)
		{
			WaitForSingleObject(hThread, INFINITE);
			GetExitCodeThread(hThread, &thread_exit_code);
		}

		target_process.ReadMemory((void*)&apc_sync_wait_event, user_data, sizeof(HANDLE));

		target_process.FreeMemory(code_cave, remote_buffer.size());
		FlushAssembler();
	}
	return status;
}

void ASMFactory::TerminateThreadSaveStatus(uintptr_t result = 0)
{
	/*
	mov rcx, rax
	mov r13, [terminator]
	call r13
	*/
	/*
	AddBytes({ 0x48, 0x89, 0xC1 });
	AddBytes({ 0x49, 0xBD });
	AddLong64((uintptr_t)thread_terminator);
	AddBytes({ 0x41, 0xFF, 0xD5 });
	*/

	/*
	mov rdx, result
	mov qword ptr[rdx], rax
	mov rdx, rax
	mov rcx, 0
	mov rax, NtTerminateThread
	call rax
	*/
	uintptr_t NtTerminateThread = target_process.GetModuleExport("ntdll.dll", "NtTerminateThread");
	if (result != 0)
	{
		AddBytes({ 0x48, 0xBA });
		AddLong64(result);
		AddBytes({ 0x48, 0x89, 0x02 });
	}
	AddBytes({ 0x48, 0x8B, 0xD0 });
	AddBytes({ 0x48, 0xC7, 0xC1 });
	AddLong(0);
	AddBytes({ 0x48, 0xB8 });
	AddLong64(NtTerminateThread);
	AddBytes({ 0xFF, 0xD0 });
}