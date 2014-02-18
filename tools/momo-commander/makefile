OBJS = \
	tmp/main.o \
	tmp/Shell.o \
	tmp/CommanderShell.o \
	tmp/FTDIShell.o \
	tmp/FTDIManager.o \
	tmp/FTDIConnection.o

all: out/momo-commander

out/momo-commander: $(OBJS)
	g++ -o out/momo-commander $^ -L./lib/D2XX/mac/bin/10.5-10.7/ -lftd2xx.1.2.2

tmp/%.o: src/%.cpp
	g++ -g -o $@ -c $<

clean:
	rm -rf tmp/* out/*