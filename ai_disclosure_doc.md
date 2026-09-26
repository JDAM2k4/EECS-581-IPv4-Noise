Date: 
Model Used: 
Prompt: 
Edits: 
Why Edits were made (if any):
Future Changes:

I will use the above template for each prompt given to gen AI. I also intend to push the AI written code to Github, along with the associated prompt, before review.
This is to make it clear what was written by AI and what was changed/corrected by me, but I understand that that might be tedious to look through, so I will also make notes here.

Date: 09.25.26
Model Used: GPT 6 - Luna
Prompt: Write a C or C++ program that reads a line of text and extracts a single valid IPv4 address — optionally followed by a port number — embedded anywhere in that text. Only digits, periods (.), and colons (:) are ever part of a valid token; every other character is garbage and is skipped. A candidate token must match the address grammar in full — no partial matches, no truncating to find a valid piece inside a longer run.

An address is four octets separated by periods (octet.octet.octet.octet), each octet 1–3 digits, value 0–255, no leading zero unless the value is exactly 0. An optional :port may follow the fourth octet: 1–5 digits, value 0–65535, same leading-zero rule. If a colon is present, the port must be fully valid or the entire match — address included — is rejected.

You are not allowed to use any libraries that do any string-to-number conversion functions, any address-parsing library functions, or any regular-expression facilities. This program should reject any inputs that do not follow the format given above and only one input should be validated per line ( everything else in the line is either garbage (skipped) or part of a candidate token that fails validation)

Upon a success, the program should output the following `Extracted IPv4 address: A.B.C.D (decimal value: N, port: P)` where N is the 32-bit decimal value and P is the port number or the literal text none. Main should constantly prompt the user for another input string until the user types a case sensitive `END` which causes the program to print `Program Terminated` before the program closes.

Edits: Began Commenting Code
Why Edits were made (if any): For my understanding
Future Changes: Add different prints for why something was not identified.

