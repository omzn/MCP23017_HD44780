DESTDIR	=	/usr
PREFIX	=	/local
CC	=	gcc
SRC	=	lcdecho.c
OBJ	=	lcdecho
LIBS	=	-lwiringPi -lwiringPiDev

all:		lcdecho
lcdecho:	$(SRC)
		@echo [make lcdecho]
		@$(CC) -o $(OBJ) $(SRC) $(LIBS)
clean:
	@echo "[Clean]"
	@rm -f $(OBJ) *~ core *.bak

install:
	@echo "[Install]"
	@sudo cp $(OBJ)		$(DESTDIR)$(PREFIX)/bin
	@sudo chmod 4755 	$(DESTDIR)$(PREFIX)/bin/$(OBJ)
