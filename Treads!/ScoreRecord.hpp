#pragma once
#include <iostream>
#include <string>
#include <vector>

class ScoreRecord
{
public:
	ScoreRecord(std::string nameStr = "-",  int scoreVal = 0, float timeVal = 0.f);
	void write(std::ostream& strm) const;
	void read(std::istream& strm);

	std::string getName() const { return name; }
	int getScore() const { return score; }
	float getTime() const { return time; }

private:
	std::string name;
	int score;
	float time;
};