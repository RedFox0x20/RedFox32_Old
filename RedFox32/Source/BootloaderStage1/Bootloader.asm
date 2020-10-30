; ************************************
; Boot.asm
; Define the boot sectory for the disk
; Given how small the first stage has shown itself to be, it may actually be
; possible for us to take the 16bit code in stage 2 and contain it within the
; boot sector thus reducing the disk footprint we have
; ************************************
[BITS 16]		; This is the code first run by the system loaded by the BIOS
[ORG 0x7C00]	; It is located at 0x7C00 on all x86 systems so we can hard code
				; this value so the compiler knows where we are.
%define Stage2Location 0x0500 	; I plan on loading the Kernel to 0x0500
								; Using a NASM %define function here to make the
								; value easier to change in the future in the
								; event I decide to move it.
; Execution begins at 0x7C00 => Jump over the params to our entry point
jmp Boot	; Jump over the data
nop			; Add a nop for padding - This is a FAT thing however I am bringing
			; this forward to my Filesystem

; FILESYSTEM Data
FSDiskName: db "REDFOX  "	; The name of the disk (Some form of UID would be
							; useful too)
FSRootDirectoryLBA: dw 1	; Where can we find the actual filesystem definition
FSSectorMapLBA: dw 2		; Where can we find the sector mapping

; Data
BootDevice: db 0	; We're going to reserve a single byte of data to be used to
					; store the ID of the boot device. According to most sources
					; it is standard for the BIOS to leave the boot disk ID in
					; the dl register after boot.
Boot:
    mov [BootDevice], dl	; Save the Boot Device ID into the location above
    
	; Set segment registers to 0
    xor ax, ax	; Clear AX
    cli			; Disable Interrupts
    mov ds, ax	; DS = AX = 0x00
    mov es, ax	; ES = AX = 0x00
    sti			; Enable interrupts
    
; This Reset routine is used to interact with the BIOS 0x13 function for
; resetting the boot device.
Reset:
    xor ax, ax				; Clear AX
    mov dl, [BootDevice]	; Load the BootDevice ID
    int 0x13				; Call the BIOS 0x13 function
    jc Reset				; If the command failed try again.

    mov ah, 0x0E	; BIOS Print TTY
    mov al, 'B'		; Character B for Boot
    xor bx, bx		; Page 0
    int 0x10		; Call the BIOS function 0x10

; Physical hardware doesn't always like reading across multiple tracks so if we
; need to read beyond the sector we must used multiple calls. Thankfully for us
; BootloaderStage2 uses less than the sectors we have given it.
LoadStage2:
    mov     ah, 0x02        	; 0x02 means to read from disk
    mov     al, 14          	; read 16 sector at a time (Maximum as we start
								; reading from 2 sectors in
    mov     bx, Stage2Location	; Set the Write position to the KernelLocation
								; constant
    mov     ch, 0           	; sector 3 is still on track 1
    mov     cl, 4           	; sector to read (second sector)
    mov     dh, 0           	; head number
    mov     dl, [BootDevice]	; drive 0 is floppy drive
    int     0x13            	; read the
    jc Error					; In the event of an error jump to an error
								; handling routine
								;
	; This is used for tracking purposes, it is actually a waste of bytes once
	; we get so far. 
    mov ah, 0x0E	; Print TTY
    mov al, 'L'		; Character L for Loaded
    xor bx, bx		; Page 0
    int 0x10		; Call BIOS function 0x10


    jmp 0x0000:Stage2Location	; Jump to the KernelLocation ensuring that the
								; segments are zeroed.

; Error handling routine
Error:
    pusha			; Save register state
    mov ah, 0x0E	; Print TTY
    mov al, 'E'		; We want to print the letter 'E'
    xor bx, bx		; Page 0
    int 0x10		; Call the BIOS function 0x10
    popa			; Restore registers
    jmp LoadStage2	; Attempt to load the sectors again


; Boot sig
times 510 - ($-$$) db 0	; Fill with zeroes until we get to 2 bytes before the
						; end of the boot sector.
BootSig: dw 0xAA55		; Write the boot sector identifier bytes 0xAA55

; Fill to size of floppy
; times 1474560 - ($-$$) db 0 
