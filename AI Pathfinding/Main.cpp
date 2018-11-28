#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

struct Map {
	int m_width = 0;
	int m_height = 0;
	int** m_mapData = nullptr;
	sf::Vector2i m_start;
	sf::Vector2i m_end;
};

bool loadMap(const char* _filePath, Map& _map) {
	std::ifstream file(_filePath);

	std::string sWidth;
	std::string sHeight;

	file >> sWidth;
	file >> sHeight;

	_map.m_width = atoi(sWidth.c_str());
	_map.m_height = atoi(sHeight.c_str());

	_map.m_mapData = new int*[_map.m_width];

	for (int i = 0; i < _map.m_width; i++) {
		_map.m_mapData[i] = new int[_map.m_height];
		std::fill_n(_map.m_mapData[i], _map.m_height, 0);
	}

	//Read in column then row because this is how the
	//file is setup.
	for (int y = 0; y < _map.m_height; y++) {
		for (int x = 0; x < _map.m_width; x++) {
			std::string data;
			file >> data;
			int id = atoi(data.c_str());
			_map.m_mapData[x][y] = id;

			if (id == 2) {
				_map.m_start = sf::Vector2i(x, y);
			}
			else if (id == 3) {
				_map.m_end = sf::Vector2i(x, y);
			}
		}
	}

	return 0;
}

void drawGrid(sf::RenderTarget& target, Map& map, sf::FloatRect _region, std::vector<sf::Vector2i> _path) {
	float cellWidth = _region.width / (float)map.m_width;
	float cellHeight = _region.height / (float)map.m_height;

	for (int x = 0; x < map.m_width; x++) {
		for (int y = 0; y < map.m_height; y++) {
			sf::RectangleShape shape;
			shape.setSize(sf::Vector2f(cellWidth, cellHeight));
			shape.setPosition(sf::Vector2f(_region.left + (float)x * cellWidth, _region.top + (float)y * cellHeight));

			switch (map.m_mapData[x][y]) {
				case 0:
					shape.setFillColor(sf::Color(150, 150, 150, 255));
					break;
				case 1:
					shape.setFillColor(sf::Color::Black);
					break;
				case 2:
					shape.setFillColor(sf::Color::Red);
					break;
				case 3:
					shape.setFillColor(sf::Color::Green);
					break;
			}

			target.draw(shape);
		}
	}

	for (int x = 0; x <= map.m_width; x++) {
		sf::Vertex v[] = {
			sf::Vertex({_region.left + x * cellWidth, _region.top}),
			sf::Vertex({_region.left + x * cellWidth, _region.top + _region.height})
		};

		target.draw(v, 2, sf::PrimitiveType::Lines);
	}

	for (int y = 0; y <= map.m_height; y++) {
		sf::Vertex v[] = {
			sf::Vertex({_region.left, _region.top + y * cellHeight}),
			sf::Vertex({_region.left + _region.width, _region.top + y * cellHeight})
		};

		target.draw(v, 2, sf::PrimitiveType::Lines);
	}

	for (sf::Vector2i node : _path) {
		sf::RectangleShape shape;
		shape.setSize(sf::Vector2f(cellWidth, cellHeight));
		shape.setPosition(sf::Vector2f(_region.left + (float)node.x * cellWidth, _region.top + (float)node.y * cellHeight));
		shape.setFillColor(sf::Color(0, 0, 255, 100));
		target.draw(shape);
	}
}

struct Node {
	std::shared_ptr<Node> m_parent = nullptr;
	float m_cost = 0.0;
	float m_huristic = 0.0;
	float m_total = 0.0;
	sf::Vector2i m_position;
};

