;******************************************************************************
; KEntry.asm
; Initialize the system 
;******************************************************************************
[BITS 16]		; The first set of code is in 16 bit mode as we have yet
				; to switch over to Protected Mode. Some of the following code
				; does.
jmp	0x00:Stage2	; Jump over data to Stage 2 code, ensuring to set registers to
				; 0x00 implicitly (This should be done already, it's just a
				; precaution)

BootDevice: db 0

;******************************************************************************
; Global Descriptor Table (GDT)
;******************************************************************************
; Creates a flat style memory layout
;******************************************************************************
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
mov byte [BootDevice], dl

call VideoSetup

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

sti
mov dword [0x8000], 0	; Set the number of entries to 0
call CreateMemoryMap
call VideoSetup
; Let's load the kernel
call LoadKernel

cli
; Loading the Global Descriptor Table
; Interrupts are still disabled from setting the registers
lgdt [end_of_gdt] 	; Load the GDT

; Switch to protected mode
mov	eax, cr0	; Get the value of the cr0 register
or	eax, 1		; Set the PE mode bit
mov	cr0, eax	; Set cr0

; Jump to the 32bit code
jmp	0x08:Stage232 ; far jump to fix CS. Remember that the code selector is 0x8!

VideoSetup:
; Enter a known video mode, 80 columns
mov ax, 0x0003	; Mode 3, 80 column mode, 16 colour
int 0x10		; Call the 0x10 BIOS interrupt

; Disable the text mode cursor, it is not required
mov ax, 0x0100	; ah 0x01, al 0x00
mov cx, 0x3F00	; ch 0x3F, cl 0x00
int 0x10		; Call the 0x10 BIOS interrupt
ret

KERNEL_BASE: equ 0x2900
KERNEL_LOAD_ADDR: dw 0x2900
KERNEL_CYLINDER_COUNT: dw 0x0001 	; 1 based - MAXIMUM 4 without es, this may
									; need future changes to resolve in the 
									; event I need more than 1-4 cylinders 
LoadKernel:
	; Reset the drive
	xor ax, ax	
	mov dl, [BootDevice]
	int 0x13

	mov cx, 1
.Loop:
	push cx
	; Put an L on the screen to show a sector load attempt
	pusha
	mov ah, 0x0E
	mov al, 'L'
	xor bx, bx
	xor cx, cx
	xor dx, dx
	int 0x10
	popa
	; Call to load at given position this may need future changes, see [L:99]
	mov bx, [KERNEL_LOAD_ADDR]
	mov ax, cx
	call LoadCylinder

	mov bx, word [KERNEL_LOAD_ADDR]
	add bx, 0x2400
	mov word [KERNEL_LOAD_ADDR], bx
	
	pop cx
	add cx, 1
	cmp cx, word [KERNEL_CYLINDER_COUNT]
	jl .Loop
	ret

; LoadCylinder
; AX = Cylinder
; BX = Address
LoadCylinder:
	jmp .Try
.LoadError:
	mov ah, 1
	mov dl, byte [BootDevice]
	int 0x13
	call Error
	popa
	nop
.Try:
	pusha
	; Calculate the head
	push ax
	push bx

	mov cx, 2
	div cl
	mov dh, ah

	pop  bx
	pop  ax

	mov cx, ax
	;shl cx, 6
	;or cl, 1
	mov ah, 2
	mov al, 18
	mov dl, byte [BootDevice]
	clc
	int 0x13
	jc .LoadError

	popa
	ret

Error:
	mov ah, 0x0E
	mov al, 'E'
	mov bx, 0
	mov cx, 0
	mov dx, 0
	int 0x10
.Loop:
	cli
	hlt
	jmp .Loop

