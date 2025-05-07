# controller

all:
	make -C lib/
	make -C server/
	make -C server/

install:
	make install -C lib/

clean:
	make clean -C lib/
	make clean -C client/
	make clean -C server/