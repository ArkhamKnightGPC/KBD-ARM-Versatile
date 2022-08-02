#include "defines.h"
#include "kbd.c"
#include <stdio.h>
#include <string.h>

void copy_vectors(){
	extern u32 vectors_start, vectors_end;
	u32 *vectors_src = &vectors_start;
	u32 *vectors_dst = (u32 *)0;
	while(vectors_src < &vectors_end)
		*vectors_dst++ = *vectors_src++;
}

void IRQ_handler(){
	int vicstatus = VIC_STATUS;	//le status da interrupcao
	int sicstatus = SIC_STATUS;	//le origem da interrupcao
 	if(vicstatus & (1<<31)){
 		if(sicstatus & (1<<3)){
 			kdb_handler();
 		}
 	}
}

int main(){
	int i;
	char line[128];
	fbuf_init();
	VIC_INTENABLE = 0;
	VIC_INTENABLE |= (1<<31);
	SIC_INTENABLE = 0;
	SIC_INTENABLE |= (1<<3);
	kdb_init();	//inicializa driver do teclado
	printf("Driver do teclado foi inicializado!\n");
	while(1){
		printf("Escreva uma linha no teclado: ");
		kgets(line);
		printf("linha = %s\n", line);
	}
}
