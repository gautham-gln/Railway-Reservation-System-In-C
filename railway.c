#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#else
#define SLEEP(ms) usleep((ms)*1000)
#endif

void setColorBlack() { printf("\033[30m"); }

void setColorRed() { printf("\033[31m"); }

void setColorGreen() { printf("\033[32m"); }

void setColorYellow() { printf("\033[33m"); }

void setColorBlue() { printf("\033[34m"); }

void setColorMagenta() { printf("\033[35m"); }

void setColorCyan() { printf("\033[36m"); }

void setColorWhite() { printf("\033[37m"); }

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

void setColor(int color) { printf("\033[1;%dm", color); }

void resetColor() { printf("\033[0m"); }

void setFontSize(int size) { printf("\033[6;%dH", size); }

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>

void disableEcho() {
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  tty.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void enableEcho() {
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  tty.c_lflag |= ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

char getch() {
  char ch;
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}
#endif

void getPassword(char *password, int maxLength) {
  int i = 0;
  char ch;

#ifdef _WIN32

#else
  disableEcho();
#endif

  while (1) {
#ifdef _WIN32
    ch = _getch();
#else
    ch = getch();
#endif
    if (ch == '\n' || ch == '\r') {
      password[i] = '\0';
      printf("\n");
      break;
    } else if (ch == 127 || ch == 8) {
      if (i > 0) {
        i--;
        printf("\b \b");
      }
    } else if (i < maxLength - 1) {
      password[i++] = ch;
      printf("*");
    }
  }

#ifdef _WIN32

#else
  enableEcho();
#endif
}

void initializeRandom() { srand(time(NULL)); }

int numberExistsInFile(const char *filename, int number) {
  FILE *file = fopen(filename, "r");
  if (file != NULL) {
    int num;
    while (fscanf(file, "%d", &num) != EOF) {
      if (num == number) {
        fclose(file);
        return 1;
      }
    }
    fclose(file);
  }
  return 0;
}

int generateRandom(int min, int max, const char *filename) {
  int randomNumber;
  do {
    randomNumber = min + rand() % (max - min + 1);
  } while (numberExistsInFile(filename, randomNumber));
  return randomNumber;
}

void writeRandomNumberToFile(const char *filename, int number) {
  FILE *file = fopen(filename, "a");
  if (file != NULL) {
    fprintf(file, "%d\n", number);
    fclose(file);
    //printf("Random number %d has been written to '%s'.\n", number, filename);
  } else {
    printf("Error opening file for writing.\n");
  }
}

void display_histogram(int positive_count, int negative_count) {
  printf("Positive: ");
  for (int i = 0; i < positive_count; i++) {
    printf("😄");
  }
  printf(" (%d)\n", positive_count);

  printf("Negative: ");
  for (int i = 0; i < negative_count; i++) {
    printf("🙁");
  }
  printf(" (%d)\n", negative_count);
}

#define DAYTRAIN_DATABASE "daytrains.bin"
#define NIGHTTRAIN_DATABASE "nighttrains.bin"
#define FEEDBACKS "feedbacks.txt"
#define MAX_TRAINS 20
#define MAX_STRINGS 100
#define MAX_LENGTH 200
#define TOTAL_STATIONS 11
#define MAX_PASSENGERS 20

#define MAX_LINE_LENGTH 1024
#define MAX_FIELD_LENGTH 100
#define MAX_FIELDS 20

#define MAX_USERS 100
#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 50
#define MAX_QUESTION_LEN 100
#define MAX_ANSWER_LEN 100
#define USER_DATABASE "users.csv"
#define MIN_BALANCE 2000

#define MAX_WORDS 5000
#define MAX_SENTENCES 1000
#define MAX_WORD_LENGTH 50
#define MAX_SENTENCE_LENGTH 256

typedef struct {
  char word[MAX_WORD_LENGTH];
  int positive_count;
  int negative_count;
} WordCount;

typedef struct {
  char sentence[MAX_SENTENCE_LENGTH];
  int label; // 1 for positive, 0 for negative
} Sentence;

WordCount word_counts[MAX_WORDS];
int word_count_index = 0;
int total_positive_words = 0;
int total_negative_words = 0;
int total_positive_sentences = 0;
int total_negative_sentences = 0;

double nightSleeperFare = 1.5;
double nightAC3Fare = 2.5;
double nightAC2Fare = 3.5;
double nightAC1Fare = 4.5;
double dayNormalFare = 1.0;
double dayACFare = 2.0;
double dayExecFare = 3.0;

void preprocess_sentence(char *sentence) {
  for (int i = 0; sentence[i]; i++) {
    sentence[i] = tolower(sentence[i]);
    if (ispunct(sentence[i])) {
      sentence[i] = ' ';
    }
  }
}

void add_word(char *word, int label) {
  for (int i = 0; i < word_count_index; i++) {
    if (strcmp(word_counts[i].word, word) == 0) {
      if (label == 1) {
        word_counts[i].positive_count++;
        total_positive_words++;
      } else {
        word_counts[i].negative_count++;
        total_negative_words++;
      }
      return;
    }
  }

  strcpy(word_counts[word_count_index].word, word);
  if (label == 1) {
    word_counts[word_count_index].positive_count = 1;
    word_counts[word_count_index].negative_count = 0;
    total_positive_words++;
  } else {
    word_counts[word_count_index].positive_count = 0;
    word_counts[word_count_index].negative_count = 1;
    total_negative_words++;
  }
  word_count_index++;
}

void train_model(Sentence sentences[], int num_sentences) {
  for (int i = 0; i < num_sentences; i++) {
    preprocess_sentence(sentences[i].sentence);
    char *word = strtok(sentences[i].sentence, " ");
    while (word != NULL) {
      add_word(word, sentences[i].label);
      word = strtok(NULL, " ");
    }
    if (sentences[i].label == 1) {
      total_positive_sentences++;
    } else {
      total_negative_sentences++;
    }
  }
}

double calculate_word_probability(char *word, int label) {
  for (int i = 0; i < word_count_index; i++) {
    if (strcmp(word_counts[i].word, word) == 0) {
      if (label == 1) {
        return (double)(word_counts[i].positive_count + 1) /
               (total_positive_words + word_count_index);
      } else {
        return (double)(word_counts[i].negative_count + 1) /
               (total_negative_words + word_count_index);
      }
    }
  }
  return 1.0 / (label == 1 ? total_positive_words + word_count_index
                           : total_negative_words + word_count_index);
}

int classify_sentence(char *sentence) {
  preprocess_sentence(sentence);
  double positive_prob = (double)total_positive_sentences /
                         (total_positive_sentences + total_negative_sentences);
  double negative_prob = (double)total_negative_sentences /
                         (total_positive_sentences + total_negative_sentences);

  char *word = strtok(sentence, " ");
  while (word != NULL) {
    positive_prob *= calculate_word_probability(word, 1);
    negative_prob *= calculate_word_probability(word, 0);
    word = strtok(NULL, " ");
  }

  return positive_prob > negative_prob ? 1 : 0;
}

char stations[TOTAL_STATIONS][20] = {
    "Chennai Central", "Chennai Egmore", "Salem",        "Tiruppur",
    "Coimbatore",      "Kathpadi",       "Jollarpettai", "Mettupalayam",
    "Erode",           "Bengaluru",      "Tirunelveli"};

typedef struct {
  long int pnr;
  char name[50];
  char gender;
  int seatNo;
  char class[3];
  char source[20];
  char destination[20];
  char status[5];

} passenger;

typedef struct {
  char user[20];
  long int Train_ID;
  long int pnr;
  int numberOfTickets;
  double amount;
  char status[5];   // Size 4 to include the null terminator
  char class[3];    // Size 3 to include the null terminator
  char dayNight[2]; // Size 2 to include the null terminator
} book;

book bookings[3000];
int numBookings = 0;

typedef struct {
  long int trainID;
  char trainName[50];
  long int time[11];
  char route[TOTAL_STATIONS][20];
  long int date;
  int sleeperAvail[864];
  char sleeperPref[864];
  int ac3Avail[256];
  char ac3Pref[256];
  int ac2Avail[96];
  char ac2Pref[96];
  int ac1Avail[16];
  char ac1Pref[16];
  int distance[TOTAL_STATIONS];
  passenger nightbookings[3000];
  int numnightbookings;

} nightTrain;

nightTrain nighttrains[MAX_TRAINS];
int num_nighttrains = 0;

typedef struct {
  long int trainID;
  char trainName[50];
  long int time[11];
  char route[TOTAL_STATIONS][20];
  long int date;
  int normalAvail[840];
  char normalPref[840];
  int acAvail[300];
  char acPref[300];
  int execAvail[90];
  char execPref[90];
  int distance[TOTAL_STATIONS];
  passenger daybookings[3000];
  int numdaybookings;
  int acwl[100];
  int acwlnum;
  long int acwlpnr[100];

} dayTrain;

dayTrain daytrains[MAX_TRAINS];
int num_daytrains = 0;

typedef struct {
  char username[MAX_USERNAME_LEN];
  char password[MAX_PASSWORD_LEN];
  char security_question[MAX_QUESTION_LEN];
  char security_answer[MAX_ANSWER_LEN];
  long int account_number;
  double balance;

} User;

User users[MAX_USERS];
int num_users = 0;

typedef struct {
  char ticketName[100];
  char trainName[100];
  long int PNR;
  char source[100];
  char destination[100];
  long int dateOfTrain;
  long int departureTime;
  char timeOfBooking[100];
  char names[MAX_PASSENGERS][100];
  char class[MAX_PASSENGERS][4];
  char type[MAX_PASSENGERS];
  int seatNumber[MAX_PASSENGERS];
  char gender[MAX_PASSENGERS];
  int numberOfPassengers;
} TrainTicket;

TrainTicket tickets[3000];
int numTickets = 0;

char feedbacks[MAX_STRINGS][MAX_LENGTH];
int num_feedbacks = 0;

int registerUser();
int loginUser();
void forgotPassword();
void saveUsersToFile();
void loadUsersFromFile();

int admin_login();
int ad_menu();
int addTrain();
int displayallTrains();
int removeTrain();
void readDAYTrainsFromFile(dayTrain daytrains[], int *num_daytrains);
void readNIGHTTrainsFromFile(nightTrain nighttrains[], int *num_nighttrains);
void writeDAYTrainsToFile(dayTrain daytrains[], int num_daytrains);
void writeNIGHTTrainsToFile(nightTrain nighttrains[], int num_nighttrains);

void printBookings(const book *bookings, int numBookings);


void readTicketsFromFile(TrainTicket tickets[], int *numTickets);
void writeTicketsToFile(TrainTicket tickets[], int numTickets);
void getTimeOfBooking(char *buffer, int len);
void printTicket(TrainTicket tickets[], int numTickets);
void displayTicket(TrainTicket tickets[], int numTickets);
int findStringIndex(char array[][20], int size, const char *target);
int findBookingByPNR(long int pnr, int size);
int findTicketByPNR(long int PNR, int size);
int findDayTrainByTrainID(long int trainID, int size);
void deleteBookingsByPNRFromTrain(int trainIndex, long int pnr);
void deleteTicketByPNR(long int PNR, int *size);
int CancelTickets(long int pnr);
int* findPassengerIndices(long int pnr, int trainIndex, int numOccurrences);
void deleteFirstElement(int* array, int* size);
void deleteFirstElementLong(long int* array, int* size);

void modifyString(char *str, int num);
int check_status(long int pnr);
int extractDigitsFromString( char* str);

int user_menu();
int generatePNR();
int pay(double amount);
int viewCustomerFeedback();
int checkRefundEligibility(int index);
void updateWaitlist(int tin,int type);

int mainMenu();
void combineStringAndLong(const char *str, long int num, char *result,size_t resultSize);

long int getDateAsLong();
void clearInputBuffer();
void longIntToDateStr(long int, char *);
void getTimeFromUser(int *, int *);
long int convertTimeToLongInt(int, int);
void convertTime(long int, char *);
void initializeRandom();
int numberExistsInFile(const char *, int);
int generateRandom(int, int, const char *);
void writeRandomNumberToFile(const char *, int);
void writeFeedbacksToFile(const char *filename, char strings[][MAX_LENGTH], int count);
void readFeedbacksFromFile(const char *filename, char strings[][MAX_LENGTH], int *count);

long int checkRouteAndPrintInfo(dayTrain daytrains[], int num_daytrains, nightTrain nighttrains[], int num_nighttrains);
void swapDayTrains(dayTrain *a, dayTrain *b);
void swapNightTrains(nightTrain *a, nightTrain *b);
void sortDayTrains(dayTrain daytrains[], int num_daytrains);
void sortNightTrains(nightTrain nighttrains[], int num_nighttrains);

int countOnes(int arr[], int);
void writeBookingsTobin(book bookings[],int numBookings);
void readBookingsFrombin(book bookings[],int* numBookings);
int sumArrayElements(int *arr, int size);

void allocateSleeperLowerBerthsToRAC(nightTrain *trains, int trainIndex);
void printSeparator(int* columnWidths, int numColumns);
void printRow(char** fields, int* columnWidths, int numColumns);
int refund(double refund_amt, int status);
void printCsvAsTable(const char* filename);
void saveFeedbacksToFile(const char *filename);
void loadFeedbacksFromFile(const char *filename);

void saveFeedbacksToFile(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    // Write the number of feedbacks
    fwrite(&num_feedbacks, sizeof(int), 1, file);

    // Write the feedbacks array
    fwrite(feedbacks, sizeof(feedbacks[0]), num_feedbacks, file);

    fclose(file);
}

void loadFeedbacksFromFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    // Read the number of feedbacks
    fread(&num_feedbacks, sizeof(int), 1, file);

    // Read the feedbacks array
    fread(feedbacks, sizeof(feedbacks[0]), num_feedbacks, file);

    fclose(file);
}


void writeBookingsTobin(book bookings[],int numBookings) {
    // FILE *file = fopen(filename, "w");
    // if (!file) {
    //     perror("Failed to open file");
    //     return;
    // }

  // Write the header line (optional)
    // fprintf(file, "User,Train ID,PNR,Number,Fare,Status,Class,DayNight\n");

    // for (int i = 0; i < size; i++) {
        
    //     fprintf(file, "%s,%ld,%ld,%d,%lf,%s,%s,%s\n", bookings[i].user,bookings[i].Train_ID, bookings[i].pnr,bookings[i].numberOfTickets,bookings[i].amount,bookings[i].status, bookings[i].class,bookings[i].dayNight);
    // }

    // fclose(file);
    FILE *file = fopen("bookings.bin", "wb");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  fwrite(bookings, sizeof(book), numBookings, file);

  fclose(file);

}

void printBookings(const book *bookings, int numBookings) {
    // Print header
    system("clear");
    setColorYellow();
    printf("%-20s %-10s %-10s %-15s %-10s %-5s %-5s\n",
           "User", "Train ID", "PNR", "Tickets", "Amount", "Status", "Class");
    resetColor();

    // Print each booking
    for (int i = 0; i < numBookings; i++) {
        printf("%-20s %-10ld %-10ld %-15d %-10.2lf %-5s %-5s\n",
               bookings[i].user,
               bookings[i].Train_ID,
               bookings[i].pnr,
               bookings[i].numberOfTickets,
               bookings[i].amount,
               bookings[i].status,
               bookings[i].class);
    }
}

void readBookingsFrombin(book bookings[],int* numBookings) {
  // FILE *file = fopen(filename, "r");
  // if (!file) {
  //   perror("Failed to open file");
  //   return 0;
  // }

  // char line[256];
  // int count = 0;

  // // Skip the header line (optional)
  // if (fgets(line, sizeof(line), file) == NULL) {
  //   fclose(file);
  //   return 0;
  // }

  // while (fgets(line, sizeof(line), file) && count < maxSize) {
  //   // Parse the CSV line
  //   sscanf(line, "%20s,%ld,%ld,%d,%lf,%3s,%2s,%1s", bookings[count].user,&bookings[count].Train_ID, &bookings[count].pnr,&bookings[count].numberOfTickets, &bookings[count].amount,bookings[count].status, bookings[count].class,bookings[count].dayNight);
  //   count++;
  // }

  // fclose(file);
  // return count; // Return the number of records read
  FILE *file = fopen("bookings.bin", "rb");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  *numBookings = fread(bookings, sizeof(book), 3000, file);

  fclose(file);
}

int main() {
  loadUsersFromFile();
  readDAYTrainsFromFile(daytrains, &num_daytrains);
  readNIGHTTrainsFromFile(nighttrains, &num_nighttrains);
  //readFeedbacksFromFile(FEEDBACKS, feedbacks, &num_feedbacks);
  readTicketsFromFile(tickets, &numTickets);
  readBookingsFrombin(bookings,&numBookings);
  loadFeedbacksFromFile("feedbacks.dat");
  //waitlist
  for(int ijk=0;ijk<num_daytrains;ijk++){
    if (daytrains[ijk].trainID==109)
    {
      for(int z=0;z<297;z++){
        daytrains[ijk].acAvail[z]=0;
      }
    }
  }
  //rac
  int index;
  for (int abc = 0; abc<num_nighttrains; abc++)
  {
    if (nighttrains[abc].trainID == 110)
    {
      index = abc;
      break;
    }
  }
  allocateSleeperLowerBerthsToRAC(nighttrains,index);

  for (int def = 0; def<834; def++)
  {
    nighttrains[index].sleeperAvail[def] = 0;
  }

  Sentence training_data[] = {{"I love this product", 1},{"This is the best thing ever", 1},{"I am very happy with this", 1},{"very easy and effective booking system",1},{"I like it", 1},{"This is terrible", 0},{"I hate it", 0},{"This is the worst purchase", 0}};
  int num_training_sentences = sizeof(training_data) / sizeof(training_data[0]);

  train_model(training_data, num_training_sentences);

  char text[] = "W E L C O M E    T O    R A I L W A Y    R E S E R V A T I O N    S Y S T E M";
  int textLength = strlen(text);
  int colors[] = {31, 33, 32, 36, 35};
  int numColors = sizeof(colors) / sizeof(colors[0]);
  int fontSize = 6;

  system("clear");;
  setFontSize(fontSize);

   for (int i = 0; i < textLength; i++) {
    setColor(colors[i % numColors]);
     putchar(text[i]);
     fflush(stdout);
     SLEEP(100);
   }

  resetColor();

  mainMenu();

  writeDAYTrainsToFile(daytrains, num_daytrains);
  writeNIGHTTrainsToFile(nighttrains, num_nighttrains);
  //writeFeedbacksToFile(FEEDBACKS, feedbacks, num_feedbacks);
  saveFeedbacksToFile("feedbacks.dat");
  writeBookingsTobin(bookings,numBookings);
  writeTicketsToFile(tickets, numTickets);
  saveUsersToFile();

  // checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,
  // num_nighttrains);

  return 0;
}

int mainMenu() {

  int choice;
  int lflag = 0;

  do {
    if (lflag == 1) {
      break;
    }

    printf("\n");
    printf("\n");
    printf("1. Register\n");
    printf("\n");
    printf("2. Login\n");
    printf("\n");
    printf("3. Forgot Password\n");
    printf("\n");
    printf("4. Admin Login\n");
    printf("\n");
    printf("5. Exit\n");
    printf("\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    switch (choice) {
    case 1:
      registerUser();
      break;
    case 2: {
      lflag = loginUser();
      if (lflag == 1) {
        user_menu();
        lflag = 0;
        break;
      } else
        break;
    }

    case 3:
      forgotPassword();
      break;

    case 4: {
      int flag;
      flag = admin_login();
      if (flag == 1) {
        setColorGreen();
        printf("Admin Login Success");
        printf("\n");
        resetColor();
        ad_menu();
        system("clear");;

      } else {
        printf("Login fail");
      }
      break;
    }

    case 5:

      printf("Exiting...\n");

      break;

    default:
      printf("Invalid choice!!!!\n");
    }

  } while (choice != 5 && lflag == 0);

  return 0;
}

int registerUser() {
    system("clear");;
    if (num_users >= MAX_USERS) {
        printf("Cannot register more users!!!!\n");
        return 0;
    }

    printf("Enter username: ");
    char username[MAX_USERNAME_LEN];
    scanf("%s", username);
    clearInputBuffer();

    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0) {
            setColorRed();
            printf("Username already exists!!!! TRY AGAIN\n");
            resetColor();
            return 0;
        }
    }

  strcpy(users[num_users].username, username);
  printf("\n");

  char password[128];

  printf("Enter password: ");
  getPassword(password, sizeof(password));

  strcpy(users[num_users].password, password);
  printf("\n");

  system("clear");;
  printf("Select a security question:\n");
  printf("\n");
  printf("1. What is your favorite color?\n");
  printf("\n");
  printf("2. What is your favorite food?\n");
  printf("\n");
  printf("3. What is the name of your first pet?\n");
  printf("\n");
  int choice;
  printf("Enter choice: ");
  scanf("%d", &choice);

  switch (choice) {
  case 1:
    strcpy(users[num_users].security_question,"What is your favorite color?");
    break;
  case 2:
    strcpy(users[num_users].security_question, "What is your favorite food?");
    break;
  case 3:
    strcpy(users[num_users].security_question,"What is the name of your first pet?");
    break;
  default:
    printf("Invalid choice!!!!\n");
    return 0;
  }
  printf("\n");
  printf("Enter answer to the security question: ");
  scanf("%s", users[num_users].security_answer);

  printf("\nEnter 8 digit number for your account: ");
  scanf("%ld", &users[num_users].account_number);

  do {
    printf("\nEnter initial balance (minimum %d): ", MIN_BALANCE);
    scanf("%lf", &users[num_users].balance);

    if (users[num_users].balance < MIN_BALANCE) {
      printf("Initial balance must be at least %d.\n", MIN_BALANCE);
    }
  } while (users[num_users].balance < MIN_BALANCE);

  num_users++;
  setColorGreen();
  printf("--------------------------------------Registration "
         "successful--------------------------------------\n");
  printf("          Your JDG Railway eWallet has been created with the entered "
         "initial balance                 ");
  printf("\n\n");
  resetColor();

  return 0;
}

