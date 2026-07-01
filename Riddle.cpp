#include "Riddle.h"
#include <algorithm>
#include <cctype>

bool Riddle::checkAnswer(const std::string& player_answer)
{
	std::string ans_lower = answer;
	std::string player_lower = player_answer;

	std::transform(ans_lower.begin(), ans_lower.end(), ans_lower.begin(), ::tolower);
	std::transform(player_lower.begin(), player_lower.end(), player_lower.begin(), ::tolower);

	if (player_lower == ans_lower)
	{
		is_solved = true;
	}

	return is_solved;
}

void Riddle::save(std::ostream& out) const
{
	GameObject::save(out);
	out << is_solved << " ";
	out << question.size() << " " << question << " ";
	out << answer.size() << " " << answer << " ";
}

void Riddle::load(std::istream& in)
{
	GameObject::load(in);
	in >> is_solved;
	size_t size;

	in >> size;
	char temp;

	in.get(temp);
	question.resize(size);
	in.read(&question[0], size);

	in >> size;
	in.get(temp);
	answer.resize(size);
	in.read(&answer[0], size);
}
