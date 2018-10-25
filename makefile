all: zformat zinspect zfilez zmkdir zrmdir
	gcc project3.c -o project3
erase:
	$(shell ls -1 | grep -Ev '^(.*\.[hc]|README\.?[[:alnum:]]*|makefile)$$' -|xargs rm -r)
clean:erase all

zformat: 
	gcc zformat.c -o zformat
zinspect: 
	gcc zinspect.c -o zinspect individual blocks on the virtual disk
zfilez: 
	gcc zfilez.c -o zfilez
zmkdir: 
	gcc zmkdir.c -o zmkdir
zrmdir: 
	gcc zrmdir.c -o zrmdir

