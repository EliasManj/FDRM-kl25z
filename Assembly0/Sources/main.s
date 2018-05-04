/* This assembly file uses GNU syntax */
	.text
	.section	.rodata
	.align	2
	
	
.LC0:
	
	.text
	.align	2
	.global	main
	.type main function
	M: .int 
	
main:
	push {r3, lr}
	add	r3, sp, #4
	
	@Set bit 0 in R0
	LDR R0, =0x00110000		@R0 = 0x00110000		 
	MOV R2, #1				@R2 = 1
	ORR R0, R0, R2			@R0 = R0|R1
	
	@Shift right R0 by 2
	LSR R0, R0, #2			@R0 = R0>>2
	
	@Shift left R0 by 2
	LSL R0, R0, #2			@R0 =  R0<<2
	
	@Add 10 to a 32-bit 
	LDR R0, =0x00000001		@R0 = 1
	ADD R0, R0, #10			@R0 = R0 + 10
	
	@If statement (if R0 > 7) R0 = 0 else R0 = 9
	LDR R0, =9				@R0 = 8
	CMP R0, #7				@Do R0 - 7
	BLE else				@is R0 > 7? (ALU less or equal to zero flag) if not, go to else tag
	MOV R0, #0				@if true, R0 = 0
	

	@While loop structure

loop:
	LDR R0, =50
	LDR R1, =45
	CMP R0, R1				@R0 - R1
	BLS next				@if (R0 < R1) go to next
	BL body					@go to body of while loop
	B loop

body:
	SUB R0, #1				@R0--
	
next:
	B exit

else:
	MOV R0, #9				@R0 = 9
	
exit:
	B exit					@Infinite loop
	
	mov	r3, #0
	mov	r0, r3
	pop {r3, pc}
	
	.align	2
.L3:
	.word	.LC0
	.end
