;************************************
; KEntry.asm
; Initialize the system and begin to
; enter into the main 32 bit kernel
;************************************
[BITS 16]		; The first set of code is in 16 bit mode as we have yet
				; to switch over to Protected Mode. Some of the following code
				; does.
section .text	; All of this code is in the .text section
jmp	0x00:Stage2	; Jump over data to Stage 2 code, ensuring to set registers to
				; 0x00 implicitly

;*******************************************
; Global Descriptor Table (GDT)
;*******************************************
; I forgot where I got this GDT from, all I know is it works the way I want
; it too... I should definately take another look at what is going on here!
gdt_data: 
dq 0							; null descriptor

; gdt code:						; code descriptor
dw 0xFFFF 						; limit low
dw 0 							; base low
db 0 							; base middle
db 10011010b 					; access
db 11001111b 					; granularity
db 0 							; base high

; gdt data:						; data descriptor
dw 0xFFFF 						; limit low (Same as code)
dw 0 							; base low
db 0 							; base middle
db 10010010b 					; access
db 11001111b 					; granularity
db 0							; base high

end_of_gdt:						; Save the end point
dw end_of_gdt - gdt_data - 1 	; limit (Size of GDT)
dd gdt_data						; base of GDT

; Define a string to be printed when we exit from the C code
EnteringStopLoopStr: db "C has exited, halting system!", 0

Stage2:

; Enter a known video mode, 80 columns
mov ax, 0x0003	; Mode 3, 80 column mode, 16 colour
int 0x10		; Call the 0x10 BIOS interrupt

; Disable the text mode cursor, it is not required
mov ax, 0x0100	; ah 0x01, al 0x00
mov cx, 0x3F00	; ch 0x3F, cl 0x00
int 0x10		; Call the 0x10 BIOS interrupt

; Identify stage 2 entry
mov ah, 0x0E	; TTY Print
mov al, '2'		; Character value of 2 (2 | 0x30)
xor bx, bx		; Screen 0
int 0x10		; Call the 0x10 BIOS interrupt

; Set segment registers
cli				; Safety first! Disable interrupts.
xor	ax, ax		; Zero AX so we can zero the segment registers
mov	ds, ax		; DS = 0x00
mov	es, ax		; ES = 0x00
mov	ax, 0x9000	; Set AX to the stack base. The stack begins at 0x9000-0xffff
mov	ss, ax		; Set the Stack base register to 0x9000 (AX);
mov	sp, 0xFFFF	; Set the End of the stack to 0xFFFF

; Loading the Global Descriptor Table
; Interrupts are still disabled from setting the registers
lgdt [end_of_gdt] 	; Load the GDT
sti					; Enable interrupts so we can use BIOS interrupts 0x10

; Identify that we are about to switch to Protocted Mode
mov ah, 0x0E	; TTY Print
mov al, 'P'		; Char P
int 0x10		; Call the 0x10 BIOS interrupt

; Switch to protected mode
cli				; Turn interrupts off again
mov	eax, cr0	; Get the value of the cr0 register
or	eax, 1		; Set the PE mode bit
mov	cr0, eax	; Set cr0

; Jump to the 32bit code
jmp	0x08:Stage232 ; far jump to fix CS. Remember that the code selector is 0x8!


[BITS 32] 	; Welcome to the 32 bit world!
			; All code from this point will be compiled in 32 bit operations
[EXTERN Stage2_CEntry]	; We can now link to C code so we need to reference to
						; the linker that we want to access the Stage2_CEntry
						; symbol
[EXTERN puts]			; Same again for the puts function
						;
section .text			; This code will also be found in the "text" section
						;
Stage232:				; Stage2 32 bit label

; Initialize the segment registers
mov ax, 0x10	; Load the value 0x10 into EAX and clear higher bits
mov ds, ax		; DS = 0x10
mov es, ax		; ES = 0x10
mov fs, ax		; FS = 0x10
mov gs, ax		; GS = 0x10
mov ss, ax			; SS = 0x10 as per GDT

; Setup the stack (Ensure that there's no incorrect bits in high)
; The stack is set to be the memory region: 0x9000-0xb8000
mov ebp, 0x9000		; Set the base pointer
mov esp, 0xFFFF		; Set the top of the stack to b8000

; Enable the A20 addressing line
call EnableA20

; Call into the C-Code
call Stage2_CEntry

; C code has returned
push 0x0C					; Colour
push EnteringStopLoopStr	; String
call puts					; Call to the c function:
							; puts(char *str, char colour);

; This would be where a system shutdown would take place however this works
; to just halt the system once the C-Code returns, if at all.
STOP32:
cli			; Disable interrupts
hlt			; Halt the CPU (Only interrupts can unhalt however we disabled them)
jmp STOP32	; If we somehow to continue just lockup again.

;---------------;
; Enable A20
; - Disables interrupts
; - Returns nothing, with interrupts disabled
; - This allows us to create an interrupt table (See C code) and then reenable
;   them without issues at a later date!
; ----------- This code was copied from either a forum post or wiki.osdev.org
; ----------- I would format this to match but the code squares are nice :P
EnableA20:
cli					; Absolutely ensure that interrupts are disabled

call    a20wait		; Call to the a20wait function
mov     al, 0xAD	; Set AL to 0xAD
out     0x64, al	; Write to port 0x64

call    a20wait		; Call to the a20wait function
mov     al,0xD0		; Set AL to 0xD0
out     0x64,al		; Write to port 0x64

call    a20wait2	; Call to the a20wait function
in      al,0x60		; Read port 0x60
push    eax			; Push eax to the stack

call    a20wait		; Call the a20wait function
mov     al,0xD1		; Set AL to 0xD1
out     0x64,al		; Write to port 0x64

call    a20wait		; Call the a20wait function
pop     eax			; Pop the value back off the stack
or      al,2		; Or AL with 2
out     0x60,al		; Write the result to port 0x60

call    a20wait		; Call to the a20wait function
mov     al,0xAE		; AL = 0xAE 
out     0x64,al		; Write to port 0x64

call    a20wait		; Call the a20wait function
ret					; Return to the init code

a20wait:
in      al,0x64		; Read form port 0x64
test    al,2		; Test the value with bit 2
jnz     a20wait		; If the value is 2 then jump to a20wait2
ret					; Else return now

a20wait2:			;
in      al,0x64		; Read from port 0x64
test    al,1		; Compare the value with 1
jz      a20wait2	; If the value is 0 repeat
ret					; Else we're returning to the caller within the EnableA20
					; function


