; KEntry.asm
; KEntry.asm is used toumake the transition to the kernel as easy as calling
; its start address!
[BITS 32]
[section .text]

; Ensure we get linked to Kernel.c and know about the KMain function
[EXTERN KMain]

; We call into the kernel from the bootloader, this code is linked to the start
; of our binary kernel.kernel file, placing it as the first bytes to run. This
; allows us to continue the call further onto the KMain function without linking
; the kernel and bootloader or sharing any symbols.
KEntry:
	; Enter our C code
	; A jump is used here as we have already passed parameters on the stack.
	; This is just a wrapper
	jmp KMain
