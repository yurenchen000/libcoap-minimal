LIBCOAP?=libcoap-2-openssl

pkgconfig=$(shell pkg-config $(1) $(2))
#CPPFLAGS=-Wall -Wextra $(call pkgconfig,--cflags,$(LIBCOAP))
#CPPFLAGS=-Wall -Wextra -I /usr/local/include -std=gnu++0x
#CPPFLAGS=-Wall -Wextra -I /usr/local/include -std=c++0x
CPPFLAGS=-Wall -Wextra -I /usr/local/include
#LDLIBS=$(call pkgconfig,--libs,$(LIBCOAP))
LDLIBS= -lcoap-2

LINK.o=$(LINK.cc)

all: client server

client:: client.o common.o

server:: server.o common.o

clean:
	rm -f *.o client server
