	.text
	.arm
	.global	reset_handler, vectors_start, vectors_end
	.global lock, unlock
reset_handler:
	LDR	sp, =svc_stack_top	@atribuir stack SVC
	BL	copy_vectors		@chama copy_vectors() em C
	MSR	cpsr, #0x92		@ir para modo IRQ
	LDR	sp, =irq_stack_top	@atribuir stack IRQ
	MSR	cpsr, #0x13		@voltar para modo SVC
	BL	main			@executa programa principal
	B	.
irq_handler:
	SUB	lr, lr, #4
	STMFD	sp!, {r0-r12, lr}
	BL	IRQ_handler		@chama IRQ_handler() em C
	LDMFD	sp!, {r0-r12, pc}^
lock:
	MRS	r0, cpsr
	ORR	r0, r0, #0x80		@desabilita IRQ interrupts
	MSR	cpsr, r0
	MOV	pc, lr
unlock:
	MRS	r0, cpsr
	BIC	r0, r0, #0x80		@habilita IRQ interrupts
	MSR	cpsr, r0
	MOV	pc, lr
vectors_start:
	LDR	pc, reset_handler_addr
	B	.
	B	.
	B	.
	B	.
	B	.
	LDR	pc, irq_handler_addr
	B	.
reset_handler_addr:	.word	reset_handler
irq_handler_addr:	.word	irq_handler
vectors_end:
	B	.
