CC=arm-linux-gnueabihf-gcc

CFLAGS=-O2
LIBS=-lloragw -lrt -lm -lpthread
LDFLAGS=-I./inc/gw -I./inc -I../common/inc -L./lib

TARGET=main
OBJS= ../common/src/fifo.o ../common/src/ipc.o ../common/src/common.o ../common/src/gpio.o ../common/src/sx1276.o ../common/src/sx1276-board.o ../common/src/sx1276api.o ./src/rx.o ./src/parse_data.o  ./src/base64.o ./src/main.o

$(TARGET): $(OBJS)
	 $(CC) -o $(TARGET) $(OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS)
$(OBJS):%.o:%.c
	 $(CC) -c $(CFLAGS) $(LDFLAGS) $< -o $@ $(LIBS)

clean:
	rm $(TARGET) $(OBJS)
