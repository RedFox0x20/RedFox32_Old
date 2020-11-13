; THIS IS BAD... This should be generated but I don't want to have to write a
; program to do this for me just yet

times 5 db 0xFF
EmptyFiller: times 355 db 0
UsedSectors: dw 0
FreeSectors: dw 2880
