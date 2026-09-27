# Test Cases
This will document the different test cases that the program will be tested with, the expected outputs, and the test string used to test the error.

#1 Octets should have a maximum value of 255  
    If an octet's value is above 255, the program should print the error `Error: Octet had value higher than 255`  

#2 Port numbers should only range from 0 to 65535.  
    If the port number is lower than zero the error should read `Error: Port number should be positive`. If the port number is greater than 65535 the error should read `Error: port number must be less than 65535`  

#3 No leading zeros should be accepted *unless* the value is exactly zero. This leading zero rule should also apply to port numbers.  
    If leading zeros exist without the value being zero the error should read `Error: Leading Zeros are not permitted`  

#4 Port numbers should be one to 5 digits. If the port number is not valid, the address is not valid either.  
    Error should read `Error: invalid port number`  

#5 IPv4 octets are made up of numerical values.  
    If given a string with only alphabetical or special characters the program should return the statement `Error: no valid address found`  

#6 Octets should be positive numbers.  
    If given a negative octet the error should be `Error: Octet cannot be negative`