int loginUser() {
  system("clear");;
  char username[MAX_USERNAME_LEN];
  char password[MAX_PASSWORD_LEN];
  printf("\n");
  printf("Enter username: ");
  scanf("%s", username);
  clearInputBuffer();
  printf("\n");
  printf("Enter password: ");
  getPassword(password, sizeof(password));

  printf("\n");

  for (int i = 0; i < num_users; i++) {
    if (strcmp(users[i].username, username) == 0 &&
        strcmp(users[i].password, password) == 0) {
      setColorGreen();
      printf("--------------------------------------Login "
             "successful--------------------------------------\n");
      resetColor();
      return 1;
    }
  }

  printf("Invalid username or password !!!!\n");
  return 0;
}

void forgotPassword() {
  system("clear");;
  char username[MAX_USERNAME_LEN];
  char answer[MAX_ANSWER_LEN];
  printf("\n");
  printf("Enter your username: ");
  scanf("%s", username);

  for (int i = 0; i < num_users; i++) {
    if (strcmp(users[i].username, username) == 0) {
      printf("Security Question: %s\n", users[i].security_question);
      printf("Enter your answer: ");
      scanf("%s", answer);
      if (strcmp(users[i].security_answer, answer) == 0) {
        setColorCyan();
        printf("Your password is: %s\n", users[i].password);
        resetColor();
        return;
      } else {
        printf("Incorrect answer\n");
        return;
      }
    }
  }

  printf("Username not found !!!!\n");
}

void saveUsersToFile() {
  FILE *file = fopen(USER_DATABASE, "w");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  for (int i = 0; i < num_users; i++) {
    fprintf(file, "%s,%s,%s,%s,%ld,%lf\n", users[i].username, users[i].password,users[i].security_question, users[i].security_answer,users[i].account_number, users[i].balance);
  }

  fclose(file);
}

void loadUsersFromFile() {
  FILE *file = fopen(USER_DATABASE, "r");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%ld,%lf\n",users[num_users].username, users[num_users].password,users[num_users].security_question,users[num_users].security_answer,&users[num_users].account_number,&users[num_users].balance) == 6) {
    num_users++;
  }

  fclose(file);
}

int admin_login() {
  system("clear");;
  char a_pwd[100];
  printf("\n");
  printf("Enter admin password: ");
  clearInputBuffer();

  getPassword(a_pwd, sizeof(a_pwd));
  char o_pwd[100] = "jdg";

  if (strcmp(a_pwd, o_pwd) == 0)
    return 1;
  else
    return -1;
}

int ad_menu() {
  system("clear");;
  int choice;

  do {
    printf("\nAdmin Menu:\n");
    printf("\n");
    printf("1. Add a Train \n");
    printf("\n");
    printf("2. Remove a Train\n");
    printf("\n");
    printf("3. View list of all Trains\n");
    printf("\n");
    printf("4. View Customer Feedback\n");
    printf("\n");
    printf("5. View all bookings.\n");
    printf("\n");
    printf("\n");
    printf("6. Exit\n");
    printf("\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      addTrain();
      system("clear");;
      break;
    case 2:
      removeTrain();
      break;
    case 3:
      displayallTrains();
      break;
    case 4:
      viewCustomerFeedback();
      break;
    case 5:
      printBookings(bookings,numBookings);
      break;
    case 6:
      printf("Logging out...\n");
      system("clear");;

      break;

    default:
      printf("Invalid choice. Please enter a number between 1 and 5.\n");
    }
  } while (choice != 6);

  return 0;
}

int user_menu() {
  
  int uchoice;
  printf("\n");
  do {
    printf("\n1. Search for Trains and book ticket\n");
    printf("\n");
    printf("2. Check Ticket Status\n");
    printf("\n");
    printf("3. Open JDG e-wallet\n\n");
    printf("4. Cancel Booked Ticket\n\n");
    printf("5. Logout\n\n");

    printf("Enter your choice: ");

    scanf("%d", &uchoice);
    getchar();
    long int p;
    switch (uchoice) {
    case 1:
      system("clear");
      checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,num_nighttrains);
      break;

    case 2:
      system("clear");
      printf("\nEnter a valid PNR:");
      scanf("%ld",&p);
      //clearInputBuffer();
      check_status(p);
      break;

    case 3:
      pay(0);
      system("clear");;
      break;

    case 4:
      system("clear");
      printf("Enter PNR:");
      scanf("%ld",&p);
      clearInputBuffer();
      CancelTickets(p);
      break;

    case 5:
      printf("Logging out...\n");
      system("clear");;
      break;
    }

  } while (uchoice != 5);
  return 0;
}

void readDAYTrainsFromFile(dayTrain daytrains[], int *num_daytrains) {
  FILE *file = fopen(DAYTRAIN_DATABASE, "rb");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  *num_daytrains = fread(daytrains, sizeof(dayTrain), MAX_TRAINS, file);

  fclose(file);
}

void readNIGHTTrainsFromFile(nightTrain nighttrains[], int *num_nighttrains) {
  FILE *file = fopen(NIGHTTRAIN_DATABASE, "rb");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  *num_nighttrains = fread(nighttrains, sizeof(nightTrain), MAX_TRAINS, file);

  fclose(file);
}

void writeDAYTrainsToFile(dayTrain daytrains[], int num_daytrains) {
  FILE *file = fopen(DAYTRAIN_DATABASE, "wb");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  fwrite(daytrains, sizeof(dayTrain), num_daytrains, file);

  fclose(file);
}

void writeNIGHTTrainsToFile(nightTrain nighttrains[], int num_nighttrains) {
  FILE *file = fopen(NIGHTTRAIN_DATABASE, "wb");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  fwrite(nighttrains, sizeof(nightTrain), num_nighttrains, file);

  fclose(file);
}

