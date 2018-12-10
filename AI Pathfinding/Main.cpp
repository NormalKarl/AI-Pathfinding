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

struct GridNode {
	std::shared_ptr<GridNode> m_parent = nullptr;
	float m_cost = 0.0;
	float m_huristic = 0.0;
	float m_total = 0.0;
	sf::Vector2i m_position;
};

bool aStar(Map& _map, std::vector<sf::Vector2i>& _output) {
	std::vector<std::shared_ptr<GridNode>> openList;
	std::vector<std::shared_ptr<GridNode>> closedList;

	float initCost = powf((float) abs(_map.m_end.x - _map.m_start.x), 2) + (float)powf(abs(_map.m_end.y - _map.m_start.y), 2);

	openList.push_back(std::shared_ptr<GridNode>(new GridNode{ nullptr, 0.0, 0.0, 0.0, _map.m_start }));

	std::shared_ptr<GridNode> currentNode = openList[0];

	int currentNodeIndex = 0;

	while (!openList.empty()) {
		currentNode = openList[0];

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
				childNode->m_huristic = powf(abs(_map.m_end.x - x), 2) + powf(abs(_map.m_end.y - y), 2);
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

float sigmoid(float x) {
	return x / (1 + abs(x));
}
struct Neuron;

struct Signal {
	std::shared_ptr<Neuron> emitter;
	std::shared_ptr<Neuron> reciever;
	float weight = 0.0f;
};

struct Neuron {
	std::vector<std::shared_ptr<Signal>> m_inSignals;
	//std::vector<std::shared_ptr<Signal>> m_outSignals;
	float m_value;
};

struct Network {
	//This is a vector (neural network) of vectors (layers) of shared neurons (Ordered neuron)
	std::vector<std::vector<std::shared_ptr<Neuron>>> m_network;
	std::vector<std::shared_ptr<Signal>> m_signals;
};

std::vector<float> NetworkToChromosome(std::shared_ptr<Network>& _network) {
	std::vector<float> chromosome;

	for(std::shared_ptr<Signal>& signal : _network->m_signals)
		chromosome.push_back(signal->weight);

	return chromosome;
}

void ChromosomeToNetwork(std::shared_ptr<Network>& _network, std::vector<float> _chromosome) {
	for (int i = 0; i < _chromosome.size(); i++) {
		_network->m_signals[i]->weight = _chromosome[i];
	}
}

std::shared_ptr<Network> createANN() {
	std::shared_ptr<Network> network = std::shared_ptr<Network>(new Network());

	auto pair = [&network](std::shared_ptr<Neuron> _neuronA, std::shared_ptr<Neuron> _neuronB) {
		std::shared_ptr<Signal> signal = std::shared_ptr<Signal>(new Signal());

		signal->emitter = _neuronA;
		signal->reciever = _neuronB;
		network->m_signals.push_back(signal);
		_neuronB->m_inSignals.push_back(signal);

		return false;
	};

	std::vector<std::shared_ptr<Neuron>> inputNeurons;
	inputNeurons.push_back(std::shared_ptr<Neuron>(new Neuron()));
	inputNeurons.push_back(std::shared_ptr<Neuron>(new Neuron()));
	inputNeurons.push_back(std::shared_ptr<Neuron>(new Neuron()));
	inputNeurons.push_back(std::shared_ptr<Neuron>(new Neuron()));
	inputNeurons.push_back(std::shared_ptr<Neuron>(new Neuron()));
	inputNeurons.push_back(std::shared_ptr<Neuron>(new Neuron()));

	std::vector<std::shared_ptr<Neuron>> hiddenLayer;
	hiddenLayer.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer.push_back(std::shared_ptr<Neuron>(new Neuron()));

	std::vector<std::shared_ptr<Neuron>> hiddenLayer2;
	hiddenLayer2.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer2.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer2.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer2.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer2.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer2.push_back(std::shared_ptr<Neuron>(new Neuron()));

	std::vector<std::shared_ptr<Neuron>> hiddenLayer3;
	hiddenLayer3.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer3.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer3.push_back(std::shared_ptr<Neuron>(new Neuron()));
	hiddenLayer3.push_back(std::shared_ptr<Neuron>(new Neuron()));

	std::vector<std::shared_ptr<Neuron>> outputNeurons;
	outputNeurons.push_back(std::shared_ptr<Neuron>(new Neuron()));
	outputNeurons.push_back(std::shared_ptr<Neuron>(new Neuron()));
	outputNeurons.push_back(std::shared_ptr<Neuron>(new Neuron()));
	outputNeurons.push_back(std::shared_ptr<Neuron>(new Neuron()));

	network->m_network.push_back(inputNeurons);
	network->m_network.push_back(hiddenLayer);
	//network->m_network.push_back(hiddenLayer2);
	//network->m_network.push_back(hiddenLayer3);
	network->m_network.push_back(outputNeurons);

	//Pair all the nodes with each other.
	for (int layerIndex = 0; layerIndex < network->m_network.size() - 1; layerIndex++) {
		std::vector<std::shared_ptr<Neuron>>& layer = network->m_network[layerIndex];
		std::vector<std::shared_ptr<Neuron>>& nextLayer = network->m_network[layerIndex + 1];

		for (int neuronIndex = 0; neuronIndex < layer.size(); neuronIndex++)
		{
			std::shared_ptr<Neuron> neuron = layer[neuronIndex];

			for (int nextNeuronIndex = 0; nextNeuronIndex < nextLayer.size(); nextNeuronIndex++)
			{
				std::shared_ptr<Neuron> nextNeuron = nextLayer[nextNeuronIndex];

				pair(neuron, nextNeuron);

				std::cout << "Paired Node " << layerIndex << ", " << neuronIndex << " with " << (layerIndex + 1) << ", " << nextNeuronIndex << std::endl;
			}
		}
	}

	return network;
}

void run(std::shared_ptr<Network> network) {

	for (int index = 1; index < network->m_network.size(); index++) {
		for (std::shared_ptr<Neuron>& neuron : network->m_network[index]) {
			float currentValue = 0.0f;

			for (std::shared_ptr<Signal>& signal : neuron->m_inSignals) {
				currentValue += signal->emitter->m_value * signal->weight;
			}

			currentValue = sigmoid(currentValue);
			neuron->m_value = currentValue;
		}
	}

	/*for (std::shared_ptr<Neuron>& neuron : network->m_network[network->m_network.size() - 1]) {
		std::cout << neuron->m_value << std::endl;
	}*/
}

#define MAX_ITERATIONS 1000

bool positive(float value) {
	return value > 0.0f;
}

int nextUp(int x, int y, Map& _map) {
	if (y > 0) {
		for (int cY = x; cY > 0; cY--) {
			if (_map.m_mapData[x][cY] == 1)
				return y - cY;
		}

		return y;
	}
	else {
		return 1;
	}
}

int nextDown(int x, int y, Map& _map) {
	if (y < _map.m_height - 1) {
		for (int cY = x; cY < _map.m_height; cY++) {
			if (_map.m_mapData[x][cY] == 1)
				return cY - y;
		}

		return _map.m_height - y;
	}
	else {
		return 1;
	}
}

int nextLeft(int x, int y, Map& _map) {
	if (x > 0) {
		for (int cX = x; cX > 0; cX--) {
			if (_map.m_mapData[cX][y] == 1)
				return x - cX;
		}

		return x;
	}
	else {
		return 1;
	}
}

int nextRight(int x, int y, Map& _map) {
	if (x < _map.m_width - 1) {
		for (int cX = x; cX < _map.m_width; cX++) {
			if (_map.m_mapData[cX][y] == 1)
				return cX - x;
		}

		return _map.m_width - x;
	}
	else {
		return 1;
	}
}

float length(sf::Vector2f _a) {
	return sqrtf(_a.x * _a.x + _a.y * _a.y);
}

float fitness(Map& _map, std::shared_ptr<Network> _network, bool& foundPath)  {
	float fitness = 0.0f;
	foundPath = false;


	float maxFit = (float)(_map.m_width * _map.m_height) * 10.0f;
	int iterations = 0;
	int maxIterations = _map.m_width * _map.m_height * 10;

	sf::Vector2i currentPos = _map.m_start;

	std::vector<sf::Vector2i> path;
	path.push_back(_map.m_start);

	while (!foundPath && iterations < maxIterations) {
		//Update the relative position in the neural network.
		_network->m_network[0][0]->m_value = static_cast<float>(currentPos.x - _map.m_end.x);
		_network->m_network[0][1]->m_value = static_cast<float>(currentPos.y - _map.m_end.y);

		_network->m_network[0][2]->m_value = nextUp(currentPos.x, currentPos.y, _map);
		_network->m_network[0][3]->m_value = nextDown(currentPos.x, currentPos.y, _map);
		_network->m_network[0][4]->m_value = nextLeft(currentPos.x, currentPos.y, _map);
		_network->m_network[0][5]->m_value = nextRight(currentPos.x, currentPos.y, _map);

		run(_network);

		bool up = positive(_network->m_network[_network->m_network.size() - 1][0]->m_value);
		bool down = positive(_network->m_network[_network->m_network.size() - 1][1]->m_value);
		bool left = positive(_network->m_network[_network->m_network.size() - 1][2]->m_value);
		bool right = positive(_network->m_network[_network->m_network.size() - 1][3]->m_value);

		if (up && currentPos.y > 0 && _map.m_mapData[currentPos.x][currentPos.y - 1] != 1) currentPos.y--;
		if (down && currentPos.y < _map.m_height - 1 && _map.m_mapData[currentPos.x][currentPos.y + 1] != 1) currentPos.y++;
		if (left && currentPos.x > 0 && _map.m_mapData[currentPos.x - 1][currentPos.y] != 1) currentPos.x--;
		if (right && currentPos.x < _map.m_width - 1 && _map.m_mapData[currentPos.x + 1][currentPos.y] != 1) currentPos.x++;

		path.push_back(currentPos);

		sf::Vector2i lastLastPos = path.size() >= 3 ? path[path.size() - 3] : path.size() >= 2 ? path[path.size() - 2] : path[path.size() - 1];
		sf::Vector2i lastPos = path.size() >= 2 ? path[path.size() - 2] : path[path.size() - 1];

		if (lastPos != currentPos) {
			fitness++;
		}

		if (lastPos != currentPos && currentPos == lastLastPos) {
			fitness--;
		}

		iterations++;

		if (currentPos == _map.m_end) {
			foundPath = true;
			break;
		}
	}

	return fitness;
}

float random(float min = 0.0f, float max = 1.0f) {
	return min + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * (max - min));
}

#define POPULATION 50
#define HALF_POPULATION 25

std::vector<sf::Vector2i> evolve(Map& _map, std::shared_ptr<Network>& _network) {
	std::vector<std::vector<float>> chromosomes;
	std::vector<float> fitnesses;
	float maxFit = (float)(_map.m_width * _map.m_height) * 10.0f;

	//First fill the chromosomes with 4 parents.
	while (chromosomes.size() < POPULATION) {
		std::vector<float> rand;

		for (int i = 0; i < _network->m_signals.size(); i++)
			rand.push_back(random(-1, 1));

		chromosomes.push_back(rand);
	}

	std::vector<float> fittest;

	int generations = 0;
	int maxGenerations = 100000;
	bool foundPath = false;

	for(int gen = 0; gen < maxGenerations; gen++) {
		generations++;
		std::vector<std::vector<float>> children;
		fitnesses.clear();

		for (int f = 0; f < POPULATION; f++) {
			ChromosomeToNetwork(_network, chromosomes[f]);

			float fit = fitness(_map, _network, foundPath);
			fitnesses.push_back(fit);

			if (foundPath) {
				gen = maxGenerations;
				fittest = chromosomes[f];
			}

			//if (fit == 0) {

			//}
			/*else {
				for (int cell = 0; cell < chromosomes[f].size(); cell++) {
					if (random() < 0.2f) {
						chromosomes[f][cell] = random(-1.0, 1.0);
					}
				}
			}*/
		}
		
		if (foundPath || gen == maxGenerations) {
			break;
		}

		float maxVal = 0.0f;

		for (float f : fitnesses)
			maxVal += f;

		for (int j = 0; j < HALF_POPULATION; j++) {
			float r1 = random(0.0f, maxVal);
			float currentCheck = 0.0f;
			int index1 = 0;

			for (int f = 0; f < fitnesses.size(); f++) {
				currentCheck += fitnesses[f];

				if (r1 < currentCheck) {
					index1 = f;
					break;
				}
			}

			int index2 = index1;

			while (index2 == index1) {
				r1 = random(0.0f, maxVal);
				currentCheck = 0.0f;

				for (int f = 0; f < fitnesses.size(); f++) {
					currentCheck += fitnesses[f];

					if (r1 < currentCheck) {
						index2 = f;
						break;
					}
				}
			}

			std::vector<float> chromosomeA = chromosomes[index1];
			std::vector<float> chromosomeB = chromosomes[index2];

			//int crossOver = (int)floor(chromosomeA.size() * random());
			int crossOver = chromosomeA.size() / 2 - 1;

			std::vector<float> childA;
			std::vector<float> childB;

			if (random() < 0.7) {
				for (int cell = 0; cell < chromosomeA.size(); cell++) {
					childA.push_back(cell <= crossOver ? chromosomeB[cell] : chromosomeA[cell]);
				}

				//Random Mutation
				for (int cell = 0; cell < childA.size(); cell++) {
					if (random() < 0.05f) {
						childA[cell] = random(-1.0f, 1.0f);
					}
				}
			}
			else {
				childA = chromosomeA;
			}

			if (random() < 0.7) {
				for (int cell = 0; cell < chromosomeA.size(); cell++) {
					childB.push_back(cell <= crossOver ? chromosomeA[cell] : chromosomeB[cell]);
				}

				//Random Mutation
				for (int cell = 0; cell < childA.size(); cell++) {
					if (random() < 0.05f) {
						childB[cell] = random(-1.0f, 1.0f);
					}
				}
			}
			else {
				childB = chromosomeB;
			}

			children.push_back(childA);
			children.push_back(childB);
		}

		chromosomes = children;
	}

	
	std::vector<sf::Vector2i> path;

	if (foundPath) {
		ChromosomeToNetwork(_network, fittest);
		bool searching = true;
		sf::Vector2i currentPos = _map.m_start;

		while (searching) {
			_network->m_network[0][0]->m_value = static_cast<float>(currentPos.x - _map.m_end.x);
			_network->m_network[0][1]->m_value = static_cast<float>(currentPos.y - _map.m_end.y);

			_network->m_network[0][2]->m_value = nextUp(currentPos.x, currentPos.y, _map);
			_network->m_network[0][3]->m_value = nextDown(currentPos.x, currentPos.y, _map);
			_network->m_network[0][4]->m_value = nextLeft(currentPos.x, currentPos.y, _map);
			_network->m_network[0][5]->m_value = nextRight(currentPos.x, currentPos.y, _map);

			path.push_back(currentPos);

			run(_network);

			bool up = positive(_network->m_network[_network->m_network.size() - 1][0]->m_value);
			bool down = positive(_network->m_network[_network->m_network.size() - 1][1]->m_value);
			bool left = positive(_network->m_network[_network->m_network.size() - 1][2]->m_value);
			bool right = positive(_network->m_network[_network->m_network.size() - 1][3]->m_value);

			if (up && currentPos.y > 0 && _map.m_mapData[currentPos.x][currentPos.y - 1] != 1) currentPos.y--;
			if (down && currentPos.y < _map.m_height - 1 && _map.m_mapData[currentPos.x][currentPos.y + 1] != 1) currentPos.y++;
			if (left && currentPos.x > 0 && _map.m_mapData[currentPos.x - 1][currentPos.y] != 1) currentPos.x--;
			if (right && currentPos.x < _map.m_width - 1 && _map.m_mapData[currentPos.x + 1][currentPos.y] != 1) currentPos.x++;

			if (currentPos == _map.m_end)
				searching = false;
		}


		path.push_back(currentPos);
	}

	return path;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(640, 480), "AI Pathfinding");

	srand(time(NULL));

	Map map;
	std::vector<sf::Vector2i> path;

	loadMap("File8.txt", map);
	//aStar(map, path);

	std::shared_ptr<Network> network = createANN();
	path = evolve(map, network);


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