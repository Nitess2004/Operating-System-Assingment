#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define STORED_USER "nitess"
#define STORED_PASS "9999"
#define XOR_KEY 0x5A

void audit_log(const char *user, const char *action, const char *file) {
    FILE *log = fopen("audit.log", "a");
    if (!log) return;
    time_t now = time(NULL);
    char t[26];
    ctime_r(&now, t);
    t[strcspn(t, "\n")] = '\0';
    fprintf(log, "[%s] user=%s action=%s file=%s\n", t, user, action, file);
    fclose(log);
}

void xor_crypt(char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) buf[i] ^= XOR_KEY;
}

int login(char *user_out) {
    char user[32], pass[32];
    printf("Username: "); scanf("%31s", user);
    printf("Password: "); scanf("%31s", pass);
    if (strcmp(user, STORED_USER) == 0 && strcmp(pass, STORED_PASS) == 0) {
        strcpy(user_out, user);
        audit_log(user, "LOGIN_SUCCESS", "-");
        return 1;
    }
    audit_log(user, "LOGIN_FAILED", "-");
    printf("Login failed.\n");
    return 0;
}

void create_file(const char *user, const char *fname) {
    FILE *fp = fopen(fname, "w");
    if (!fp) { perror("Create failed"); return; }
    fclose(fp);
    chmod(fname, 0600);
    printf("Created '%s' (mode 0600)\n", fname);
    audit_log(user, "CREATE", fname);
}

void write_file(const char *user, const char *fname, int encrypt) {
    char text[100];
    getchar();
    printf("Text: "); fgets(text, sizeof(text), stdin);
    FILE *fp = fopen(fname, "ab");
    if (!fp) { perror("Write failed"); return; }
    size_t len = strlen(text);
    if (encrypt) xor_crypt(text, len);
    fwrite(text, 1, len, fp);
    fclose(fp);
    audit_log(user, encrypt ? "WRITE_ENCRYPTED" : "WRITE", fname);
    printf("Saved.\n");
}

void read_file(const char *user, const char *fname, int decrypt) {
    FILE *fp = fopen(fname, "rb");
    if (!fp) { printf("Error: '%s' not found.\n", fname); audit_log(user, "READ_FAILED", fname); return; }
    char buf[2048];
    size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
    buf[n] = '\0';
    fclose(fp);
    if (decrypt) xor_crypt(buf, n);
    printf("--- %s ---\n%s\n------------\n", fname, buf);
    audit_log(user, decrypt ? "READ_DECRYPTED" : "READ", fname);
}

void delete_file(const char *user, const char *fname) {
    if (remove(fname) == 0) { printf("Deleted.\n"); audit_log(user, "DELETE", fname); }
    else { perror("Delete failed"); audit_log(user, "DELETE_FAILED", fname); }
}

void set_permissions(const char *user, const char *fname) {
    printf("1)0600 owner-only  2)0644 owner+read-all  3)0444 read-only  4)0750 owner+group\nChoice: ");
    int c; scanf("%d", &c);
    mode_t modes[] = {0600, 0644, 0444, 0750};
    mode_t mode = (c >= 1 && c <= 4) ? modes[c - 1] : 0600;
    chmod(fname, mode);
    printf("Permissions set to %o\n", mode);
    audit_log(user, "CHMOD", fname);
}

int main(void) {
    char user[32];
    if (!login(user)) return 1;
    printf("Welcome, %s.\n", user);

    int choice;
    char fname[64];
    do {
        printf("\n1.Create 2.Write 3.Write(encrypted) 4.Read 5.Read(decrypt) "
               "6.Delete 7.SetPermissions 8.Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }
        if (choice >= 1 && choice <= 7) { printf("Filename: "); scanf("%63s", fname); }

        switch (choice) {
            case 1: create_file(user, fname); break;
            case 2: write_file(user, fname, 0); break;
            case 3: write_file(user, fname, 1); break;
            case 4: read_file(user, fname, 0); break;
            case 5: read_file(user, fname, 1); break;
            case 6: delete_file(user, fname); break;
            case 7: set_permissions(user, fname); break;
            case 8: printf("Goodbye.\n"); audit_log(user, "LOGOUT", "-"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 8);

    return 0;
}
