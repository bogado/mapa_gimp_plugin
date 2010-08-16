CFLAGS=`gimp-config --cflags` `gtk-config --cflags` -g2
LDFLAGS=`gimp-config --libs` `gtk-config --libs`

mapheat: mapheat.o

install: mapheat
	gimp-config --install-bin mapheat
	
clean:
	rm *.o mapmake
