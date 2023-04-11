#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "scan.h"

void scan_table_init(struct scan_table_st *tt)
{
	/* initialize the count to zero, like we did for the map in lab01 */
	tt->head = NULL;
	tt->len = 0;
}

struct scan_token_st *scan_table_new_token(struct scan_table_st *tt) {
	struct scan_token_st *tp = calloc(1, sizeof(struct scan_token_st));
	if (!tt->head) {
		tt->head = tp;
	} else {
		struct scan_token_st *walk = tt->head;
		while (1) {
			if (!walk->next) {
				walk->next = tp;
				break;
			} else {
				walk = walk->next;
			}
		}
	}

	/* increment the length of reserved tokens */
	tt->len += 1;
	return tp;
}

/* scan_read_token is a helper function to generalize populating a token */
char *scan_read_token(struct scan_token_st *tp, char *p,
		      int len, enum scan_token_enum id)
{

	/* fill in the ID of the new token */
	tp->id = id;

	/* fill in the name of the new token for 'len' bytes */
	int i;
	for (i = 0; i < len; i++) {
		tp->name[i] = *p;
		p += 1;
	}
	tp->name[i] = '\0'; /* null-terminate the name string */
	return p;
}

bool scan_is_whitespace(char c)
{
	return c == ' ' || c == '\t';
}

char *scan_whitespace(char *p, char *end)
{
	while (scan_is_whitespace(*p) && p < end) {
		p += 1;
	}
	return p;
}

bool scan_is_digit(char c)
{
	return c >= '0' && c <= '9';
}

char *scan_intlit(struct scan_token_st *tp, char *p, char *end)
{
	tp->id = TK_INTLIT;

	int i;
	for (i = 0; p < end; i++) {
		if (!scan_is_digit(*p))
			break;
		tp->name[i] = *p;
		p++;
	}
	tp->name[i + 1] = '\0';

	return p;
}

char *scan_binlit(struct scan_token_st *tp, char *p, char *end)
{
	tp->id = TK_BINLIT;

	p+=2; /* 0b */
	int i;
	for (i = 0; p < end; i++) {
		if (*p != '0' && *p != '1')
			break;
		tp->name[i] = *p;
		p++;
	}
	tp->name[i + 1] = '\0';

	return p;
}

bool scan_is_alpha(char c)
{
	return c >= 'a' && c <= 'f';
}

char *scan_hexlit(struct scan_token_st *tp, char *p, char *end)
{
	tp->id = TK_HEXLIT;

	p+=2; /* 0x */
	int i;
	for (i = 0; p < end; i++) {
		if (!scan_is_digit(*p) && !scan_is_alpha(*p))
			break;
		tp->name[i] = *p;
		p++;
	}
	tp->name[i + 1] = '\0';

	return p;
}

char *scan_token(struct scan_token_st *tp, char *p, char *end)
{
	if (p == end) {
		p = scan_read_token(tp, p, 0, TK_EOT);
	} else if (scan_is_whitespace(*p)) {
		/* skip over the whitespace */
		p = scan_whitespace(p, end);
		/* recurse to get the next token */
		p = scan_token(tp, p, end);
	} else if (*p == '0') {
		if (*(p + 1) == 'x') {
			p = scan_hexlit(tp, p, end);
		} else if (*(p + 1) == 'b') {
			p = scan_binlit(tp, p, end);
		} else {
			printf("unexpected token: %c\n", *(p + 1));
			exit(-1);
		}
	} else if (scan_is_digit(*p)) {
		p = scan_intlit(tp, p, end);
	} else if (*p == '+') {
		p = scan_read_token(tp, p, 1, TK_PLUS);
	} else if (*p == '-') {
		p = scan_read_token(tp, p, 1, TK_MINUS);
	} else if (*p == '*') {
		p = scan_read_token(tp, p, 1, TK_MULT);
	} else if (*p == '/') {
		p = scan_read_token(tp, p, 1, TK_DIV);
	} else if (*p == '(') {
		p = scan_read_token(tp, p, 1, TK_LPAREN);
	} else if (*p == ')') {
		p = scan_read_token(tp, p, 1, TK_RPAREN);
	} else {
		printf("unexpected token: %c\n", *p);
		exit(-1);
	}
	return p;
}

void scan_table_scan(struct scan_table_st *tt, char *p)
{
	char *end = p + strlen(p);

	/*
	 * loop over the input text, getting one token each time through the
	 * loop
	 */
	while(true) {
		struct scan_token_st *tp = scan_table_new_token(tt);
		p = scan_token(tp, p, end);
		if (tp->id == TK_EOT) {
			break;
		}
	}
}

void scan_table_print(struct scan_table_st *tt)
{
	/* define a variable containing the list of token ID names */
	char *id_names[NUM_TOKENS] = SCAN_STRINGS;

	struct scan_token_st *tp = tt->head;
	while (tp) {
		/* print the ID and name of that token */
		printf("%s(\"%s\")\n", id_names[tp->id], tp->name);
		tp = tp->next;
	}
}

int main(int argc, char **argv)
{
	struct scan_table_st table;

	if (argc == 1) {
		printf("Usage: lab02 <expression>");
		return -1;
	}

	scan_table_init(&table);
	scan_table_scan(&table, argv[1]);
	scan_table_print(&table);

	return 0;
}
