#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <iomanip>    // For formatting output
#include <sstream>    // For stringstream
#include <stdexcept>  // For exception handling
#include <cstdlib>    // For rand()
#include <ctime>      // For time

using namespace std;

// Patient Class: Represents a patient with an ID, gender, arrival time, type, and arrival in minutes
class Patient {
    string id;               // Patient ID
    char gender;             // 'M' or 'F' for gender
    string arrival_time;     // Arrival time in HH:MM format
    string type;             // Type of patient: "Urgent" or "Normal"
    int arrival_minute;      // Arrival time in minutes (relative to the start time)

public:
    // Constructor to initialize patient details
    Patient(string id, char gender, string time, string type, int arrival_minute)
        : id(id), gender(gender), arrival_time(time), type(type), arrival_minute(arrival_minute) {}

    // Getters for patient attributes
    string getId() const { return id; }
    char getGender() const { return gender; }
    string getArrivalTime() const { return arrival_time; }
    string getType() const { return type; }
    int getArrivalMinute() const { return arrival_minute; }
};

// PatientGenerator Class: Generates random patient data for simulation
class PatientGenerator {
public:
    // Generate a random patient at a given minute
    static Patient generateRandomPatient(int minute) {
        // Random 14-digit ID starting with 2 or 3
        int first_digit = rand() % 2 + 2;  // Randomly select between 2 and 3
        string id = to_string(first_digit);  // Start the ID with 2 or 3

        // Generate the remaining 13 digits
        for (int i = 0; i < 13; i++) {
            id += to_string(rand() % 10);  // Append random digits (0-9)
        }

        char gender = (rand() % 2 == 0) ? 'M' : 'F';  // Random gender (M or F)
        string arrival_time = to_string(rand() % 24) + ":" + to_string(rand() % 60);  // Random time in HH:MM format
        string type = (rand() % 2 == 0) ? "Urgent" : "Normal";  // Random type ("Urgent" or "Normal")

        return Patient(id, gender, arrival_time, type, minute);  // Return the generated patient
    }

    // Generate a list of patients given a count and start time
    static vector<Patient> generatePatients(int count, int start_minute) {
        vector<Patient> patients;
        for (int i = 0; i < count; i++) {
            patients.push_back(generateRandomPatient(start_minute));  // Add each random patient to the list
        }
        return patients;
    }
};

// Scheduler Class: Handles the queuing and serving of patients
class Scheduler {
    queue<Patient> urgent_queue;        // Queue for urgent patients
    queue<Patient> normal_queue;        // Queue for normal patients
    vector<Patient> served_patients;    // List of patients who have been served
    int total_patients = 0;             // Total number of patients in the system
    int total_urgent = 0;               // Count of urgent patients
    int total_normal = 0;               // Count of normal patients
    int total_waiting_time = 0;         // Total waiting time for served patients
    int total_served = 0;               // Total number of patients served

public:
    void addPatient(const Patient& patient);   // Add patient to the appropriate queue
    void servePatients(int max_to_serve, int minute);  // Serve patients based on available slots
    void displayQueues();                    // Display current state of queues
    void displayStatistics();                // Display simulation statistics
    bool isUrgentQueueEmpty() const { return urgent_queue.empty(); }  // Check if the urgent queue is empty
    bool isNormalQueueEmpty() const { return normal_queue.empty(); }  // Check if the normal queue is empty
};

// Add a patient to the correct queue based on their type
void Scheduler::addPatient(const Patient& patient) {
    if (patient.getType() == "Urgent") {
        urgent_queue.push(patient);   // Add to urgent queue
        total_urgent++;
    } else {
        normal_queue.push(patient);   // Add to normal queue
        total_normal++;
    }
    total_patients++;  // Increment total patients count
}

// Serve patients with priority given to urgent cases
void Scheduler::servePatients(int max_to_serve, int minute) {
    int served = 0;

    // Serve urgent patients first
    while (served < max_to_serve && !urgent_queue.empty()) {
        try {
            if (!urgent_queue.empty()) {
                Patient p = urgent_queue.front();
                urgent_queue.pop();  // Remove the patient from the queue

                // Calculate the waiting time for the patient
                int waiting_time = minute - p.getArrivalMinute();
                
                if (waiting_time > 10) {
                    // Skip serving if the patient has been waiting too long (more than 10 minutes)
                    continue;
                }

                served_patients.push_back(p);  // Add patient to served list
                total_waiting_time += waiting_time;  // Add waiting time to the total
                served++;  // Increment the number of patients served
            } else {
                throw runtime_error("Urgent queue is empty!");  // Error if urgent queue is empty
            }
        } catch (const exception& e) {
            cout << "Error while serving urgent patients: " << e.what() << endl;
        }
    }

    // Serve normal patients if there's room
    while (served < max_to_serve && !normal_queue.empty()) {
        try {
            if (!normal_queue.empty()) {
                Patient p = normal_queue.front();
                normal_queue.pop();  // Remove patient from normal queue

                // Calculate waiting time for normal patients
                int waiting_time = minute - p.getArrivalMinute();
                
                if (waiting_time > 10) {
                    // Skip serving if the patient has been waiting too long
                    continue;
                }

                served_patients.push_back(p);  // Add patient to the served list
                total_waiting_time += waiting_time;  // Add waiting time to the total
                served++;  // Increment the served patient count
            } else {
                throw runtime_error("Normal queue is empty!");  // Error if normal queue is empty
            }
        } catch (const exception& e) {
            cout << "Error while serving normal patients: " << e.what() << endl;
        }
    }

    total_served += served;  // Update total number of served patients
}

