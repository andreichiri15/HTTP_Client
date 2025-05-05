#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson/parson.h"

#define HOST "34.246.184.49"
#define PORT 8080

#define COMMAND_MAX 100

int check_if_string_contains_numbers(char *string) {
    for (int i = 0; i < strlen(string); i++) {
        if (string[i] >= '0' && string[i] <= '9') {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    char *jwt = (char *)calloc(2048, sizeof(char));
    char *cookies = (char *)calloc(2048, sizeof(char));
    int logged_in = 0;
    int enter_library = 0;

    while (1) {
        char command[COMMAND_MAX];
        scanf("%s", command);

        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "register") == 0) {
            char username[100];
            char password[100];

            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}

            printf("username=");
            fgets(username, 100, stdin);
            username[strcspn(username, "\n")] = 0;

            printf("password=");
            fgets(password, 100, stdin);
            password[strcspn(password, "\n")] = 0;

            if (strchr(username, ' ') != NULL) {
                printf("ERROR: Username cannot contain spaces\n");
                continue;
            }

            if (strchr(password, ' ') != NULL) {
                printf("ERROR: Password cannot contain spaces\n");
                continue;
            }

            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            JSON_Value *value = json_value_init_object();
            JSON_Object *object = json_value_get_object(value);
            json_object_set_string(object, "username", username);
            json_object_set_string(object, "password", password);
            char *payload = json_serialize_to_string_pretty(value);
            int size = strlen(payload);


            message = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json", &payload, 1, NULL, 0, jwt);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            close_connection(sockfd);

            // extract the JSON from the response
            JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
            JSON_Object *root_object = json_value_get_object(root_value);

            if (strstr(response, "error") != NULL) {
                printf("ERROR: %s\n", json_object_get_string(root_object, "error"));
            } else {
                printf("Successfully registered\n");
            }

            json_value_free(root_value);
            free(message);
            free(response);
        } else if (strcmp(command, "login") == 0) {
            char username[100];
            char password[100];

            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}

            printf("username=");
            fgets(username, 100, stdin);
            username[strcspn(username, "\n")] = 0;

            printf("password=");
            fgets(password, 100, stdin);
            password[strcspn(password, "\n")] = 0;

            if (strchr(username, ' ') != NULL) {
                printf("ERROR: Username cannot contain spaces\n");
                continue;
            }

            if (strchr(password, ' ') != NULL) {
                printf("ERROR: Password cannot contain spaces\n");
                continue;
            }

            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            JSON_Value *value = json_value_init_object();
            JSON_Object *object = json_value_get_object(value);
            json_object_set_string(object, "username", username);
            json_object_set_string(object, "password", password);
            char *payload = json_serialize_to_string_pretty(value);
            int size = strlen(payload);

            message = compute_post_request(HOST, "/api/v1/tema/auth/login", "application/json", &payload, 1, NULL, 0, jwt);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            close_connection(sockfd);

            JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
            JSON_Object *root_object = json_value_get_object(root_value);

            if (strstr(response, "error") != NULL) {
                printf("ERROR: %s\n", json_object_get_string(root_object, "error"));
            } else {
                logged_in = 1;

                const char *jwt_token = json_object_get_string(root_object, "token");
                if (jwt_token) {
                    strcpy(jwt, jwt_token);
                }

                const char *cookie_start = strstr(response, "Set-Cookie: ");
                if (cookie_start) {
                    cookie_start += strlen("Set-Cookie: ");
                    const char *cookie_end = strstr(cookie_start, "\r\n");
                    if (cookie_end) {
                        strncpy(cookies, cookie_start, cookie_end - cookie_start);
                        cookies[cookie_end - cookie_start] = '\0';
                    }
                }

                printf("Successfully logged in\n");
            }

            json_value_free(root_value);
            free(message);
            free(response);
                
        } else if (strcmp(command, "enter_library") == 0) {
            if (!logged_in) {
                printf("ERROR: You must be logged in to enter the library\n");
                continue;
            }

            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            char *cookies_array[1];
            cookies_array[0] = cookies;

            message = compute_get_request(HOST, "/api/v1/tema/library/access", NULL, cookies_array, 1, NULL);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            close_connection(sockfd);

            JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
            JSON_Object *root_object = json_value_get_object(root_value);

            if (strstr(response, "error") != NULL) {
                printf("ERROR: %s\n", json_object_get_string(root_object, "error"));
            } else {
                enter_library = 1;
                const char *new_token = json_object_get_string(root_object, "token");
                if (new_token) {
                    strcpy(jwt, new_token);
                }

                printf("Successfully entered the library\n");
            }

            json_value_free(root_value);
            free(message);
            free(response);

        } else if (strcmp(command, "get_books") == 0) {
            if (!logged_in) {
                printf("ERROR: You must be logged in to get the books\n");
                continue;
            }

            if (!enter_library) {
                printf("ERROR: You don't have access to the library!\n");
                continue;
            }

            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            char *cookie_array[1];
            cookie_array[0] = cookies;

            message = compute_get_request(HOST, "/api/v1/tema/library/books", NULL, cookie_array, 1, jwt);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            close_connection(sockfd);

            JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
            JSON_Object *root_object = json_value_get_object(root_value);

            if (strstr(response, "error") != NULL) {
                printf("ERROR: %s\n", json_object_get_string(root_object, "error"));
            } else {
                JSON_Array *books = json_object_get_array(root_object, "books");

                char *found_books = (char *)strstr(response, "[");
                if (found_books != NULL) {
                    printf("%s\n", found_books);
                } else {
                    printf("Books not found\n");
                }           
            }

            json_value_free(root_value);
            free(message);

        } else if (strcmp(command, "get_book") == 0) {
            if (!logged_in) {
                printf("ERROR: You must be logged in to get the book\n");
                continue;
            }

            if (!enter_library) {
                printf("ERROR: You don't have access to the library!\n");
                continue;
            }

            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            char book_id[100];

            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}

            printf("id=");
            fgets(book_id, 100, stdin);
            book_id[strcspn(book_id, "\n")] = 0;

            char url[100];

            char *cookies_array[1];
            cookies_array[0] = cookies;

            sprintf(url, "/api/v1/tema/library/books/%s", book_id);

            message = compute_get_request(HOST, url, NULL, cookies_array, 1, jwt);

            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            if (response == NULL) {
                printf("Error receiving response from server\n");
                free(message);
                close_connection(sockfd);
                continue;
            }

            close_connection(sockfd);

            JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
            JSON_Object *root_object = json_value_get_object(root_value);

            if (strstr(response, "error") != NULL) {
                printf("ERROR: %s\n", json_object_get_string(root_object, "error"));
            } else {
                JSON_Object *book = json_object_get_object(root_object, "book");
                char *found_book = (char *)strstr(response, "{");
                if (found_book != NULL) {
                    printf("%s\n", found_book);
                } else {
                    printf("Book not found\n");
                }
            }

        } else if (strcmp(command, "add_book") == 0) {
            if (!logged_in) {
                printf("ERROR: You must be logged in to add a book\n");
                continue;
            }

            if (!enter_library) {
                printf("ERROR: You don't have access to the library!\n");
                continue;
            }

            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}

            char title[100];
            char author[100];
            char genre[100];
            char publisher[100];
            char page_count[100];

            printf("title=");
            fgets(title, 100, stdin);
            title[strcspn(title, "\n")] = 0;

            printf("author=");
            fgets(author, 100, stdin);
            author[strcspn(author, "\n")] = 0;

            printf("genre=");
            fgets(genre, 100, stdin);
            genre[strcspn(genre, "\n")] = 0;

            printf("publisher=");
            fgets(publisher, 100, stdin);
            publisher[strcspn(publisher, "\n")] = 0;

            printf("page_count=");
            fgets(page_count, 100, stdin);
            page_count[strcspn(page_count, "\n")] = 0;

            int is_invalid = 0;

            for (int i = 0; i < strlen(page_count); i++) {
                if (page_count[i] < '0' || page_count[i] > '9') {
                    printf("ERROR: Page count must be a number\n");
                    is_invalid = 1;
                    break;
                }
            }

            if (is_invalid) {
                continue;
            }

            if (check_if_string_contains_numbers(author) || check_if_string_contains_numbers(genre) || check_if_string_contains_numbers(publisher)) {
                printf("ERROR: Invalid format for author, genre or publisher\n");
                continue;
            }

            JSON_Value *value = json_value_init_object();
            JSON_Object *object = json_value_get_object(value);
            json_object_set_string(object, "title", title);
            json_object_set_string(object, "author", author);
            json_object_set_string(object, "genre", genre);
            json_object_set_string(object, "publisher", publisher);
            json_object_set_number(object, "page_count", atoi(page_count));
            char *payload = json_serialize_to_string_pretty(value);
            int size = strlen(payload);

            char *cookies_array[1];
            cookies_array[0] = cookies;

            message = compute_post_request(HOST, "/api/v1/tema/library/books", "application/json", &payload, 1, cookies_array, 1, jwt);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            close_connection(sockfd);

            JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
            JSON_Object *root_object = json_value_get_object(root_value);

            if (strstr(response, "error") != NULL) {
                printf("ERROR: %s\n", json_object_get_string(root_object, "error"));
            } else {
                printf("Successfully added the book\n");
            }

            json_value_free(root_value);
            free(message);
            free(response);
        } else if (strcmp(command, "delete_book") == 0) {
            char book_id[100];

            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}

            printf("id=");
            fgets(book_id, 100, stdin);
            book_id[strcspn(book_id, "\n")] = 0;

            if (!logged_in) {
                printf("ERROR: You must be logged in to delete a book\n");
                continue;
            }

            if (!enter_library) {
                printf("ERROR: You don't have access to the library!\n");
                continue;
            }

            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            char url[100];
            sprintf(url, "/api/v1/tema/library/books/%s", book_id);

            char *cookies_array[1];
            cookies_array[0] = cookies;

            message = compute_delete_request(HOST, url, NULL, cookies_array, 1, jwt);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            close_connection(sockfd);

            JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
            JSON_Object *root_object = json_value_get_object(root_value);

            if (strstr(response, "error") != NULL) {
                printf("ERROR: %s\n", json_object_get_string(root_object, "error"));
            } else {
                printf("Successfully deleted the book\n");
            }

            json_value_free(root_value);
            free(message);
            free(response);

        } else if (strcmp(command, "logout") == 0) {
            if (!logged_in) {
                printf("ERROR: You must be logged in to logout\n");
                continue;
            }

            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            char *cookies_array[1];
            cookies_array[0] = cookies;

            message = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, cookies_array, 1, jwt);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);
            close_connection(sockfd);

            JSON_Value *root_value = json_parse_string(basic_extract_json_response(response));
            JSON_Object *root_object = json_value_get_object(root_value);

            if (strstr(response, "error") != NULL) {
                printf("ERROR: %s\n", json_object_get_string(root_object, "error"));
            } else {
                logged_in = 0;
                printf("Successfully logged out\n");
            }
        }

    }

    return 0;
}
