DIR     = .
BIN     = $(DIR)/libnscom.so

CC      = gcc
CFLAGS  = -g -fPIC -Wall
LDFLAGS = -shared -lpthread
AR      = ar -cr

SRC     = $(wildcard $(DIR)/*.c)
OBJ     = $(patsubst %.c, %.o, $(SRC))

.PHONY: all clean install

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(DIR)/%.o: $(DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

install:
	cp $(BIN) /usr/lib32/

clean:
	rm -f $(OBJ) $(BIN)