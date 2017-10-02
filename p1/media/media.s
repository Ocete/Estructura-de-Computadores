#Variables
.section .data
lista:											#Lista de n numeros a sumar
	.int 1, 2, 3, -1, -2, -3
longlista:									#Número n, cantidad de números a sumar.
	.int (.-lista)/4
media:											#Media
	.int -1
error:											#Error
	.int -1

.section .text
main: .global main

	mov    $lista, %ebx				#dirección de la lista en memoria
	mov longlista, %ecx				#cantidad de elementos a sumar
	call suma									#Llamamos a la función suma

	idiv %ecx									#Realizamos la división concatenando %eax y %edx,dividiendo esto entre el valor introducido
	mov %eax, media						#Y guardando el cociente en %eax y el resto en %ecx.
	mov %edx, error

	mov $1, %eax
	mov $0, %ebx
	int $0x80									#Terminamos la ejecución del programa

suma:
	mov $0, %ebp							#acum1, LSB
	mov $0, %edi							#acum2, MSB
	mov $0, %esi							#Índice de la lista
bucle:
	mov (%ebx,%esi,4), %eax		#Tomamos el siguiente elemento de la lista desde memoria
	cdq												#Ampliamos el valor tomado desde memoria, predeterminado %eax -> %eax (LSB) y %edx (MSB)
	inc %esi									#Incrementamos el índice

	add %eax, %ebp						#Sumamos la parte menos significativa
	adc %edx, %edi						#Sumamos la parte más significativa, añadiendo el acarreo de la orden anterio

	cmp %esi, %ecx						#Comprobamos si hemos terminado de sumar elementos
	jne bucle									#Si aún no hemos terminado, repetimos el bucle

	mov %ebp, %eax						#Cuando retornamos de la función, guardamos los resultados en %eax y %edx.
	mov %edi, %edx



	ret												#Si hemos terminado, volvemos al main
