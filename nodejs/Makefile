.PHONY: syscalls.list
syscalls.list:
	@echo "#include <asm/unistd_64.h>" | gcc -E -dM - | grep __NR_ | sed 's/#define\s//' | sort -k2n,2 >syscalls.list
