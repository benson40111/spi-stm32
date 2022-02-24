# for NXP
CC = aarch64-linux-gnu-gcc

# for raspberry
#CC = arm-linux-gnueabihf-gcc

APP_NAME = spi-stm32

all: clean $(APP_NAME).o spi.o
	$(CC) -o $(APP_NAME) $(APP_NAME).o spi.o

$(APP_NAME).o:
	$(CC) -c $(APP_NAME).c

spi.o:
	$(CC) -c spi.c

clean:
	rm -rf *.o $(APP_NAME)