int addTrain() {
  system("clear");;
  setColorCyan();
  printf("\nPlease enter details of the train to be added as asked\n");
  printf("---------------------------------------------------------------------"
         "--------------------------\n");
  resetColor();
  printf("\nIs the train a Day Train or a Night Train?\n");
  printf("\n");
  printf("Enter 0 for day train and 1 for night train:");
  int a;
  scanf("%d", &a);
  printf("\n");
  if (a == 0) {
    // DAY TRAIN
    system("clear");;
    setColorYellow();
    printf("You have Selected Day Train\n");
    resetColor();
    printf("Enter Train ID:");
    long int trainid;
    scanf("%ld", &trainid);
    clearInputBuffer();
    daytrains[num_daytrains].trainID = trainid;
    printf("\n");
    printf("Enter the Train Name:");
    fgets(daytrains[num_daytrains].trainName, 50, stdin);
    printf("\n");
    printf("Input the stations to which the train travels from the list of "
           "stations:\n");
    for (int i = 0; i < 11; i++) {
      printf("  %d.     %s\n", i + 1, stations[i]);
    }

    printf("\n");
    printf("ENTER ORIGIN STATION (1-11) :");
    int os;
    scanf("%d", &os);
    clearInputBuffer();
    strcpy(daytrains[num_daytrains].route[0], stations[os - 1]);
    printf("\n");
    printf("Enter the time at which train reaches origin station in HH:MM -");
    int hours1, minutes1;
    getTimeFromUser(&hours1, &minutes1);
    long int time1 = convertTimeToLongInt(hours1, minutes1);
    daytrains[num_daytrains].time[0] = time1;
    printf("\n");
    printf("ENTER NUMBER OF STOPS: ");
    int n;
    scanf("%d", &n);
    clearInputBuffer();

    for (int i = 0; i < n; i++) {
      int stop, hours, minutes;
      printf("\n");
      printf("Choose Stop %d from list of stops  :- ", i + 1);
      scanf("%d", &stop);
      clearInputBuffer();
      strcpy(daytrains[num_daytrains].route[i + 1], stations[stop - 1]);
      printf("\n");
      printf("Enter Time at which %s is reached in HH:MM :",
             stations[stop - 1]);
      getTimeFromUser(&hours, &minutes);
      long int time = convertTimeToLongInt(hours, minutes);
      daytrains[num_daytrains].time[i + 1] = time;
      printf("Enter distance from previous stop to this stop in KM : ");
      scanf("%d", &daytrains[num_daytrains].distance[i]);
      clearInputBuffer();
    }

    printf("\n");
    printf("ENTER DESTINATION STATION (1-11) :");
    int ds;
    scanf("%d", &ds);
    clearInputBuffer();
    printf("Enter distance from previous stop to this stop in KM : ");
    scanf("%d", &daytrains[num_daytrains].distance[n]);
    clearInputBuffer();
    strcpy(daytrains[num_daytrains].route[n + 1], stations[ds - 1]);
    printf("\n");
    printf(
        "Enter the time at which train reaches destination station in HH:MM -");
    int hours2, minutes2;
    getTimeFromUser(&hours2, &minutes2);
    long int time2 = convertTimeToLongInt(hours2, minutes2);
    daytrains[num_daytrains].time[n + 1] = time2;

    for (int i = n + 2; i < 11; i++) {
      strcpy(daytrains[num_daytrains].route[i], "O");
      daytrains[num_daytrains].time[i] = 0;
    }
    printf("\n");
    printf("ENTER DATE OF THE TRAIN (YYYY-MM-DD):");
    printf("\n");
    daytrains[num_daytrains].date = getDateAsLong();

    // seat available=1 not available=0
    for (int i = 0; i < 840; i++) {
      daytrains[num_daytrains].normalAvail[i] =
          1; // initially all seats available
      if (i % 2 == 0) {
        daytrains[num_daytrains].normalPref[i] = 'w';
      } // w for window
      else {
        daytrains[num_daytrains].normalPref[i] = 'a';
      } // a for aisle
    }
    for (int i = 0; i < 300; i++) {
      daytrains[num_daytrains].acAvail[i] = 1; // initially all seats available
      if (i % 2 == 0) {
        daytrains[num_daytrains].acPref[i] = 'w';
      } // w for window
      else {
        daytrains[num_daytrains].acPref[i] = 'a';
      } // a for aisle
    }
    for (int i = 0; i < 90; i++) {
      daytrains[num_daytrains].execAvail[i] = 1; // initially all seats
                                                 // available
      if (i % 2 == 0) {
        daytrains[num_daytrains].execPref[i] = 'w';
      } // w for window
      else {
        daytrains[num_daytrains].execPref[i] = 'a';
      } // a for aisle
    }
    daytrains[num_daytrains].numdaybookings = 0;
    daytrains[num_daytrains].acwlnum=0;
    num_daytrains++;

    setColorGreen();
    printf("TRAIN SUCCESSFULLY ADDED TO DATABASE");
    resetColor();

    return 0;

  } else if (a == 1) {
    // NIGHT TRAIN
    printf("\n");
    system("clear");;
    setColorBlue();
    printf("You have Selected Night Train\n");
    resetColor();
    printf("Enter Train ID:");
    long int trainid;
    scanf("%ld", &trainid);
    clearInputBuffer();
    nighttrains[num_nighttrains].trainID = trainid;
    printf("\n");
    printf("Enter the Train Name:");
    fgets(nighttrains[num_nighttrains].trainName, 50, stdin);
    printf("\n");
    printf("Input the stations to which the train travels from the list of "
           "stations:\n");
    for (int i = 0; i < 11; i++) {
      printf("  %d.     %s\n", i + 1, stations[i]);
    }
    printf("\n");
    printf("ENTER ORIGIN STATION (1-11) :");
    int os;
    scanf("%d", &os);
    clearInputBuffer();
    strcpy(nighttrains[num_nighttrains].route[0], stations[os - 1]);
    printf("\n");
    printf("Enter the time at which train reaches origin station in HH:MM -");
    int hours1, minutes1;
    getTimeFromUser(&hours1, &minutes1);
    long int time1 = convertTimeToLongInt(hours1, minutes1);
    nighttrains[num_nighttrains].time[0] = time1;
    printf("\n");
    printf("ENTER NUMBER OF STOPS: ");
    int n;
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
      int stop, hours, minutes;
      printf("\n");
      printf("Choose Stop %d from list of stops  :- ", i + 1);
      scanf("%d", &stop);
      clearInputBuffer();
      strcpy(nighttrains[num_nighttrains].route[i + 1], stations[stop - 1]);
      printf("\n");
      printf("Enter Time at which %s is reached in HH:MM :",
             stations[stop - 1]);
      getTimeFromUser(&hours, &minutes);
      long int time = convertTimeToLongInt(hours, minutes);
      nighttrains[num_nighttrains].time[i + 1] = time;
      printf("Enter distance from previous stop to this stop in KM : ");
      scanf("%d", &nighttrains[num_nighttrains].distance[i]);
      clearInputBuffer();
    }

    printf("\n");
    printf("ENTER DESTINATION STATION (1-11) :");
    int ds;
    scanf("%d", &ds);
    clearInputBuffer();
    printf("Enter distance from previous stop to this stop in KM : ");
    scanf("%d", &nighttrains[num_nighttrains].distance[n]);
    clearInputBuffer();
    strcpy(nighttrains[num_nighttrains].route[n + 1], stations[ds - 1]);
    printf("\n");
    printf("Enter the time at which train reaches destination station in HH:MM: ");
    int hours2, minutes2;
    getTimeFromUser(&hours2, &minutes2);
    long int time2 = convertTimeToLongInt(hours2, minutes2);
    nighttrains[num_nighttrains].time[n + 1] = time2;

    for (int i = n + 2; i < 11; i++) {
      strcpy(nighttrains[num_nighttrains].route[i], "O");
      nighttrains[num_nighttrains].time[i] = 0;
    }
    printf("\n");
    printf("ENTER DATE OF THE TRAIN (YYYY-MM-DD):");
    nighttrains[num_nighttrains].date = getDateAsLong();
    // seat available=1 not available=0
    // SLEEPER
    for (int i = 0; i < 864; i++) {
      nighttrains[num_nighttrains].sleeperAvail[i] = 1; // initially all seats available
    }
    for (int i = 0; i < 864; i += 3) {
      nighttrains[num_nighttrains].sleeperPref[i] = 'l';     // l=lower
      nighttrains[num_nighttrains].sleeperPref[i + 1] = 'm'; // m=middle
      nighttrains[num_nighttrains].sleeperPref[i + 2] = 'u'; // u=upper
    }

    // 3rd AC
    for (int i = 0; i < 256; i++) {
      nighttrains[num_nighttrains].ac3Avail[i] =
          1; // initially all seats available
    }

    char pattern[] = {'l', 'm', 'u'};
    int patternSize = sizeof(pattern) / sizeof(pattern[0]);

    for (int i = 0; i < 256; i++) {
      nighttrains[num_nighttrains].ac3Pref[i] = pattern[i % patternSize];
    }

    // 2nd AC
    for (int i = 0; i < 96; i++) {
      nighttrains[num_nighttrains].ac2Avail[i] =
          1; // initially all seats available
    }

    for (int i = 0; i < 96; ++i) {
      nighttrains[num_nighttrains].ac2Pref[i] = pattern[i % patternSize];
    }

    // 1st AC
    for (int i = 0; i < 16; i++) {
      nighttrains[num_nighttrains].ac1Avail[i] =
          1; // initially all seats available
    }

    for (int i = 0; i < 16; i += 3) {
      nighttrains[num_nighttrains].ac1Pref[i] = 'l';     // l=lower
      nighttrains[num_nighttrains].ac1Pref[i + 1] = 'm'; // m=middle
      nighttrains[num_nighttrains].ac1Pref[i + 2] = 'u'; // u=upper
    }
    nighttrains[num_nighttrains].numnightbookings = 0;
    num_nighttrains++;
    printf("\n");
    setColorGreen();
    printf("TRAIN SUCCESSFULLY ADDED TO DATABASE");
    resetColor();
    return 0;
  }

  return 0;
}

int removeTrain() {
  system("clear");;
  setColorCyan();
  printf("\nPlease enter details of the train to be removed as asked\n");
  printf("---------------------------------------------------------------------"
         "--------------------------\n");
  resetColor();
  printf("\nIs the train a Day Train or a Night Train?\n");
  printf("\n");
  printf("Enter 0 for day train and 1 for night train: ");
  int a;
  scanf("%d", &a);
  printf("\n");

  printf("Enter Train ID: ");
  long int trainid;
  scanf("%ld", &trainid);
  printf("\n");

  printf("Enter DATE OF THE TRAIN (YYYYMMDD): ");
  long int date;
  scanf("%ld", &date);
  printf("\n");

  if (a == 0) {

    int index = -1;
    for (int i = 0; i < num_daytrains; i++) {
      if (daytrains[i].trainID == trainid && daytrains[i].date == date) {
        index = i;
        break;
      }
    }
    if (index == -1) {
      setColorRed();
      printf("TRAIN NOT FOUND IN DATABASE\n");
      resetColor();
      return 0;
    }

    for (int i = index; i < num_daytrains - 1; i++) {
      daytrains[i] = daytrains[i + 1];
    }
    num_daytrains--;
    setColorGreen();
    printf("TRAIN SUCCESSFULLY REMOVED FROM DATABASE\n");
    resetColor();
  } else if (a == 1) {

    int index = -1;
    for (int i = 0; i < num_nighttrains; i++) {
      if (nighttrains[i].trainID == trainid && nighttrains[i].date == date) {
        index = i;
        break;
      }
    }
    if (index == -1) {
      setColorRed();
      printf("TRAIN NOT FOUND IN DATABASE\n");
      resetColor();
      return 0;
    }
    // Shift trains in array to remove the found train
    for (int i = index; i < num_nighttrains - 1; i++) {
      nighttrains[i] = nighttrains[i + 1];
    }
    num_nighttrains--;
    setColorGreen();
    printf("TRAIN SUCCESSFULLY REMOVED FROM DATABASE\n");
    resetColor();
  } else {
    setColorRed();
    printf("INVALID SELECTION\n");
    resetColor();
  }

  return 0;
}

int pay(double amount) {
  system("clear");;
  char username[MAX_USERNAME_LEN];
  long int account_number;
  //int amount;
  int choice;
  int deposit_amount;
  int index;
  int valid_input = 0;
  if(amount!=0){printf("\nYour Calculated Fare is %lf\n",amount);}

  do {
    
    printf("Enter account number: ");
    scanf("%ld", &account_number);
    printf("Enter your username: ");
    scanf("%s", username);
    system("clear");;

    for (int i = 0; i < num_users; i++) {
      if (users[i].account_number == account_number &&
          strcmp(users[i].username, username) == 0) {
        valid_input = 1;
        index = i;
        break;
      }
    }

    if (!valid_input) {
      printf("Wrong username or account number. Please try again.\n");
      continue;
    }

    do {
      printf("\nMenu:\n\n");
      printf("1. View Balance\n\n");
      printf("2. Make Payment\n\n");
      printf("3. Deposit\n\n");
      printf("4. Exit\n\n");
      printf("Enter your choice: ");
      scanf("%d", &choice);

      switch (choice) {
      case 1:
        system("clear");;
        setColorYellow();
        printf("Your balance is: %lf\n", users[index].balance);
        resetColor();
        break;

      case 2:
        system("clear");;
        // printf("Enter amount to be paid:");
        // scanf("%lf", &amount);
        if (users[index].balance >= amount) {
          users[index].balance -= amount;
          setColorGreen();
          printf("Payment successful. Remaining balance: %lf\n", users[index].balance);
          resetColor();
          saveUsersToFile();
          return 1;
        } else {
          setColorRed();
          printf("Insufficient balance for payment.\n");
          resetColor();
          return 0;
        }
        break;

      case 3:
        system("clear");;
        printf("Enter amount to deposit: ");
        scanf("%d", &deposit_amount);
        if (deposit_amount > 0) {
          users[index].balance += deposit_amount;
          setColorGreen();
          printf("Deposit successful. New balance: %lf\n", users[index].balance);
          resetColor();
          saveUsersToFile();
        } else {
          printf("Invalid deposit amount.\n");
        }
        break;

      case 4:
        printf("Exiting...\n");
        system("clear");;
        break;

      default:
        printf("Invalid choice. Please try again.\n");
      }
    } while (choice != 4);

  } while (!valid_input);
}

int generatePNR() {

  initializeRandom();
  const char *filename = "random_numbers3.txt";

  int randomNumber = generateRandom(100000, 999999, filename);
  setColorCyan();
  // printf("Your PNR generated is: %d\n", randomNumber);
  resetColor();

  writeRandomNumberToFile(filename, randomNumber);

  return randomNumber;
}

long int convertTimeToLongInt(int hours, int minutes) {
  return hours * 100 + minutes;
}

void getTimeFromUser(int *hours, int *minutes) {
  char input[6];

  if (fgets(input, sizeof(input), stdin) != NULL) {

    if (sscanf(input, "%2d:%2d", hours, minutes) != 2) {
      printf("Invalid time format.\n");
    }
  } else {
    printf("Error reading input.\n");
  }
}

long int getDateAsLong() {
  char dateStr[11];
  char cleanDateStr[9];
  int i, j;

  scanf("%10s", dateStr);

  for (i = 0, j = 0; i < 10; i++) {
    if (dateStr[i] != '-') {
      cleanDateStr[j++] = dateStr[i];
    }
  }
  cleanDateStr[j] = '\0';

  long int dateLong = atol(cleanDateStr);

  return dateLong;
}

int displayallTrains() {
  system("clear");;
  printf("FOLLOWING TRAINS HAVE BEEN ADDED SO FAR\n");
  setColorMagenta();
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DAY "
         "TRAINS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  resetColor();
  for (int i = 0; i < num_daytrains; i++) {

    char dateStr[11];
    longIntToDateStr(daytrains[i].date, dateStr);

    printf("Date:     %s\n", dateStr);

    printf("\nTrain ID:     %ld\n", daytrains[i].trainID);
    printf("\nTrain Name:     %s\n", daytrains[i].trainName);
    printf("\n");

    printf("Stops:\n");
    long int j = 0;
    while (daytrains[i].time[j] != 0) {
      char buffer[6];
      convertTime(daytrains[i].time[j], buffer);
      printf("%s  -------->   %s\n", buffer, daytrains[i].route[j]);
      j++;
    }
    // printf("Distance: ");
    // for (int ij = 0; ij < 10; ij++) {
    //   if (daytrains[i].distance[ij] != 0) {
    //     printf("%d, ", daytrains[i].distance[ij]);
    //   }
    // }

    setColorYellow();
    printf("-------------------------------------------------------------------"
           "--------------------------------");
    printf("\n");
    resetColor();
  }

  setColorMagenta();
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~NIGHT "
         "TRAINS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  resetColor();
  for (int i = 0; i < num_nighttrains; i++) {

    char dateStr[11];
    longIntToDateStr(nighttrains[i].date, dateStr);

    printf("Date:     %s\n", dateStr);

    printf("\nTrain ID:     %ld\n", nighttrains[i].trainID);
    printf("\nTrain Name:     %s\n", nighttrains[i].trainName);
    printf("\n");

    printf("Stops:\n");
    long int j = 0;
    while (nighttrains[i].time[j] != 0) {
      char buffer[6];
      convertTime(nighttrains[i].time[j], buffer);
      printf("%s  -------->   %s\n", buffer, nighttrains[i].route[j]);
      j++;
    }

    // printf("Distance");
    // for (int ij = 0; ij < 10; ij++) {
    //   if (nighttrains[i].distance[ij] != 0) {
    //     printf("%d, ", nighttrains[i].distance[ij]);
    //   }
    // }

    setColorYellow();
    printf("-------------------------------------------------------------------"
           "--------------------------------");
    printf("\n");
    resetColor();
  }

  return 0;
}

void longIntToDateStr(long int date, char *dateStr) {
  int year, month, day;
  year = date / 10000;
  month = (date / 100) % 100;
  day = date % 100;

  sprintf(dateStr, "%04d-%02d-%02d", year, month, day);
}

void combineStringAndLong(const char *str, long int num, char *result,size_t resultSize) {
  // Ensure that the result buffer is large enough
  if (snprintf(result, resultSize, "%s%ld", str, num) >= resultSize) {
    printf("Error: Result buffer is too small.\n");
    exit(1);
  }
}

