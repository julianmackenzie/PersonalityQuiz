// CS251 Project 2 driver.h file
// by Julian Mackenzie 662056584
//
// This is a file containing many functions to be called
// inside of main.cpp. They are used in necessary steps
// of the personality test, to normalize a set of scores,
// compare a set of scores, and output which set matches
// the user's input the closest.


#include <math.h>
#include <limits>
#include <string>
#include <map>
#include <set>
#include <utility>
#include "myrandom.h"

using namespace std;

constexpr double lowest_double = std::numeric_limits<double>::lowest();

/* Type: Question
 *
 * Type representing a personality quiz question.
 */
struct Question {
    string questionText;  // Text of the question
    map<char, int> factors;   // Map from factors to +1 or -1
    friend bool operator< (const Question& lhs, const Question& rhs) {
        return lhs.questionText < rhs.questionText;
    }
    friend bool operator== (const Question& lhs, const Question& rhs) {
        return lhs.questionText == rhs.questionText;
    }
    friend bool operator!= (const Question& lhs, const Question& rhs) {
        return lhs.questionText != rhs.questionText;
    }
};

/* Type: Person
 *
 * Type representing a person, used to represent people when determining
 * who's the closest match to the user.
 */
struct Person {
    string name;      // Name of the person
    map<char, int> scores;  // Map from factors to +1 or -1
    friend bool operator< (const Person& lhs,   const Person& rhs) {
        return lhs.name < rhs.name;
    }
    friend bool operator== (const Person& lhs, const Person& rhs) {
        return lhs.name == rhs.name;
    }
    friend bool operator!= (const Person& lhs, const Person& rhs) {
        return lhs.name != rhs.name;
    }
};



/* randomElement
 *
 * This function selects, at random, a Question from the inputted questions set
 * and returns the question.  Note, this function does not remove the randomly
 * selected question from the set.
*/
Question randomElement(set<Question>& questions) {
    int ind = randomInteger(0, (int)questions.size()-1);
    int i = 0;
    for (auto e : questions) {
        if (i == ind) {
            return e;
        }
        i++;
    }
    return {};
}



// From a full set of questions, use randomElement() to choose a random
// questions from a set to prompt the user with
Question randomQuestionFrom(set<Question>& questions) {
    Question toAsk;  // space to be filled with a question from the set

    if (questions.size() == 0) throw runtime_error("ourvector: empty");
    else if (questions.size() == 1) toAsk = *questions.begin();
    else
    {
      toAsk = randomElement(questions);
    }
    questions.erase(toAsk);
    return toAsk;
}



// Operates on each OCEAN score (factor.second) using user-provided
// question answers. This will be the final un-normalized OCEAN
// point value.
int doOperations(pair<const char, int> factor, int answer) {
  int result = factor.second;

  if (answer == 1) {  // Strongly disagree
    result *= -2;
  } else if (answer == 2) {  // Disagree
    result *= -1;
  } else if (answer == 3) {  // Neutral
    result *= 0;
  } else if (answer == 5) {  // Strongly agree
    result *= 2;
  }

  return result;
}



// Using a map of Questions and ints, this function takes each factor
// of each question and sends it to doOperations() to be processed. The
// returned values are tallied up and placed in the scores map.
map<char, int> scoresFrom(map<Question, int>& answers) {
  map<char, int> scores;

  for (pair<const Question, int>& current : answers) {
    // for each OCEAN key and value of each question
    for (pair<const char, int> factor : current.first.factors) {
      // send to be operated on
      int result = doOperations(factor, current.second);

      if (scores.count(factor.first)) {  // if the key exists already
        scores[factor.first] += result;  // add to its score
      }
      else
      {
        // create key that doesn't exist yet
        scores.emplace(factor.first, result);
      }
    }
  }
  return scores;
}



// From calculated OCEAN scores, iterate across every key/value pair,
// square all values, add them together, and take the square root. iterate
// through each pair again, dividing values by that square root to
// normalize them. Outputs a map of keys and values that have been normalized.
map<char, double> normalize(map<char, int>& scores) {
  map<char, double> normalized;

  double acc = 0;  // accumulator

  map<char, int>::iterator it = scores.begin();
  while (it != scores.end()) {
    double powered = pow(static_cast<double>(it->second), 2);
    acc += powered;  // square each value, add them
    it++;
  }
  if (acc == 0) throw runtime_error("**Error: All scores are 0, cannot proceed");
  double squarert = sqrt(acc);  // square root of their sum

  it = scores.begin();
  while (it != scores.end()) {
    // divide each value by square root
    double normalscore = static_cast<double>(it->second) / squarert;
    normalized.emplace(it->first, normalscore);  // add to normalized list
    it++;
  }

  return normalized;
}

// This function creates a value from two normalized personality scores,
// stored in the value of each inputted map. The higher this value is,
// the more similar they are.
double cosineSimilarityOf(const map<char, double>& lhs,
                          const map<char, double>& rhs) {
    double acc = 0;  // accumulator

    for (auto it = lhs.begin(); it != lhs.end(); it++) {
      if (rhs.count(it->first)) {  // if the current key is in the other map
        auto it2 = rhs.find(it->first);  // get its index

        acc += it->second * it2->second;
      }
    }
    return acc;
}





// Iterates through each Person file inside of the people set
// and normalizes them. These normalized scores are then compared
// using cosineSimilarityOf() to the user's scores in the scores
// map. The closest match (highest "max" value) is output as the
// most similar person to the user.
Person mostSimilarTo(map<char, int>& scores, set<Person>& people) {
  Person mostSimilar;

  if (people.size() == 0) throw runtime_error("**Error: no people provided!");

  map<char, double> normalized = normalize(scores);

  double max = -999;  // cos similarity is between -1 and 1, -999 will work fine

  set<Person>::iterator itp = people.begin();
  while (itp != people.end()) {
    // extract scores since passing iterator -> into function doesn't compile
    map<char, int> pscores = itp->scores;
    map<char, double> pnormalized = normalize(pscores);

    double similarity = cosineSimilarityOf(normalized, pnormalized);
    if (similarity >= max) {
      max = similarity;
      mostSimilar = *itp;
    }
    itp++;
  }

    return mostSimilar;
}
