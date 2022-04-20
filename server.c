// Server side C/C++ program to demonstrate Socket programming
// Source: https://www.geeksforgeeks.org/socket-programming-cc/
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 15635

int parse_ext (char* resource)
{
    if (strlen(resource) == 0)
        return 0; // return index.html if no resource requested
    const char *start_of_ext = strchr(resource, '.');
    const char *end_of_ext = strchr(start_of_ext, '\0');

    /* Get the right amount of memory */
    char ext[end_of_ext - start_of_ext];

    /* Copy the strings into our memory */
    strncpy(ext, start_of_ext,  end_of_ext - start_of_ext);

    /* Null terminators (because strncpy does not provide them) */
    ext[sizeof(ext)] = 0;

    if (strcmp(ext, ".html") == 0)
        return 1;
    else if (strcmp(ext, ".txt") == 0)
        return 2;
    else if (strcmp(ext, ".jpg") == 0)
        return 3;
    else if (strcmp(ext, ".png") == 0)
        return 4;
    else if (strcmp(ext, ".gif") == 0)
        return 5;
    else
        return -1;
}

int main(int argc, char const *argv[]) {    
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 15635
    // Note: https://stackoverflow.com/questions/58599070/socket-programming-setsockopt-protocol-not-available
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 15635
    if (bind(server_fd, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1) {
        new_socket = accept(server_fd, NULL, NULL);
        valread = read(new_socket, buffer, 1024);

        // TODO: make extracting filename a separate function
        const char *start_of_filename = strchr(buffer, ' ') + 1;
        const char *end_of_filename = strchr(start_of_filename + 1, ' ');

        /* Get the right amount of memory */
        char filename[end_of_filename - start_of_filename - 1];

        /* Copy the strings into our memory */
        strncpy(filename, start_of_filename + 1,  end_of_filename - start_of_filename);

        /* Null terminators (because strncpy does not provide them) */
        filename[sizeof(filename)] = 0;

        int ext = parse_ext(filename);
        
        char line[100];
        char response_data[1048576];

        char http_header[1048576];

        FILE *file_data;

        // TODO: refactor to modularize cases
        switch (ext)
        {
            case 0: // redirect to index.html if no path specified
                strcpy(filename, "index.html");
            case 1:
                file_data = fopen(filename, "r");
                while (fgets(line, 100, file_data) != NULL) {
                    strcat(response_data, line);
                }
                strcpy(http_header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 129\r\n\n");
                strcat(http_header, response_data);
                break;
            case 2: 
                file_data = fopen(filename, "r");
                while (fgets(line, 100, file_data) != NULL) {
                    strcat(response_data, line);
                }
                strcpy(http_header, "HTTP/1.1 200 OK\r\nContent-Type: text/txt\r\nContent-Length: 129\r\n\n");
                strcat(http_header, response_data);
                break;
            case 3: 
                file_data = fopen(filename, "rb");
                fread(response_data, 1, 10414, file_data);
                fclose(file_data);
                strcpy(http_header, "HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\nContent-Length: 10414\r\nConnection: keep-alive\r\n\n");
                memcpy(http_header + strlen(http_header), response_data, sizeof(response_data));
                break;
            case 4: 
                file_data = fopen(filename, "rb");
                fread(response_data, 1, 7154, file_data);
                fclose(file_data);
                strcpy(http_header, "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: 7154\r\nConnection: keep-alive\r\n\n");
                memcpy(http_header + strlen(http_header), response_data, sizeof(response_data));
                break;
            case 5: 
                file_data = fopen(filename, "rb");
                fread(response_data, 1, 992595, file_data);
                fclose(file_data);
                strcpy(http_header, "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\nContent-Length: 992595\r\nConnection: keep-alive\r\n\n");
                memcpy(http_header + strlen(http_header), response_data, sizeof(response_data));
                break;
            default:
                strcpy(http_header, "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\n");
        }

        send(new_socket, http_header, sizeof(http_header), 0);
        close(new_socket);
    }
    /*
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    */
    return 0;
}

