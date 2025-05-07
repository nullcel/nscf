DIR     = .
BIN     = $(DIR)/server
LIBDIR  = ../../lib

CC      = gcc
CFLAGS  = -g -Wall -I$(LIBDIR)
LDFLAGS = -L$(LIBDIR) -lnscom -lpthread
AR      = ar -cr

SRC     = $(wildcard $(DIR)/*.c)
OBJ     = $(patsubst %.c, %.o, $(SRC))

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(DIR)/%.o: $(DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJ) $(BIN)