#include <iostream>
#include <cstdlib>
#include <ctime>
#include <pthread.h>

using namespace std;

const int MAX_SIZE = 100;

// Global variables
int matris1length, unitMatrisLength;
int rastgeleMatris[MAX_SIZE][MAX_SIZE];
int unitMatris[MAX_SIZE][MAX_SIZE];
int newMatrisLength;
int newMatris[MAX_SIZE][MAX_SIZE];
int resultMatris[MAX_SIZE][MAX_SIZE];

// Mutex for synchronizing access to the output matrix
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to display the contents of the matrix
void displayMatrix(int matrix[MAX_SIZE][MAX_SIZE], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

// Function to compute a single element of the result matrix using a separate thread
void* computeElement(void* arg) {
    int index = *((int*)arg);
    int row = index / matris1length;
    int col = index % matris1length;

    int temporary = 0;
    for (int j = 0; j < unitMatrisLength; j++) {
        temporary += newMatris[row + j][col + j] * unitMatris[j][j];
    }

    // Acquire lock before updating the result matrix
    pthread_mutex_lock(&mutex);

    resultMatris[row][col] = temporary;

    // Display the matrix whenever a thread sets a value
    cout << "Thread " << pthread_self() << " sets value at (" << row << ", " << col << ")" << endl;
    displayMatrix(resultMatris, matris1length, matris1length);

    // Release lock after updating the result matrix
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    cout << "Enter the length of the first matris: ";
    cin >> matris1length;

    cout << "Enter the length of the unit matris: ";
    cin >> unitMatrisLength;

    srand(time(0));

    // Initialize matrices
    cout << "Random Matris:" << endl;
    for (int i = 0; i < matris1length; ++i) {
        for (int j = 0; j < matris1length; ++j) {
            rastgeleMatris[i][j] = rand() % 10;
            cout << rastgeleMatris[i][j] << "\t";
        }
        cout << endl;
    }

    cout << "Unit Matris:" << endl;
    for (int i = 0; i < unitMatrisLength; i++) {
        for (int j = 0; j < unitMatrisLength; j++) {
            unitMatris[i][j] = (i == j) ? 1 : 0;
            cout << unitMatris[i][j] << "\t";
        }
        cout << endl;
    }

    newMatrisLength = matris1length + (unitMatrisLength - 1);

    // Initialize result matrix with -1
    for (int i = 0; i < matris1length; i++) {
        for (int j = 0; j < matris1length; j++) {
            resultMatris[i][j] = -1;
        }
    }

    // Create threads and compute result matrix elements
    pthread_t threads[MAX_SIZE * MAX_SIZE];
    int threadIndices[MAX_SIZE * MAX_SIZE];

    int threadCount = 0;
    for (int i = 0; i < matris1length * matris1length; i++) {
        threadIndices[threadCount] = i;
        pthread_create(&threads[threadCount], NULL, computeElement, &threadIndices[threadCount]);
        threadCount++;
    }

    // Join threads
    for (int i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

