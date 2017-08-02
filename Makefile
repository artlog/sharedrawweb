
# set ARTLOG_TOOLBOX
include toolbox.param

CC=gcc
LD=gcc
CPPFLAGS=-g

BUILD=build

libsrcexp=c/fieldreader.c c/drawlineexpander.c c/imareader.c
libsrccomp=c/bitfieldwriter.c c/drawlinecompressor.c  c/imawriter.c

libsrc=c/drawlinecommon.c c/pointlist.c c/sdpoint.c c/sdlines.c c/drawlinetools.c $(libsrcexp) $(libsrccomp)

libraries=alima

# expander main
src=c/main.c
# compressor main
srccomp=c/compressor.c

objects=$(patsubst c/%.c,$(BUILD)/obj/%.o,$(src))
objectscomp=$(patsubst c/%.c,$(BUILD)/obj/%.o,$(srccomp))
libobjects=$(patsubst c/%.c,$(BUILD)/obj/%.o,$(libsrc))

# default target is to build all
all: libs libinclude

libs: $(patsubst %,$(BUILD)/lib/lib%.a,$(libraries))

$(BUILD)/lib/libalima.a: $(libobjects)
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

$(BUILD)/expander: $(BUILD)/lib/libalima.a $(objects) 
	@echo link expander objects $(objects) and libalima
	$(LD) -o $@ $(LDFLAGS) $(objects) -L$(BUILD)/lib -Wl,-Bstatic -lalima -L$(ARTLOG_TOOLBOX)/build/lib -lalcommon -laldev -Wl,-Bdynamic

$(BUILD)/compressor: $(BUILD)/lib/libalima.a $(objectscomp) 
	@echo link compressor objects $(objectscomp) and libalima
	$(LD) -o $@ $(LDFLAGS) $(objectscomp) -L$(BUILD)/lib -Wl,-Bstatic -lalima -L$(ARTLOG_TOOLBOX)/build/lib -lalcommon -laldev -Wl,-Bdynamic

$(BUILD)/svgparser: c/svgparser.c
	gcc `xml2-config --cflags` $^ `xml2-config --libs` -o $@

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
	@$(CC) -Wall -I$(ARTLOG_TOOLBOX)/build/include -c $(CFLAGS) $(CPPFLAGS) $< -o $@

dist/bezier: c/bezier.c c/svgpath.c
	$(CC) -Wall  -I$(ARTLOG_TOOLBOX)/build/include -o $@ $^ -Wl,-Bstatic -L$(ARTLOG_TOOLBOX)/build/lib  -lallist -Wl,-Bdynamic

test_bezier: dist/bezier
	dist/bezier 10000 test_bezier.ima "M 114.28571,532.3622 C 382.94634,912.06137 349.99183,265.85526 488.57143,518.07649 z"

clean:
	rm -rf $(BUILD)

.PHONY:clean test libs libinclude test_bezier all

# needed to keep those files within include after make ( remove unused )
.PRECIOUS: $(BUILD)/include/%.h
