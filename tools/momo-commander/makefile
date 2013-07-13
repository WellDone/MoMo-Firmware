OBJS = \
	tmp/main.o

all: out/momo-commander

out/momo-commander: $(OBJS)
	gcc -o out/momo-commander $^ -L./lib/D2XX/mac/bin/10.5-10.7/ -lftd2xx.1.2.2

tmp/%.o: src/%.cpp
	gcc -g -o $@ -c $<

clean:
	rm -rf tmp/* out/*