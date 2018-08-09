

disabled=-fno-strict-aliasing \
		 -Wno-incompatible-pointer-types-discards-qualifiers \
		 -Wno-parentheses \
		 -Wno-format
all: wirmpht

.SILENT:
wirmpht: *.c
	clang -x c $(disabled) -g wirmpht_main.c -o wirmpht

.PHONY: clean run
clean:
	rm -f wirmpht