void convertTime(long int time, char *buffer) {
  int hours = time / 100;
  int minutes = time % 100;

  if (hours > 23 || minutes > 59) {
    fprintf(stderr, "Invalid time format\n");
    exit(1);
  }

  sprintf(buffer, "%02d:%02d", hours, minutes);
}

void clearInputBuffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}


void writeFeedbacksToFile(const char *filename, char strings[][MAX_LENGTH], int count) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    perror("Error opening file for writing");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < count; ++i) {
    fprintf(file, "%s\n", strings[i]);
  }

  fclose(file);
}

void readFeedbacksFromFile(const char *filename, char strings[][MAX_LENGTH], int *count) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening file for reading");
    exit(EXIT_FAILURE);
  }

  *count = 0;
  while (fgets(strings[*count], MAX_LENGTH, file) != NULL) {
    // Remove newline character if present
    strings[*count][strcspn(strings[*count], "\n")] = '\0';
    (*count)++;
  }

  fclose(file);
}

int viewCustomerFeedback() {
  int ftype[100];
  int positive = 0;
  int negative = 0;
  // 1 for positive and 0 for negative
  for (int i = 0; i < num_feedbacks; i++) {
    int temp;
    temp = classify_sentence(feedbacks[i]);
    if (temp == 1) {
      positive++;
    } else {
      negative++;
    }
    ftype[i] = temp;
  }
  setColorGreen();
  printf("Total Number of Positive feedbacks:");
  printf("%d", positive);
  resetColor();
  printf("\n--------------------------------------------------------\n");
  setColorRed();
  printf("Total Number of Negative feedbacks:");
  printf("%d", negative);
  resetColor();
  printf("\n--------------------------------------------------------\n");
  printf("Overall Plot of the feedbacks:\n");
  display_histogram(positive, negative);
  printf("--------------------------------------------------------");
  positive > negative ? printf("\nOverall feedback is POSITIVE\n")
                      : printf("\nOverall feedback is NEGATIVE\n");
  return 0;
}

void readTicketsFromFile(TrainTicket tickets[], int *numTickets) {
  FILE *file = fopen("tickets.bin", "rb");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  *numTickets = fread(tickets, sizeof(TrainTicket), 3000, file);
  fclose(file);
}

void writeTicketsToFile(TrainTicket tickets[], int numTickets) {
  FILE *file = fopen("tickets.bin", "wb");
  if (file == NULL) {
    printf("Error opening file\n");
    return;
  }

  fwrite(tickets, sizeof(TrainTicket), numTickets, file);
  fclose(file);
}

void getTimeOfBooking(char *buffer, int len) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  snprintf(buffer, len, "%02d-%02d-%02d %02d:%02d:%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void printTicket(TrainTicket tickets[], int numTickets) {
  FILE *file;
  char filename[150];

  // Create filename using ticketName
  snprintf(filename, sizeof(filename), "%s.txt",
           tickets[numTickets].ticketName);

  file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Error opening file for writing\n");
    return;
  }

  TrainTicket *ticket = &tickets[numTickets];

  fprintf(file, "Ticket Name: %s\n", ticket->ticketName);
  fprintf(file, "Train Name: %s\n", ticket->trainName);
  fprintf(file, "PNR: %ld\n", ticket->PNR);
  fprintf(file, "Source: %s\n", ticket->source);
  fprintf(file, "Destination: %s\n", ticket->destination);
  fprintf(file, "Date of Train: %ld\n", ticket->dateOfTrain);
  fprintf(file, "Departure Time: %ld\n", ticket->departureTime);
  fprintf(file, "Time of Booking: %s\n", ticket->timeOfBooking);
  fprintf(file, "Number of Passengers: %d\n\n", ticket->numberOfPassengers);

  for (int i = 0; i < ticket->numberOfPassengers; i++) {
    fprintf(file, "Passenger %d:\n", i + 1);
    fprintf(file, "  Name: %s\n", ticket->names[i]);
    fprintf(file, "  Class: %s\n", ticket->class[i]);
    fprintf(file, "  Type: %c\n", ticket->type[i]);
    fprintf(file, "  Seat Number: %d\n", ticket->seatNumber[i]);
    fprintf(file, "  Gender: %c\n\n", ticket->gender[i]);
  }

  fclose(file);
}

void displayTicket(TrainTicket tickets[], int numTickets) {
  
  //printf("\033[H\033[J");

  TrainTicket *ticket = &tickets[numTickets];

  printf("=============================================================\n");
  printf("                      Train Ticket                           \n");
  printf("=============================================================\n");
  printf("Ticket Name       : %s\n", ticket->ticketName);
  printf("Train Name        : %s\n", ticket->trainName);
  printf("PNR               : %ld\n", ticket->PNR);
  printf("Source            : %s\n", ticket->source);
  printf("Destination       : %s\n", ticket->destination);
  printf("Date of Train     : %ld\n", ticket->dateOfTrain);
  printf("Departure Time    : %ld\n", ticket->departureTime);
  printf("Time of Booking   : %s\n", ticket->timeOfBooking);
  printf("-------------------------------------------------------------\n");
  printf("Number of Passengers: %d\n", ticket->numberOfPassengers);
  printf("-------------------------------------------------------------\n");

  for (int i = 0; i < ticket->numberOfPassengers; i++) {
    printf("Passenger %d:\n", i + 1);
    printf("  Name       : %s\n", ticket->names[i]);
    printf("  Class      : %s\n", ticket->class[i]);
    printf("  Type       : %c\n", ticket->type[i]);
    printf("  Seat Number: %d\n", ticket->seatNumber[i]);
    printf("  Gender     : %c\n", ticket->gender[i]);
    printf("-------------------------------------------------------------\n");
  }
  printf("=============================================================\n");
}

int countOnes(int arr[], int size) {
  int count = 0;
  for (int i = 0; i < size; i++) {
    if (arr[i] == 1) {
      count++;
    }
  }
  return count;
}

void modifyString(char *str, int num) {
    // Ensure the number is in the valid range (0-99) for a 5-char string
    if (num < 0 || num > 99) {
        printf("Number out of range.\n");
        return;
    }

    // Format the string as "WL" followed by the integer
    snprintf(str, 5, "WL%d", num);

    // Ensure the string is null-terminated
    str[4] = '\0';
}

int findStringIndex(char array[][20], int size, const char *target) {
  for (int i = 0; i < size; i++) {
    if (strcmp(array[i], target) == 0) {
      return i; // Return the index if the target string is found
    }
  }
  return -1; // Return -1 if the target string is not found
}

void swapDayTrains(dayTrain *a, dayTrain *b) {
  dayTrain temp = *a;
  *a = *b;
  *b = temp;
}
void swapNightTrains(nightTrain *a, nightTrain *b) {
  nightTrain temp = *a;
  *a = *b;
  *b = temp;
}

void sortDayTrains(dayTrain daytrains[], int num_daytrains) {
  int i, j;
  for (i = 0; i < num_daytrains - 1; i++) {
    for (j = 0; j < num_daytrains - i - 1; j++) {
      if (daytrains[j].date > daytrains[j + 1].date ||
          (daytrains[j].date == daytrains[j + 1].date &&
           daytrains[j].time[0] > daytrains[j + 1].time[0])) {
        swapDayTrains(&daytrains[j], &daytrains[j + 1]);
      }
    }
  }
}

void sortNightTrains(nightTrain nighttrains[], int num_nighttrains) {
  int i, j;
  for (i = 0; i < num_nighttrains - 1; i++) {
    for (j = 0; j < num_nighttrains - i - 1; j++) {
      if (nighttrains[j].date > nighttrains[j + 1].date ||
          (nighttrains[j].date == nighttrains[j + 1].date &&
           nighttrains[j].time[0] > nighttrains[j + 1].time[0])) {
        swapNightTrains(&nighttrains[j], &nighttrains[j + 1]);
      }
    }
  }
}

double calculateFare(long int trainID, const char* source, const char* destination, int numPassengers, const char* seatClass) {
    // Find the train by ID and type
    int found = 0;
    int isDayTrain = 0;
    int sourceIndex = -1;
    int destinationIndex = -1;
    int ddistance = 0;
    int ndistance=0;

    // Search in day trains
    for (int i = 0; i < num_daytrains; i++) {
        if (daytrains[i].trainID == trainID) {
            found = 1;
            isDayTrain = 1;
            for (int j = 0; j < TOTAL_STATIONS; j++) {
                if (strcmp(daytrains[i].route[j], source) == 0) {
                    sourceIndex = j;
                }
                if (strcmp(daytrains[i].route[j], destination) == 0) {
                    destinationIndex = j;
                }
            }
            if (sourceIndex != -1 && destinationIndex != -1 && sourceIndex < destinationIndex) {
                for (int j = sourceIndex; j < destinationIndex; j++) {
                    ddistance += daytrains[i].distance[j];
                }
            } else {
                setColorRed();
                printf("Invalid source or destination stations.\n");
                resetColor();
                return -1;
            }
            break;
        }
    }

    // Search in night trains if not found in day trains
    if (!found) {
        for (int i = 0; i < num_nighttrains; i++) {
            if (nighttrains[i].trainID == trainID) {
                found = 1;
                for (int j = 0; j < TOTAL_STATIONS; j++) {
                    if (strcmp(nighttrains[i].route[j], source) == 0) {
                        sourceIndex = j;
                    }
                    if (strcmp(nighttrains[i].route[j], destination) == 0) {
                        destinationIndex = j;
                    }
                }
                if (sourceIndex != -1 && destinationIndex != -1 && sourceIndex < destinationIndex) {
                    for (int j = sourceIndex; j < destinationIndex; j++) {
                        ndistance += nighttrains[i].distance[j];
                    }
                } else {
                    setColorRed();
                    printf("Invalid source or destination stations.\n");
                    resetColor();
                    return -1;
                }
                break;
            }
        }
    }

    if (!found) {
        setColorRed();
        printf("Train ID not found.\n");
        resetColor();
        return -1;
    }

    // Calculate fare
    double farePerKm = 0;
    if (isDayTrain) {
        if (strcmp(seatClass, "NC") == 0) {
            farePerKm = dayNormalFare;
        } else if (strcmp(seatClass, "AC") == 0) {
            farePerKm = dayACFare;
        } else if (strcmp(seatClass, "EC") == 0) {
            farePerKm = dayExecFare;
        } else {
            setColorRed();
            printf("Invalid seat class for day train.\n");
            resetColor();
            return -1;
        }
    } else {
        if (strcmp(seatClass, "SL") == 0) {
            farePerKm = nightSleeperFare;
        } else if (strcmp(seatClass, "3A") == 0) {
            farePerKm = nightAC3Fare;
        } else if (strcmp(seatClass, "2A") == 0) {
            farePerKm = nightAC2Fare;
        } else if (strcmp(seatClass, "1A") == 0) {
            farePerKm = nightAC1Fare;
        } else {
            setColorRed();
            printf("Invalid seat class for night train.\n");
            resetColor();
            return -1;
        }
    }

    if(ddistance!=0){
      return farePerKm * ddistance * numPassengers;
    }
    return farePerKm * ndistance * numPassengers;

    // double totalFare = farePerKm * distance * numPassengers;
    // return totalFare;
}

