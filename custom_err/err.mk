GITTOPLEVEL:=$(shell git rev-parse --show-toplevel)
CFLAGS:=$(CFLAGS) -I$(GITTOPLEVEL)/custom_err/include

LDFLAGS:=$(LDFLAGS) -L$(GITTOPLEVEL)/custom_err
LDLIBS:=$(LDLIBS) -lcustom_err

