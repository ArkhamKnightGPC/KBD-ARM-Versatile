#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;

int release;

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10;    // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;       // ARM manual says clock=8
  kp->head = kp->tail = 0;       // circular buffer char buf[128]
  kp->data = 0; kp->room = 128;
  release = 0;
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;

  color = YELLOW;

  scode = *(kp->base + KDATA);   // get scan code of this interrpt

  if (scode == 0xF0){  // ignora liberacao de tecla
    release = 1;
    return;
  }

  if(release == 1){
    release = 0;
    return;
  }

  // map scode to ASCII in lowercase 
  c = ltab[scode];

  kputs("caractere digitado: "); kputc(c); kputs("\n");

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;
}
