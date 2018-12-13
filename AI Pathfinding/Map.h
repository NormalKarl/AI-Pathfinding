#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Map
{
private:
	int m_width = 0;
	int m_height = 0;
	int** m_mapData = nullptr;
	sf::Vector2i m_start;
	sf::Vector2i m_end;
public:
	Map(const char* _path);
	~Map();

	inline int getWidth() { return m_width; }
	inline int getHeight() { return m_height; }
	inline int** getMapData() { return m_mapData; }
	inline int* operator[](int index) { return m_mapData[index]; }
	inline sf::Vector2i getStart() { return m_start; }
	inline sf::Vector2i getEnd() { return m_end; }

	std::vector<sf::Vector2i> getPath();
};

