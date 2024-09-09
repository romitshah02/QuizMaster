#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUESTIONS 20
#define MAX_PLAYERS 50
#define MAX_OPTIONS 100
#define MAX_NAME_LENGTH 100

// Color escape sequences (adjust as needed)
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

struct Question {
    char question[256];
    char option1[MAX_OPTIONS];
    char option2[MAX_OPTIONS];
    char option3[MAX_OPTIONS];
    char option4[MAX_OPTIONS];
    int correctOption;
};

struct Score {
    char users[MAX_PLAYERS][MAX_NAME_LENGTH];
    int scores[MAX_PLAYERS][MAX_QUESTIONS];
    int total[MAX_PLAYERS];
};

void shuffle(struct Question *array, size_t n) {
    if (n <= 1) return;

    srand((unsigned int)time(NULL));

    for (size_t i = n - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        struct Question temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void loadScores(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

int getNextQuestionNumber(const char *filename) {
    FILE *file = fopen(filename, "r");
    int maxNumber = 0;
    int questionNumber;
    char line[256];

    if (file == NULL) {
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d.", &questionNumber) == 1) {
            if (questionNumber > maxNumber) {
                maxNumber = questionNumber;
            }
        }
    }

    fclose(file);
    return maxNumber + 1;
}

void addQuestion(const char *filename) {
    FILE *file = fopen(filename, "a");

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    struct Question newQuestion;
    int questionNumber = getNextQuestionNumber(filename);

    printf("Enter the question (avoid special characters): ");
    scanf(" %[^\n]", newQuestion.question); // Using scanf with %[^\n]

    printf("Enter option 1: ");
    scanf(" %[^\n]", newQuestion.option1);

    printf("Enter option 2: ");
    scanf(" %[^\n]", newQuestion.option2);

    printf("Enter option 3: ");
    scanf(" %[^\n]", newQuestion.option3);

    printf("Enter option 4: ");
    scanf(" %[^\n]", newQuestion.option4);

    do {
        printf("Enter the correct option number (1/2/3/4): ");
        scanf("%d", &newQuestion.correctOption);
        if (newQuestion.correctOption < 1 || newQuestion.correctOption > 4) {
            printf("Invalid option. Please enter a number between 1 and 4.\n");
        }
    } while (newQuestion.correctOption < 1 || newQuestion.correctOption > 4);

    fprintf(file, "\n%d. %s\n", questionNumber, newQuestion.question);
    fprintf(file, "1) %s\n", newQuestion.option1);
    fprintf(file, "2) %s\n", newQuestion.option2);
    fprintf(file, "3) %s\n", newQuestion.option3);
    fprintf(file, "4) %s\n", newQuestion.option4);
    fprintf(file, "%d\n", newQuestion.correctOption);

    fclose(file);

    printf("Question added successfully!\n");
}

void game() {
    FILE *file = fopen("questions.txt", "r");

    if (file == NULL) {
        printf("Could not open file\n");
        return;
    }

    struct Question questions[MAX_QUESTIONS];
    int i = 0;
    int questionNumber;

    while (fscanf(file, "%d. %[^\n] %[^\n] %[^\n] %[^\n] %[^\n] %d",
                 &questionNumber,
                 questions[i].question,
                 questions[i].option1,
                 questions[i].option2,
                 questions[i].option3,
                 questions[i].option4,
                 &questions[i].correctOption) != EOF) {
        i++;
    }

    fclose(file);

    shuffle(questions, i);

    int n;
    int q;
    int answer;

    printf("Enter the number of players: ");
    scanf("%d", &n);

    do {
        printf("Enter the game length (5/10/15/20): ");
        scanf("%d", &q);
        if (q != 5 && q != 10 && q != 15 && q != 20) {
            printf("Please enter a valid length\n");
        }
    } while (q != 5 && q != 10 && q != 15 && q != 20);

    struct Score players = {0};

    for (int i = 0; i < n; i++) {
        printf("Enter Name of player %d: ", i + 1);
        scanf("%s", players.users[i]);
    }

    printf("\n");

    for (int j = 0; j < q; j++) {
        for (int k = 0; k < n; k++) {
            printf("Question %d: %s\n", j + 1, questions[j].question);
            printf("%s\n", questions[j].option1);
            printf("%s\n", questions[j].option2);
            printf("%s\n", questions[j].option3);
            printf("%s\n", questions[j].option4);
            printf("%s, enter your answer (1/2/3/4): ", players.users[k]);
            scanf("%d", &answer);

            if (answer == questions[j].correctOption) {
                players.scores[k][j] = 1;
            } else {
                players.scores[k][j] = 0;
            }
            printf("\n");
        }
    }

    for (int i = 0; i < n; i++) {
        int total = 0;
        printf("Name: %s\n", players.users[i]);
        for (int j = 0; j < q; j++) {
            if (players.scores[i][j] == 1) {
                printf("Question %d: correct\n", j + 1);
            } else {
                printf("Question %d: wrong\n", j + 1);
            }
            total += players.scores[i][j];
        }
        players.total[i] = total;
        printf("Total Score: %d\n", total);
    }

    char choice2;
    do {
        printf("Want to save score to a file? (y/n): ");
        scanf(" %c", &choice2);

        switch (choice2) {
            case 'y': {
                char name[50];
                printf("Enter file name: ");
                scanf("%s", name);
                strcat(name, ".txt");
                FILE *file = fopen(name, "w");

                if (file == NULL) {
                    perror("Error opening file");
                    return;
                }

                for (int j = 0; j < q; j++) {
                    fprintf(file, "Question %d: %s\n", j + 1, questions[j].question);
                    fprintf(file, "%s\n", questions[j].option1);
                    fprintf(file, "%s\n", questions[j].option2);
                    fprintf(file, "%s\n", questions[j].option3);
                    fprintf(file, "%s\n", questions[j].option4);
                    fprintf(file, "Answer = %d\n", questions[j].correctOption);

                    for (int i = 0; i < n; i++) {
                        if (players.scores[i][j] == 1) {
                            fprintf(file, "%s answer = correct\n", players.users[i]);
                        } else {
                            fprintf(file, "%s answer = wrong\n", players.users[i]);
                        }
                    }

                    fprintf(file, "\n");
                }

                fprintf(file, "\nFinal Score\n");
                for (int i = 0; i < n; i++) {
                    fprintf(file, "%s = %d\n", players.users[i], players.total[i]);
                }

                fclose(file);
                printf("Scores saved in %s.txt file.\n", name);
                return;
            }
            case 'n':
                break;
            default:
                printf("Invalid choice.\n");
                break;
        }
    } while (choice2 != 'n');
}

void printTitleBanner() {
    printf("\n");
    for (int i = 0; i < 50; i++) {
        printf("=");
    }
    printf("\n");
    printf("     " BOLD CYAN "Welcome to the Quiz Master!" RESET "     \n");
    for (int i = 0; i < 50; i++) {
        printf("=");
    }
    printf("\n\n");
}

void printMenuSeparator() {
    printf("\n");
    for (int i = 0; i < 50; i++) {
        printf("-" BLUE);
    }
    printf("\n");
}

void printEmphasizedText(char *text) {
    printf("\n" BOLD YELLOW "* " RESET "%s" BOLD YELLOW " *" RESET "\n", text);
}

int main() {
    int choice1;
    char filename[50];

    printTitleBanner();

    while (1) {
        printMenuSeparator();
        printf(BOLD CYAN "1) Start Quiz\n" RESET);
        printf(BOLD CYAN "2) Load Scores from File\n" RESET);
        printf(BOLD CYAN "3) Add New Question\n" RESET);
        printf(BOLD CYAN "4) Exit\n" RESET);
        printMenuSeparator();
        printf("Enter Choice : ");
        scanf("%d", &choice1);

        switch (choice1) {
            case 1:
                game();
                break;
            case 2:
                printf("Enter the filename to load scores from (including .txt extension): ");
                scanf("%s", filename);
                loadScores(filename);
                break;
            case 3:
                printf("Enter the filename to save the new question (including .txt extension): ");
                scanf("%s", filename);
                addQuestion(filename);
                break;
            case 4:
                exit(0);
            default:
                printEmphasizedText("Invalid choice. Please enter a number between 1 and 4.");
                break;
        }
    }

    return 0;
}
