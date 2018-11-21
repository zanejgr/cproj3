cc = gcc
cflags = -Wall
dep_objs = vdisk.o oufs_lib.o oufs_goodies.o

all: rebug deps ztouch zcreate zappend zmore zremove zlink zformat zinspect zfilez zmkdir zrmdir	

erase:
	$(shell ls -1 | grep -Ev '^(.*\.[hc]|README\.?[[:alnum:]]*|makefile)$$' -|xargs rm -r)
clean: erase all
	-rm *.o
deps: $(dep_objs)

secret: debug deps ztouch zcreate zappend zmore zremove zlink zmkdir zrmdir zformat zinspect zfilez
debug: 
	$(shell sed -i 's/define debug 0/define debug 1/' *.c *.h)
rebug: 
	$(shell sed -i 's/define debug 1/define debug 0/' *.c *.h)


zformat: deps
	$(cc) zformat.c -o $@ $(dep_objs) $(cflags)
zinspect: deps
	$(cc) zinspect.c -o $@ $(dep_objs) $(cflags)
zfilez: deps
	$(cc) zfilez.c -o $@ $(dep_objs) $(cflags)
zmkdir: deps
	$(cc) zmkdir.c -o $@ $(dep_objs) $(cflags)
zrmdir: deps
	$(cc) zrmdir.c -o $@ $(dep_objs) $(cflags)

ztouch: deps
	$(cc) ztouch.c -o $@ $(dep_objs) $(cflags)
zcreate:deps
	$(cc) zcreate.c -o $@ $(dep_objs) $(cflags)
zappend:deps
	$(cc) zappend.c -o $@ $(dep_objs) $(cflags) 
zmore:	deps
	$(cc) zmore.c -o $@ $(dep_objs) $(cflags) 
zremove:deps
	$(cc) zremove.c -o $@ $(dep_objs) $(cflags) 
zlink:	deps
	$(cc) zlink.c -o $@ $(dep_objs) $(cflags) 


vdisk.o:
	$(cc) -c vdisk.c $(cflags)
oufs_lib.o:
	$(cc) -o $@ -c oufs_lib_support.c $(cflags)
oufs_goodies.o:
	$(cc) -c oufs_goodies.c $(cflags)

