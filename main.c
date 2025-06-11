#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CREDENTIALS_FILE "users.txt"

#define MAX_USERNAME 32
#define MAX_PASSWORD 32

void load_users() {
    FILE *file = fopen(CREDENTIALS_FILE, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char line[128];

    printf("=== Users in System ===\n");
    while (fgets(line, sizeof(line), file)) {
        printf(line); // ⚠️ Format string vulnerability
    }

    fclose(file);
}

void log_failed_attempt(char *username) {
    char cmd[256];

    // ⚠️ Command Injection Vulnerability
    // Attacker-controlled input passed directly to system()
    snprintf(cmd, sizeof(cmd), "echo 'Failed login for user: %s' >> failed.log", username);

    // If username is: `bob'; rm -rf / #`
    // The command becomes: echo 'Failed login for user: bob'; rm -rf / #' >> failed.log
    system(cmd);
}

int login() {
    char username[32];
    char password[32];

    printf("Username: ");
    gets(username); // ⚠️ Buffer overflow

    printf("Password: ");
    gets(password); // ⚠️ Buffer overflow

    FILE *file = fopen(CREDENTIALS_FILE, "r");
    if (file == NULL) {
        printf("Could not open credentials file.\n");
        return 0;
    }

    char file_user[64];
    char file_pass[64];
    int authenticated = 0;

    while (fscanf(file, "%63s %63s", file_user, file_pass) == 2) {
        if (strcmp(username, file_user) == 0 && strcmp(password, file_pass) == 0) {
            authenticated = 1;
            break;
        }
    }

    fclose(file);

    if (!authenticated) {
        log_failed_attempt(username); // ⚠️ Unsanitized input → command injection
    }

    return authenticated;
}

int main() {
    printf("=== Welcome to InsecureLogin v1.2 ===\n");

    load_users(); // Format string vulnerability

    if (login()) {
        printf("Access granted.\n");
    } else {
        printf("Access denied.\n");
    }

    return 0;
}

