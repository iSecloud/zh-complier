.section .rotate
.L1:
	.ascii "HelloWorld!\000"
.L0:
	.ascii "%d\000"
.data
	.global s
s:
	.word .L1
.text
	.global main
main:
	# Enter the fun main
	mov ip, sp
	stmfd sp!, {fp, ip, lr, pc}
	sub fp, ip, #4
	# Allocate the stack space
	sub sp, sp, #32
	# Load the args
	# Enter the fun block
	mov r8, #1
	str r8, [fp, #-16]
	mov r8, #2
	str r8, [fp, #-20]
	mov r8, #5
	str r8, [fp, #-24]
	mov r8, #5
	ldr r9, =s
	ldr r9, [r9, #0]
	add r8, r8, r9
	str r8, [fp, #-28]
	ldr r8, [fp, #-28]
	ldrb r8, [r8, #0]
	strb r8, [fp, #-36]
	ldrb r8, [fp, #-36]
	strb r8, [fp, #-40]
	mov r8, #3
	str r8, [fp, #-44]
	mov r8, #3
	b .L2
	.L2 :
	# Exit the fun main
	mov r0, r8
	ldmea fp, {fp, sp, pc}
