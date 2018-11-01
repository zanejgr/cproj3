all: zformat zfilez zmkdir zrmdir zinspect 

erase:
	$(shell ls -1 | grep -Ev '^(.*\.[hc]|README\.?[[:alnum:]]*|makefile)$$' -|xargs rm -r)
clean: erase all

zformat: 
	gcc zformat.c -o zformat
zinspect: 
	gcc zinspect.c -o zinspect
zfilez: 
	gcc zfilez.c -o zfilez
zmkdir: 
	gcc zmkdir.c -o zmkdir
zrmdir: 
	gcc zrmdir.c -o zrmdir

