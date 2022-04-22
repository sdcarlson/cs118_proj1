// Server side C/C++ program to demonstrate Socket programming
// Source: https://www.geeksforgeeks.org/socket-programming-cc/
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 15635

int parse_ext (char* ext)
{
    if (strcmp(ext, "html") == 0)
        return 1;
    else if (strcmp(ext, "txt") == 0)
        return 2;
    else if (strcmp(ext, "jpg") == 0)
        return 3;
    else if (strcmp(ext, "png") == 0)
        return 4;
    else if (strcmp(ext, "gif") == 0)
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

    new_socket = accept(server_fd, NULL, NULL);
    valread = read(new_socket, buffer, 1024);

    // TODO: make extracting filename a separate function
    char *start_of_file = strchr(buffer, '/') + 1;
    char *end_of_file = strchr(start_of_file, '/') - 5;
    
    int file_length = end_of_file - start_of_file;
    if (file_length < 1)
        file_length = 1;
    char file[file_length];
    strncpy(file, start_of_file, end_of_file - start_of_file); 
    file[sizeof(file) == 1 ? 0 : sizeof(file)] = 0; 

    char processed_file[file_length];
    int j = 0;
    for (int i = 0; i < file_length;) 
    {
        if (i < file_length - 2 && file[i] == '%' && file[i + 1] == '2' && file[i + 2] == '0')
        {
            processed_file[j] = ' ';
            j++;
            i += 3;
        }
        else
        {
            processed_file[j] = file[i];
            i++;
            j++;
        }
    }

    processed_file[j] = 0;

    char *end_of_filename = strchr(processed_file, '.');
    char *start_of_ext;
    char *end_of_ext; 

    if (end_of_filename == NULL)
    {
        start_of_ext = end_of_filename = processed_file + strlen(processed_file);
        end_of_ext = start_of_ext;
    }
    else
    {
        start_of_ext = strchr(end_of_filename, '.') + 1;
        if (start_of_ext == NULL)
        {
            start_of_ext = file + strlen(processed_file);
            end_of_ext = start_of_ext;
        }
        else
            end_of_ext = end_of_file;
    }
    
    // Get the right amount of memory

    int filename_length = end_of_filename - processed_file;
    if (filename_length < 1)
        filename_length = 1;
    
    int ext_length = end_of_ext - start_of_ext;
    if (ext_length < 1)
        ext_length = 1;
    char filename[filename_length];
    char ext[ext_length];
    
    //Copy the strings into our memory
    strncpy(filename, processed_file, end_of_filename - processed_file);
    strncpy(ext, start_of_ext, end_of_ext - start_of_ext);
    
    // Null terminators (because strncpy does not provide them)
    filename[sizeof(filename) == 1 ? 0 : sizeof(filename)] = 0;
    ext[sizeof(ext) == 1 ? 0 : sizeof(ext)] = 0;
    
    int ext_type = parse_ext(ext);
    
    char line[100];
    char response_data[1048576];

    char http_header[1048576];

    FILE *file_data;

    int nbytes;

    char file_type[15];

    char fopen_mode[3];

    char default_ext[13] = {'\0'}; // assigning more bytes for string "octal-stream" since ext might not be big enough
    
    // TODO: refactor to modularize cases
    switch (ext_type)
    {
        case 2: 
            strcpy(ext, "plain");
        case 1:
            strcpy(file_type, "text");
            strcpy(fopen_mode, "r");
            break; 
        case 3: 
        case 4: 
        case 5:
            strcpy(file_type, "image");
            strcpy(fopen_mode, "rb");
            break; 
        default: 
            strcpy(file_type, "application");
            strcpy(default_ext, "octet-stream");
            strcpy(fopen_mode, "rb"); 
    } 
        
        file_data = fopen(processed_file, fopen_mode);
        nbytes = fread(response_data, 1, sizeof(response_data), file_data);
        fclose(file_data); 
        sprintf(http_header, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: %s/%s\r\n\n", nbytes, file_type, strlen(default_ext) == 0 ? ext : default_ext);
        memcpy(http_header + strlen(http_header), response_data, sizeof(response_data));
        send(new_socket, http_header, sizeof(http_header), 0);
        close(new_socket); 
    
    return 0;
}

