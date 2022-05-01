# Sample of running process as a daemon

## Directory

```
LICENSE --- MIT License
src/
	Makefile
	mywebserver.c --- application lintening to localhost:8080
```

## Usage

```sh
## build
$ cd src
$ make                    # build with glibc daemon(3)
$ make USE_CUSTOM_IMPL=y  # build with custom daemonize routine

## run
$ ./mywebserver

## http service
$ curl http://localhost:8080/

$ telnet localhost 8080
GET /

## check pid and kill
$ make getpid
1234
$ kill -9 1234
```