; use the INT 0x15, eax= 0xE820 BIOS function to get a memory map
; note: initially di is 0, be sure to set it to a value so that the BIOS
;       code will not be overwritten. The consequence of overwriting the BIOS
;       code will lead to problems such as getting stuck in `int 0x15`
; inputs: es:di -> destination buffer for 24 byte entries
; outputs: bp = entry count, trashes all registers except esi
; CODE FROM:
; https://wiki.osdev.org/Detecting_Memory_(x86)#Getting_an_E820_Memory_Map
mmap_ent equ 0x8000             ; the number of entries will be stored at 0x8000
; 0x8000 = uint32 num entries
; 0x8004+ = Entries see mmap_qemu.txt
CreateMemoryMap:
	; Set di to 0x8004. Otherwise this code will get stuck in
	; `int 0x15` after some entries are fetched 
    mov di, 0x8004
	xor ebx, ebx			; ebx must be 0 to start
	xor bp, bp				; keep an entry count in bp
	mov edx, 0x0534D4150	; Place "SMAP" into edx
	mov eax, 0xe820
	mov [di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24				; ask for 24 bytes
	int 0x15
	jc short .failed		; carry set on first call means "unsupported function"
	mov edx, 0x0534D4150	; Some BIOSes apparently trash this register?
	cmp eax, edx			; on success, eax must have been reset to "SMAP"
	jne short .failed
	test ebx, ebx			; ebx = 0 implies list is only 1 entry long
							; A single entry here is useless information
	je short .failed
	jmp short .jmpin
.e820lp:
	mov eax, 0xe820			; eax, ecx get trashed on every int 0x15 call
	mov [di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24				; ask for 24 bytes again
	int 0x15
	jc short .e820f			; carry set means "end of list already reached"
	mov edx, 0x0534D4150	; repair potentially trashed register
.jmpin:
	jcxz .skipent			; skip any 0 length entries
	cmp cl, 20				; got a 24 byte ACPI 3.X response?
	jbe short .notext
	test byte [di + 20], 1	; if so: is the "ignore this data" bit clear?
	je short .skipent
.notext:
	mov ecx, [di + 8]		; get lower uint32_t of memory region length
	or ecx, [di + 12]		; "or" it with upper uint32_t to test for zero
	jz .skipent				; if length uint64_t is 0, skip entry
	inc bp					; got a good entry: ++count, move to next storage spot
	add di, 24
.skipent:
	test ebx, ebx			; if ebx resets to 0, list is complete
	jne short .e820lp
.e820f:
	mov [mmap_ent], bp		; store the entry count
	clc						; there is "jc" on end of list to this point,
							; so the carry must be cleared
	ret
.failed:
	stc						; "function unsupported" error exit
	ret

;******************************************************************************
; This is where all the 32 bit code will happen, this will do further setup and
; perform the switch into C code, the switch to the Kernel will happen from
; within the C code
;******************************************************************************
[BITS 32] 				; Welcome to the 32 bit world!
[EXTERN Stage2_CEntry]	; We can now link to C code so we need to reference to
						; the linker that we want to access the Stage2_CEntry
						; symbol
[EXTERN puts]			; Same again for puts
Stage232:				; Stage2 32 bit label

; Initialize the segment registers
mov ax, 0x10	; Load the value 0x10 into EAX and clear higher bits
mov ds, ax		; DS = 0x10
mov es, ax		; ES = 0x10
mov fs, ax		; FS = 0x10
mov gs, ax		; GS = 0x10
mov ss, ax		; SS = 0x10 as per GDT

; Setup the stack (Ensure that there's no incorrect bits in high)
; Need to figure out an appropriate stack size :)
mov esp, 0x00105000; Set the top of the stack to 
mov ebp, 0x00100000

; Enable the A20 addressing line
call EnableA20

; Push our memory map location
push dword mmap_ent 
call 0x2900

; This would be where a system shutdown would take place however this works
; to just halt the system once the C-Code returns, if at all.
STOP32:
cli			; Disable interrupts
hlt			; Halt the CPU (Only interrupts can unhalt however we disabled them)
jmp STOP32	; If we somehow to continue just lockup again.

;******************************************************************************
; Enable A20
;******************************************************************************
EnableA20:

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
ret					; Else we're returning to the caller

