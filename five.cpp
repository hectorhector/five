#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>

void find_five();

struct word
{
	std::string s;
	uint32_t n = 0;

	bool operator<(const word& other)  const { return n < other.n; }
};

struct answer
{
	std::vector<struct word> w;
	uint32_t n = 0;
};

uint32_t ston(std::string s)
{
	uint32_t n = 0;

	for (int i = 0; i < 5; i++)
	{
		uint32_t bit = s.at(i) - 'a';

		//check for duplicate letters
		if (((n >> bit) & (uint32_t)1) == 1)
		{
			return 0;
		}

		n |= 1 << bit;
	}

	return n;
}

std::map<struct word, std::vector<struct word>> word_map;
//std::map<uint32_t, std::vector<struct word>> letter_map;
struct answer current_answers;

int main()
{
	std::vector<struct word> words;
	std::set<uint32_t> anagram_checker;
	std::string line;

	time_t start;
	time(&start);

	printf("hello, searching for 5 5-letter words with no letters in common!\n");

	std::ifstream allowed_words("wordle-allowed-guesses.txt");
	while (std::getline(allowed_words, line))
	{
		struct word w;
		w.n = ston(line);

		if (w.n == 0)
		{
			continue;
		}

		auto res = anagram_checker.insert(w.n);
		if (res.second == false)
		{			
			continue;
		}

		w.s = line;
		words.push_back(w);
	}
	printf("loaded allowed wordle guesses\n");

	std::ifstream answer_words("wordle-answers-alphabetical.txt");
	while (std::getline(answer_words, line))
	{
		struct word w;
		w.n = ston(line);

		if (w.n == 0)
		{
			continue;
		}

		auto res = anagram_checker.insert(w.n);
		if (res.second == false)
		{
			continue;
		}

		w.s = line;
		words.push_back(w);
	}
	printf("loaded wordle answers\n");

	// for each word, build a list of all the words that don't have letters in common
	for (size_t i = 0; i < words.size(); i++)
	{
		for (size_t j = i + 1; j < words.size(); j++)
		{
			if ((words[i].n & words[j].n) == 0)
			{
				word_map[words[i]].push_back(words[j]);
			}
		}
	}
	printf("created the word map\n");

	// for every word, try to build an answer list
	for (size_t i = 0; i < words.size(); i++)
	{
		current_answers.w.clear();
		current_answers.n = 0;

		struct word current_word = words[i];
		current_answers.w.push_back(current_word);
		current_answers.n = current_word.n;
		find_five();
	}

	time_t end;
	time(&end);
	printf("finished in %f seconds\n", difftime(end, start));
	return 0;
}

void find_five()
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
		// search for next word to add to current_answers
		for (auto next : word_map[current_answers.w.back()])
		{
			if ((current_answers.n & next.n) == 0)
			{
				// we found a new word to add
				current_answers.n |= next.n;
				current_answers.w.push_back(next);
				find_five();
			}
		}
	}

	// current word is a dead-end, remove it from the current_answers
	current_answers.n ^= current_answers.w.back().n;
	current_answers.w.pop_back();
}