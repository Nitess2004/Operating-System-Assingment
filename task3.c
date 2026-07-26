#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main() {
    char user[20], pass[20], text[100], ch;
    int choice;
    FILE *fp;

    printf("Username: ");
    scanf("%s", user);
    printf("Password: ");
    scanf("%s", pass);

    if (strcmp(user, "admin") || strcmp(pass, "1234")) {
        printf("Login Failed!\n");
        return 0;
    }

    do {
        printf("\n1.Create Files\n2.Write Notes\n3.Read File\n4.Delete Files\n5.Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {

        case 1:
            fp = fopen("notes.txt", "w");
            fprintf(fp, "Student Notes\n");
            fclose(fp);
            chmod("notes.txt", 0600);

            fp = fopen("report.txt", "w");
            fprintf(fp, "Final Report\n");
            fclose(fp);
            chmod("report.txt", 0444);

            printf("Files Created\n");
            break;

        case 2:
            getchar();
            printf("Enter Text: ");
            fgets(text, 100, stdin);

            fp = fopen("notes.txt", "a");
            fprintf(fp, "%s", text);
            fclose(fp);

            printf("Saved\n");
            break;

        case 3:
            printf("1.notes.txt\n2.report.txt\n");
            scanf("%d", &choice);

            fp = fopen(choice == 1 ? "notes.txt" : "report.txt", "r");

            while ((ch = fgetc(fp)) != EOF)
                putchar(ch);

            fclose(fp);
            break;

        case 4:
            remove("notes.txt");
            remove("report.txt");
            printf("Files Deleted\n");
            break;

        case 5:
            printf("Exit\n");
            break;

        default:
            printf("Invalid Choice\n");
        }

    } while (choice != 5);

    return 0;
}