bool aStar(Map& _map, std::vector<sf::Vector2i>& _output) {
	std::vector<std::shared_ptr<Node>> openList;
	std::vector<std::shared_ptr<Node>> closedList;

	float initCost = powf((float) abs(_map.m_end.x - _map.m_start.x), 2) + (float)powf(abs(_map.m_end.y - _map.m_start.y), 2);

	openList.push_back(std::shared_ptr<Node>(new Node { nullptr, 0.0, 0.0, 0.0, _map.m_start }));

	std::shared_ptr<Node> currentNode = openList[0];

	int currentNodeIndex = 0;

	while (!openList.empty()) {
		currentNode = openList[0];

		//Find the node with the smallest total cost.
		for (int i = 0; i < openList.size(); i++)
		{
			std::shared_ptr<Node> node = openList[i];

			if (node->m_total < currentNode->m_total) {
currentNodeIndex = i;
currentNode = node;
			}
		}

		openList.erase(openList.begin() + currentNodeIndex, openList.begin() + currentNodeIndex + 1);
		closedList.push_back(currentNode);

		if (currentNode->m_position == _map.m_end) {
			//Found the path.
			printf("Found End.");
			break;
		}

		sf::Vector2i pos = currentNode->m_position;

		for (int y = pos.y - 1; y <= pos.y + 1; y++) {
			for (int x = pos.x - 1; x <= pos.x + 1; x++) {
				sf::Vector2i childPos(x, y);

				if (childPos == currentNode->m_position)
					continue;

				if (x < 0 || x >= _map.m_width || y < 0 || y >= _map.m_height || _map.m_mapData[x][y] == 1)
					continue;

				bool alreadyClosed = false;

				for (std::shared_ptr<Node> closedNode : closedList) {
					if (closedNode->m_position == childPos)
						alreadyClosed = true;
				}

				if (alreadyClosed)
					continue;

				bool addNode = true;
				std::shared_ptr<Node> childNode = std::shared_ptr<Node>(new Node());

				childNode->m_parent = currentNode;
				//childNode->m_cost = sqrt(powf(fabs(x - currentNode->m_position.x), 2) + powf(fabs(y - currentNode->m_position.y), 2));
				childNode->m_cost = currentNode->m_cost + 1;
				childNode->m_huristic = powf(abs(_map.m_end.x - x), 2) + powf(abs(_map.m_end.y - y), 2);
				childNode->m_total = childNode->m_cost + childNode->m_huristic;
				childNode->m_position = childPos;

				for (int i = 0; i < openList.size(); i++) {
					std::shared_ptr<Node> openNode = openList[i];

					if (openNode->m_position == childNode->m_position) {
						if (childNode->m_cost > openNode->m_cost) {
							addNode = false;
						}
					}
				}

				if (addNode)
					openList.push_back(childNode);
			}
		}
	}

	if (openList.empty()) {
		return false;
	}
	else {
		bool allocatingPath = true;

		while (allocatingPath) {
			if (currentNode->m_position == _map.m_start)
				allocatingPath = false;

			_output.push_back(currentNode->m_position);
			currentNode = currentNode->m_parent;
		}

		return true;
	}
}

int main() {
	sf::RenderWindow window(sf::VideoMode(640, 480), "AI Pathfinding");


	Map map;
	std::vector<sf::Vector2i> path;

	loadMap("File3.txt", map);
	aStar(map, path);

	while (window.isOpen()) {

		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::F1) {
					char filename[MAX_PATH];
					OPENFILENAMEA ofn;
					ZeroMemory(&filename, sizeof(filename));
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = window.getSystemHandle();  // If you have a window to center over, put its HANDLE here
					ofn.lpstrFilter = "Map File (*.txt)\0*.txt\0Any File\0*.*\0";
					ofn.lpstrFile = filename;
					ofn.nMaxFile = MAX_PATH;
					ofn.lpstrTitle = "Select a Map File!";

					ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

					if (GetOpenFileNameA(&ofn) != false) {
						loadMap(filename, map);
						path.clear();
						aStar(map, path);
					}
				}
			}
		}

		window.clear();




		sf::Vector2u size = window.getSize();
		window.setView(sf::View(sf::Vector2f(size.x / 2, size.y / 2), sf::Vector2f(size.x, size.y)));

		sf::FloatRect region;

		float sqSizeX = 0.0f;
		float sqSizeY = 0.0f;

		if (size.x < size.y) {
			sqSizeX = (float)size.x * 0.7f;
			sqSizeY = (float)size.x * 0.7f;

			if (map.m_width < map.m_height) {
				sqSizeX *= (float)map.m_width / (float)map.m_height;
			}
			else {
				sqSizeY *= (float)map.m_height / (float)map.m_width;
			}
		}
		else {
			sqSizeX = (float)size.y * 0.7f;
			sqSizeY = (float)size.y * 0.7f;

			if (map.m_width < map.m_height) {
				sqSizeX *= (float)map.m_width / (float)map.m_height;
			}
			else {
				sqSizeY *= (float)map.m_height / (float)map.m_width;
			}
		}

		region.width = sqSizeX;
		region.height = sqSizeY;
		region.left = (size.x - sqSizeX) / 2;
		region.top = (size.y - sqSizeY) / 2;

		drawGrid(window, map, region, path);

		window.display();
	}
}