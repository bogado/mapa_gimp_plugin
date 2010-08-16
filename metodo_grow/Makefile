CFLAGS=`gimptool --cflags`
LDFLAGS=`gimptool --libs`

mapmake: mapmake.o map_gui.o

install: mapmake
	gimptool --install-bin mapmake
	
clean:
	rm *.o mapmake
