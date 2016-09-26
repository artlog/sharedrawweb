CC=gcc
LD=gcc
CPPFLAGS=-g

BUILD=build

libsrc=c/drawlineexpander.c c/fieldreader.c c/pointlist.c c/sdpoint.c c/inputstream.c c/imareader.c c/sdlines.c
src=c/main.c
libraries=alexpander

objects=$(patsubst c/%.c,$(BUILD)/obj/%.o,$(src))
libobjects=$(patsubst c/%.c,$(BUILD)/obj/%.o,$(libsrc))

# default target is to build libraries
libs: $(patsubst %,$(BUILD)/lib/lib%.a,$(libraries))

$(BUILD)/lib/libalexpander.a: $(libobjects)
	ar rccs $@ $^

$(objects): | $(BUILD)/obj

$(libobjects): | $(BUILD)/lib


test: $(BUILD)/expander
	$(BUILD)/expander alphabet/j.imc jchar
	mv jchar generated/j_data.h

$(BUILD)/expander: $(BUILD)/lib/libalexpander.a $(objects) 
	@echo link expander objects $(objects) and libalexpander
	$(LD) -o $@ $(LDFLAGS) $(objects) -L$(BUILD)/lib -Wl,-Bstatic -lalexpander -Wl,-Bdynamic

$(BUILD)/obj:
	mkdir -p $@

$(BUILD)/lib:
	mkdir -p $@

$(BUILD)/include:
	mkdir -p $@

$(BUILD)/include/%.h: c/%.h $(BUILD)/include
	cp $< $@

$(BUILD)/obj/%.o: c/%.c $(BUILD)/obj
	@echo compile $< 
	@$(CC) -Wall -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(BUILD)

.PHONY:clean test libs

# needed to keep those files within include after make ( remove unused )
.PRECIOUS: $(BUILD)/include/%.h
