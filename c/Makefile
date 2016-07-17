expander: pointlist.c sdpoint.c main.c fieldreader.c drawlineexpander.c
	gcc $^ -o $@


test: expander
	./expander ../philippe.imc philippe >generated/philippe.h
	gcc generated/philippe.h

.PHONY: test
