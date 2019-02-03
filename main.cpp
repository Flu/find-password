#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

constexpr unsigned int PASSWORD_LENGTH = 40u;
constexpr unsigned int GEN_POPULATION = 600u;
constexpr unsigned int BEST_SAMPLE_N = 45u;
constexpr unsigned int LUCKY_FEW_N = 15u;
constexpr float MUTATION_CHANCE = 0.08f;

// Class for holding data about a single individual
class Individual {
  string word;
  float fitness_value;
public:
  Individual(const string& word, const string& password) : word(word) {
    fitness_value = this->fitness(password);
  }
  Individual(const Individual& indiv) {
    this->word = indiv.word;
    this->fitness_value = indiv.fitness_value;
  }
  // Compute fitness based on how many characters are different from the password
  float fitness(const string& password) {
    int score = 0;
    for (unsigned int index = 0; index < PASSWORD_LENGTH; index++)
      score += (password[index] == this->word[index]) ? 1 : 0;
    this->fitness_value = (float)score / PASSWORD_LENGTH * 100;
    return this->fitness_value;
  }
  const string& getWord() const {
    return this->word;
  }
  string& getWord() {
    return this->word;
  }
  const float getFitness() const {
    return this->fitness_value;
  }
  // For std::sort
  bool operator<(const Individual& inv) const {
    if (this->fitness_value <= inv.fitness_value)
      return false;
    return true;
  }
};

// Generates a random word with an ASCII value between 32 and 126 - letters, digits and punctuation
string random_word(const int length) {
  if (length <= 0)
    throw "Nyet";
  string ret;
  ret.reserve(length);
  for (int index = 0; index < length; index++)
    ret.push_back(rand() % (126 - 32 + 1) + 32);
  return ret;
}

// Function to generate the first generation of individuals, all of them being initialized with a random_password
void generate_first_generation(vector<Individual>& pop, const int size, const string& password) {
  pop.reserve(size);
  for (int index = 0; index < size; index++) {
    pop.emplace_back(random_word(40), password);
  }
}

// Check the ordered vector for any individual with 100% fitness score. If one exists, it is on the first position
bool check_fitness(const vector<Individual>& gen, const string& password) {
  if (gen[0].getFitness() == 100.0f)
    return true;
  return false;
}

// Select the best parents to add to the breeders vector
void select_parents(const vector<Individual>& population, vector<Individual>& breeders) {
  breeders.reserve(BEST_SAMPLE_N + LUCKY_FEW_N);
  // Select the fittest
  for (unsigned int index = 0; index < BEST_SAMPLE_N; index++)
    breeders.push_back(population[index]);

  // Select random individuals from the rest of the population
  for (unsigned int index = 0; index < LUCKY_FEW_N; index++)
    breeders.push_back(population[rand() % 600]);
}

// For every character in the child, there is a chance that it mutates
void mutate_child(Individual& child) {
  for (unsigned int index = 0; index < child.getWord().size(); index++) {
    if ((float)rand()/RAND_MAX < MUTATION_CHANCE) {
      child.getWord()[index] = (char)(' ' + (rand() % 95));
    }
  }
}

// Function that takes two individuals, and creates they child with the uniform cross-over method
Individual create_child(const Individual& parent1, const Individual& parent2, const string& password) {
  string child_word;
  // For every character, choose a random character from one of the parents
  for (unsigned int character = 0; character < PASSWORD_LENGTH; character++) {
    int randint = rand() % 2;
    child_word.push_back(randint == 0 ? parent1.getWord()[character] : parent2.getWord()[character]);
  }

  // Create child
  Individual child(child_word, password);
  // Mutate the child
  mutate_child(child);
  // Recalculate its fitness
  child.fitness(password);
  return child;
}

// Erase the last population, and start again, creating children from the breeders vector. The new generation shall have the same number of individuals as the last one
void create_children(vector<Individual>& population, const vector<Individual>& breeders, const string& password) {
  // Delete all the individuals from the last population, won't need them anymore
  population.erase(population.begin(), population.end());
  
  // For every two breeders, make two children
  for (unsigned int index = 0; index < breeders.size()/2; index++) {
    for (int child = 0; child < 20; child++) {
      population.push_back(create_child(breeders[index], breeders[breeders.size() - index - 1], password));
    }
  }
}

int main() {
  srand(time(NULL));
  
  // Generate the random password
  string pass = random_word(PASSWORD_LENGTH);

  cout.width(60);
  cout << left << "Password to guess: " << pass << endl;
  vector<Individual> population;

  // The first generation is initialized with 600 random individuals
  int generation = 0;
  generate_first_generation(population, GEN_POPULATION, pass);
  sort(population.begin(), population.end());
  
  // While no individual has 100% fitness, breed them again
  while (!check_fitness(population, pass)) {
    generation++;
    // This vector will hold the parents
    vector<Individual> breeders;
    
    // Select parents for the next generation
    select_parents(population, breeders);
    
    // Create the children for the next generation
    create_children(population, breeders, pass);
    sort(population.begin(), population.end());
  }

  // Display some noice statistics
  cout.width(60);
  cout << "Number of generations needed: " << left << generation << endl;
  cout.width(60);
  cout << "Best child so far: " << left << population[0].getWord() << endl;
  cout.width(60);
  cout << "With a fitness value of: " << left << population[0].getFitness() << endl;
  
  return 0;
}
