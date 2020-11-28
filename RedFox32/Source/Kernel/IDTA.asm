[BITS 32]
[SECTION .text]

[EXTERN InterruptHandlerStub]

%macro Interrupt 1
	[GLOBAL Int_%1]
	Int_%1:
		pushad
		push %1
		call InterruptHandlerStub
		pop eax
		popad
		iret
%endmacro

Interrupt 0
Interrupt 1
Interrupt 2
Interrupt 3
Interrupt 4
Interrupt 5
Interrupt 6
Interrupt 7
Interrupt 8
Interrupt 9
Interrupt 10
Interrupt 11
Interrupt 12
Interrupt 13
Interrupt 14
Interrupt 15

[GLOBAL SetIDT]
SetIDT:
	cli
	mov edx, [esp+4]
	lidt [edx]
	ret
