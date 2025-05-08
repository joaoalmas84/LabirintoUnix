all: jogoUI motor

jogoUI: jogoUI.o utils.o jogoUI_main.o
	gcc jogoUI.o utils.o jogoUI_main.o -o jogoUI -lncurses

motor: motor.o utils.o motor_main.o
	gcc motor.o utils.o motor_main.o -o motor -lpthread
	gcc bot.o -o bot

jogoUI.o: jogoUI.c
	gcc -c jogoUI.c

motor.o: motor.c
	gcc -c motor.c

utils.o: utils.c
	gcc -c utils.c

motor_main.o: motor_main.c
	gcc -c motor_main.c
	gcc -c bot.c

jogoUI_main.o: jogoUI_main.c
	gcc -c jogoUI_main.c

clean:
	rm *.o  jogoUI motor bot
	clear
