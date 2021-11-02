.globl _start
j _start
.align 7
_start: j _reset_entry
.align 2
j mtvec_handler
.align 2
j mtvec_handler
.align 2
j mtvec_handler

_reset_entry:
	li    x5, 123
        li    x1, 0x80000000
	jalr  x0, x1, 0

end_simulation:
        li    x1, 0x60000000
        li    x2, 0x000000FF
        sw    x2, (x1)

mtvec_handler:
	li    x5, 678
	li    x1, 0x80000000
	jalr  x0, x1, 0
	j     mtvec_handler
