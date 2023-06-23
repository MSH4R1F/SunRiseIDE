all: assemble emulate

assemble:
	$(MAKE) -C assembler
	mv assembler/assemble .

emulate:
	$(MAKE) -C emulator
	mv emulator/emulate .

clean:
	$(MAKE) -C assemble clean
	$(MAKE) -C emulate clean
	rm -f assemble emulate

.PHONY: all assemble emulate clean
