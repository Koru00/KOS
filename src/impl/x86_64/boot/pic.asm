; Initialize the master PIC
mov al, 0x11        ; Send the "start initialization" command
out 0x20, al        ; Send the command to the master PIC

mov al, 0x20        ; The interrupt vector offset for the master PIC
out 0x21, al        ; Send the vector to the master PIC

mov al, 0x04        ; Tell the master PIC that IRQ2 is connected to the slave PIC
out 0x21, al        ; Send this info to the master PIC

mov al, 0x01        ; Set the master PIC to fully nested mode (IR2 triggers the slave)
out 0x21, al        ; Send the mode to the master PIC

; Initialize the slave PIC
mov al, 0x11        ; Send the "start initialization" command
out 0xA0, al        ; Send the command to the slave PIC

mov al, 0x28        ; The interrupt vector offset for the slave PIC (should be offset by 8)
out 0xA1, al        ; Send the vector to the slave PIC

mov al, 0x02        ; Tell the slave PIC that it is connected to IRQ2 on the master
out 0xA1, al        ; Send this info to the slave PIC

mov al, 0x01        ; Set the slave PIC to fully nested mode
out 0xA1, al        ; Send the mode to the slave PIC

; Mask all interrupts by default (disable all IRQs)
mov al, 0xFF        ; Mask all IRQs
out 0x21, al        ; Mask IRQs on the master PIC
out 0xA1, al        ; Mask IRQs on the slave PIC
