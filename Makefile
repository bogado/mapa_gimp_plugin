CFLAGS=`gimp-config --cflags` `gtk-config --cflags`
LDFLAGS=`gimp-config --libs` `gtk-config --libs`

mapmake: mapmake.o map_gui.o

install: mapmake
	gimp-config --install-bin mapmake
	
clean:
	rm *.o mapmake
