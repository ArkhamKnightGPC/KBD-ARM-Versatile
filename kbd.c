#include "keymap"
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
	if(c != '\r')
		printf("kbd interrupt: c=%x %c\n", c, c);
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

int kgets(char s[]){//funcao chamada para recuperar string
	char c;
	while((c=kgetc()) != '\r'){
		*s++ = c;
	}
	*s = 0;
	return strlen(s);
}
