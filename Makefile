all: clean compile ShareDraw.jar
	

ShareDraw.jar:	
	/usr/local/kaffe/bin/jar -cvf ShareDraw.jar -C classes/ lasnier/

compile:	compile_kjc
	@echo compilation done

compile_gcj:
	@echo use gcj	
	gcj -C -d classes ./lasnier/sharedraw/*.java

compile_kjc:
	@echo use kjc
	/usr/local/kaffe/bin/kjc -O0 -d classes ./lasnier/sharedraw/*.java

compile_sun:
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/progs/javalibs ${JAVA_HOME}/bin/javac -d classes ./lasnier/sharedraw/*.java
	
clean:	
	rm -f ./classes/lasnier/sharedraw/*.class
	rm -f ShareDraw.jar

