#ifndef _H_MAP
#define _H_MAP

#include <SFML/Graphics.hpp>
#include <vector>

//Map loads and stores a 2D grid with obstacles, start and end positions.
class Map
{
private:
	int m_width = 0;
	int m_height = 0;
	int** m_mapData = nullptr;
	sf::Vector2i m_start;
	sf::Vector2i m_end;
public:
	//Loads in a map from a file parsed into _path
	Map(const char* _path);
	~Map();

	//Returns the width of the map.
	inline int getWidth() { return m_width; }
	//Returns the height of the map.
	inline int getHeight() { return m_height; }
	//Returns the map data as a 2D array [x][y].
	inline int** getMapData() { return m_mapData; }
	//Returns the column of the parsed index in the map data.
	inline int* operator[](int index) { return m_mapData[index]; }
	//Returns the start position as a sf::Vector2i
	inline sf::Vector2i getStart() { return m_start; }
	//Returns the end position as a sf::Vector2i
	inline sf::Vector2i getEnd() { return m_end; }
	//Calculates a path using A*
	std::vector<sf::Vector2i> getPath();
};

#endif