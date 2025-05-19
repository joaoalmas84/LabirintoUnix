all: bot jogoUI motor

jogoUI: ObjectFiles/jogoUI.o ObjectFiles/jogoUI_main.o ObjectFiles/utils.o
	gcc ObjectFiles/jogoUI.o ObjectFiles/jogoUI_main.o ObjectFiles/utils.o -o jogoUI -lncurses

bot: ObjectFiles/bot.o ObjectFiles/motor_main.o ObjectFiles/utils.o
	gcc ObjectFiles/bot.o -o bot

motor: 	ObjectFiles/motor.o ObjectFiles/motor_main.o ObjectFiles/utils.o
	gcc ObjectFiles/motor.o ObjectFiles/motor_main.o ObjectFiles/utils.o -o motor -lpthread
	gcc ObjectFiles/bot.o -o bot

ObjectFiles/jogoUI.o: JogoUI/jogoUI.c
	gcc -c JogoUI/jogoUI.c -o ObjectFiles/jogoUI.o

ObjectFiles/motor.o: Motor/motor.c
	gcc -c Motor/motor.c -o ObjectFiles/motor.o

ObjectFiles/utils.o: Util/utils.c
	gcc -c Util/utils.c -o ObjectFiles/utils.o

ObjectFiles/motor_main.o: Motor/motor_main.c
	gcc -c Motor/motor_main.c -o ObjectFiles/motor_main.o

ObjectFiles/jogoUI_main.o: JogoUI/jogoUI_main.c
	gcc -c JogoUI/jogoUI_main.c -o ObjectFiles/jogoUI_main.o

ObjectFiles/bot.o: Bot/bot.c
	gcc -c Bot/bot.c -o ObjectFiles/bot.o

clean:
	rm ObjectFiles/*.o jogoUI motor bot
	clear