long int checkRouteAndPrintInfo(dayTrain daytrains[], int num_daytrains, nightTrain nighttrains[], int num_nighttrains) 
{
  char source[20];
  char destination[20];

  printf("Enter source station: ");
  fgets(source, sizeof(source), stdin);
  source[strcspn(source, "\n")] = '\0'; // Remove the newline character

  printf("Enter destination station: ");
  fgets(destination, sizeof(destination), stdin);
  destination[strcspn(destination, "\n")] = '\0'; // Remove the newline character

  long int date;
  char sdate[20];
  printf("Enter date(YYYYMMDD): ");
  scanf("%ld", &date);
  longIntToDateStr(date, sdate);

  int i, j;
  int found = 0; // Flag to check if any train with the given route was found

  sortDayTrains(daytrains, num_daytrains);
  sortNightTrains(nighttrains, num_nighttrains);

  // Check day trains
  for (i = 0; i < num_daytrains; i++) 
  {
    // Check if both source and destination are present in the route of the
    // current day train
    int sourceIndex = -1, destinationIndex = -1;
    for (j = 0; j < TOTAL_STATIONS; j++) {
      if (strcmp(daytrains[i].route[j], source) == 0)
        sourceIndex = j;
      if (strcmp(daytrains[i].route[j], destination) == 0)
        destinationIndex = j;
    }
    char str[20];
    longIntToDateStr(daytrains[i].date, str);

    // If both source and destination are present
    if (sourceIndex != -1 && destinationIndex != -1 &&
        sourceIndex < destinationIndex && strcmp(sdate, str) == 0) {
      printf("\n---------------------------------------------------------------------------------------------------------\n");

      setColorYellow();
      printf("Train Name: %s\n", daytrains[i].trainName);
      printf("Train ID: %ld\n", daytrains[i].trainID);
      resetColor();
      printf("Date: ");
      longIntToDateStr(daytrains[i].date, str);
      printf("%s", str);
      printf("\n\n");
      printf("Time: ");
      char buffer[20];
      convertTime(daytrains[i].time[0], buffer);
      printf("\n%s", buffer);
      setColorYellow();
      printf("\n---------------------------------------------------------------------------------------------------------\n");
      resetColor();
      printf("\n\n");
      found = 1;
    }
  }

  // Check night trains
  for (i = 0; i < num_nighttrains; i++) 
  {
    // Check if both source and destination are present in the route of the
    // current night train
    int sourceIndex = -1, destinationIndex = -1;
    for (j = 0; j < TOTAL_STATIONS; j++) {
      if (strcmp(nighttrains[i].route[j], source) == 0)
        sourceIndex = j;
      if (strcmp(nighttrains[i].route[j], destination) == 0)
        destinationIndex = j;
    }
    // If both source and destination are present
    char str[20];
    longIntToDateStr(nighttrains[i].date, str);

    if (sourceIndex != -1 && destinationIndex != -1 &&
        sourceIndex < destinationIndex && strcmp(sdate, str) == 0) {
      printf("Train Name: %s\n", nighttrains[i].trainName);
      printf("Train ID: %ld\n", nighttrains[i].trainID);
      printf("Date: ");

      longIntToDateStr(nighttrains[i].date, str);
      printf("%s", str);
      printf("\n\n");
      printf("Time: ");
      char buffer[20];
      convertTime(nighttrains[i].time[0], buffer);
      printf("\n%s", buffer);
      printf("\n\n");
      found = 1;
    }
  }

  // If no train with the given route was found
  if (!found) 
  {
    printf("No trains found with route from %s to %s.\n", source, destination);
    return -1;
  } 
  else 
  {
    long int f_tid;
    char output[20];
    printf("Enter train ID: ");
    scanf("%ld", &f_tid);
    getchar();
    char final[10];
    // snprintf(output, 20, "%s 0", f_tid);
    // strcat(f_tid," 2");
    int type = 0; // type=1 for day, type=-1 for night
    int ind = 0;
    for (int i = 0; i < num_daytrains; i++) 
    {
      if (daytrains[i].trainID == f_tid) 
      {
        type = 1;
        ind = i;
        break;
      }
    }
    for (int j = 0; j < num_nighttrains; j++) 
    {
      if (nighttrains[j].trainID == f_tid) 
      {
        type = -1;
        ind = j;
        break;
      }
    }

    if (type == 1) 
    {
      int numPass;
      char names[100][20];
      char pref[20];
      char gender[20];
      int seats[20];
      char class[3][20];
      char usern[20];
      system("clear");;
      printf("Enter your account username:");
      fgets(usern, sizeof(usern), stdin);
      for(int z=0;z<20;z++){
        if(usern[z]=='\n'){
          usern[z]='\0';
        }
      }
      printf("Enter the Number of Passengers:");
      scanf("%d", &numPass);
      clearInputBuffer();
      printf("\n\n");
      setColorBlue();
      printf("Enter Type of Seat:\n\n");
      printf("NC : Normal Chair Car\n\n");
      printf("AC : AC Chair Car\n\n");
      printf("EC : Executive Chair Car\n");
      resetColor();
      char type[3];
      scanf("%s", type);
      clearInputBuffer();
      if (strcmp(type, "NC") == 0) {
        if (countOnes(daytrains[ind].normalAvail, 840) >= numPass) {
          printf("Seats Available\n");

          for (int i = 0; i < numPass; i++) {
            char pre;
            char gen;
            char name[100];
            int seat;

            printf("Enter Name of Passenger (%d):", i + 1);
            scanf("%s", name);
            clearInputBuffer();
            strcpy(names[i], name);
            printf("Enter Gender(M or F):");
            scanf("%c", &gen);
            clearInputBuffer();
            gender[i] = gen;
            printf("Enter Seat Preference(w or a) for Passenger-");
            scanf("%c", &pre);
            clearInputBuffer();
            pref[i] = pre;
            strcpy(class[i], "NC");
            int seatalot=0;
            for (int k = 0; k < 840; k++) {
              if (daytrains[ind].normalPref[k] == pre) {
                if (daytrains[ind].normalAvail[k] == 1) {
                  daytrains[ind].normalAvail[k] = 0;
                  seat = k + 1;
                  seatalot=1;
                  break;
                }
              }
            }
            if(seatalot!=1)
            {
              for(int q=0;q<840;q++)
              {
                if(daytrains[ind].normalAvail[q]==1)
                {
                  daytrains[ind].normalAvail[q] = 0;
                  pref[i]=daytrains[ind].normalPref[q];
                  seat =  + 1;
                  seatalot=1;
                  break;
                }
              }

            }
            seats[i] = seat;
          }
          double fare = calculateFare(f_tid, source, destination, numPass, type); // calculate the fare with source and destination and
                           // display it GLN
          int payflag = pay(fare); // use payment module, payflag=0 for failed payment,
                           // payflag=1 for successful payment
          // USE PAYMENT MODULE
          if (payflag == 0) {
            printf("PAYMENT FAILED!!!");
            for (int i = 0; i < numPass; i++) {
              daytrains[ind].normalAvail[seats[i] - 1] =
                  1; // making the seats available again
            }
            checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,
                                   num_nighttrains);
          } else if (payflag == 1) {
            // PAYMENT SUCCESSFUL
            printf("Your Ticket has been SuccessfullY Booked\n");
            char review[200];
            printf("Please Give your Valuable feedback before PNR generation:");
            fgets(review, sizeof(review), stdin);
            strcpy(review, feedbacks[num_feedbacks]);
            num_feedbacks++;
            long int pnr;
            pnr = generatePNR();
            strcpy(bookings[numBookings].user, usern);
            bookings[numBookings].Train_ID = f_tid;
            bookings[numBookings].pnr = pnr;
            bookings[numBookings].numberOfTickets = numPass;
            bookings[numBookings].amount = fare;
            strcpy(bookings[numBookings].status, "CNF");
            strcpy(bookings[numBookings].class, "NC");
            strcpy(bookings[numBookings].dayNight, "D");
            numBookings++;
            for (int i = daytrains[ind].numdaybookings; i < numPass; i++) {
              daytrains[ind].daybookings[i].pnr = pnr;
              strcpy(daytrains[ind].daybookings[i].name, names[i]);
              daytrains[ind].daybookings[i].gender = gender[i];
              daytrains[ind].daybookings[i].seatNo = seats[i];
              strcpy(daytrains[ind].daybookings[i].class, "NC");
              strcpy(daytrains[ind].daybookings[i].source, source);
              strcpy(daytrains[ind].daybookings[i].destination, destination);
              strcpy(daytrains[ind].daybookings[i].status, "CNF");
            }
            daytrains[ind].numdaybookings += numPass;
            // generate ticket and print it and save it as text file
            char ticketname[100];
            combineStringAndLong(usern, pnr, ticketname, sizeof(ticketname));
            strcpy(tickets[numTickets].ticketName, ticketname);
            strcpy(tickets[numTickets].trainName, daytrains[ind].trainName);
            tickets[numTickets].PNR = pnr;
            strcpy(tickets[numTickets].source, source);
            strcpy(tickets[numTickets].destination, destination);
            tickets[numTickets].dateOfTrain = daytrains[ind].date;
            getTimeOfBooking(tickets[numTickets].timeOfBooking,
                             sizeof(tickets[numTickets].timeOfBooking));
            tickets[numTickets].numberOfPassengers = numPass;
            tickets[numTickets].departureTime =
                daytrains[ind].time[findStringIndex(daytrains[ind].route, 11, source)];

            for (int z = 0; z < numPass; z++) {
              strcpy(tickets[numTickets].names[z], names[z]);
              strcpy(tickets[numTickets].class[z], "NC");
              tickets[numTickets].type[z] = pref[z]; // Window seat
              tickets[numTickets].seatNumber[z] = seats[z];
              tickets[numTickets].gender[z] = gender[z];
            }

            printTicket(tickets, numTickets);
            displayTicket(tickets, numTickets);

            numTickets++;
            return 0;
          }
        }

      }

      else if (strcmp(type, "AC") == 0) {
        if (countOnes(daytrains[ind].acAvail, 300) >= numPass) {
          setColorGreen();
          printf("Seats Available\n");
          resetColor();

          for (int i = 0; i < numPass; i++) {
            char pre;
            char gen;
            char name[100];
            int seat=0;
            setColorYellow();
            printf("\n--------------------------------------------------------------------------------------------\n");
            resetColor();

            printf("Enter Name of Passenger (%d):", i + 1);
            scanf("%s", name);
            clearInputBuffer();
            strcpy(names[i], name);
            printf("Enter Gender(M or F):");
            scanf("%c", &gen);
            clearInputBuffer();
            gender[i] = gen;
            printf("Enter Seat Preference(w or a) for Passenger-");
            scanf("%c", &pre);
            clearInputBuffer();
            setColorYellow();
            printf("\n--------------------------------------------------------------------------------------------\n");
            resetColor();
            pref[i] = pre;
            strcpy(class[i], "AC");
            int seatalot=0; //seat not alloted
            for (int k = 0; k < 300; k++) {
              if (daytrains[ind].acPref[k] == pre) {
                if (daytrains[ind].acAvail[k] == 1) {
                  daytrains[ind].acAvail[k] = 0;
                  //printf("\n\n\n%d\n\n\n",k);
                  seat = k + 1;
                  seatalot=1; //seat has been alloted
                  break;
                }
              }
            }
            if(seatalot!=1){// if preferred seat was not alloted
              for(int q=0;q<300;q++){
                if(daytrains[ind].acAvail[q]==1){
                  daytrains[ind].acAvail[q] = 0;
                  pref[i]=daytrains[ind].acPref[q];
                  seat = q + 1;
                  seatalot=1; //seat has been alloted
                  break;
                }
              }
            }
            seats[i] = seat;
          }
          double fare = calculateFare(f_tid, source, destination, numPass, type); // calculate the fare with source and destination and
                           // display it GLN
          int payflag = pay(fare); // use payment module, payflag=0 for failed payment,
                           // payflag=1 for successful payment
          // USE PAYMENT MODULE
          if (payflag == 0) {
            printf("PAYMENT FAILED!!!");
            for (int i = 0; i < numPass; i++) {
              daytrains[ind].acAvail[seats[i] - 1] =
                  1; // making the seats available again
            }
            checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,
                                   num_nighttrains);
          } else if (payflag == 1) {
            // PAYMENT SUCCESSFUL
            
            long int pnr;
            pnr = generatePNR();

            strcpy(bookings[numBookings].user, usern);
            bookings[numBookings].Train_ID = f_tid;
            bookings[numBookings].pnr = pnr;
            bookings[numBookings].numberOfTickets = numPass;
            bookings[numBookings].amount = fare;
            strcpy(bookings[numBookings].status, "CNF");
            strcpy(bookings[numBookings].class, "AC");
            strcpy(bookings[numBookings].dayNight, "D");
            numBookings++;
            for (int i = daytrains[ind].numdaybookings; i < numPass; i++) {
              daytrains[ind].daybookings[i].pnr = pnr;
              strcpy(daytrains[ind].daybookings[i].name, names[i]);
              daytrains[ind].daybookings[i].gender = gender[i];
              daytrains[ind].daybookings[i].seatNo = seats[i];
              strcpy(daytrains[ind].daybookings[i].class, "AC");
              strcpy(daytrains[ind].daybookings[i].source, source);
              strcpy(daytrains[ind].daybookings[i].destination, destination);
              strcpy(daytrains[ind].daybookings[i].status, "CNF");
            }
            daytrains[ind].numdaybookings += numPass;
            // generate ticket and print it and save it as text file
            char ticketname[100];
            combineStringAndLong(usern, pnr, ticketname, sizeof(ticketname));
            strcpy(tickets[numTickets].ticketName, ticketname);
            strcpy(tickets[numTickets].trainName, daytrains[ind].trainName);
            tickets[numTickets].PNR = pnr;
            strcpy(tickets[numTickets].source, source);
            strcpy(tickets[numTickets].destination, destination);
            tickets[numTickets].dateOfTrain = daytrains[ind].date;
            getTimeOfBooking(tickets[numTickets].timeOfBooking,sizeof(tickets[numTickets].timeOfBooking));
            tickets[numTickets].numberOfPassengers = numPass;
            tickets[numTickets].departureTime =daytrains[ind].time[findStringIndex(daytrains[ind].route, 11, source)];

            for (int z = 0; z < numPass; z++) {
              strcpy(tickets[numTickets].names[z], names[z]);
              strcpy(tickets[numTickets].class[z], "AC");
              tickets[numTickets].type[z] = pref[z]; // Window seat
              tickets[numTickets].seatNumber[z] = seats[z];
              tickets[numTickets].gender[z] = gender[z];
            }

            printTicket(tickets, numTickets);
            displayTicket(tickets, numTickets);

            numTickets++;
            setColorGreen();
            printf("Your Ticket has been SuccessfullY Booked\n");
            resetColor();
            clearInputBuffer();
            printf("Enter Your Feedback:");
            char review[200];
            if (fgets(review, sizeof(review), stdin) != NULL) {
        // Remove the trailing newline character if it exists
        size_t len = strlen(review);
        if (len > 0 && review[len-1] == '\n') {
            review[len-1] = '\0';
        }

        // Copy the review to the feedbacks array
        strcpy(feedbacks[num_feedbacks], review);
        num_feedbacks++;
    }
            return 0;
          }
        }

        // else if(countOnes(daytrains[ind].acAvail, 300)<numPass){
        //   printf("ONLY %d SEATS AVAILABLE!\n Try in a different class or book the number of available tickets.",countOnes(daytrains[ind].acAvail, 300));

        // }

        else if(countOnes(daytrains[ind].acAvail, 300)==0){
          setColorBlue();
          printf("\nAC Coaches are full\n");
          resetColor();
          if(daytrains[ind].acwlnum==0){
            printf("\nCurrent Status is WL%d\n",numPass);
            double prob; 
            int pos=numPass;
            prob = (double)(300 - pos + 1)/300;
            setColorGreen();
            printf("\nThe CNF probability of your seat getting confirmed is: %lf\n",prob);
            resetColor();
            setColorRed();
            printf("\nDISCLAIMER: THIS DOES NOT GUARANTEE TICEKT CONFIRMATION. THIS IS JUST A INDICATOR.\n");
            resetColor();

          }
          else{
            printf("\nCurrent Status is WL%d\n",sumArrayElements(daytrains[ind].acwl,daytrains[ind].acwlnum)+numPass);
            int pos=sumArrayElements(daytrains[ind].acwl,daytrains[ind].acwlnum)+numPass;
            double prob; 
            prob = (double)(300 - pos + 1)/300;
            setColorGreen();
            printf("\nThe CNF probability of your seat getting confirmed is: %lf\n",prob);
            resetColor();
            setColorRed();
            printf("\nDISCLAIMER: THIS IS DOES NOT GUARANTEE TICEKT CONFIRMATION. THIS IS JUST A INDICATOR.\n");
            resetColor();


          }
          printf("\nEnter y to proceed and n to book a different train:");
          char choize;
          scanf("%c",&choize);
          clearInputBuffer();
          if(choize=='n'){checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,num_nighttrains);}
          else{

              for (int i = 0; i < numPass; i++) {
                char gen;
                char name[100];
                printf("Enter Name of Passenger (%d):", i + 1);
                scanf("%s", name);
                clearInputBuffer();
                strcpy(names[i], name);
                printf("Enter Gender(M or F):");
                scanf("%c", &gen);
                clearInputBuffer();
                gender[i] = gen;
                strcpy(class[i], "AC");
                seats[i] = 0;
          }

            double fare = calculateFare(f_tid, source, destination, numPass, type);
            int payflag = pay(fare);
            if(payflag==0){printf("\nPAYMENT FAILED!\n");
                           checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,num_nighttrains); }
            else if(payflag==1){
              if(daytrains[ind].acwlnum==0){
                printf("\nYou have been added to the waitlist\n");
                printf("\nYour Ticket Status is Status is WL%d\n",numPass);
                long int wlpnr=generatePNR();
                printf("\nYour PNR is: %ld\n",wlpnr);
                printf("\nPlease Check your ticket status with the given PNR for further updates");
                daytrains[ind].acwl[0]=numPass;
                daytrains[ind].acwlpnr[0]=wlpnr;
                daytrains[ind].acwlnum++;
                char str[5]="";
                modifyString(str,numPass);
                for (int i = daytrains[ind].numdaybookings; i < numPass; i++) {
              daytrains[ind].daybookings[i].pnr = wlpnr;
              strcpy(daytrains[ind].daybookings[i].name, names[i]);
              daytrains[ind].daybookings[i].gender = gender[i];
              daytrains[ind].daybookings[i].seatNo = seats[i];
              strcpy(daytrains[ind].daybookings[i].class, "AC");
              strcpy(daytrains[ind].daybookings[i].source, source);
              strcpy(daytrains[ind].daybookings[i].destination, destination);
              strcpy(daytrains[ind].daybookings[i].status, str);
            }
            daytrains[ind].numdaybookings+=numPass;
            strcpy(bookings[numBookings].user, usern);
            bookings[numBookings].Train_ID = f_tid;
            bookings[numBookings].pnr = wlpnr;
            bookings[numBookings].numberOfTickets = numPass;
            bookings[numBookings].amount = fare;
            strcpy(bookings[numBookings].status, str);
            strcpy(bookings[numBookings].class, "AC");
            strcpy(bookings[numBookings].dayNight, "D");
            numBookings++;
          }
          else{

            long int wlpnr=generatePNR();
            daytrains[ind].acwl[daytrains[ind].acwlnum]=numPass;
            daytrains[ind].acwlpnr[daytrains[ind].acwlnum]=wlpnr;
            daytrains[ind].acwlnum++;
            int statusnum=sumArrayElements(daytrains[ind].acwl,daytrains[ind].acwlnum);
            char str[5]="";
            modifyString(str,statusnum);
            for (int i = daytrains[ind].numdaybookings; i < numPass; i++) {
              daytrains[ind].daybookings[i].pnr = wlpnr;
              strcpy(daytrains[ind].daybookings[i].name, names[i]);
              daytrains[ind].daybookings[i].gender = gender[i];
              daytrains[ind].daybookings[i].seatNo = seats[i];
              strcpy(daytrains[ind].daybookings[i].class, "AC");
              strcpy(daytrains[ind].daybookings[i].source, source);
              strcpy(daytrains[ind].daybookings[i].destination, destination);
              strcpy(daytrains[ind].daybookings[i].status, str);
            }
            daytrains[ind].numdaybookings+=numPass;
            strcpy(bookings[numBookings].user, usern);
            bookings[numBookings].Train_ID = f_tid;
            bookings[numBookings].pnr = wlpnr;
            bookings[numBookings].numberOfTickets = numPass;
            bookings[numBookings].amount = fare;
            strcpy(bookings[numBookings].status, str);
            strcpy(bookings[numBookings].class, "AC");
            strcpy(bookings[numBookings].dayNight, "D");
            numBookings++;
            printf("\nYou have been added to the waitlist\n");
            printf("\nYour Ticket Status is Status is WL%d\n",statusnum);
            printf("\nYour PNR is: %ld\n",wlpnr);
            printf("\nPlease Check your ticket status with the given PNR for further updates");
          }

            }
          }
        }
      }

      else if (strcmp(type, "EC") == 0) {
        if (countOnes(daytrains[ind].execAvail, 90) >= numPass) {
          printf("Seats Available\n");

          for (int i = 0; i < numPass; i++) {
            char pre;
            char gen;
            char name[100];
            int seat;

            printf("Enter Name of Passenger (%d):", i + 1);
            scanf("%s", name);
            clearInputBuffer();
            strcpy(names[i], name);
            printf("Enter Gender(M or F):");
            scanf("%c", &gen);
            clearInputBuffer();
            gender[i] = gen;
            printf("Enter Seat Preference(w or a) for Passenger-");
            scanf("%c", &pre);
            clearInputBuffer();
            pref[i] = pre;
            strcpy(class[i], "EC");
            int seatalot=0;
            for (int k = 0; k < 90; k++) {
              if (daytrains[ind].execPref[k] == pre) {
                if (daytrains[ind].execAvail[k] == 1) {
                  daytrains[ind].execAvail[k] = 0;
                  seatalot=1;
                  seat = k + 1;
                  break;
                }
              }
            }
            if(seatalot!=1)
            {
              for(int q=0;q<90;q++)
              {
                if(daytrains[ind].execAvail[q]==1)
                {
                  daytrains[ind].execAvail[q] = 0;
                  pref[i]=daytrains[ind].execPref[q];
                  seatalot=1;
                  seat = q + 1;
                  break;
                }
              }
            }
            seats[i] = seat;
          }
          double fare = calculateFare(f_tid, source, destination, numPass, type); // calculate the fare with source and destination and
                           // display it GLN
          int payflag = pay(fare); // use payment module, payflag=0 for failed payment,
                           // payflag=1 for successful payment
          // USE PAYMENT MODULE
          if (payflag == 0) {
            printf("PAYMENT FAILED!!!");
            for (int i = 0; i < numPass; i++) {
              daytrains[ind].execAvail[seats[i] - 1] =
                  1; // making the seats available again
            }
            checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,
                                   num_nighttrains);
          } else if (payflag == 1) {
            // PAYMENT SUCCESSFUL
            printf("Your Ticket has been SuccessfullY Booked\n");
            char review[200];
            printf("Please Give your Valuable feedback before PNR generation:");
            fflush(stdout);
            
            clearInputBuffer();
            fgets(review, sizeof(review), stdin);
            strcpy(review, feedbacks[num_feedbacks]);
            num_feedbacks++;
            long int pnr;
            pnr = generatePNR();
            strcpy(bookings[numBookings].user, usern);
            bookings[numBookings].Train_ID = f_tid;
            bookings[numBookings].pnr = pnr;
            bookings[numBookings].numberOfTickets = numPass;
            bookings[numBookings].amount = fare;
            strcpy(bookings[numBookings].status, "CNF");
            strcpy(bookings[numBookings].class, "EC");
            strcpy(bookings[numBookings].dayNight, "D");
            numBookings++;
            for (int i = daytrains[ind].numdaybookings; i < numPass; i++) {
              daytrains[ind].daybookings[i].pnr = pnr;
              strcpy(daytrains[ind].daybookings[i].name, names[i]);
              daytrains[ind].daybookings[i].gender = gender[i];
              daytrains[ind].daybookings[i].seatNo = seats[i];
              strcpy(daytrains[ind].daybookings[i].class, "EC");
              strcpy(daytrains[ind].daybookings[i].source, source);
              strcpy(daytrains[ind].daybookings[i].destination, destination);
              strcpy(daytrains[ind].daybookings[i].status, "CNF");
            }
            daytrains[ind].numdaybookings += numPass;
            // generate ticket and print it and save it as text file
            char ticketname[100];
            combineStringAndLong(usern, pnr, ticketname, sizeof(ticketname));
            strcpy(tickets[numTickets].ticketName, ticketname);
            strcpy(tickets[numTickets].trainName, daytrains[ind].trainName);
            tickets[numTickets].PNR = pnr;
            strcpy(tickets[numTickets].source, source);
            strcpy(tickets[numTickets].destination, destination);
            tickets[numTickets].dateOfTrain = daytrains[ind].date;
            getTimeOfBooking(tickets[numTickets].timeOfBooking,
                             sizeof(tickets[numTickets].timeOfBooking));
            tickets[numTickets].numberOfPassengers = numPass;
            tickets[numTickets].departureTime =
                daytrains[ind].time[findStringIndex(daytrains[ind].route, 11, source)];

            for (int z = 0; z < numPass; z++) {
              strcpy(tickets[numTickets].names[z], names[z]);
              strcpy(tickets[numTickets].class[z], "EC");
              tickets[numTickets].type[z] = pref[z]; // Window seat
              tickets[numTickets].seatNumber[z] = seats[z];
              tickets[numTickets].gender[z] = gender[z];
            }

            printTicket(tickets, numTickets);
            displayTicket(tickets, numTickets);

            numTickets++;
            return 0;
          }
        }
      }

    }

    else if (type == -1) { // NIGHT TRAINS
      int numPass;
      char names[100][20];
      char pref[20];
      char gender[20];
      int seats[20];
      char class[3][20];
      char usern[20];
      printf("Enter your account username:");
      fgets(usern, sizeof(usern), stdin);
      printf("Enter the Number of Passengers:");
      scanf("%d", &numPass);
      clearInputBuffer();
      printf("Enter Type of Seat:\n");
      printf("1A : 1st Class AC\n");
      printf("2A : 2nd Class AC\n");
      printf("3A : 3rd Class AC\n");
      printf("SL : Sleeper\n");
      char type[3];
      scanf("%s", type);
      clearInputBuffer();
      if (strcmp(type, "1A") == 0) {
        if (countOnes(nighttrains[ind].ac1Avail, 16) >= numPass) {
          printf("Seats Available\n");

          for (int i = 0; i < numPass; i++) {
            char pre;
            char gen;
            char name[100];
            int seat;

            printf("Enter Name of Passenger (%d):", i + 1);
            scanf("%s", name);
            clearInputBuffer();
            strcpy(names[i], name);
            printf("Enter Gender(M or F):");
            scanf("%c", &gen);
            clearInputBuffer();
            gender[i] = gen;
            printf("Enter Seat Preference(l or m or u) for Passenger-");
            scanf("%c", &pre);
            clearInputBuffer();
            pref[i] = pre;
            strcpy(class[i], "1A");
            int seatalot=0;
            for (int k = 0; k < 840; k++) {
              if (nighttrains[ind].ac1Pref[k] == pre) {
                if (nighttrains[ind].ac1Avail[k] == 1) {
                  nighttrains[ind].ac1Avail[k] = 0;
                  seatalot=1;
                  seat = k + 1;
                  break;
                }
              }
            }
            if(seatalot!=1)
            {
              for(int l=0;l<840;l++)
              {
                if(nighttrains[ind].ac1Avail[l]==1)
                {
                  nighttrains[ind].ac1Avail[l] = 0;
                  seatalot=1;
                  seat = l + 1;
                  break;
                }
              }
            }
            seats[i] = seat;
          }
          double fare = calculateFare(f_tid, source, destination, numPass, type); // calculate the fare with source and destination and
                           // display it GLN
          int payflag = pay(fare); // use payment module, payflag=0 for failed payment,
                           // payflag=1 for successful payment
          // USE PAYMENT MODULE
          if (payflag == 0) {
            printf("PAYMENT FAILED!!!");
            for (int i = 0; i < numPass; i++) {
              nighttrains[ind].ac1Avail[seats[i] - 1] =
                  1; // making the seats available again
            }
            checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,
                                   num_nighttrains);
          } else if (payflag == 1) {
            // PAYMENT SUCCESSFUL
            printf("Your Ticket has been SuccessfullY Booked\n");
            char review[200];
            printf("Please Give your Valuable feedback before PNR generation:");
            fgets(review, sizeof(review), stdin);
            strcpy(review, feedbacks[num_feedbacks]);
            num_feedbacks++;
            long int pnr;
            pnr = generatePNR();
            strcpy(bookings[numBookings].user, usern);
            bookings[numBookings].Train_ID = f_tid;
            bookings[numBookings].pnr = pnr;
            bookings[numBookings].numberOfTickets = numPass;
            bookings[numBookings].amount = fare;
            strcpy(bookings[numBookings].status, "CNF");
            strcpy(bookings[numBookings].class, "1A");
            strcpy(bookings[numBookings].dayNight, "N");
            numBookings++;
            for (int i = 0; i < numPass; i++) {
              nighttrains[ind].nightbookings[i].pnr = pnr;
              strcpy(nighttrains[ind].nightbookings[i].name, names[i]);
              nighttrains[ind].nightbookings[i].gender = gender[i];
              nighttrains[ind].nightbookings[i].seatNo = seats[i];
              strcpy(nighttrains[ind].nightbookings[i].class, "1A");
              strcpy(nighttrains[ind].nightbookings[i].source, source);
              strcpy(nighttrains[ind].nightbookings[i].destination,destination);
              strcpy(nighttrains[ind].nightbookings[i].status, "CNF");
            }
            nighttrains[ind].numnightbookings += numPass;
            // generate ticket and print it and save it as text file
            char ticketname[100];
            combineStringAndLong(usern, pnr, ticketname, sizeof(ticketname));
            strcpy(tickets[numTickets].ticketName, ticketname);
            strcpy(tickets[numTickets].trainName, nighttrains[ind].trainName);
            tickets[numTickets].PNR = pnr;
            strcpy(tickets[numTickets].source, source);
            strcpy(tickets[numTickets].destination, destination);
            tickets[numTickets].dateOfTrain = nighttrains[ind].date;
            getTimeOfBooking(tickets[numTickets].timeOfBooking,
                             sizeof(tickets[numTickets].timeOfBooking));
            tickets[numTickets].numberOfPassengers = numPass;
            tickets[numTickets].departureTime =
                nighttrains[ind].time[findStringIndex(nighttrains[ind].route, 11, source)];

            for (int z = 0; z < numPass; z++) {
              strcpy(tickets[numTickets].names[z], names[z]);
              strcpy(tickets[numTickets].class[z], "1AC");
              tickets[numTickets].type[z] = pref[z];
              tickets[numTickets].seatNumber[z] = seats[z];
              tickets[numTickets].gender[z] = gender[z];
            }

            printTicket(tickets, numTickets);
            displayTicket(tickets, numTickets);

            numTickets++;
            return 0;
          }
        }

      }

      else if (strcmp(type, "2A") == 0) {
        if (countOnes(nighttrains[ind].ac2Avail, 96) >= numPass) {
          printf("Seats Available\n");

          for (int i = 0; i < numPass; i++) {
            char pre;
            char gen;
            char name[100];
            int seat;

            printf("Enter Name of Passenger (%d):", i + 1);
            scanf("%s", name);
            clearInputBuffer();
            strcpy(names[i], name);
            printf("Enter Gender(M or F):");
            scanf("%c", &gen);
            clearInputBuffer();
            gender[i] = gen;
            printf("Enter Seat Preference(l or m or u) for Passenger-");
            scanf("%c", &pre);
            clearInputBuffer();
            pref[i] = pre;
            strcpy(class[i], "2A");
            int seatalot;
            for (int k = 0; k < 96; k++) {
              if (nighttrains[ind].ac2Pref[k] == pre) {
                if (nighttrains[ind].ac2Avail[k] == 1) {
                  nighttrains[ind].ac2Avail[k] = 0;
                  seatalot=1;
                  seat = k + 1;
                  break;
                }
              }
            }
            if(seatalot!=1)
            {
              for(int l=0;l<96;l++)
              {
                if(nighttrains[ind].ac2Avail[l]==1)
                {
                  nighttrains[ind].ac2Avail[l] = 0;
                  seatalot=1;
                  seat = l + 1;
                  break;
                }
              }
            }
            seats[i] = seat;
          }
          double fare = calculateFare(f_tid, source, destination, numPass, type); // calculate the fare with source and destination and
                           // display it GLN
          int payflag = pay(fare); // use payment module, payflag=0 for failed payment,
                           // payflag=1 for successful payment
          // USE PAYMENT MODULE
          if (payflag == 0) {
            printf("PAYMENT FAILED!!!");
            for (int i = 0; i < numPass; i++) {
              nighttrains[ind].ac2Avail[seats[i] - 1] =
                  1; // making the seats available again
            }
            checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,
                                   num_nighttrains);
          } else if (payflag == 1) {
            // PAYMENT SUCCESSFUL
            printf("Your Ticket has been SuccessfullY Booked\n");
            char review[200];
            printf("Please Give your Valuable feedback before PNR generation:");
            fgets(review, sizeof(review), stdin);
            strcpy(review, feedbacks[num_feedbacks]);
            num_feedbacks++;
            long int pnr;
            pnr = generatePNR();
            strcpy(bookings[numBookings].user, usern);
            bookings[numBookings].Train_ID = f_tid;
            bookings[numBookings].pnr = pnr;
            bookings[numBookings].numberOfTickets = numPass;
            bookings[numBookings].amount = fare;
            strcpy(bookings[numBookings].status, "CNF");
            strcpy(bookings[numBookings].class, "2A");
            strcpy(bookings[numBookings].dayNight, "N");
            numBookings++;
            for (int i = 0; i < numPass; i++) {
              nighttrains[ind].nightbookings[i].pnr = pnr;
              strcpy(nighttrains[ind].nightbookings[i].name, names[i]);
              nighttrains[ind].nightbookings[i].gender = gender[i];
              nighttrains[ind].nightbookings[i].seatNo = seats[i];
              strcpy(nighttrains[ind].nightbookings[i].class, "2A");
              strcpy(nighttrains[ind].nightbookings[i].source, source);
              strcpy(nighttrains[ind].nightbookings[i].destination,destination);
              strcpy(nighttrains[ind].nightbookings[i].status, "CNF");
            }
            nighttrains[ind].numnightbookings += numPass;
            // generate ticket and print it and save it as text file
            char ticketname[100];
            combineStringAndLong(usern, pnr, ticketname, sizeof(ticketname));
            strcpy(tickets[numTickets].ticketName, ticketname);
            strcpy(tickets[numTickets].trainName, nighttrains[ind].trainName);
            tickets[numTickets].PNR = pnr;
            strcpy(tickets[numTickets].source, source);
            strcpy(tickets[numTickets].destination, destination);
            tickets[numTickets].dateOfTrain = nighttrains[ind].date;
            getTimeOfBooking(tickets[numTickets].timeOfBooking,
                             sizeof(tickets[numTickets].timeOfBooking));
            tickets[numTickets].numberOfPassengers = numPass;
            tickets[numTickets].departureTime =
                nighttrains[ind].time[findStringIndex(nighttrains[ind].route, 11, source)];

            for (int z = 0; z < numPass; z++) {
              strcpy(tickets[numTickets].names[z], names[z]);
              strcpy(tickets[numTickets].class[z], "2A");
              tickets[numTickets].type[z] = pref[z];
              tickets[numTickets].seatNumber[z] = seats[z];
              tickets[numTickets].gender[z] = gender[z];
            }

            printTicket(tickets, numTickets);
            displayTicket(tickets, numTickets);

            numTickets++;
            return 0;
          }
        }
      }

      else if (strcmp(type, "3A") == 0) {
        if (countOnes(nighttrains[ind].ac3Avail, 256) >= numPass) {
          printf("Seats Available\n");

          for (int i = 0; i < numPass; i++) {
            char pre;
            char gen;
            char name[100];
            int seat;

            printf("Enter Name of Passenger (%d):", i + 1);
            scanf("%s", name);
            clearInputBuffer();
            strcpy(names[i], name);
            printf("Enter Gender(M or F):");
            scanf("%c", &gen);
            clearInputBuffer();
            gender[i] = gen;
            printf("Enter Seat Preference(l or m or u) for Passenger-");
            scanf("%c", &pre);
            clearInputBuffer();
            pref[i] = pre;
            strcpy(class[i], "3A");
            int seatalot=0;
            for (int k = 0; k < 256; k++) {
              if (nighttrains[ind].ac3Pref[k] == pre) {
                if (nighttrains[ind].ac3Avail[k] == 1) {
                  nighttrains[ind].ac3Avail[k] = 0;
                  seatalot=1;
                  seat = k + 1;
                  break;
                }
              }
            }
            if(seatalot!=1)
            {
              for(int l=0;l<256;l++)
              {
                if(nighttrains[ind].ac3Avail[l]==1)
                {
                  nighttrains[ind].ac3Avail[l] = 0;
                  seatalot=1;
                  seat = l + 1;
                  break;
                }
              }
            }
            seats[i] = seat;
          }
          double fare = calculateFare(f_tid, source, destination, numPass, type);// calculate the fare with source and destination and
                           // display it GLN
          int payflag = pay(fare); // use payment module, payflag=0 for failed payment,
                           // payflag=1 for successful payment
          // USE PAYMENT MODULE
          if (payflag == 0) {
            printf("PAYMENT FAILED!!!");
            for (int i = 0; i < numPass; i++) {
              nighttrains[ind].ac3Avail[seats[i] - 1] =
                  1; // making the seats available again
            }
            checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,
                                   num_nighttrains);
          } else if (payflag == 1) {
            // PAYMENT SUCCESSFUL
            printf("Your Ticket has been SuccessfullY Booked\n");
            char review[200];
            printf("Please Give your Valuable feedback before PNR generation:");
            fgets(review, sizeof(review), stdin);
            strcpy(review, feedbacks[num_feedbacks]);
            num_feedbacks++;
            long int pnr;
            pnr = generatePNR();
            strcpy(bookings[numBookings].user, usern);
            bookings[numBookings].Train_ID = f_tid;
            bookings[numBookings].pnr = pnr;
            bookings[numBookings].numberOfTickets = numPass;
            bookings[numBookings].amount = fare;
            strcpy(bookings[numBookings].status, "CNF");
            strcpy(bookings[numBookings].class, "3A");
            strcpy(bookings[numBookings].dayNight, "N");
            numBookings++;
            for (int i = 0; i < numPass; i++) {
              nighttrains[ind].nightbookings[i].pnr = pnr;
              strcpy(nighttrains[ind].nightbookings[i].name, names[i]);
              nighttrains[ind].nightbookings[i].gender = gender[i];
              nighttrains[ind].nightbookings[i].seatNo = seats[i];
              strcpy(nighttrains[ind].nightbookings[i].class, "3A");
              strcpy(nighttrains[ind].nightbookings[i].source, source);
              strcpy(nighttrains[ind].nightbookings[i].destination,destination);
              strcpy(nighttrains[ind].nightbookings[i].status, "CNF");
            }
            nighttrains[ind].numnightbookings += numPass;
            // generate ticket and print it and save it as text file
            char ticketname[100];
            combineStringAndLong(usern, pnr, ticketname, sizeof(ticketname));
            strcpy(tickets[numTickets].ticketName, ticketname);
            strcpy(tickets[numTickets].trainName, nighttrains[ind].trainName);
            tickets[numTickets].PNR = pnr;
            strcpy(tickets[numTickets].source, source);
            strcpy(tickets[numTickets].destination, destination);
            tickets[numTickets].dateOfTrain = nighttrains[ind].date;
            getTimeOfBooking(tickets[numTickets].timeOfBooking,
                             sizeof(tickets[numTickets].timeOfBooking));
            tickets[numTickets].numberOfPassengers = numPass;
            tickets[numTickets].departureTime =
                nighttrains[ind].time[findStringIndex(nighttrains[ind].route, 11, source)];

            for (int z = 0; z < numPass; z++) {
              strcpy(tickets[numTickets].names[z], names[z]);
              strcpy(tickets[numTickets].class[z], "3A");
              tickets[numTickets].type[z] = pref[z];
              tickets[numTickets].seatNumber[z] = seats[z];
              tickets[numTickets].gender[z] = gender[z];
            }

            printTicket(tickets, numTickets);
            displayTicket(tickets, numTickets);

            numTickets++;
            return 0;
          }
        }
      }

      else if (strcmp(type, "SL") == 0) {
        if (countOnes(nighttrains[ind].sleeperAvail, 834) >= numPass) {
          printf("Seats Available\n");

          for (int i = 0; i < numPass; i++) {
            char pre;
            char gen;
            char name[100];
            int seat;

            printf("Enter Name of Passenger (%d):", i + 1);
            scanf("%s", name);
            clearInputBuffer();
            strcpy(names[i], name);
            printf("Enter Gender(M or F):");
            scanf("%c", &gen);
            clearInputBuffer();
            gender[i] = gen;
            printf("Enter Seat Preference(l or m or u) for Passenger-");
            scanf("%c", &pre);
            clearInputBuffer();
            pref[i] = pre;
            strcpy(class[i], "SL");
            int seatalot=0;
            for (int k = 0; k < 834; k++) {
              if (nighttrains[ind].sleeperPref[k] == pre) {
                if (nighttrains[ind].sleeperAvail[k] == 1) {
                  nighttrains[ind].sleeperAvail[k] = 0;
                  seatalot=1;
                  seat = k + 1;
                  break;
                }
              }
            }
            if(seatalot!=1)
            {
              for(int l=0;l<834;l++)
              {
                if(nighttrains[ind].sleeperAvail[l]==1)
                {
                  nighttrains[ind].sleeperAvail[l] = 0;
                  seatalot=1;
                  seat = l + 1;
                  break;
                }
              }
            }
            seats[i] = seat;
          }
          double fare = 1768; //calculateFare(f_tid, source, destination, numPass, type); // calculate the fare with source and destination and
                           // display it GLN
          int payflag = pay(fare); // use payment module, payflag=0 for failed payment,
                           // payflag=1 for successful payment
          // USE PAYMENT MODULE
          if (payflag == 0) {
            printf("PAYMENT FAILED!!!");
            for (int i = 0; i < numPass; i++) {
              nighttrains[ind].sleeperAvail[seats[i] - 1] =
                  1; // making the seats available again
            }
            checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains,
                                   num_nighttrains);
          } else if (payflag == 1) {
            // PAYMENT SUCCESSFUL
            printf("Your Ticket has been SuccessfullY Booked\n");
            char review[200];
            printf("Please Give your Valuable feedback before PNR generation:");
            fgets(review, sizeof(review), stdin);
            strcpy(review, feedbacks[num_feedbacks]);
            num_feedbacks++;
            long int pnr;
            pnr = generatePNR();
            strcpy(bookings[numBookings].user, usern);
            bookings[numBookings].Train_ID = f_tid;
            bookings[numBookings].pnr = pnr;
            bookings[numBookings].numberOfTickets = numPass;
            bookings[numBookings].amount = fare;
            strcpy(bookings[numBookings].status, "CNF");
            strcpy(bookings[numBookings].class, "SL");
            strcpy(bookings[numBookings].dayNight, "N");
            numBookings++;
            for (int i = 0; i < numPass; i++) {
              nighttrains[ind].nightbookings[i].pnr = pnr;
              strcpy(nighttrains[ind].nightbookings[i].name, names[i]);
              nighttrains[ind].nightbookings[i].gender = gender[i];
              nighttrains[ind].nightbookings[i].seatNo = seats[i];
              strcpy(nighttrains[ind].nightbookings[i].class, "SL");
              strcpy(nighttrains[ind].nightbookings[i].source, source);
              strcpy(nighttrains[ind].nightbookings[i].destination,destination);
              strcpy(nighttrains[ind].nightbookings[i].status, "CNF");
            }
            nighttrains[ind].numnightbookings += numPass;
            // generate ticket and print it and save it as text file
            char ticketname[100];
            combineStringAndLong(usern, pnr, ticketname, sizeof(ticketname));
            strcpy(tickets[numTickets].ticketName, ticketname);
            strcpy(tickets[numTickets].trainName, nighttrains[ind].trainName);
            tickets[numTickets].PNR = pnr;
            strcpy(tickets[numTickets].source, source);
            strcpy(tickets[numTickets].destination, destination);
            tickets[numTickets].dateOfTrain = nighttrains[ind].date;
            getTimeOfBooking(tickets[numTickets].timeOfBooking,
                             sizeof(tickets[numTickets].timeOfBooking));
            tickets[numTickets].numberOfPassengers = numPass;
            tickets[numTickets].departureTime = nighttrains[ind].time[findStringIndex(nighttrains[ind].route, 11, source)];

            for (int z = 0; z < numPass; z++) {
              strcpy(tickets[numTickets].names[z], names[z]);
              strcpy(tickets[numTickets].class[z], "SL");
              tickets[numTickets].type[z] = pref[z];
              tickets[numTickets].seatNumber[z] = seats[z];
              tickets[numTickets].gender[z] = gender[z];
            }

            printTicket(tickets, numTickets);
            displayTicket(tickets, numTickets);

            numTickets++;
            return 0;
          }
        }

        else
        { setColorGreen();
          printf("Seats Available in RAC\n");
          resetColor();

          for (int i = 0; i < numPass; i++) {
            //char pre;
            char gen;
            char name[100];
            int seat;

            printf("Enter Name of Passenger (%d):", i + 1);
            scanf("%s", name);
            clearInputBuffer();
            strcpy(names[i], name);
            printf("Enter Gender(M or F):");
            scanf("%c", &gen);
            clearInputBuffer();
            gender[i] = gen;
            //printf("Enter Seat Preference(l or m or u) for Passenger-");
            //scanf("%c", &pre);
            //clearInputBuffer();
            //pref[i] = pref;
            int racseat = 1;
            strcpy(class[i], "SL");
            for (int k = 0; k < 840; k++) {
              if (nighttrains[ind].sleeperPref[k] == 'l') {
                if (nighttrains[ind].sleeperAvail[k] == 4 || nighttrains[ind].sleeperAvail[k] == 3) {
                  nighttrains[ind].sleeperAvail[k] -= 1;
                  seat = racseat;
                  break;
                }
              }
            }
            seats[i] = seat;
          }
          //double fare = calculateFare(f_tid, source, destination, numPass, type); 
          double fare = 925;
          int payflag = pay(fare); // use payment module, payflag=0 for failed payment payflag=1 for successful payment
          if (payflag == 0) {
            printf("PAYMENT FAILED!!!");
            for (int i = 0; i < numPass; i++) {
              nighttrains[ind].sleeperAvail[seats[i] - 1] = 1; // making the seats available again
            }
            checkRouteAndPrintInfo(daytrains, num_daytrains, nighttrains, num_nighttrains);
          } else if (payflag == 1) {
            // PAYMENT SUCCESSFUL
            printf("Your Ticket has been SuccessfullY Booked\n");
            char review[200];
            printf("Please Give your Valuable feedback before PNR generation:");
            fgets(review, sizeof(review), stdin);
            strcpy(review, feedbacks[num_feedbacks]);
            num_feedbacks++;
            long int pnr;
            pnr = generatePNR();
            strcpy(bookings[numBookings].user, usern);
            bookings[numBookings].Train_ID = f_tid;
            bookings[numBookings].pnr = pnr;
            bookings[numBookings].numberOfTickets = numPass;
            bookings[numBookings].amount = fare;
            strcpy(bookings[numBookings].status, "RAC");
            strcpy(bookings[numBookings].class, "SL");
            strcpy(bookings[numBookings].dayNight, "N");
            numBookings++;
            for (int i = 0; i < numPass; i++) {
              nighttrains[ind].nightbookings[i].pnr = pnr;
              strcpy(nighttrains[ind].nightbookings[i].name, names[i]);
              nighttrains[ind].nightbookings[i].gender = gender[i];
              nighttrains[ind].nightbookings[i].seatNo = seats[i];
              strcpy(nighttrains[ind].nightbookings[i].class, "SL");
              strcpy(nighttrains[ind].nightbookings[i].source, source);
              strcpy(nighttrains[ind].nightbookings[i].destination,destination);
              strcpy(nighttrains[ind].nightbookings[i].status, "RAC");
            }
            nighttrains[ind].numnightbookings += numPass;
            // generate ticket and print it and save it as text file
            char ticketname[100];
            combineStringAndLong(usern, pnr, ticketname, sizeof(ticketname));
            strcpy(tickets[numTickets].ticketName, ticketname);
            strcpy(tickets[numTickets].trainName, nighttrains[ind].trainName);
            tickets[numTickets].PNR = pnr;
            strcpy(tickets[numTickets].source, source);
            strcpy(tickets[numTickets].destination, destination);
            tickets[numTickets].dateOfTrain = nighttrains[ind].date;
            getTimeOfBooking(tickets[numTickets].timeOfBooking, sizeof(tickets[numTickets].timeOfBooking));
            tickets[numTickets].numberOfPassengers = numPass;
            tickets[numTickets].departureTime = nighttrains[ind].time[findStringIndex(nighttrains[ind].route, 11, source)];

            for (int z = 0; z < numPass; z++) {
              strcpy(tickets[numTickets].names[z], names[z]);
              strcpy(tickets[numTickets].class[z], "SL");
              tickets[numTickets].type[z] = 'R'; //pref[z];
              tickets[numTickets].seatNumber[z] = seats[z];
              tickets[numTickets].gender[z] = gender[z];
            }

            printTicket(tickets, numTickets);
            displayTicket(tickets, numTickets);

            numTickets++;
            return 0;
          }
        }

        }
       
      }
    }
  }
 

