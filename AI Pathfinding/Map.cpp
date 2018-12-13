#include "Map.h"

#include <fstream>
#include <memory>

Map::Map(const char* _path)
{
	std::ifstream file(_path);

	if (!file.is_open()) {
		throw std::exception("Cannot open file.");
	}

	std::string sWidth;
	std::string sHeight;

	file >> sWidth;
	file >> sHeight;

	m_width = atoi(sWidth.c_str());
	m_height = atoi(sHeight.c_str());

	m_mapData = new int*[m_width];

	for (int i = 0; i < m_width; i++) {
		m_mapData[i] = new int[m_height];
		std::fill_n(m_mapData[i], m_height, 0);
	}

	//Read in column then row because this is how the
	//file is setup.
	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			std::string data;
			file >> data;
			int id = atoi(data.c_str());
			m_mapData[x][y] = id;

			if (id == 2) {
				m_start = sf::Vector2i(x, y);
			}
			else if (id == 3) {
				m_end = sf::Vector2i(x, y);
			}
		}
	}
}

Map::~Map()
{
	for (int i = 0; i < m_width; i++)
		delete[] m_mapData[i];

	delete[] m_mapData;
}

std::vector<sf::Vector2i> Map::getPath() {
	struct GridNode {
		std::shared_ptr<GridNode> m_parent = nullptr;
		float m_cost = 0.0;
		float m_huristic = 0.0;
		float m_total = 0.0;
		sf::Vector2i m_position;
	};

	std::vector<std::shared_ptr<GridNode>> openList;
	std::vector<std::shared_ptr<GridNode>> closedList;

	float initCost = powf((float)abs(m_end.x - m_start.x), 2) + (float)powf(abs(m_end.y - m_start.y), 2);

	openList.push_back(std::shared_ptr<GridNode>(new GridNode{ nullptr, 0.0, 0.0, 0.0, m_start }));

	std::shared_ptr<GridNode> currentNode = openList[0];

	int currentNodeIndex = 0;

	while (!openList.empty()) {
		currentNode = openList[0];
		currentNodeIndex = 0;

		//Find the node with the smallest total cost.
		for (int i = 0; i < openList.size(); i++)
		{
			std::shared_ptr<GridNode> node = openList[i];

			if (node->m_total < currentNode->m_total) {
				currentNodeIndex = i;
				currentNode = node;
			}
		}

		openList.erase(openList.begin() + currentNodeIndex, openList.begin() + currentNodeIndex + 1);
		closedList.push_back(currentNode);

		if (currentNode->m_position == m_end) {
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

				if (x < 0 || x >= m_width || y < 0 || y >= m_height || m_mapData[x][y] == 1)
					continue;

				bool alreadyClosed = false;

				/*for (std::shared_ptr<Node> openNode : openList) {
					if (openNode->m_position == childPos)
						alreadyClosed = true;
				}*/

				for (std::shared_ptr<GridNode> closedNode : closedList) {
					if (closedNode->m_position == childPos)
						alreadyClosed = true;
				}

				if (alreadyClosed)
					continue;

				bool addNode = true;
				std::shared_ptr<GridNode> childNode = std::shared_ptr<GridNode>(new GridNode());

				childNode->m_parent = currentNode;
				//childNode->m_cost = sqrt(powf(fabs(x - currentNode->m_position.x), 2) + powf(fabs(y - currentNode->m_position.y), 2));
				childNode->m_cost = currentNode->m_cost + 1;
				childNode->m_huristic = powf(abs(m_end.x - x), 2) + powf(abs(m_end.y - y), 2);
				childNode->m_total = childNode->m_cost + childNode->m_huristic;
				childNode->m_position = childPos;

				//Change it so it will update the node and not add it.
				for (int i = 0; i < openList.size(); i++) {
					std::shared_ptr<GridNode> openNode = openList[i];

					if (openNode->m_position == childPos) {
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

	if (!openList.empty()) {
		std::vector<sf::Vector2i> output;
		bool allocatingPath = true;

		while (allocatingPath) {
			if (currentNode->m_position == m_start)
				allocatingPath = false;


			output.push_back(currentNode->m_position);
			currentNode = currentNode->m_parent;
		}

		return output;
	}

	return std::vector<sf::Vector2i>();
}