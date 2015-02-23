JAVAC=javac
JAR=jar

all: clean compile ShareDraw.jar

ShareDraw.jar:	
	$(JAR) -cvfe ShareDraw.jar lasnier.sharedraw.ShareDrawServer -C classes/ lasnier/ 

compile:	compile_default
	@echo compilation done

compile_gcj:
	@echo use gcj	
	gcj -C -d classes ./lasnier/sharedraw/*.java

compile_kjc:
	@echo use kjc
	/usr/local/kaffe/bin/kjc -O0 -d classes ./lasnier/sharedraw/*.java

compile_default:
	mkdir -p classes
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/progs/javalibs $(JAVAC) -d classes ./lasnier/sharedraw/*.java

clean:	
	rm -f ./classes/lasnier/sharedraw/*.class
	rm -f ShareDraw.jar


.PHONY: all clean compile
