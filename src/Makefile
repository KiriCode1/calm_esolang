ALL = calm

all: $(ALL)

%: %.c
	$(CC) -g -O3 $< -o $@ 

dump:
	objdump -dS ./calm

clean:
	rm -rf $(ALL) *.o *.dSYM
