#include "ScoreRecord.hpp"


using namespace std;

ScoreRecord::ScoreRecord(std::string nameStr, int scoreVal, float timeVal) : name(nameStr), score(scoreVal), time(timeVal)
{
}

void ScoreRecord::write(std::ostream & strm) const
{
	strm << name << " ";
	strm << score << " ";
	strm << time << endl;
}

void ScoreRecord::read(std::istream & strm)
{
	strm >> name >> score >> time;
}
