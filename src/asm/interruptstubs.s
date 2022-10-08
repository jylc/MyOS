.set IRQ_BASE, 0x20
.section .text
.extern __ZN4myos6kernel16InterruptManager15HandleInterruptEhj

.macro HandleException num
.global __ZN4myos6kernel16InterruptManager19HandleException\num\()Ev
 __ZN4myos6kernel16InterruptManager19HandleException\num\()Ev:
	movb $\num,(interrupternumber)
	jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global __ZN4myos6kernel16InterruptManager26HandleInterruptRequest\num\()Ev
__ZN4myos6kernel16InterruptManager26HandleInterruptRequest\num\()Ev:
	movb $\num+IRQ_BASE,(interrupternumber)
	jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x02
HandleInterruptRequest 0x03
HandleInterruptRequest 0x04
HandleInterruptRequest 0x05
HandleInterruptRequest 0x06
HandleInterruptRequest 0x07
HandleInterruptRequest 0x08
HandleInterruptRequest 0x09
HandleInterruptRequest 0x0A
HandleInterruptRequest 0x0B
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x0D
HandleInterruptRequest 0x0E
HandleInterruptRequest 0x0F
HandleInterruptRequest 0x31


HandleException 0x00
HandleException 0x01
HandleException 0x02
HandleException 0x03
HandleException 0x04
HandleException 0x05
HandleException 0x06
HandleException 0x07
HandleException 0x08
HandleException 0x09
HandleException 0x0A
HandleException 0x0B
HandleException 0x0C
HandleException 0x0D
HandleException 0x0E
HandleException 0x0F
HandleException 0x10
HandleException 0x11
HandleException 0x12
HandleException 0x13

int_bottom:
	pusha
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs

	pushl %esp
	push (interrupternumber)
	call __ZN4myos6kernel16InterruptManager15HandleInterruptEhj

	movl %eax,%esp

	popl %gs
	popl %fs
	popl %es
	popl %ds
	popa

.global __ZN4myos6kernel16InterruptManager15InterruptIgnoreEv
__ZN4myos6kernel16InterruptManager15InterruptIgnoreEv:

	iret

.data
	interrupternumber: .byte 0

