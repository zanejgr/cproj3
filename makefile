all: rebug zmkdir zrmdir zformat zinspect zfilez 

erase:
	$(shell ls -1 | grep -Ev '^(.*\.[hc]|README\.?[[:alnum:]]*|makefile)$$' -|xargs rm -r)

clean: erase all

secret: debug zmkdir zrmdir zformat zinspect zfilez


debug: 
	$(shell sed -i 's/define debug 0/define debug 1/' *.c *.h)

rebug: 
	$(shell sed -i 's/define debug 1/define debug 0/' *.c *.h)

zformat: 
	gcc zformat.c oufs_lib_support.c vdisk.c -o zformat
zinspect: 
	gcc zinspect.c oufs_lib_support.c vdisk.c -o zinspect
zfilez: 
	gcc zfilez.c -o zfilez
zmkdir: 
	gcc zmkdir.c oufs_lib_support.c vdisk.c -o zmkdir
zrmdir:
	gcc zmkdir.c oufs_lib_support.c vdisk.c -o zrmdir
