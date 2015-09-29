CC=g++
CFLAGS_SHOT= -W `pkg-config --cflags --libs opencv`  -Iinclude -lswscale -lavdevice -lavformat -lavcodec -lavutil -lswresample -lz -lconfig++ `xml2-config --cflags --libs` -w 


PROGS_shot:= shot-boundary-detector view-shot-boundaries subshot-from-template

all: $(PROGS_shot) 

shot-boundary-detector: src/shot-boundary-detector.cc 
	$(CC) src/shot-boundary-detector.cc -o bin/$@  $(CFLAGS_SHOT)
	src/make-bundle bin/$@  bin/$@.bundle

view-shot-boundaries: src/view-shot-boundaries.cc 
	$(CC) src/view-shot-boundaries.cc -o  bin/$@  $(CFLAGS_SHOT)
	src/make-bundle bin/$@  bin/$@.bundle
	
subshot-from-template: src/subshot-from-template.cc 
	$(CC) src/subshot-from-template.cc -o  bin/$@  $(CFLAGS_SHOT)
	src/make-bundle bin/$@  bin/$@.bundle

clean:
	rm -rf  bin/*

mrproper: clean
	rm -rf  bin/*
