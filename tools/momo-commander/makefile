OBJS = \
	tmp/main.o

all: out/momo-commander

out/momo-commander: $(OBJS)
	gcc -o out/momo-commander $^

tmp/%.o: src/%.cpp
	gcc -g -o $@ -c $<

clean:
	rm -rf tmp/* out/*