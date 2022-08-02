arm-none-eabi-gcc -c -mcpu=arm926ej-s -g kbd.c -o kbd.o
arm-none-eabi-gcc -c -mcpu=arm926ej-s -g t.c -o t.o
arm-none-eabi-as -c -mcpu=arm926ej-s -g ts.s -o ts.o
arm-none-eabi-ld -T t.ld kbd.o t.o ts.o -o t.elf
arm-none-eabi-objcopy -O binary t.elf t.bin
qemu-system-arm -M versatilepb -m 128M -kernel t.bin -nographic -serial /dev/null
