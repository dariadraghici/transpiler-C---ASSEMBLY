#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int loop_deschis, index;
char registru_incrementat = '0';

void eliminare_tab(char *linie) {
	index = 0;
	while (linie[index] == '\t')
		index++;
}

void operatie(char *linie) {
	char registru = linie[index];
	char *egal = strchr(linie, '=');
	if (!egal)
		return;

	static const char *const operatori[] = {"&", "^", "|",  "-", "+",
											"*", "/", ">>", "<<"};
	static const char *const operator[] = {"AND", "XOR", "OR", "SUB",
											"ADD", "MUL", "DIV", "SHR",
											"SHL"};

	for (int i = 0; i < 9; i++) {
		char *semn = strchr(linie, operatori[i][0]);
		if (semn) {
			char operand[20];
			int k = 0, j = index + 8;

			if ((strcmp(operatori[i], "<<") == 0) ||
				(strcmp(operatori[i], ">>") == 0))
				j++;

			while (linie[j] != ';')
				operand[k++] = linie[j++];
			operand[k] = '\0';

			if (operatori[i][0] == '*' || operatori[i][0] == '/') {
				if (operand[0] >= '0' && operand[0] <= '9') {
					if (operand[0] == '0' && operatori[i][0] == '/') {
						printf("Error\n");
						return;
					}
					if (registru != 'a') {
						printf("MOV eax, e%cx\n", registru);
						printf("%s %s\n", operator[i], operand);
						printf("MOV e%cx, eax\n", registru);
					} else
						printf("%s %s\n", operator[i], operand);
				} else {
					printf("MOV eax, e%cx\n", registru);
					printf("%s e%cx\n", operator[i], operand[0]);
					printf("MOV e%cx, eax\n", registru);
				}
			} else {
				if (operand[0] >= '0' && operand[0] <= '9') {
					printf("%s e%cx, %s\n", operator[i], registru, operand);
				} else {
					printf("%s e%cx, e%cx\n", operator[i], registru,
					       operand[0]);
				}
			}
			return;
		}
	}

	if (linie[index + 4] >= 'a' && linie[index + 4] <= 'd') {
		printf("MOV e%cx, e%cx\n", registru, linie[index + 4]);
	} else if (linie[index + 4] >= '0' && linie[index + 4] <= '9') {
		char numar[20];
		int k = 0, i = index + 5;
		numar[k++] = linie[index + 4];
		while (linie[i] != ';')
			numar[k++] = linie[i++];
		numar[k] = '\0';
		printf("MOV e%cx, %s\n", registru, numar);
	}
}

void if_operatie(char *linie) {
	char registru = linie[4];
	char operand[20];
	int k = 0, j = index + 8;
	char *egal = strchr(linie, '=');
	if (egal)
		j++;
	while (linie[j] != ')') {
		operand[k] = linie[j];
		k++;
		j++;
	}
	operand[k] = '\0';
	if (linie[index + 9] >= 'a' && linie[index + 9] <= 'd')
		printf("CMP e%cx, e%cx\n", registru, operand[0]);
	else
		printf("CMP e%cx, %s\n", registru, operand);

	static const char *const operatori[] = {"<=", ">=", "==", "<", ">"};
	static const char *const operator[] = { "JG", "JL", "JNE", "JGE", "JLE" };

	for (int i = 0; i < 5; i++) {
		char *semn = strstr(linie, operatori[i]);
		if (semn) {
			printf("%s end_label\n", operator[i]);
			return;
		}
	}
}

void acolada(void) {
	if (loop_deschis == 2) {
		printf("ADD e%cx, 1\n", registru_incrementat);
		registru_incrementat = '0';
	}
	if (loop_deschis != 0)
		printf("JMP start_loop\n");
	if (loop_deschis != 2)
		printf("end_label:\n");
	else
		printf("end_loop:\n");
	loop_deschis = 0;
}

void while_operatie(char *linie) {
	printf("start_loop:\n");
	loop_deschis = 1;

	char registru = linie[7];
	char operand[20];
	int k = 0, j = index + 11;
	char *egal = strchr(linie, '=');
	if (egal)
		j++;

	while (linie[j] != ')') {
		operand[k] = linie[j];
		k++;
		j++;
	}
	operand[k] = '\0';

	printf("CMP e%cx, %s\n", registru, operand);

	static const char *const operatori[] = {"<=", ">=", "==", "<", ">"};
	static const char *const operator[] = { "JG", "JL", "JNE", "JGE", "JLE" };

	for (int i = 0; i < 5; i++) {
		if (strstr(linie, operatori[i])) {
			printf("%s end_label\n", operator[i]);
			return;
		}
	}
}

void for_operatie(char *linie) {
	char registru = linie[5];
	char operand[20];
	int k = 0, j = index + 9;
	while (linie[j] != ';') {
		operand[k] = linie[j];
		k++;
		j++;
	}
	operand[k] = '\0';
	printf("MOV e%cx, %s\n", registru, operand);
	printf("start_loop:\n");
	loop_deschis = 2;

	if (linie[j + 5] == '=')
		j++;
	j = j + 6;
	k = 0;

	while (linie[j] != ';') {
		operand[k] = linie[j];
		k++;
		j++;
	}
	operand[k] = '\0';
	printf("CMP e%cx, %s\n", registru, operand);
	registru_incrementat = registru;

	static const char *const operatori[] = {"<=", ">=", "==", "<", ">"};
	static const char *const operator[] = { "JG", "JL", "JNE", "JGE", "JLE" };

	for (int i = 0; i < 5; i++) {
		if (strstr(linie, operatori[i])) {
			printf("%s end_label\n", operator[i]);
			return;
		}
	}
}

int main(void) {
	char linie[256];
	while (fgets(linie, sizeof(linie), stdin)) {
		eliminare_tab(linie);
		if (linie[index] >= 'a' && linie[index] <= 'd')
			operatie(linie);
		else if (linie[index + 0] == 'i')
			if_operatie(linie);
		else if (linie[index + 0] == '}')
			acolada();
		else if (linie[0] == 'w')
			while_operatie(linie);
		else if (linie[0] == 'f')
			for_operatie(linie);
	}

	return 0;
}
