MK_TAG = ast_test
OBJSDIR =./objs

CFLAGS +=\
	 -DLINUX_OS -DIO_DEBUG=0 \
	 -o $(MK_TAG) *.c \

all : 
	-mkdir $(OBJSDIR)
	gcc $(CFLAGS)
	mv $(MK_TAG) $(OBJSDIR)


clean:
	rm -f $(MK_TAG) *.o *~
