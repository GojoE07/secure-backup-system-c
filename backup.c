#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PATH 260
#define MAX_PASSWORD 100
#define BUFFER_SIZE 1024
#define HEADER "SBKP"
#define HEADER_SIZE 4

void xor_encrypt_decrypt(unsigned char *data, int data_len, const char *password) {
    int pass_len = strlen(password);
    for (int i = 0; i < data_len; i++) {
        data[i] ^= password[i % pass_len];
    }
}

int main() {
    char input_file[MAX_PATH];
    char output_file[MAX_PATH];
    char password[MAX_PASSWORD];

    printf("Enter file to back up: ");
    scanf("%259s", input_file);

    printf("Enter password: ");
    scanf("%99s", password);

    FILE *in = fopen(input_file, "rb");
    if (in == NULL) {
        printf("Error: Cannot open input file.\n");
        return 1;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[50];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H%M", t);

    snprintf(output_file, sizeof(output_file), "backups/%s_%s.bak", input_file, timestamp);

    FILE *out = fopen(output_file, "wb");
    if (out == NULL) {
        printf("Error: Cannot create backup file.\n");
        fclose(in);
        return 1;
    }

    unsigned char header[HEADER_SIZE] = HEADER;
    xor_encrypt_decrypt(header, HEADER_SIZE, password);
    fwrite(header, 1, HEADER_SIZE, out);

    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
        xor_encrypt_decrypt(buffer, (int)bytes_read, password);
        fwrite(buffer, 1, bytes_read, out);
    }

    fclose(in);
    fclose(out);

    FILE *log = fopen("backups/backup_log.txt", "a");
    if (log != NULL) {
        fprintf(log, "Backed up: %s -> %s\n", input_file, output_file);
        fclose(log);
    }

    printf("Backup created successfully: %s\n", output_file);
    return 0;
}