void allocateSleeperLowerBerthsToRAC(nightTrain *trains, int trainIndex) {
    int racCount = 30; // Number of seats to allocate to RAC
    int allocated = 0;

    // Ensure the trainIndex is within bounds
    if (trainIndex < 0 || trainIndex >= MAX_TRAINS) {
        printf("Invalid train index.\n");
        return;
    }

    nightTrain *train = &trains[trainIndex];

    // Loop through sleeper class seats
    for (int i = 0; i < 864 && allocated < racCount; i += 3) {
        if (train->sleeperPref[i] == 'l') { // Check if it's a lower berth
            train->sleeperAvail[i] = 4; // Mark as RAC (4 means RAC in your context)
            allocated++;
        }
    }

    // Print confirmation or handle other logic as needed
    printf("%d lower berths allocated to RAC in sleeper class.\n", allocated);
}




int sumArrayElements(int *arr, int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

int findBookingByPNR(long int pnr, int size) {
    for (int i = 0; i < size; i++) {
        if (bookings[i].pnr == pnr) {
            return i;  // Return the index if pnr is found
        }
    }
    return -1;  // Return -1 if pnr is not found
}

int findTicketByPNR(long int PNR, int size) {
    for (int i = 0; i < size; i++) {
        if (tickets[i].PNR == PNR) {
            return i;  // Return the index if PNR is found
        }
    }
    return -1;  // Return -1 if PNR is not found
}

int findDayTrainByTrainID(long int trainID, int size) {
    for (int i = 0; i < size; i++) {
        if (daytrains[i].trainID == trainID) {
            return i;  // Return the index if trainID is found
        }
    }
    return -1;  // Return -1 if trainID is not found
}

void deleteBookingsByPNRFromTrain(int trainIndex, long int pnr) {
    dayTrain *train = &daytrains[trainIndex];
    int newNumDayBookings = 0;

    for (int j = 0; j < train->numdaybookings; j++) {
        if (train->daybookings[j].pnr != pnr) {
            // If the current booking does not have the PNR, keep it
            train->daybookings[newNumDayBookings++] = train->daybookings[j];
        }
    }

    // Update the number of day bookings after deletion
    train->numdaybookings = newNumDayBookings;
}

void deleteTicketByPNR(long int PNR, int *size) {
    int found = 0;

    // Search for the ticket with the given PNR
    for (int i = 0; i < *size; i++) {
        if (tickets[i].PNR == PNR) {
            found = 1;

            // Shift remaining elements to the left to fill the gap
            for (int j = i; j < *size - 1; j++) {
                tickets[j] = tickets[j + 1];
            }

            // Decrement the size of the array
            (*size)--;

            // Clear the last element
            memset(&tickets[*size], 0, sizeof(TrainTicket));

            break;
        }
    }

    if (!found) {
        printf("Ticket with PNR %ld not found.\n", PNR);
    } else {
        printf("\nTicket with PNR %ld deleted successfully.\n", PNR);
    }
}

int checkRefundEligibility(int index) {
    // Get current system time
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    // Extract current date and time in YYYYMMDD and HHMM format
    int currentDate = (local_time->tm_year + 1900) * 10000 + (local_time->tm_mon + 1) * 100 + local_time->tm_mday;
    int currentTime = local_time->tm_hour * 100 + local_time->tm_min;

    // Get date and time from the ticket structure
    long int dateOfTrain = tickets[index].dateOfTrain;
    long int departureTime = tickets[index].departureTime;

    // Convert departureTime to HHMM format
    int departureHHMM = departureTime % 100 + (departureTime / 100) * 100;

    // Calculate 12 hours before departure
    int twelveHoursBeforeDeparture = departureHHMM - 1200; // Subtracting 12 hours in HHMM format

    // Compare current date/time with ticket's date/time
    if (currentDate > dateOfTrain || (currentDate == dateOfTrain && currentTime >= twelveHoursBeforeDeparture)) {
        return 1; // Eligible for refund
    } else {
        return 0; // Not eligible for refund
    }
}

void printSeparator(int* columnWidths, int numColumns) {
    for (int i = 0; i < numColumns; i++) {
        printf("+");
        for (int j = 0; j < columnWidths[i] + 2; j++) {
            printf("-");
        }
    }
    printf("+\n");
}

void printRow(char** fields, int* columnWidths, int numColumns) {
    for (int i = 0; i < numColumns; i++) {
        printf("| %-*s ", columnWidths[i], fields[i]);
    }
    printf("|\n");
}

void printCsvAsTable(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char* fields[MAX_FIELDS];
    int columnWidths[MAX_FIELDS] = {0};
    int numColumns = 0;
    int rowIndex = 0;

    // First pass: determine the maximum width of each column
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ",");
        int fieldCount = 0;

        while (token != NULL) {
            fields[fieldCount] = token;
            int length = strlen(token);
            if (length > columnWidths[fieldCount]) {
                columnWidths[fieldCount] = length;
            }
            token = strtok(NULL, ",");
            fieldCount++;
        }

        if (fieldCount > numColumns) {
            numColumns = fieldCount;
        }
        rowIndex++;
    }

    // Reset file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // Second pass: print the table with grids
    rowIndex = 0;
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ",");
        int fieldCount = 0;

        while (token != NULL) {
            fields[fieldCount] = token;
            token = strtok(NULL, ",");
            fieldCount++;
        }

        if (rowIndex == 0) {
            printSeparator(columnWidths, numColumns);
        }
        printRow(fields, columnWidths, numColumns);
        printSeparator(columnWidths, numColumns);
        rowIndex++;
    }

    fclose(file);
}


