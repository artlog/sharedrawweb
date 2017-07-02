
# set ARTLOG_TOOLBOX
include toolbox.param

CC=gcc
LD=gcc
CPPFLAGS=-g

BUILD=build

# FIXME imareader should not be part of that
libsrc=c/drawlinecommon.c c/drawlineexpander.c c/fieldreader.c c/pointlist.c c/sdpoint.c c/inputstream.c c/sdlines.c c/imareader.c
libsrccomp=c/drawlinecommon.c c/drawlinecompressor.c c/bitfieldwriter.c c/pointlist.c c/sdpoint.c c/outputstream.c c/sdlines.c c/imareader.c c/inputstream.c
src=c/main.c
libraries=alexpander
srccomp=c/compressor.c

objects=$(patsubst c/%.c,$(BUILD)/obj/%.o,$(src))
libobjects=$(patsubst c/%.c,$(BUILD)/obj/%.o,$(libsrc))
objectscomp=$(patsubst c/%.c,$(BUILD)/obj/%.o,$(srccomp))
libobjectscomp=$(patsubst c/%.c,$(BUILD)/obj/%.o,$(libsrccomp))



# default target is to build libraries
libs: $(patsubst %,$(BUILD)/lib/lib%.a,$(libraries))

$(BUILD)/lib/libalexpander.a: $(libobjects)
	ar rccs $@ $^

$(BUILD)/lib/libalcompressor.a: $(libobjectscomp)
	ar rccs $@ $^

libinclude:
	mkdir -p $(BUILD)/include
	cp c/*.h $(BUILD)/include/

$(objects): | $(BUILD)/obj

$(libobjects): | $(BUILD)/lib


test: $(BUILD)/expander $(BUILD)/compressor
	$(BUILD)/expander alphabet/j.imc jchar
	mv jchar generated/j_data.h
	$(BUILD)/compressor flat3.1.ima flattest.imc

$(BUILD)/expander: $(BUILD)/lib/libalexpander.a $(objects) 
	@echo link expander objects $(objects) and libalexpander
	$(LD) -o $@ $(LDFLAGS) $(objects) -L$(BUILD)/lib -Wl,-Bstatic -lalexpander -Wl,-Bdynamic

$(BUILD)/compressor: $(BUILD)/lib/libalcompressor.a $(objectscomp) 
	@echo link compressor objects $(objectscomp) and libalcompressor
	$(LD) -o $@ $(LDFLAGS) $(objectscomp) -L$(BUILD)/lib -Wl,-Bstatic -lalcompressor -Wl,-Bdynamic

$(BUILD)/svgparser: c/svgparser.c
	gcc `xml2-config --cflags` `xml2-config --libs` $^ -o $@

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

dist/bezier: c/bezier.c c/svgpath.c
	$(CC) -Wall  -I$(ARTLOG_TOOLBOX)/build/include -o $@ $^ -Wl,-Bstatic -L$(ARTLOG_TOOLBOX)/build/lib  -lallist -Wl,-Bdynamic

test_bezier: dist/bezier
	dist/bezier 10000 test_bezier.ima "M 114.28571,532.3622 C 382.94634,912.06137 349.99183,265.85526 488.57143,518.07649 z"

clean:
	rm -rf $(BUILD)

.PHONY:clean test libs libinclude test_bezier

# needed to keep those files within include after make ( remove unused )
.PRECIOUS: $(BUILD)/include/%.h
