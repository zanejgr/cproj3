all: zmkdir zrmdir zformat zinspect zfilez 

erase:
	$(shell ls -1 | grep -Ev '^(.*\.[hc]|README\.?[[:alnum:]]*|makefile)$$' -|xargs rm -r)
clean: erase all

zformat: 
	gcc zformat.c -o zformat
zinspect: 
	gcc zinspect.c oufs_lib_support.c vdisk.c -o zinspect
zfilez: 
	gcc zfilez.c -o zfilez
zmkdir: 
	gcc zmkdir.c oufs_lib_support.c vdisk.c -o zmkdir
zrmdir:
	gcc zmkdir.c oufs_lib_support.c vdisk.c -o zrmdir
