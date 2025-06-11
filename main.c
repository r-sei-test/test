#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CREDENTIALS_FILE "users.txt"

#define MAX_USERNAME 32
#define MAX_PASSWORD 32

// Function to simulate loading users (still vulnerable to format string attack)
void load_users()
{
    FILE *file = fopen(CREDENTIALS_FILE, "r");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    char line[128];
    printf("=== Users in System ===\n");

    while (fgets(line, sizeof(line), file))
    {
        printf(line); // ⚠️ Format string vulnerability
    }

    fclose(file);
}

// Function with command injection vulnerability
void log_failed_attempt(char *username)
{
    char cmd[256];

    // ⚠️ Command Injection
    snprintf(cmd, sizeof(cmd), "echo 'Failed login for user: %s' >> failed.log", username);
    system(cmd);
}

// ✅ Secure input reading function replacing gets()
void read_input(char *buffer, size_t size)
{
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline
}

// ⚠️ Function with newly added integer overflow
char *allocate_buffer(size_t count, size_t size)
{
    // ⚠️ Integer Overflow Vulnerability
    size_t total = count * size;

    // If count and size are large enough, this overflows and malloc gets small size
    return (char *)malloc(total);
}

int login()
{
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("Username: ");
    read_input(username, sizeof(username)); // ✅ Secure input

    printf("Password: ");
    read_input(password, sizeof(password)); // ✅ Secure input

    FILE *file = fopen(CREDENTIALS_FILE, "r");
    if (file == NULL)
    {
        printf("Could not open credentials file.\n");
        return 0;
    }

    char file_user[64];
    char file_pass[64];
    int authenticated = 0;

    while (fscanf(file, "%63s %63s", file_user, file_pass) == 2)
    {
        if (strcmp(username, file_user) == 0 && strcmp(password, file_pass) == 0)
        {
            authenticated = 1;
            break;
        }
    }

    fclose(file);

    if (!authenticated)
    {
        log_failed_attempt(username); // ⚠️ Command injection
    }

    return authenticated;
}

int main()
{
    printf("=== Welcome to InsecureLogin v1.3 ===\n");

    load_users(); // ⚠️ Format string

    // ⚠️ Trigger point: allocate user input buffer using vulnerable function
    size_t input_count, input_size;
    printf("How many input blocks do you want to allocate? ");
    scanf("%zu", &input_count);

    printf("Size of each block? ");
    scanf("%zu", &input_size);
    getchar(); // Flush leftover newline

    // ⚠️ Integer Overflow Vulnerability: large values overflow and lead to malloc(too-small)
    char *user_input = allocate_buffer(input_count, input_size);

    if (user_input == NULL)
    {
        printf("Allocation failed.\n");
        return 1;
    }

    printf("Allocated buffer. Please type your input: ");
    read_input(user_input, input_count * input_size); // May cause heap overflow if under-allocated

    free(user_input);

    if (login())
    {
        printf("Access granted.\n");
    }
    else
    {
        printf("Access denied.\n");
    }

    return 0;
}
