
default:	build

clean:
	rm -rf Makefile objs

build:
	$(MAKE) -f objs/Makefile

install:
	$(MAKE) -f objs/Makefile install

modules:
	$(MAKE) -f objs/Makefile modules

upgrade:
	run/sbin/nginx -t

	kill -USR2 `cat run/logs/nginx.pid`
	sleep 1
	test -f run/logs/nginx.pid.oldbin

	kill -QUIT `cat run/logs/nginx.pid.oldbin`
