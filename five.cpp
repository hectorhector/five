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
};

struct answer
{
	std::vector<std::string> s;
	uint32_t n;
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

std::map<std::string, std::vector<struct word>> word_map;
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
				word_map[words[i].s].push_back(words[j]);
			}
		}
	}
	printf("created the word map\n");

	// for every word, try to build an answer list
	for (size_t i = 0; i < words.size(); i++)
	{
		current_answers.s.clear();
		current_answers.n = 0;

		struct word current_word = words[i];
		current_answers.s.push_back(current_word.s);
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
	if (current_answers.s.size() == 5)
	{
		printf("solution found: ");
		for (auto answer : current_answers.s)
		{
			printf("%s, ", answer.c_str());
		}
		printf("\n");
	}
	else
	{
		// search for next word to add to current_answers
		for (auto next : word_map[current_answers.s.back()])
		{
			if ((current_answers.n & next.n) == 0)
			{
				// we found a new word to add
				current_answers.n |= next.n;
				current_answers.s.push_back(next.s);
				find_five();
			}
#if 0
			else if ((ston(current_answers.s.front()) & it->n) != 0)
			{
				it = word_map[current_answers.s.back()].erase(it);
			}
#endif
		}
	}

	// current word is a dead-end, remove it from the current_answers
	current_answers.n ^= ston(current_answers.s.back());
	current_answers.s.pop_back();
}