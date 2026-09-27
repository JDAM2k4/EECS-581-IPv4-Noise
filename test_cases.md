# Test Cases
This will document the different test cases that the program will be tested with, the expected outputs, and the test string used to test the error.
I've used Gemini to help organize these text cases. You can view the conversation here. https://share.gemini.google/5OSh32m9VKxk

# Test Case 1

Description: Octets should have a maximum value of 255.

Test String: 256.100.0.1

Expected Result: Error: Octet had value higher than 255
# Test Case 2

Description: Port numbers should only range from 0 to 65535 (Port number greater than 65535).

Test String: 192.168.1.1:65536

Expected Result: Error: port number must be less than 65535
# Test Case 3

Description: Port numbers should only range from 0 to 65535 (Port number lower than zero).

Test String: 192.168.1.1:-80

Expected Result: Error: Port number should be positive
# Test Case 4

Description: No leading zeros should be accepted unless the value is exactly zero (Invalid octet leading zero).

Test String: 192.168.01.1

Expected Result: Error: Leading Zeros are not permitted
# Test Case 5

Description: No leading zeros should be accepted unless the value is exactly zero (Invalid port leading zero).

Test String: 192.168.1.1:080

Expected Result: Error: Leading Zeros are not permitted
# Test Case 6

Description: Port numbers should be one to 5 digits. If the port number is not valid, the address is not valid either.

Test String: 192.168.1.1:123456

Expected Result: Error: invalid port number
# Test Case 7

Description: IPv4 octets are made up of numerical values.

Test String: abc.def.ghi.jkl

Expected Result: Error: no valid address found
# Test Case 8

Description: Octets should be positive numbers.

Test String: -10.0.0.1

Expected Result: Error: Octet cannot be negative
# Test Case 9

Description: Extracted valid IPv4 address without a port number.

Test String: 192.168.1.1

Expected Result: Extracted IPv4 address: 192.168.1.1 (decimal value: 3232235777, port: none)
# Test Case 10

Description: Extracted valid IPv4 address with a valid port number.

Test String: 10.0.0.1:8080

Expected Result: Extracted IPv4 address: 10.0.0.1 (decimal value: 167772161, port: 8080)
# Test Case 11

Description: Valid boundary case using the maximum allowed octet values (255.255.255.255) and port number (65535).

Test String: 255.255.255.255:65535

Expected Result: Extracted IPv4 address: 255.255.255.255 (decimal value: 4294967295, port: 65535)
# Test Case 12

Description: Valid single-digit zero octets and zero port number.

Test String: 0.0.0.0:0

Expected Result: Extracted IPv4 address: 0.0.0.0 (decimal value: 0, port: 0)
# Test Case 13

Description: Incomplete IPv4 address missing octets (less than 4 octets).

Test String: 192.168.1

Expected Result: Error: no valid address found
# Test Case 14

Description: IPv4 address containing extra octets (more than 4 octets).

Test String: 1.2.3.4.5

Expected Result: Error: no valid address found
# Test Case 15

Description: IPv4 address with empty octets or trailing dot.

Test String: 192.168.1.1.

Expected Result: Error: no valid address found
# Test Case 16

Description: Colon present for port specification, but no port digits provided.

Test String: 127.0.0.1:

Expected Result: Error: invalid port number
# Test Case 17

Description: Valid IPv4 address embedded within surrounding text.

Test String: Server active at 172.16.254.1:443 status OK

Expected Result: Extracted IPv4 address: 172.16.254.1 (decimal value: 2886794753, port: 443)