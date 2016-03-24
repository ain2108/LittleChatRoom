TGT=./client ./server


all: $(TGT)

$(TGT): stop
	@ $(MAKE) -C $@

.PHONY: stop
stop :;

