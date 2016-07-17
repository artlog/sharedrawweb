expander: pointlist.c sdpoint.c main.c fieldreader.c drawlineexpander.c
	gcc $^ -o $@


test: expander
	./expander ../philippe.imc philippe >generated/philippe_data.h
	gcc generated/philippe_data.h

.PHONY: test
