.section .rotate
.L0:
	.ascii "%d\000"
.data
.text
	.global main
main:
	# Enter the fun main
	mov ip, sp
	stmfd sp!, {fp, ip, lr, pc}
	sub fp, ip, #4
	# Allocate the stack space
	sub sp, sp, #12
	# Load the args
	# Enter the fun block
	mov r8, #5
	str r8, [fp, #-16]
	mov r8, #0
	str r8, [fp, #-20]
	b .L3
	.L3 :
	b .L5
	.L5 :
	b .L7
	.L7 :
	mov r8, #0
	str r8, [fp, #-24]
	mov r8, #0
	str r8, [fp, #-20]
	.L2 :
	.L1 :
	# Exit the fun main
	mov r0, r8
	ldmea fp, {fp, sp, pc}
