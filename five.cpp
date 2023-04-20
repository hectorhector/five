#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <array>

void find_five_ll(uint32_t l);

struct word
{
	std::string s;
	uint32_t n = 0;

	bool operator<(const word& other)  const { return n < other.n; }

	bool operator==(const word& other)  const { return n == other.n; }
};

struct answer
{
	std::vector<struct word> w;
	uint32_t n = 0;
};

struct answer current_answers;

std::array<std::set<struct word>, 26> letter_list;

void ston(std::string s)
{
	struct word w;
	uint32_t n = 0;

	for (int i = 0; i < 5; i++)
	{
		uint32_t bit = s.at(i) - 'a';

		//check for duplicate letters
		if (((n >> bit) & (uint32_t)1) == 1)
		{
			return;
		}

		n |= (1 << bit);
	}

	w.n = n;
	w.s = s;
	auto i = 0;
	while ((n & (uint32_t)1) == 0)
	{
		n >>= 1;
		i++;
	}
	letter_list[i].insert(w);
}


int main()
{
	//std::vector<struct word> words;
	std::set<uint32_t> anagram_checker;
	std::string line;

	time_t start;
	time(&start);

	printf("hello, searching for 5 5-letter words with no letters in common!\n");

	std::ifstream allowed_words("wordle-allowed-guesses.txt");
	while (std::getline(allowed_words, line))
	{
		(void)ston(line);
	}
	printf("loaded allowed wordle guesses\n");

	std::ifstream answer_words("wordle-answers-alphabetical.txt");
	while (std::getline(answer_words, line))
	{
		(void)ston(line);
	}
	printf("loaded wordle answers\n");

	// for every letter_list, try to build an answer list
	for (uint32_t i = 0; i < letter_list.size(); i++)
	{
		for(auto &current_word : letter_list[i])
		{
			current_answers.w.clear();
			current_answers.n = 0;

			current_answers.w.push_back(current_word);
			current_answers.n = current_word.n;
			find_five_ll(i);
		}
	}

	time_t end;
	time(&end);
	printf("finished in %f seconds\n", difftime(end, start));
	return 0;
}


void find_five_ll(uint32_t l)
{
	if (current_answers.w.size() == 5)
	{
		printf("solution found: ");
		for (auto answer : current_answers.w)
		{
			printf("%s, ", answer.s.c_str());
		}
		printf("\n");
	}
	else
	{
		// search for the next letter we need
		uint32_t i = l + 1;
		uint32_t letters = current_answers.n >> i;

		
		while ((letters & (uint32_t)1) == 1)
		{
			letters >>= 1;
			i++;
		}

		// found a missing letter
		for (auto next : letter_list[i])
		{
			if ((current_answers.n & next.n) == 0)
			{
				// we found a new word to add
				current_answers.n |= next.n;
				current_answers.w.push_back(next);
				find_five_ll(i);
			}
		}
	}

	// current word is a dead-end, remove it from the current_answers
	current_answers.n ^= current_answers.w.back().n;
	current_answers.w.pop_back();
}