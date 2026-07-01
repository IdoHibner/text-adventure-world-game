#pragma once
#include "GameObject.h"
#include <string>

class Riddle : public GameObject
{
	std::string question;
	std::string answer;
	bool is_solved;

  public:
	Riddle(int x, int y, std::string q, std::string a, char c = '?')
	    : GameObject(x, y, c), question(q), answer(a), is_solved(false)
	{
	}

	Riddle() : GameObject(0, 0, '?'), question(""), answer(""), is_solved(false)
	{
	}

	void setQuestion(const std::string& q)
	{
		question = q;
	}

	void setAnswer(const std::string& a)
	{
		answer = a;
	}

	std::string getQuestion() const
	{
		return question;
	}

	bool isSolved() const
	{
		return is_solved;
	}

	void setSolved(bool solved)
	{
		is_solved = solved;
	}

	bool checkAnswer(const std::string& player_answer);
	void save(std::ostream& out) const override;
	void load(std::istream& in) override;
};
