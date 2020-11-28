[BITS 32]
[SECTION .text]

; Ensure that we know what the InterruptHandlerStub is.
[EXTERN InterruptHandlerStub]

; Macro Interrupt %1
; This macro creates an interrupt handler of name Int_%1
; Pushes the registers onto the stack followed by the ID (%1). This allows for
; their usage within our InterruptHandlerStub C function without the need for
; using a register to pass the interrupt ID. This becomes much easier to deal
; with in C.
%macro Interrupt 1
	[GLOBAL Int_%1]
	Int_%1:
		pushad
		push byte %1
		call InterruptHandlerStub
		pop eax
		popad
		iretd
%endmacro

; Hardware interrupts
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

; Software interrupts
; IntSyscallHandler
; Does exactly the same as the above however has been named for ease of
; identification thus creating the need to manually define it otherwise I would
; have two symbols for the same function.
[GLOBAL IntSyscallHandler]
IntSyscallHandler:
	pushad
	push word 0x0080
	call InterruptHandlerStub
	pop word ax	
	popad
	iretd


; SetIDT
; A wrapper around the lidt command, allowing for simplicity when it is desired
; to change the current IDT. Takes a pointer to the IDT_Pointer structure
; (IDT_Pointer *ptr)
; Leaves interrupts disabled
[GLOBAL SetIDT]
SetIDT:
	cli
	mov edx, [esp+4]
	lidt [edx]
	ret
