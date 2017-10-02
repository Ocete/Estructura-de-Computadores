.section .data
lista:
	.int 1, 2, 3
longlista:
	.int (.-lista)/4
resultado:
	.quad -1

.section .text
_start: .global _start

	mov    $lista, %ebx
	mov longlista, %ecx
	call suma

	mov %eax, resultado
	mov %esi, resultado+4

	mov $1, %eax
	mov $0, %ebx
	int $0x80

suma:
	push 	%esi
	mov $0, %eax
	mov $0, %edx
	mov $0, %esi
bucle:
	add (%ebx,%esi,4), %eax
	inc %esi
	adc $0, %edx

	cmp %esi, %ecx
	jne bucle

	pop %esi
	ret
