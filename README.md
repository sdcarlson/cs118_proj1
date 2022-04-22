# CS118 Project 1

This is the repo for spring 2022 cs118 project 1.

## Group Members

Evan He evanhe27@gmail.com 705-619-333
Seth Carlson scarlson896@gmail.com 605-600-334

## Makefile

This provides a couple make targets for things.
By default (all target), it makes the `server` executables.

It provides a `clean` target, and `tarball` target to create the submission file as well.

You will need to modify the `Makefile` USERID to add your userid for the `.tar.gz` turn-in at the top of the file.

## Academic Integrity Note

You are encouraged to host your code in private repositories on [GitHub](https://github.com/), [GitLab](https://gitlab.com), or other places.  At the same time, you are PROHIBITED to make your code for the class project public during the class or any time after the class.  If you do so, you will be violating academic honestly policy that you have signed, as well as the student code of conduct and be subject to serious sanctions.

## Provided Files

`server.c` is the entry points for the server part of the project.

### High Level Design Overview

The server's design largely follows the template outlined in the demo from class during discussion 1C in Week 1. The steps for setting up the server socket, attaching the socket to a port, accepting a client socket conection, and reading data from the client socket remain unchanged. The only differences occur in how the data from the client socket is processed and how the server generates a response. First, the requested file name and extension are extracted from the request header and processed to convert `%20` (the URL encoding for a space) back into a space. Next, the file extension is examined to see if it matches one of the supported formats (`.html`, `.txt`, `.jpg`, `.png`, and `.gif`). This result is used to set the `Content-Type` field in the response. Then, the requested file is read and its contents are appended to the end of the response header. Finally, the entire response is transmitted back to the client.

### Difficulties Encountered

The first difficulty with this project was extracting the filename and extension. We struggled to come of with delimiters that would work for all cases that we were required to handle. The ultimate solution was to use the `strchr` function to identify the beginning of the file identifier after the first character following the first `\` symbol, and the end of the identifier 5 characters before the next `\` symbol (since this symbol was guaranteed to correspond with the `\` in `HTTP/1.1`). This string was copied into a separate buffer, and the filename was separated from the file extension using the `.` symbol as the delimiter.

The next difficulty encountered was converting `%20` back into spaces. An in-place approach was proving challenging due to the difference in length between the input string containing `%20` and the desired output string containing spaces. The solution was to copy the input string into a new buffer character-by-character and simply replace `%20` with a space whenever it appeared during the transcription.

The final difficulty was actually reading data from the requested file. The initial approach for using the `fgets` function failed for non-text output because the function automatically appended null bytes after newlines. The solution was to use the `fread` function to simply read the entire file at once. 

### Resources Consulted

Discussion 1C Week 1 TA Slides
https://stackoverflow.com/questions/41286260/parse-http-request-line-in-c
https://dev-notes.eu/2018/06/http-server-in-c/
https://www.geeksforgeeks.org/socket-programming-cc/
