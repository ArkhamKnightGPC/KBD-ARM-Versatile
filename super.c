/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

// defines.h file

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
//#define printf kprintf

#define UART0_BASE_ADDR 0x101f1000
#define UART0_DR   (*((volatile u32 *)(UART0_BASE_ADDR + 0x000)))
#define UART0_FR   (*((volatile u32 *)(UART0_BASE_ADDR + 0x018)))
#define UART0_IMSC (*((volatile u32 *)(UART0_BASE_ADDR + 0x038)))

#define UART1_BASE_ADDR 0x101f2000
#define UART1_DR   (*((volatile u32 *)(UART1_BASE_ADDR + 0x000)))
#define UART1_FR   (*((volatile u32 *)(UART1_BASE_ADDR + 0x018)))
#define UART1_IMSC (*((volatile u32 *)(UART1_BASE_ADDR + 0x038)))

#define KBD_BASE_ADDR 0x10006000
#define KBD_CR (*((volatile u32 *)(KBD_BASE_ADDR + 0x000)))
#define KBD_DR (*((volatile u32 *)(KBD_BASE_ADDR + 0x008)))

#define TIMER0_BASE_ADDR 0x101E2000
#define TIMER0_LR (*((volatile u32 *)(UART0_BASE_ADDR + 0x000)))
#define TIMER0_BR (*((volatile u32 *)(UART0_BASE_ADDR + 0x032)))

#define VIC_BASE_ADDR 0x10140000
#define VIC_STATUS    (*((volatile u32 *)(VIC_BASE_ADDR + 0x000)))
#define VIC_INTENABLE (*((volatile u32 *)(VIC_BASE_ADDR + 0x010)))
#define VIC_VADDR     (*((volatile u32 *)(VIC_BASE_ADDR + 0x030)))

#define SIC_BASE_ADDR 0x10003000
#define SIC_STATUS    (*((volatile u32 *)(SIC_BASE_ADDR + 0x000)))
#define SIC_INTENABLE (*((volatile u32 *)(SIC_BASE_ADDR + 0x008)))
#define SIC_ENSET     (*((volatile u32 *)(SIC_BASE_ADDR + 0x008)))
#define SIC_PICENSET  (*((volatile u32 *)(SIC_BASE_ADDR + 0x020)))

#define LINES  4
#define N_SCAN 64

char unsh[N_SCAN] = {
 0,033,'1','2','3','4','5','6',        '7','8','9','0', '-','=','\b','\t',
 'q','w','e','r','t','y','u','i',      'o','p','[',']', '\r', 0,'a','s',
 'd','f','g','h','j','k','l',';',       0,  0,  0,  0,  'z','x','c','v',
 'b','n','m',',','.','/', 0,'*',        0, ' '
};

/* Scan codes to ASCII for shifted keys; unused keys are left out */
char sh[N_SCAN] = {
 0,033,'!','@','#','$','%','^',        '&','*','(',')', '_', '+','\b','\t',
 'Q','W','E','R','T','Y','U','I',      'O','P','{','}', '\r', 0, 'A', 'S',
 'D','F','G','H','J','K','L',':',       0, '~', 0, '|', 'Z', 'X','C', 'V',
 'B','N','M','<','>','?',0,'*',         0, ' '
};

#include <stdio.h>
#include <string.h>

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK 0x0C

u8 cursor;
int volatile *fb;
int row, col, scroll_row;

int fbuf_init()
{
	int i;
	fb = (int *)0x200000; // frame buffer at 2MB-4MB
	/********* for 640x480 VGA mode *******************/
	*(volatile unsigned int *)(0x1000001c) = 0x2C77;
	*(volatile unsigned int *)(0x10120000) = 0x3F1F3F9C;
	*(volatile unsigned int *)(0x10120004) = 0x090B61DF;
	*(volatile unsigned int *)(0x10120008) = 0x067F1800;
	*(volatile unsigned int *)(0x10120010) = 0x200000; // at 2MB
	*(volatile unsigned int *)(0x10120018) = 0x82B;
	/********** for 800X600 SVGA mode ******************
	 *(volatile unsigned int *)(0x1000001c) = 0x2CAC; // 800x600
	*(volatile unsigned int *)(0x10120000) = 0x1313A4C4;
	*(volatile unsigned int *)(0x10120004) = 0x0505F6F7;
	*(volatile unsigned int *)(0x10120008) = 0x071F1800;
	*(volatile unsigned int *)(0x10120010) = 0x200000;
	*(volatile unsigned int *)(0x10120018) = 0x82B;
	**********/
	cursor = 127; // cursor = row 127 in font bitmap
}

typedef volatile struct kbd{
	char *base;
	char buf[128];
	int head, tail, data, room;
}KBD;
volatile KBD kbd;

int kbd_init(){
	KBD *kp = &kbd;
	kp->base = (char *)0x10006000;
	*(kp->base+KCNTL) = 0x14;
	*(kp->base+KCLK) = 8;
	kp->data = 0;
	kp->room = 128;
	kp->head = kp->tail = 0;
}

void kbd_handler(){
	u8 scode, c;
	int i;
	KBD *kp = &kbd;
	scode = *(kp->base+KDATA);
	if (scode & 0x80) //ignorar liberacao de tecla
		return;
	c = unsh[scode];
	//if(c != '\r')
	//	printf("kbd interrupt: c=%x %c\n", c, c);
	kp->buf[kp->head++] = c; //insere no buffer
	kp->head %= 128;
	kp->data++;
	kp->room--; //atualiza contadores data e room
}

int kgetc(){//funcao chamada para recuperar caractere
	char c;
	KBD *kp = &kbd;
	unlock();
	while(kp->data <= 0);
	lock();
	c = kp->buf[kp->tail++]; //pega caractere do buffer
	kp->tail %= 128;
	kp->data--;
	kp->room++;//atualiza contadores data e room
	unlock();
	return c;
}

//int kgets(char s[]){//funcao chamada para recuperar string
//	char c;
//	while((c=kgetc()) != '\r'){
//		*s++ = c;
//	}
//	*s = 0;
//	return strlen(s);
//}

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
 			kbd_handler();
 		}
 	}
}

volatile unsigned int * const UART0DR = (unsigned int *)0x101f1000;

void print_uart0(const char *s){
	while(*s != '\0'){
		*UART0DR = (unsigned int)(*s);
		s++;
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
	kbd_init();	//inicializa driver do teclado
	//printf("Driver do teclado foi inicializado!\n");
	while(1){
		//printf("Escreva uma linha no teclado: ");
		//kgets(line);
		//printf("linha = %s\n", line);
        char c = kgetc();
        print_uart0(c);
	}
}