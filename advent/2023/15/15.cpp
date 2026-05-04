#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

// This function calculates the HASH value of a given string
int HASH(std::string s) {
    int current = 0;         // Initialize the current value to 0
    for (char c : s) {       // Loop over each character in the string
        int ascii = (int)c;  // Get the ASCII code of the character
        current += ascii;    // Increase the current value by the ASCII code
        current *= 17;       // Multiply the current value by 17
        current %= 256;      // Get the remainder of dividing the current value by 256
    }
    return current;  // Return the current value as the output of the HASH algorithm
}

// This struct represents a lens with a label and a focal length
struct Lens {
  std::string label;
  int focal_length;
};

// This function performs the HASHMAP procedure on the initialization sequence
void hashmap(std::string sequence) {
  std::vector<std::vector<Lens>> boxes(
      256);                  // Declare a vector of vectors of Lens to store the boxes and the lenses inside them
  std::string step;          // Declare a string to store each step
  for (char c : sequence) {  // Loop over each character in the sequence
      if (c == ',') {        // If the character is a comma
          std::string label = step.substr(0, 2);  // Get the label of the lens from the first two characters of the step
          int box = HASH(label);                  // Get the box number by running the HASH algorithm on the label
          char op = step[2];                      // Get the operation character from the third character of the step
          if (op == '-') {                        // If the operation is to remove a lens
              for (int i = 0; i < boxes[box].size(); i++) {      // Loop over the lenses in the box
                  if (boxes[box][i].label == label) {            // If the lens has the same label as the step
                      boxes[box].erase(boxes[box].begin() + i);  // Erase the lens from the box
                      break;                                     // Break the loop
                  }
              }
          } else if (op == '=') {                     // If the operation is to add or replace a lens
              int focal = std::stoi(step.substr(3));  // Get the focal length of the lens from the rest of the step
              Lens lens = {label, focal};             // Create a Lens struct with the label and the focal length
              bool replaced = false;                  // Declare a boolean flag to indicate if the lens was replaced
              for (int i = 0; i < boxes[box].size(); i++) {  // Loop over the lenses in the box
                  if (boxes[box][i].label == label) {        // If the lens has the same label as the step
                      boxes[box][i] = lens;                  // Replace the lens with the new one
                      replaced = true;                       // Set the flag to true
                      break;                                 // Break the loop
                  }
              }
              if (!replaced) {                 // If the lens was not replaced
                  boxes[box].push_back(lens);  // Add the lens to the end of the box
              }
          }
          step = "";  // Reset the step to an empty string
      } else {        // If the character is not a comma
          step += c;  // Append the character to the current step
      }
  }
  // Perform the last step
  std::string label = step.substr(0, 2);  // Get the label of the lens from the first two characters of the step
  int box = HASH(label);                  // Get the box number by running the HASH algorithm on the label
  char op = step[2];                      // Get the operation character from the third character of the step
  if (op == '-') {                        // If the operation is to remove a lens
      for (int i = 0; i < boxes[box].size(); i++) {      // Loop over the lenses in the box
          if (boxes[box][i].label == label) {            // If the lens has the same label as the step
              boxes[box].erase(boxes[box].begin() + i);  // Erase the lens from the box
              break;                                     // Break the loop
          }
      }
  } else if (op == '=') {                            // If the operation is to add or replace a lens
      int focal = std::stoi(step.substr(3));         // Get the focal length of the lens from the rest of the step
      Lens lens = {label, focal};                    // Create a Lens struct with the label and the focal length
      bool replaced = false;                         // Declare a boolean flag to indicate if the lens was replaced
      for (int i = 0; i < boxes[box].size(); i++) {  // Loop over the lenses in the box
          if (boxes[box][i].label == label) {        // If the lens has the same label as the step
              boxes[box][i] = lens;                  // Replace the lens with the new one
              replaced = true;                       // Set the flag to true
              break;                                 // Break the loop
          }
      }
      if (!replaced) {                 // If the lens was not replaced
          boxes[box].push_back(lens);  // Add the lens to the end of the box
      }
  }
  // Calculate the focusing power of all the lenses
  long long power = 0;                                            // Initialize the power to 0
  for (int i = 0; i < 256; i++) {                                 // Loop over the boxes
      for (int j = 0; j < boxes[i].size(); j++) {                 // Loop over the lenses in each box
          power += static_cast<long long>(i + 1) * static_cast<long long>(j + 1) *
                   boxes[i][j].focal_length;  // Add the product of the box number, the slot number,
                                              // and the focal length to the power
      }
  }
  // Print the output
  std::cout << "The focusing power of all the lenses is " << power << std::endl;
}

// This function calculates the sum of the HASH values of each step in the initialization sequence
int sum_of_HASHes(std::string sequence) {
  int sum = 0;                // Initialize the sum to 0
  std::string step;           // Declare a string to store each step
  for (char c : sequence) {   // Loop over each character in the sequence
      if (c == ',') {         // If the character is a comma
          sum += HASH(step);  // Add the HASH value of the current step to the sum
          step = "";          // Reset the step to an empty string
      } else {                // If the character is not a comma
          step += c;          // Append the character to the current step
      }
  }
  sum += HASH(step);  // Add the HASH value of the last step to the sum
  return sum;         // Return the sum as the output of the function
}

void first() {
  const auto input = readInputLines();
  cout << sum_of_HASHes(input[0]) << endl;
}

void second() {
  const auto input = readInputLines();
  hashmap(input[0]);
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}