// Display the current state of the urgent and normal queues
void Scheduler::displayQueues() {
    cout << "\nCurrent State of Queues:\n";

    // Display the IDs of patients in the urgent queue
    cout << "Urgent Queue: ";
    queue<Patient> temp_urgent = urgent_queue;
    while (!temp_urgent.empty()) {
        cout << temp_urgent.front().getId() << " ";
        temp_urgent.pop();
    }
    cout << endl;

    // Display the IDs of patients in the normal queue
    cout << "Normal Queue: ";
    queue<Patient> temp_normal = normal_queue;
    while (!temp_normal.empty()) {
        cout << temp_normal.front().getId() << " ";
        temp_normal.pop();
    }
    cout << endl;

    // Display the IDs of currently served patients
    cout << "Currently Served Patients: ";
    for (const auto& p : served_patients) {
        cout << p.getId() << " ";
    }
    cout << endl;
}

// Display the overall simulation statistics
void Scheduler::displayStatistics() {
    cout << "\nSimulation Summary:\n";
    cout << "Total Patients: " << total_patients << endl;
    cout << "Urgent Patients: " << total_urgent << endl;
    cout << "Normal Patients: " << total_normal << endl;
    cout << "Total Served Patients: " << total_served << endl;

    // Calculate and display average waiting time
    if (total_served > 0) {
        double avg_waiting_time = static_cast<double>(total_waiting_time) / total_served;
        cout << "Average Waiting Time: " << fixed << setprecision(2) << avg_waiting_time << " minutes" << endl;
    } else {
        cout << "Average Waiting Time: N/A (no patients served)" << endl;
    }
}

int main() {
    srand(time(0));  // Seed the random number generator for random patient data

    Scheduler scheduler;  // Create a scheduler instance
    int minute = 0;       // Initialize the time variable

    // Generate a list of 100 random patients and add them to the scheduler
    vector<Patient> patients = PatientGenerator::generatePatients(100, minute);
    for (auto& p : patients) {
        scheduler.addPatient(p);
    }

    cout << "Welcome to the Patient Scheduling System!\n";
    cout << "You can input patient details manually or type 'next' to advance time.\n";
    cout << "Format: ID Gender(M/F) ArrivalTime(HH:MM) Type(Urgent/Normal)\n";

    // Main program loop
    while (true) {
        // Print the current minute to track time progression
        cout << "\n--- Minute " << minute << " ---\n";
        cout << "Enter patient details or type 'next' to advance time:\n";

        string input;
        getline(cin, input);  // Get user input for patient details or commands

        // Trim whitespace from input to avoid errors from extra spaces or newline characters
        input.erase(input.find_last_not_of(" \n\r\t") + 1);

        if (input.empty()) {
            cout << "No input provided. Please try again.\n";
            continue;  // Prompt user to try again if input is empty
        }

        // If the user types 'next', advance time and serve patients
        if (input == "next") {
            // Randomly determine how many patients to serve (between 5 and 10)
            int max_to_serve = rand() % 6 + 5;  
            scheduler.servePatients(max_to_serve, minute);  // Serve patients for this minute

            // Display the current state of the queues (Urgent and Normal)
            scheduler.displayQueues();

            // Increment time (one minute has passed)
            minute++;

            // Check if both queues are empty, signaling the end of the simulation
            if (scheduler.isUrgentQueueEmpty() && scheduler.isNormalQueueEmpty()) {
                cout << "All patients have been served. Ending simulation.\n";
                break;  // Exit the loop if all patients are served
            }

            continue;  // Proceed to the next iteration of the loop (time incremented)
        }

        // Parse the patient details input by the user
        string id, arrival_time, type;
        char gender;

        try {
            // Use stringstream to parse the input into the appropriate variables
            stringstream ss(input);
            ss >> id >> gender >> arrival_time >> type;

            // Normalize the type of patient to uppercase for consistency
            for (char& c : type) c = toupper(c);  // Convert type to uppercase (Urgent/Normal)

            // Check if the patient type is valid
            if (type != "URGENT" && type != "NORMAL") {
                throw invalid_argument("Invalid patient type. Must be 'Urgent' or 'Normal'.");
            }

            // Create a new patient object using the parsed data, assigning the current minute as the arrival time
            Patient patient(id, gender, arrival_time, type, minute);
            scheduler.addPatient(patient);  // Add the patient to the scheduler
        } catch (exception& e) {
            // Catch any parsing or validation errors and provide feedback to the user
            cout << "Invalid input: " << e.what() << "\nPlease try again.\n";
        }
    }

    // After the loop ends, display the final statistics of the simulation
    scheduler.displayStatistics();

    return 0;  // Return from the main function, ending the program
}