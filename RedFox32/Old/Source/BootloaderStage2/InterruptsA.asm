[EXTERN InterruptHandlerStub]

%macro Interrupt 2
	[GLOBAL %1]
	%1:
	pushad
	push %2
	call InterruptHandlerStub
	pop eax
	popad
	iret
%endmacro

Interrupt Int0, 0
Interrupt Int1, 1
Interrupt Int2, 2
Interrupt Int3, 3
Interrupt Int4, 4
Interrupt Int5, 5
Interrupt Int6, 6 
Interrupt Int7, 7 
Interrupt Int8, 8
Interrupt Int9, 9
Interrupt Int10, 10
Interrupt Int11, 11
Interrupt Int12, 12
Interrupt Int13, 13 
Interrupt Int14, 14
Interrupt Int15, 15

[GLOBAL IntSyscall]
[EXTERN IntSyscall_Handler]
IntSyscall:
	pushad
	call IntSyscall_Handler
	popad
	iret

; Loading Interrupt Descriptor Table
[GLOBAL LoadIDT]
LoadIDT:
	cli ; Ensure interrupts are disabled
	mov edx, [esp+4]
	lidt [edx]
	ret
