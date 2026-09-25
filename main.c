#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_token_character(char character)
{
	return (character >= '0' && character <= '9') ||
		   character == '.' || character == ':';
}

static int parse_number(const char *token, size_t token_length,
						size_t *position, size_t max_digits,
						unsigned int max_value, unsigned int *value)
{
	size_t start = *position;
	unsigned int parsed_value = 0;

	while (*position < token_length &&
		   token[*position] >= '0' && token[*position] <= '9') {
		if (*position - start == max_digits) {
			return 0;
		}
		parsed_value = parsed_value * 10 +
					   (unsigned int)(token[*position] - '0');
		if (parsed_value > max_value) {
			return 0;
		}
		(*position)++;
	}

	if (*position == start ||
		(*position - start > 1 && token[start] == '0')) {
		return 0;
	}

	*value = parsed_value;
	return 1;
}

static int parse_ipv4_token(const char *token, size_t token_length,
							unsigned int octets[4], int *has_port,
							unsigned int *port)
{
	size_t position = 0;
	size_t index;

	for (index = 0; index < 4; index++) {
		if (!parse_number(token, token_length, &position, 3, 255,
						  &octets[index])) {
			return 0;
		}
		if (index < 3) {
			if (position >= token_length || token[position] != '.') {
				return 0;
			}
			position++;
		}
	}

	*has_port = 0;
	if (position < token_length && token[position] == ':') {
		position++;
		if (!parse_number(token, token_length, &position, 5, 65535, port)) {
			return 0;
		}
		*has_port = 1;
	}

	return position == token_length;
}

static void scan_line(const char *line)
{
	size_t line_length = strlen(line);
	size_t position = 0;

	while (position < line_length) {
		size_t token_start;
		size_t token_length;
		unsigned int octets[4];
		unsigned int port = 0;
		int has_port;

		while (position < line_length && !is_token_character(line[position])) {
			position++;
		}
		if (position == line_length) {
			break;
		}

		token_start = position;
		while (position < line_length && is_token_character(line[position])) {
			position++;
		}
		token_length = position - token_start;

		if (parse_ipv4_token(line + token_start, token_length, octets,
							 &has_port, &port)) {
			uint32_t decimal_value =
				((uint32_t)octets[0] << 24) |
				((uint32_t)octets[1] << 16) |
				((uint32_t)octets[2] << 8) |
				(uint32_t)octets[3];

			if (has_port) {
				printf("Extracted IPv4 address: %u.%u.%u.%u "
					   "(decimal value: %" PRIu32 ", port: %u)\n",
					   octets[0], octets[1], octets[2], octets[3],
					   decimal_value, port);
			} else {
				printf("Extracted IPv4 address: %u.%u.%u.%u "
					   "(decimal value: %" PRIu32 ", port: none)\n",
					   octets[0], octets[1], octets[2], octets[3],
					   decimal_value);
			}
			return;
		}
	}
}

static char *read_line(void)
{
	size_t length = 0;
	size_t capacity = 128;
	char *line = (char *)malloc(capacity);
	int character;

	if (line == NULL) {
		return NULL;
	}

	while ((character = getchar()) != EOF && character != '\n') {
		if (length + 1 >= capacity) {
			size_t new_capacity = capacity * 2;
			char *resized_line = (char *)realloc(line, new_capacity);
			if (resized_line == NULL) {
				free(line);
				return NULL;
			}
			line = resized_line;
			capacity = new_capacity;
		}
		line[length++] = (char)character;
	}

	if (character == EOF && length == 0) {
		free(line);
		return NULL;
	}

	if (length > 0 && line[length - 1] == '\r') {
		length--;
	}
	line[length] = '\0';
	return line;
}

int main(void)
{
	for (;;) {
		char *line;

		printf("Enter a line of text: ");
		fflush(stdout);
		line = read_line();
		if (line == NULL) {
			break;
		}

		if (strcmp(line, "END") == 0) {
			free(line);
			printf("Program Terminated\n");
			break;
		}

		scan_line(line);
		free(line);
	}

	return 0;
}