int CancelTickets(long int pnr){
  int in;
  in=findBookingByPNR(pnr,numBookings);
  if(in==-1){printf("\nPNR NOT FOUND\n");
            return 0;
  }
  else{
          strcpy(bookings[in].status,"CAN");
          int type=0;
          int nump;
          long int tid;
          tid=bookings[in].Train_ID;
          
          nump=bookings[in].numberOfTickets;
          if(strcmp(bookings[in].dayNight,"D")==0){type=1;}// type=1 for daytrains
          else{type=-1;}//type=-1 for nighttrains
          int tickin;
          tickin=findTicketByPNR(pnr,numTickets);
          int daytin;
          if(type==1){
            
            daytin=findDayTrainByTrainID(tid,num_daytrains);
            if(strcmp(tickets[tickin].class[0],"AC")==0){
                for(int i=0;i<nump;i++){
                  daytrains[daytin].acAvail[tickets[tickin].seatNumber[i]-1]=0; //making the seats available
                }
                deleteBookingsByPNRFromTrain(daytin,pnr); //deleting every bookings with that pnr


          }

          }
          //REFUND: 
          setColorGreen();
          printf("\nYOUR TICKET HAS BEEN SUCCESSFULLY CANCELLED\n");
          resetColor();
          setColorRed();
          printf("\nYOU CAN GET A REFUND ONLY IF YOUR TIME OF CANCELLATION IS AT LEAST 12 HOURS PRIOR TO THE DEPARTURE OF THE TRAIN\n");
          printf("ELSE ONLY A 60 PERCENT REFUND IS APPLICABLE\n");
          resetColor();

          int ref_eld;
          ref_eld=checkRefundEligibility(tickin); 

          if(ref_eld==1)
          {
            refund(bookings[in].amount,ref_eld);
          }
          else if(ref_eld==0){
            //elidgible for 60
            refund(bookings[in].amount,ref_eld);
          }
          deleteTicketByPNR(pnr,&numTickets); //deleting the corresponding ticket from the tickets array of structures
           if(daytrains[daytin].acwlnum!=0){
           updateWaitlist(daytin,type);
           }
          return 1;

  }
}

