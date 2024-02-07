# KBD-ARM-Versatile

Experiment involving keyboard data reception using the ***gcc-arm-none-eabi*** toolchain and the ***qemu-system-arm*** system.

Project for the course PCS 3732 Microprocessor Laboratory 2022 ([pcs.usp.br/~jkinoshi](https://www2.pcs.usp.br/~jkinoshi/2022/labmicro-22.html)), based on the book ***Embedded and Real Time Systems, K.C Wang***. 

After a series of experiments, this project was developed in the final 3 weeks on the course with the goal of allowing the students to give their own personal spin on the course material. The objective is to come up with an experiment that can be used for future classes in order to better fixate the course materials.

In the first three sections of the documents, we implement the basic keyboard driver functionality, we print each character typed into the keyboard.
![Atividade3](/printscreens/Section3.png)

In section 4, we provided final adjustments to the driver. Please use the **mk.sh** script provided in the **code** folder to execute the project locally. The final version of the river can read the whole lines and prints the input only at the end of line.  
![Desafio](/printscreens/Section4.png)

For more details, please refer to the documentation put together for the experiment.
