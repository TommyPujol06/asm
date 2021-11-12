int printf(const char* fmt, ...);

#define STACK_SIZE 0x10000 // 64K

enum OPCode {
	PSH = (0x1 << 16), // Push a value onto the stack
	POP = (0x2 << 16), // Remove last value in the stack
	SUM = (0x3 << 16), // Add all values on the stack
	HLT = (0x4 << 16), // Halt the program
	DBG = (0x5 << 16), // Print debug information
};

static int code[] = {
	PSH, 100,
	PSH, 999,
	PSH, 1,
	SUM,
	POP,
	PSH, 0,
	PSH, 9,
	DBG,
	SUM,
	POP,
	HLT,
};

static int stack[STACK_SIZE] = {};
static int cursor = 0;

void
print_stack()
{
	for(int j=0; stack[j]; j++) {
		printf("j: %d\n", stack[j]);
	}
}

int main()
{
	int i = 0;
	while(code[i] != HLT) {
		int op = code[i];
		switch(op) {
			case PSH: {
				if (code[++i]) {
					stack[cursor++] = code[i];
					printf("PSH: %d\n", code[i]);
				}
				break;
			}
			case POP: {
				printf("POP: %d\n", stack[cursor - 1]);
				stack[--cursor] = 0;
				break;
			}
			case SUM: {
				int result = 0;
				for(int j=0; stack[j]; j++) {
					result += stack[j];
					stack[j] = 0; // We consume the stack.
					cursor--;
				}
				stack[cursor++] = result;
				printf("SUM\n");
				break;
			}

			case DBG: print_stack(); break;

			case HLT:
			default:
				return 0;
		}

		i++;
	}

	return 0;
}
