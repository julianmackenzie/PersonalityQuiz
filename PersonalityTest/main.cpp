// CS251 Project 2: Personality Tests
// by Julian Mackenzie 662056584
//
// This program accepts a list of questions in a particular
// format and prompts the user to answer a random selection
// of them, tallying and processing the results to find the
// closest match inside of a .people file, which is opened
// and processed similarly to the question file upon the
// user's selection.
//
// Many of this program's functions are contained in
// "driver.h" and are explained in detail there.

#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "driver.h"

using namespace std;


// Attempts to open a specified file, outputs false if file doesn't exist or
// opens improperly, opens file in specified filestream if successful
bool openFile(ifstream& inFS, string filename) {
  inFS.open(filename);
  if (!inFS.good()) {
    cout << "**Error: could not open '" << filename << "'" << endl;
    return false;
  }
  return true;
}


// Called to handle all data file opening and closing (done in
// openFile()) and parses data from specified file into a string
// (key) and into a map which stores pairs (OCEAN factor and value)
// in specified set. Returns false if openFile() declares an error.
//
// Boolean isPerson used to differentiate between Question opening
// and Person opening. This way, this single function can process
// both types without repeated code.
bool inputData(string filename, set<Question>& questions,
set<Person>& people, bool isPerson) {
  ifstream inFS;
  // return to main() if error occurs
  if (!openFile(inFS, filename)) return false;

  string key;  // used to hold each line of file

  getline(inFS, key, '.');  // get first line until period
  while (inFS.good()) {
    Question currentquestion;  // temporary Question structure
    Person currentperson;
    key.append(".");  // re-add the period that was used as a delimiter
    // assign line's text as key
    if (!isPerson) currentquestion.questionText = key;
    else if (isPerson) currentperson.name = key;
    string statsline;
    getline(inFS, statsline);  // grab rest of line
    stringstream ss(statsline);  // create stringstream

    while (ss.good()) {
      char stat, colon;  // char to skip over
      ss >> stat;
      ss >> colon;
      string numstr;  // string to hold rest of factor sequence
      ss >> numstr;

      // convert string to int, push into map
      if (!isPerson) currentquestion.factors.emplace(stat, stoi(numstr));
      else if (isPerson) currentperson.scores.emplace(stat, stoi(numstr));
    }
    // if working with Question, add individual question to set of questions
    if (!isPerson) questions.insert(currentquestion);
    else if (isPerson) people.insert(currentperson);
    getline(inFS, key, '.');  // begin input for next question
  }
  inFS.close();
  return true;  // return true, all is well
}


// Prompts the user with chosen question and asks for an answer
int doQuestionPrompt(Question& current) {
  int answer;

  cout << endl << "How much do you agree with this statement?" << endl
  << "\"" << current.questionText << "\"" << endl << endl
  << "1. Strongly disagree" << endl << "2. Disagree\n3. Neutral"
  << endl << "4. Agree\n5. Strongly agree" << endl << endl
  << "Enter your answer here (1-5): ";

  cin >> answer;

  return answer;
}


// Responsible for prompting the player each time (in a separate function
// because there's just so much to output) and getting their choice
void doPeoplePrompt(int& choice) {
  cout << endl << "1. BabyAnimals\n2. Brooklyn99\n3. Disney"<< endl
  << "4. Hogwarts\n5. MyersBriggs\n6. SesameStreet\n7. StarWars"
  << endl << "8. Vegetables\n9. mine\n0. To end program." << endl
  << endl << "Choose test number (1-9, or 0 to end): ";
  cin >> choice;

  return;
}

// Responsible for opening the file corresponding to the user's choice
void doPeopleOpen(int& choice, set<Person>& people, set<Question>& questions) {
  if (choice == 1)
  inputData("BabyAnimals.people", questions, people, true);
  else if (choice == 2)
  inputData("Brooklyn99.people", questions, people, true);
  else if (choice == 3)
  inputData("Disney.people", questions, people, true);
  else if (choice == 4)
  inputData("Hogwarts.people", questions, people, true);
  else if (choice == 5)
  inputData("MyersBriggs.people", questions, people, true);
  else if (choice == 6)
  inputData("SesameStreet.people", questions, people, true);
  else if (choice == 7)
  inputData("StarWars.people", questions, people, true);
  else if (choice == 8)
  inputData("Vegetables.people", questions, people, true);
  else if (choice == 9)
  inputData("mine.people", questions, people, true);
  return;
}


// Main program runtime happens here, this is the "UI"
// uses the question set and the user-inputted amount of questions
// and employs the driver.h functions to execute the program
void executeProgram(set<Question>& questions, set<Person>& people,
int& numQuestions) {
  map<Question, int> answers;

  for (int i=0; i < numQuestions; i++) {
    // Get random question from set
    Question current = randomQuestionFrom(questions);
    int answer = doQuestionPrompt(current);
    answers.emplace(current, answer);
  }
  map<char, int> scores = scoresFrom(answers);  // Calculate true OCEAN scores

  int choice;

  doPeoplePrompt(choice);

  while (choice != 0) {
    people.clear();
    doPeopleOpen(choice, people, questions);

    Person mostSimilar = mostSimilarTo(scores, people);
    string similarName = mostSimilar.name;
    similarName.pop_back();
    cout << "You got " << similarName << "!" << endl;
    doPeoplePrompt(choice);
  }

  cout << "Goodbye!" << endl;
}



// Declares empty sets, calls inputData() to read in questions, and executes
// primary program function with executeProgram. Very little time spent here.
int main() {
  set<Question> questions;
  set<Person> people;
  // input questions, catch failure and end program
  if (!inputData("questions.txt", questions, people, false)) return 0;

  cout << "Welcome to the Personality Quiz!" << endl << endl;
  cout << "Choose number of questions: ";
  int numQuestions;
  cin >> numQuestions;
  executeProgram(questions, people, numQuestions);

  return 0;
}
