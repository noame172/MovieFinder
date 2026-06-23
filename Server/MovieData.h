#pragma once
#include <string>
#include <vector>

class MovieData
{
public:
	MovieData() = default;
	MovieData(const std::string& title, float runTimeInMinutes, const std::vector<std::string>& genres, const std::string& overview)
		: m_title(title), m_runTimeInMinutes(runTimeInMinutes), m_genres(genres), m_overview(overview){}
	const std::string& getTitle() const { return m_title; }
	float getRunTimeInMinutes() const { return m_runTimeInMinutes; }
	const std::vector<std::string>& getGenres() const { return m_genres; }
	const std::string& getOverview() const { return m_overview; }
private:
	std::string m_title;
	float m_runTimeInMinutes;
	std::vector<std::string> m_genres;
	std::string m_overview;

};