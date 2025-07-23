# Railway Reservation System 🚂

This project is a command-line based Railway Reservation System developed in C. It leverages core data structure concepts and file handling to provide a robust and efficient platform for managing train reservations, passenger information, and administrative tasks.

---

## ✨ Key Features

* **User Authentication:** Secure login, registration, and password recovery system for users.
* **Admin Dashboard:** A separate, password-protected module for administrators to manage the system.
* **Train Management:** Admins can add new trains, remove existing ones, and view all available trains.
* **Booking & Availability:** Users can search for trains between stations, check seat availability, and book tickets.
* **Waitlist & RAC:** Automated handling for waitlisted passengers and Reservation Against Cancellation (RAC) for night trains.
* **Ticket Management:** Users can cancel their tickets and check the real-time status of their booking using a PNR number.
* **Sentiment Analysis:** An admin feature to view and analyze customer feedback, classifying it as positive or negative using a Naïve Bayes algorithm.

---

## 🛠️ Tech Stack

* **Language:** C
* **Core Concepts:** Data Structures, File Handling (Binary and Text files)

---

## 🗃️ Database Files

The system uses several files to persist data. These are created automatically on the first run if they don't exist.

* `users.csv`: Stores registered user details, including username, password, security question, and wallet information.
* `daytrains.bin`: A binary file containing all details for day trains, such as ID, name, route, stops, and seat availability.
* `nighttrains.bin`: A binary file containing all details for night trains.
* `feedbacks.txt`: A text file that stores all feedback submitted by customers.

---

## 🚀 Getting Started

Follow these instructions to compile and run the project on your local machine.

### Prerequisites

You'll need a C compiler, such as GCC, installed on your system.

### Compilation & Execution

1.  Clone the repository (or ensure you have the `master.c` file).
    ```sh
    git clone [https://your-repository-link-here.git](https://your-repository-link-here.git)
    cd repository-folder
    ```
2.  Compile the source code using the C compiler.
    ```sh
    gcc master.c -o railway_system
    ```
3.  Run the executable.
    ```sh
    ./railway_system
    ```
4.  **Admin Login:** The password for the admin module is hardcoded.
    * **Password:** `jdg`

---

## ⚙️ System Modules & Functionality

### User Authentication

* `registerUser()`: Prompts for a new username, password, security question, and account details, saving them to `users.csv`.
* `loginUser()`: Authenticates a user against the stored credentials.
* `forgotPassword()`: Allows users to recover their password by answering their security question.

### Admin Module

* `admin_login()`: Grants access to the admin dashboard. **Password: `jdg`**.
* `addTrain()` / `removeTrain()`: Allows the admin to add or delete train records from the system.
* `displayallTrains()`: Shows a complete list of all day and night trains.
* `viewCustomerFeedback()`: Reads `feedbacks.txt`, classifies each feedback as positive or negative, and displays an overall sentiment analysis with a histogram.

### Ticket Booking & Management

* `checkRouteAndPrintInfo()`: Allows users to search for trains based on source, destination, and date. It handles seat booking and moves passengers to a waitlist or RAC if seats are full.
* `cancelTickets(long int pnr)`: Processes ticket cancellations using a PNR. It updates seat availability, manages the waitlist, and processes refunds.
* `updateWaitlist(int tin, int type)`: Automatically confirms tickets for waitlisted passengers when a cancellation occurs for a day train.
* `checkStatus(long int pnr)`: Allows users to check the current status of their booking (e.g., Confirmed, Waitlisted) by entering their PNR number.
