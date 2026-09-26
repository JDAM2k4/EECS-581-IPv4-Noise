#include <inttypes.h> // standard header file for formatted input/output functions
#include <stdint.h> // standard header file for fixed-width integer types
#include <stdio.h> // standard header file for c files
#include <stdlib.h> // standard utility library for c files
#include <string.h> // standard header file for string manipulation functions

static int is_token_character(char character)
// Check if a character is part of an IPv4 token
{
	return (character >= '0' && character <= '9') ||
		   character == '.' || character == ':';
}

static int parse_number(const char *token, size_t token_length,
						size_t *position, size_t max_digits,
						unsigned int max_value, unsigned int *value)
{
	// Parse a number from the token
	size_t start = *position; // Start position of the number
	unsigned int parsed_value = 0; // Parsed value of the number

	while (*position < token_length && // Check if the current position is within the token length
		   token[*position] >= '0' && token[*position] <= '9') {
		// Check if the current position is within the token length and is a digit
		if (*position - start == max_digits) {
			return 0;
		}
		parsed_value = parsed_value * 10 +
					   (unsigned int)(token[*position] - '0'); // Parse the digit
		if (parsed_value > max_value) { // Check if the parsed value exceeds the maximum allowed value
			return 0; // Return 0 if the parsed value exceeds the maximum allowed value
		}
		(*position)++; // Move to the next character
	}

	if (*position == start ||
		(*position - start > 1 && token[start] == '0')) {
		// Check if the number is valid (no leading zeros unless it's just "0")
		return 0;
	}

	*value = parsed_value; // Store the parsed value
	return 1;
}

static int parse_ipv4_token(const char *token, size_t token_length,
							unsigned int octets[4], int *has_port,
							unsigned int *port) // Parse an IPv4 token and extract its components
{
	size_t position = 0; // Current position in the token
	size_t index; // Index for iterating through the octets

	for (index = 0; index < 4; index++) { // Iterate through the four octets
		if (!parse_number(token, token_length, &position, 3, 255,
						  &octets[index])) { // If parsing the number fails
			return 0; // Return 0 if parsing the number fails
		}
		if (index < 3) { // If not the last octet
			if (position >= token_length || token[position] != '.') { // If the current position is out of bounds or not a dot
				return 0; // Return 0 if the octet is invalid
			}
			position++; // Move to the next character
		}
	}

	*has_port = 0; // Initialize the port flag
	if (position < token_length && token[position] == ':') { // If a colon is found (indicating a port number)
		position++; // Move to the next character
		if (!parse_number(token, token_length, &position, 5, 65535, port)) { // If parsing the port number fails
			return 0; // Return 0 if parsing the port number fails
		}
		*has_port = 1; // Set the port flag
	}

	return position == token_length; // Return 1 if the entire token was parsed successfully, 0 otherwise
}

static void scan_line(const char *line) // Scan a line of input for IPv4 addresses
{
	size_t line_length = strlen(line); // Get the length of the line
	size_t position = 0; // Position in the line

	while (position < line_length) { // Iterate through the line
		size_t token_start; // Start position of the current token
		size_t token_length; // Length of the current token
		unsigned int octets[4]; // Array to store the four octets of the IPv4 address
		unsigned int port = 0; // Port number (if present)
		int has_port; // Flag indicating whether a port number is present

		while (position < line_length && !is_token_character(line[position])) { // Skip non-token characters
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

		if (parse_ipv4_token(line + token_start, token_length, octets,
							 &has_port, &port)) { // If the token is a valid IPv4 address
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
