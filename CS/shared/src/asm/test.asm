.code
		mov rdx, 0deaddeaddeaddeadh
		mov qword ptr[rdx], rax
		mov rdx, rax
		mov rcx, 0
		mov rax, 0deaddeaddeaddeadh
		call rax
END