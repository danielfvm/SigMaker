// cc SigMaker.c -o sigmaker -ludis86

#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

#include <libudis86/extern.h>
#include <libudis86/types.h>

#define MAX_PATTERN_SIZE 512

#define MINVAL(a, b) ((a)<(b) ? (a) : (b))

#define USAGE "Usage:\n" \
              "  %s <file> <address> <size>\n\n" \
              "  file     Path to executable file\n" \
              "  address  Create signature for selected hex address\n" \
              "  size     Set the size of signature\n"


static int64_t scanPattern(int64_t start, int64_t end, const uint8_t* pattern, const char* mask, int length, int* count) {
	int64_t addr, firstAddr = -1;
	int i;


	for (addr = start; addr < end; ++ addr) {
		for (i = 0; i < length; ++ i) {
			if (mask[i] == '?')
				continue;

			if (pattern[i] != *((uint8_t*)(addr+i))) {
				break;
			}
		}

		if (i != length) {
            continue;
        }
        *count += 1;

        if (firstAddr == -1) {
            firstAddr = addr-start;
        }
	}

	return firstAddr;
}

int main(int argc, char** argv) {
    char* endptr = NULL;

	if (argc != 4) {
		printf(USAGE, argv[0]);
		return 0;
	}

    // Convert address argument to int
    const int selected_address = strtoll(argv[2], &endptr, 16);
    if (endptr != argv[2]+strlen(argv[2])) {
        printf("%s: Invalid hex number for address: %s\n", argv[0], argv[2]);
        return 0;
    }

    // Convert size argument to int
    const int selected_size = strtoll(argv[3], &endptr, 10);
    if (endptr != argv[3]+strlen(argv[3])) {
        printf("%s: Invalid number for size: %s\n", argv[0], argv[3]);
        return 0;
    }

    if (selected_size > MAX_PATTERN_SIZE) {
        printf("%s: Maximum pattern size is %d!\n", argv[0], MAX_PATTERN_SIZE);
        return 0;
    }

    // Open fd to the binary file
    const int fd = open(argv[1], O_RDONLY);

    if (fd == -1) {
        printf("%s: Failed to open file: %s\n", argv[0], argv[1]);
        return 0;
    }

    struct stat s;
    if (fstat(fd, &s) == -1) {
        printf("%s: Failed to read stats\n", argv[0]);
        close(fd);
        return 0;
    }

    const int size = s.st_size;

    void* memory = mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if ((intptr_t)memory == -1) {
        printf("%s: Failed to run mmap\n", argv[0]);
        close(fd);
        return 0;
    }

    uint8_t pattern[MAX_PATTERN_SIZE];
    char mask[MAX_PATTERN_SIZE];
    unsigned int pattern_i = 0;

    memset(mask, 0, MAX_PATTERN_SIZE);

    // Initialize libudis86 decompiler
    ud_t ud;
    ud_init(&ud);
    ud_set_mode(&ud, 64); // 64bit program TODO: Make it optional
    ud_set_syntax(&ud, UD_SYN_INTEL); // Assambly syntax
    ud_set_input_buffer(&ud, memory + selected_address, size - selected_address);

    while (ud_disassemble(&ud)) {
        const uint64_t offset = ud_insn_off(&ud);
        const size_t len = ud_insn_len(&ud);
        const char* code = ud_insn_asm(&ud);
        const ud_operand_t* op;

        printf("%lx:%ld\t%-30s", selected_address + offset, offset, code);

        // Copy instructions into pattern
        memcpy(&pattern[pattern_i], ud_insn_ptr(&ud), len);

        // we start with a mask full of 'x', later when we detect opcodes that
        // are offsets we replace them with '?'.
        memset(&mask[pattern_i], 'x', len);

        for (int i = 0; i < 2; ++ i) {
            if ((op = ud_insn_opr(&ud, i)) == NULL)
                break;

            if (op->type == UD_OP_IMM || op->type == UD_OP_CONST || op->type == UD_OP_REG)
                continue;

            // We only want operations that uses the rip to point to memory/code to be ignored '?'
            // Keep in mind there are also call instructions that do not use RIP but still are
            // relative, therefore we check here only for memory operations.
            if (op->type == UD_OP_MEM && op->base != UD_R_RIP)
                continue;

            // Convert lval to bytes
            char data[8];
            memcpy(data, &op->lval, 8);

            // Search lval in instruction and replace it with '\x00' in pattern and '?' in mask
            for (int j = 0; j < len; ++ j) {
                int op_size = MINVAL(op->size / 8, len - j);
                if (memcmp(&pattern[pattern_i + j], data, op_size) == 0) {
                    memset(&pattern[pattern_i + j], 0, op_size);
                    memset(&mask[pattern_i + j], '?', op_size);
                    break;
                }
            }
        }

        // Print binaries
        for (int i = 0; i < len; ++ i) {
            if (mask[pattern_i + i] == 'x')
                printf("\033[1m");
            printf("%02x ", pattern[pattern_i + i]);
            if (mask[pattern_i + i] == 'x')
                printf("\033[0m");
        }
        pattern_i += len;

        putchar('\n');

        if (pattern_i >= selected_size) {
            break;
        }
    }

    // We don't need '?' at the end of a pattern/mask and therefore we remove it
    int pattern_size = pattern_i;
    while (pattern_size >= 0 && mask[pattern_size-1] == '?') {
        pattern_size --;
    }
    mask[pattern_size] = '\0';

    // Print results
    printf("\nPattern: ");
    for (int i = 0; i < pattern_size; ++ i) {
        printf("\\x%02x", pattern[i]);
    }

    printf("\nMask: %s", mask);
    printf("\nSize: %d\n", pattern_size);

    // Check if we find the pattern
    int count;
    int addr = scanPattern((int64_t)memory, (int64_t)memory + size, pattern, mask, pattern_size, &count);
    printf("\nFirst result: %s", addr == selected_address ? "yes" : "no");
    printf("\nOccurrences: %d\n", count);

    // Cleanup
    munmap(memory, size);
    close(fd);

    return 0;
}
