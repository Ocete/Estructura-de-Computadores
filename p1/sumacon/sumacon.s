#Variables
.section .data
lista:									#Lista de n numeros a sumar
	.int 1, 2, 3
longlista:							#Número n, cantidad de números a sumar.
	.int (.-lista)/4
resultado:							#Resultado final
	.quad -1

.section .text
main: .global main

	mov    $lista, %ebx				#dirección de la lista en memoria
	mov longlista, %ecx				#cantidad de elementos a sumar
	call suma									#Llamamos a la función suma

	mov %eax, resultado				#Guardamos acum1 y acum2 en resultado.
	mov %edx, resultado+4

	mov $1, %eax
	mov $0, %ebx
	int $0x80							#Terminamos la ejecución del programa

suma:
	mov $0, %ebp					#acum1, LSB
	mov $0, %edi					#acum2, MSB
	mov $0, %esi					#Índice de la lista
bucle:
	mov (%ebx,%esi,4), %eax		#Tomamos el siguiente elemento de la lista desde memoria
	cdq								#Ampliamos el valor tomado desde memoria, predeterminado %eax -> %eax (LSB) y %edx (MSB)
	inc %esi						#Incrementamos el índice

	add %eax, %ebp					#Sumamos la parte menos significativa
	adc %edx, %edi					#Sumamos la parte más significativa, añadiendo el acarreo de la orden anterio

	cmp %esi, %ecx					#Comprobamos si hemos terminado de sumar elementos
	jne bucle						#Si aún no hemos terminado, repetimos el bucle

	mov %ebp, %eax					#Cuando retornamos de la función, guardamos los resultados en %eax y %edx.
	mov %edi, %edx

	ret								#Si hemos terminado, volvemos al main