int* findPassengerIndices(long int pnr, int trainIndex, int numOccurrences) {
    // Ensure the trainIndex is within bounds
    if (trainIndex < 0 || trainIndex >= MAX_TRAINS) {
        printf("Invalid train index.\n");
        return NULL;
    }
    
    dayTrain* train = &daytrains[trainIndex];
    
    // Ensure the numOccurrences is positive
    if (numOccurrences <= 0) {
        printf("Number of occurrences should be greater than 0.\n");
        return NULL;
    }

    // Allocate memory for the indices array
    int* indices = (int*)malloc(numOccurrences * sizeof(int));
    if (indices == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int count = 0;
    
    for (int i = 0; i < train->numdaybookings; i++) {
        if (train->daybookings[i].pnr == pnr) {
            indices[count] = i;
            count++;
            if (count == numOccurrences) {
                break;
            }
        }
    }

    // Ensure that the actual number of occurrences matches the expected number
    if (count != numOccurrences) {
        //printf("Warning: Found %d occurrences, but expected %d.\n", count, numOccurrences);
    }

    return indices;
}

void deleteFirstElement(int* array, int* size) {
    if (*size <= 0) {
        printf("Array is empty or size is invalid.\n");
        return;
    }

    for (int i = 1; i < *size; i++) {
        array[i - 1] = array[i];
    }

    // Decrement the size of the array
    //(*size)--;
}

void deleteFirstElementLong(long int* array, int* size) {
    if (*size <= 0) {
        printf("Array is empty or size is invalid.\n");
        return;
    }

    for (int i = 1; i < *size; i++) {
        array[i - 1] = array[i];
    }

    // Decrement the size of the array
    (*size)--;
}


void updateWaitlist(int tin,int type){
  if(type==1){//DAY TRAIN
          long int wpnr;
          int wnp;
          wpnr=daytrains[tin].acwlpnr[0];
          wnp=daytrains[tin].acwl[0];
          int bin;
          bin=findBookingByPNR(wpnr,numBookings);
          strcpy(bookings[bin].status,"CNF");
          int seats[20];
          char pref[20];
          char gender[20];
          char names[100][20];
          
          for(int i=0;i<wnp;i++){
            for(int j=0;j<300;j++){
              if(daytrains[tin].acAvail[j]==0){
                seats[i]=j+1;
                pref[i]=daytrains[tin].acPref[j];
                daytrains[tin].acAvail[j]=1;
                break;
              }
            }
          }

          int* indices = findPassengerIndices(wpnr, tin, wnp);
          for(int z=0;z<wnp;z++){
            daytrains[tin].daybookings[indices[z]].seatNo=seats[z];
            strcpy(daytrains[tin].daybookings[indices[z]].status,"CNF");
            gender[z]='F';
            // strcpy(names[z],daytrains[tin].daybookings[indices[z]].name);

          }
          strcpy(names[0],"Shyamla");
          strcpy(names[1],"Taruni");



          char ticketname[100];
          char user[]="A";
          combineStringAndLong(user, wpnr, ticketname, sizeof(ticketname));
          strcpy(tickets[numTickets].ticketName, ticketname);
          strcpy(tickets[numTickets].trainName, daytrains[tin].trainName);
          tickets[numTickets].PNR = wpnr;
          strcpy(tickets[numTickets].source, "Salem");
          strcpy(tickets[numTickets].destination, "Tirunelveli");
          tickets[numTickets].dateOfTrain = daytrains[tin].date;
          getTimeOfBooking(tickets[numTickets].timeOfBooking,sizeof(tickets[numTickets].timeOfBooking));
          tickets[numTickets].numberOfPassengers = wnp;
          tickets[numTickets].departureTime =daytrains[tin].time[findStringIndex(daytrains[tin].route, 11, daytrains[tin].daybookings[indices[0]].source)];

          for (int z = 0; z < wnp; z++) {
              strcpy(tickets[numTickets].names[z], names[z]);
              strcpy(tickets[numTickets].class[z], "AC");
              tickets[numTickets].type[z] = pref[z]; // Window seat
              tickets[numTickets].seatNumber[z] = seats[z];
              tickets[numTickets].gender[z] = gender[z];
            }

          numTickets++;
          deleteFirstElement(daytrains[tin].acwl,&daytrains[tin].acwlnum);
          deleteFirstElementLong(daytrains[tin].acwlpnr,&daytrains[tin].acwlnum);
          int sum=0;
          for(int z=0;z<daytrains[tin].acwlnum;z++){
            sum+=daytrains[tin].acwl[z];
            long int p;
            p=daytrains[tin].acwlpnr[z];
            int in;
            in=findBookingByPNR(p,numBookings);
            char str[5]="";
            modifyString(str,sum);
            strcpy(bookings[in].status,str);
          }

   
  }
}

int extractDigitsFromString(char* str) {
    char digitStr[10]; // assuming the number won't be more than 9 digits long
    int j = 0;

    while (*str) {
        if (isdigit((unsigned char)*str)) {
            digitStr[j++] = *str;
        }
        str++;
    }

    // Null-terminate the string of digits
    digitStr[j] = '\0';

    // Convert the string of digits to an integer
    if (j > 0) {
        return atoi(digitStr);
    } else {
        return -1; // Return -1 if no digits are found
    }
}


int check_status(long int pnr)
{
  int ind = -1;
  ind = findBookingByPNR(pnr , numBookings);
  if (ind == -1)
  {
    printf("\nPNR not found\n");
    return 0;
  }
  else
  {
    if (strcmp(bookings[ind].status, "CNF")==0)
    { setColorGreen();
      printf("\nYour ticket has been confirmed. \n");
      resetColor();
      int tic_ind;
      tic_ind = findTicketByPNR(pnr , numTickets);
      displayTicket(tickets , tic_ind );
      return 0;

    }

    else if(strcmp(bookings[ind].status, "CAN")==0)
    { setColorYellow();
      printf("\nTHE TICKET HAS BEEN CANCELLED\n");
      resetColor();
    }
    else
    {
      printf("\nYou are in the waiting list\n");
      printf("\nYour current waitlist position is: %s\n", bookings[ind].status);
      int pos;
      pos = extractDigitsFromString(bookings[ind].status);
      double prob; 
      prob = (double)(300 - pos + 1)/300;
      setColorGreen();
      printf("\nThe CNF probability of your seat getting confirmed is: %lf\n",prob);
      resetColor();
      setColorRed();
      printf("\nDISCLAIMER: THIS IS DOES NOT GUARANTEE TICEKT CONFIRMATION. THIS IS JUST A INDICATOR.\n");
      resetColor();
      return 0;
    }
  }
}

int refund(double refund_amt, int status) 
{
  char username[MAX_USERNAME_LEN];
  long int account_number;
  int index;
  int valid_input = 0;

  printf("Enter account number: ");
  scanf("%ld", &account_number);
  clearInputBuffer();
  printf("Enter your username: ");
  scanf("%s", username);
  clearInputBuffer();

  for (int i = 0; i < num_users; i++) {
    if (users[i].account_number == account_number && strcmp(users[i].username, username) == 0) {
      valid_input = 1;
      index = i;
      break;
    }
  }

  if (!valid_input) 
  {
    printf("\nWrong username or account number. Please try again.\n");
  }

  if (status == 1) //eligible for 100% refund

  { system("clear");
    setColorYellow();
    printf("\nYou are elidgible for a 100 percent refund \n ");
    resetColor();
    printf("\nInitial Balance is: %lf\n",users[index].balance);
    users[index].balance += refund_amt;
    setColorGreen();   
    printf("Refund successful.\n");
    resetColor();
    printf("\nYour current balance is: %lf\n\n", users[index].balance);
    return 1;
  }
 if (status == 0)  //not eligible for 60% refund
  { 
    system("clear");;
    setColorYellow();
    printf("\nYou are applicable only for a 60 percent refund\n");
    resetColor();
    printf("\nInitial Balance is: %lf\n",users[index].balance);
    users[index].balance += refund_amt * 0.6;
    setColorGreen();
    printf("Refund successful.\n");
    resetColor();
    printf("New balance: %lf\n", users[index].balance);
    return 0;
  }
  return 0;
    
}
