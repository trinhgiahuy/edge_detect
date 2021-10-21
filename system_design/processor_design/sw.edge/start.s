

.section .text.init

.align 6

.weak stvec_handler
.weak mtvec_handler

.globl _start

_start: 
	j reset_vector

.align 2 

trap_vector: 
	csrr t5, mcause
	li t6, 0x8
	beq t5, t6, write_tohost
	li t6, 0x9
	beq t5, t6, write_tohost
	li t6, 0xb
	beq t5, t6, write_tohost
	la t5, _bad_code
	beqz t5, 1f
	jr t5
1:	
	csrr t5, mcause
	bgez t5, handle_exception
	j other_exception
	
handle_exception: 
other_exception: 
1:
	ori gp, gp, 1337

write_tohost: 
	sw gp, tohost, t5
	j write_tohost

reset_vector: 
	csrr a0, mhartid
1:	
	bnez a0, 1b
	la t0, 1f
	csrw mtvec, t0
	csrwi sptbr, 0

.align 2
1:
	la t0, 1f
	csrw mtvec, t0
	li t0, -1
	csrw pmpaddr0, t0
	li t0, 0x18 | 0x01 | 0x02 | 0x04
	csrw pmpcfg0, t0

.align 2
1:
	la t0, 1f
	csrw mtvec, t0
	csrwi medeleg, 0
	csrwi mideleg, 0
	csrwi mie, 0

.align 2
1:
	li gp, 0
	la t0, trap_vector
	csrw mtvec, t0
	li a0, 1
	slli a0, a0, 31
	bgez a0, 1f
	fence
	li gp, 1
	ecall
1:
	la t0, stvec_handler
	beqz t0, 1f
	csrw stvec, t0
	li t0, (1 << 0xd) | (1 << 0xf) | (1 << 0xc) | (1 << 0x0) | (1 << 0x8) | (1 << 0x3)
	csrw medeleg, t0
	csrr t1, medeleg
	bne t0, t1, other_exception
1:	
	csrwi mstatus, 0
	#  init
	li a0, 0x00006000 & (0x00006000 >> 1)
	csrs mstatus, a0
	csrwi fcsr, 0
	
	la t0, 1f
	csrw mepc, t0
	csrr a0, mhartid
	mret
1:


_entry:
        mv x1, zero
        mv x2, zero
        mv x3, zero
        mv x4, zero
        mv x5, zero
        mv x6, zero
        mv x7, zero
        mv x8, zero
        mv x9, zero
        mv x10, zero
        mv x11, zero
        mv x12, zero
        mv x13, zero
        mv x14, zero
        mv x15, zero
        mv x16, zero
        mv x17, zero
        mv x18, zero
        mv x19, zero
        mv x20, zero
        li sp, 0x80800000

        #=== Enable Floating point unit (why, why is this not the default??)

        # li x11, 0x1111
        # mv x11, zero
        # li x11, 0x1111
        # mv x11, zero
        # li x11, 0x1111

 	# csrrw x11, mstatus, x11
	# li  x10, 0x2000
	# or  x11, x11, x10
        # #csrrw x11, mstatus, x11

        # li x10, 0x10101010
        # li x10, 0x01010101
        # mv x10, zero
        
        # csrrw x10, misa, x10
	# mv x9, x10
        # mv x8, x9
 
	# li x1, 1
	# lw x10, _bad_code
	# mv x11, zero
	# sub x11, x11, x1
	# mv x11, zero
	# sub x11, x11, x1
	# mv x11, zero
	# sub x11, x11, x1
	# mv x11, zero
	# sub x11, x11, x1
	# mv x11, zero
	# sub x11, x11, x1
	# mv x11, zero
	# li x10, 0x333
	# la x11, _bad_code
	# li x10, 0x444
	# la x11, _bad_code
	# li x10, 0x555
	# # li x11, 0x8000015c
	# li x10, 0x666
	# # li x11, 0x1234EEE8
	# csrrw x11, mtvec, x11
	# li x10, 0x1234
	# mv x10, zero

        jal main
        j done

#oops:
#	jal c_oops
#	j done

_bad_code:
	li x10, 0x666
	csrrw x11, mcause, x11
	csrrw x12, mepc, x12
 	csrrw x13, mstatus, x13
	li x14, 0x888
        li x2, 0x100
        li x3, 0x0b
        sll x3, x3, 8
        addi x3, x3, 0xad
        sll x3, x3, 8
        addi x3, x3, 0xc0
        sll x3, x3, 8
        addi x3, x3, 0xde
        sw x3, (x2)
        j _bad_code

mtvec_handler:
	j mtvec_handler

done:
	li x1, 0x60000000
        li x2, 0x000000FF
        sw x2, (x1)
        j done

.data
.pushsection .tohost,"aw",@progbits
.align 6
.global tohost
tohost: 
.dword 0
.align 6
.global fromhost
fromhost: 
.dword 0
.popsection
.align 4
.global begin_signature
begin_signature:
.align 4
.global end_signature
end_signature:

