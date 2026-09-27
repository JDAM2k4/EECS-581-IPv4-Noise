#include <inttypes.h> // standard header file for formatted input/output functions
#include <stdint.h> // standard header file for fixed-width integer types
#include <stdio.h> // standard header file for c files
#include <stdlib.h> // standard utility library for c files
#include <string.h> // standard header file for string manipulation functions

static int is_token_character(char character)
// Check if a character is part of an IPv4 token
{
	return (character >= '0' && character <= '9') ||
		   (character >= 'A' && character <= 'Z') ||
		   (character >= 'a' && character <= 'z') ||
		   character == '.' || character == ':' || character == '-';
}

static int is_token_start_character(char character)
{
	return (character >= '0' && character <= '9') ||
		   character == '.' || character == ':' || character == '-';
}

typedef enum {
	PARSE_NO_MATCH,
	PARSE_OK,
	PARSE_OCTET_TOO_HIGH,
	PARSE_PORT_TOO_HIGH,
	PARSE_LEADING_ZERO,
	PARSE_INVALID_PORT,
	PARSE_NEGATIVE_OCTET,
	PARSE_NEGATIVE_PORT
} ParseResult;

static ParseResult parse_number(const char *token, size_t token_length,
								size_t *position, size_t max_digits,
								unsigned int max_value, unsigned int *value,
								int is_port)
{
	// Parse a number from the token
	size_t start = *position; // Start position of the number
	unsigned int parsed_value = 0; // Parsed value of the number
	size_t digit_count = 0;

	if (*position < token_length && token[*position] == '-') {
		return is_port ? PARSE_NEGATIVE_PORT : PARSE_NEGATIVE_OCTET;
	}

	while (*position < token_length && // Check if the current position is within the token length
		   token[*position] >= '0' && token[*position] <= '9') {
		digit_count++;
		if (parsed_value <= max_value) {
			parsed_value = parsed_value * 10 +
						   (unsigned int)(token[*position] - '0');
			if (parsed_value > max_value) {
				parsed_value = max_value + 1;
			}
		}
		(*position)++; // Move to the next character
	}

	if (*position == start) {
		return is_port ? PARSE_INVALID_PORT : PARSE_NO_MATCH;
	}
	if (is_port && digit_count > max_digits) {
		return PARSE_INVALID_PORT;
	}
	if (digit_count > 1 && token[start] == '0') {
		return PARSE_LEADING_ZERO;
	}
	if (parsed_value > max_value) {
		return is_port ? PARSE_PORT_TOO_HIGH : PARSE_OCTET_TOO_HIGH;
	}

	*value = parsed_value; // Store the parsed value
	return PARSE_OK;
}

static ParseResult parse_ipv4_token(const char *token, size_t token_length,
									unsigned int octets[4], int *has_port,
									unsigned int *port) // Parse an IPv4 token and extract its components
{
	size_t position = 0; // Current position in the token
	size_t index; // Index for iterating through the octets

	for (index = 0; index < 4; index++) { // Iterate through the four octets
		ParseResult result = parse_number(token, token_length, &position, 3, 255,
										  &octets[index], 0);
		if (result != PARSE_OK) {
			return result;
		}
		if (index < 3) { // If not the last octet
			if (position >= token_length || token[position] != '.') { // If the current position is out of bounds or not a dot
				return PARSE_NO_MATCH;
			}
			position++; // Move to the next character
		}
	}

	*has_port = 0; // Initialize the port flag
	if (position < token_length && token[position] == ':') { // If a colon is found (indicating a port number)
		position++; // Move to the next character
		ParseResult result = parse_number(token, token_length, &position, 5, 65535,
										  port, 1);
		if (result != PARSE_OK) {
			return result;
		}
		*has_port = 1; // Set the port flag
	}

	if (position != token_length) {
		return *has_port ? PARSE_INVALID_PORT : PARSE_NO_MATCH;
	}
	return PARSE_OK;
}

static const char *parse_error_message(ParseResult result)
{
	switch (result) {
	case PARSE_OCTET_TOO_HIGH:
		return "Error: Octet had value higher than 255";
	case PARSE_PORT_TOO_HIGH:
		return "Error: port number must be less than 65535";
	case PARSE_LEADING_ZERO:
		return "Error: Leading Zeros are not permitted";
	case PARSE_INVALID_PORT:
		return "Error: invalid port number";
	case PARSE_NEGATIVE_OCTET:
		return "Error: Octet cannot be negative";
	case PARSE_NEGATIVE_PORT:
		return "Error: Port number should be positive";
	default:
		return "Error: no valid address found";
	}
}

static void scan_line(const char *line) // Scan a line of input for IPv4 addresses
{
	size_t line_length = strlen(line); // Get the length of the line
	size_t position = 0; // Position in the line
	int reported_error = 0;

	while (position < line_length) { // Iterate through the line
		size_t token_start; // Start position of the current token
		size_t token_length; // Length of the current token
		unsigned int octets[4]; // Array to store the four octets of the IPv4 address
		unsigned int port = 0; // Port number (if present)
		int has_port; // Flag indicating whether a port number is present
		ParseResult result;

		while (position < line_length && !is_token_start_character(line[position])) { // Skip non-token characters
			position++; // Move to the next character
		}
		if (position == line_length) { // If the end of the line is reached
			break; // Break if the end of the line is reached
		}

		token_start = position; // Mark the start position of the current token
		while (position < line_length && is_token_character(line[position])) { // Read the token characters
			position++; // Move to the next character
		}
		token_length = position - token_start; // Calculate the length of the current token

		result = parse_ipv4_token(line + token_start, token_length, octets,
							   &has_port, &port);
		if (result == PARSE_OK) {
			uint32_t decimal_value = 
				((uint32_t)octets[0] << 24) |
				((uint32_t)octets[1] << 16) |
				((uint32_t)octets[2] << 8) |
				(uint32_t)octets[3]; // Calculate the decimal value of the IPv4 address

			if (has_port) { // If a port number is present
				printf("Extracted IPv4 address: %u.%u.%u.%u " 
					   "(decimal value: %" PRIu32 ", port: %u)\n",
					   octets[0], octets[1], octets[2], octets[3],
					   decimal_value, port); // Print the extracted IPv4 address with port
			} else { // If no port number is present
				printf("Extracted IPv4 address: %u.%u.%u.%u "
					   "(decimal value: %" PRIu32 ", port: none)\n",
					   octets[0], octets[1], octets[2], octets[3],
					   decimal_value); // Print the extracted IPv4 address without port
			}
			return;
		}
		if (result != PARSE_NO_MATCH) {
			size_t dot_count = 0;
			size_t token_index;
			for (token_index = 0; token_index < token_length; token_index++) {
				if (line[token_start + token_index] == '.') {
					dot_count++;
				}
			}
			if (dot_count >= 3) {
				printf("%s\n", parse_error_message(result));
				reported_error = 1;
			}
		}
	}
	if (!reported_error) {
		printf("%s\n", parse_error_message(PARSE_NO_MATCH));
	}
}

static char *read_line(void) // Read a line of input from the user
{
	size_t length = 0; // Length of the line
	size_t capacity = 128; // Initial capacity of the line buffer
	char *line = (char *)malloc(capacity); // Allocate memory for the line buffer
	int character; // Character read from input

	if (line == NULL) { 
		return NULL; // Return NULL if memory allocation fails
	}

	while ((character = getchar()) != EOF && character != '\n') { // Read characters until EOF or newline
		if (length + 1 >= capacity) { // If the line buffer is full
			size_t new_capacity = capacity * 2; // Double the capacity
			char *resized_line = (char *)realloc(line, new_capacity); // Reallocate memory for the line buffer
			if (resized_line == NULL) { // If memory reallocation fails
				free(line); // Free the memory allocated for the line buffer
				return NULL; // Return NULL if memory reallocation fails
			}
			line = resized_line; // Update the line pointer to the resized buffer
			capacity = new_capacity; // Update the capacity
		}
		line[length++] = (char)character; // Store the character in the line buffer
	}

	if (character == EOF && length == 0) { // If EOF is encountered and no characters are read
		free(line); // Free the memory allocated for the line buffer
		return NULL; // Return NULL if no characters are read
	}

	if (length > 0 && line[length - 1] == '\r') { // If the last character is a carriage return
		length--; // Remove the carriage return character
	}
	line[length] = '\0'; // Null-terminate the line
	return line; // Return the pointer to the line buffer
}

int main(void) // Main function
{
	for (;;) { // Infinite loop until terminated
		char *line;

		printf("Enter a line of text: "); // get input from user
		fflush(stdout); // Flush the output buffer
		line = read_line(); // Read a line of input from the user
		if (line == NULL) { // If no input is received
			break;
		}

		if (strcmp(line, "END") == 0) { // If the user enters "END", end the program
			free(line);
			printf("Program Terminated\n");
			break;
		}

		scan_line(line); // Scan the line for IPv4 addresses
		free(line); // Free the memory allocated for the line
	}

	return 0;
}
