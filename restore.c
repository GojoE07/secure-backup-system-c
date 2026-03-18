#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    char backup_file[MAX_PATH];
    char restored_file[MAX_PATH];
    char password[MAX_PASSWORD];

    printf("Enter backup file to restore: ");
    scanf("%259s", backup_file);

    printf("Enter output restored file name: ");
    char final_output[MAX_PATH];
    scanf("%259s", restored_file);
    snprintf(final_output, sizeof(final_output), "restores/%s", restored_file);
    printf("Enter password: ");
    scanf("%99s", password);

    FILE *in = fopen(backup_file, "rb");
    if (in == NULL) {
        printf("Error: Cannot open backup file.\n");
        return 1;
    }

    FILE *out = fopen(final_output, "wb");
    if (out == NULL) {
        printf("Error: Cannot create restored file.\n");
        fclose(in);
        return 1;
    }

    unsigned char header[HEADER_SIZE];
    if (fread(header, 1, HEADER_SIZE, in) != HEADER_SIZE) {
        printf("Error: Invalid or corrupted backup file.\n");
        fclose(in);
        fclose(out);
        remove(restored_file);
        return 1;
    }

    xor_encrypt_decrypt(header, HEADER_SIZE, password);

    if (memcmp(header, HEADER, HEADER_SIZE) != 0) {
        printf("Error: Incorrect password. Restore failed.\n");
        fclose(in);
        fclose(out);
        remove(restored_file);
        return 1;
    }

    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
        xor_encrypt_decrypt(buffer, (int)bytes_read, password);
        fwrite(buffer, 1, bytes_read, out);
    }

    fclose(in);
    fclose(out);

    printf("File restored successfully: %s\n", final_output);
    return 0;
}