DIR     = .
BIN     = $(DIR)/client
LIBDIR  = ../../lib
#LDFLAGS = -L$(LIBDIR) -lnscom -lpthread --share

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