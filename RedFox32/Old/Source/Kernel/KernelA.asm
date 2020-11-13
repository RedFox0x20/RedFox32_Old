[BITS 32]
[extern KMain]
section .text
KEntry:
	mov byte [0xb8000], 'A'
	call KMain
	ret
