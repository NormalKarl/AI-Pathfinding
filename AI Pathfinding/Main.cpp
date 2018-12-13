#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "NeuralNetwork.h"
#include "Map.h"

void drawGrid(sf::RenderTarget& target, Map& map, sf::FloatRect _region, std::vector<sf::Vector2i> _path);

/*float sigmoid(float x) {
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
};*/

/*std::vector<float> NetworkToChromosome(std::shared_ptr<Network>& _network) {
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
}*/

/*void run(std::shared_ptr<Network> network) {

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

	for (std::shared_ptr<Neuron>& neuron : network->m_network[network->m_network.size() - 1]) {
		std::cout << neuron->m_value << std::endl;
	}
}*/

#define MAX_ITERATIONS 1000

bool positive(float value) {
	return value > 0.5f;
}

int nextUp(int x, int y, Map& _map) {
	if (y > 0) {
		for (int cY = x; cY > 0; cY--) {
			if (_map[x][cY] == 1)
				return y - cY;
		}

		return y;
	}
	else {
		return 1;
	}
}

int nextDown(int x, int y, Map& _map) {
	if (y < _map.getHeight() - 1) {
		for (int cY = x; cY < _map.getHeight(); cY++) {
			if (_map[x][cY] == 1)
				return cY - y;
		}

		return _map.getHeight() - y;
	}
	else {
		return 1;
	}
}

int nextLeft(int x, int y, Map& _map) {
	if (x > 0) {
		for (int cX = x; cX > 0; cX--) {
			if (_map[cX][y] == 1)
				return x - cX;
		}

		return x;
	}
	else {
		return 1;
	}
}

int nextRight(int x, int y, Map& _map) {
	if (x < _map.getWidth() - 1) {
		for (int cX = x; cX < _map.getWidth(); cX++) {
			if (_map[cX][y] == 1)
				return cX - x;
		}

		return _map.getWidth() - x;
	}
	else {
		return 1;
	}
}

float length(sf::Vector2f _a) {
	return sqrtf(_a.x * _a.x + _a.y * _a.y);
}

float fitness(Map& _map, NeuralNetwork& _network, bool& foundPath, std::vector<sf::Vector2i>* _outputPath = nullptr)  {
	float fitness = 1.0f;
	foundPath = false;

	int iterations = 0;
	int maxIterations = _map.getWidth() * _map.getHeight() * 10;

	sf::Vector2i currentPos = _map.getStart();

	std::vector<sf::Vector2i> path;
	path.push_back(_map.getStart());

	while (!foundPath && iterations < maxIterations) {
		//Update the relative position in the neural network.
		float dx = static_cast<float>(currentPos.x - _map.getEnd().x);
		float dy = static_cast<float>(currentPos.y - _map.getEnd().y);
		float dup = nextUp(currentPos.x, currentPos.y, _map);
		float ddown = nextDown(currentPos.x, currentPos.y, _map);
		float dleft = nextLeft(currentPos.x, currentPos.y, _map);
		float dright = nextRight(currentPos.x, currentPos.y, _map);

		std::vector<float> results = _network.run({ dx, dy, dup, ddown, dleft, dright });

		bool up = positive(results[0]);
		bool down = positive(results[1]);
		bool left = positive(results[2]);
		bool right = positive(results[3]);

		if (up && currentPos.y > 0 && _map[currentPos.x][currentPos.y - 1] != 1) currentPos.y--;
		else if (down && currentPos.y < _map.getHeight() - 1 && _map[currentPos.x][currentPos.y + 1] != 1) currentPos.y++;
		if (left && currentPos.x > 0 && _map[currentPos.x - 1][currentPos.y] != 1) currentPos.x--;
		else if (right && currentPos.x < _map.getWidth() - 1 && _map[currentPos.x + 1][currentPos.y] != 1) currentPos.x++;


		sf::Vector2i lastLastPos = path.size() >= 3 ? path[path.size() - 3] : path.size() >= 2 ? path[path.size() - 2] : path[path.size() - 1];
		sf::Vector2i lastPos = path.size() >= 2 ? path[path.size() - 2] : path[path.size() - 1];

		bool containsNode = false;

		for (sf::Vector2i pathNode : path) {
			if (pathNode == currentPos) {
				containsNode = true;
			}
		}

		if (!containsNode && lastPos != currentPos) {
			if (currentPos == lastLastPos) {
				fitness -= 1.0f;
				path.pop_back();
			}
			else {
				fitness += 1.0f;
				path.push_back(currentPos);
			}
		}
		else if(lastPos == currentPos) {
			fitness -= 0.1f;
		}

		iterations++;

		if (currentPos == _map.getEnd()) {
			foundPath = true;
			break;
		}

	}

	if (foundPath && _outputPath != nullptr) {
		*_outputPath = path;
	}

	return fitness < 0.0f ? 0.0f : fitness;
}

float random(float min = 0.0f, float max = 1.0f) {
	return min + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * (max - min));
}

#define POPULATION 20
#define HALF_POPULATION 10

std::vector<sf::Vector2i> evolve(Map& _map, NeuralNetwork& _network) {
	std::vector<std::vector<float>> chromosomes;
	std::vector<float> fitnesses;

	//First fill the chromosomes with 4 parents.
	while (chromosomes.size() < POPULATION) {
		std::vector<float> rand;

		for (int i = 0; i < _network.getSignalCount(); i++)
			rand.push_back(random(-5.0f, 5.0f));

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
			_network.setSignals(chromosomes[f]);
			bool validPath = false;
			float fit = fitness(_map, _network, validPath);

			/*if (fit == 0.0f) {
				for (float& val : chromosomes[f]) {
					val = random(-1, 1);
				}

				f--;
			}
			else {*/
				fitnesses.push_back(fit);
			//}

			if (validPath) {
				foundPath = true;
				gen = maxGenerations;
				fittest = chromosomes[f];
			}
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
					if (random() < 0.015f) {
						childA[cell] = random(-5.0f, 5.0f);
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
					if (random() < 0.015f) {
						childB[cell] = random(-5.0f, 5.0f);
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

	
	if (foundPath) {
		std::vector<sf::Vector2i> path;
		_network.setSignals(fittest);
		fitness(_map, _network, foundPath, &path);
		return path;
	}

	return std::vector<sf::Vector2i>();
}

int main() {
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(640, 480), "AI Pathfinding");

	NeuralNetwork network({ 6, 6, 4 });
	Map map("File13.txt");
	std::vector<sf::Vector2i> path = map.getPath();

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
						map = Map(filename);
						path = map.getPath();
					}
				}
				else if (event.key.code == sf::Keyboard::F2) {

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

			if (map.getWidth() < map.getHeight()) {
				sqSizeX *= (float)map.getWidth() / (float)map.getHeight();
			}
			else {
				sqSizeY *= (float)map.getHeight() / (float)map.getWidth();
			}
		}
		else {
			sqSizeX = (float)size.y * 0.7f;
			sqSizeY = (float)size.y * 0.7f;

			if (map.getWidth() < map.getHeight()) {
				sqSizeX *= (float)map.getWidth() / (float)map.getHeight();
			}
			else {
				sqSizeY *= (float)map.getHeight() / (float)map.getWidth();
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

void drawGrid(sf::RenderTarget& target, Map& map, sf::FloatRect _region, std::vector<sf::Vector2i> _path) {
	float cellWidth = _region.width / (float)map.getWidth();
	float cellHeight = _region.height / (float)map.getHeight();

	for (int x = 0; x < map.getWidth(); x++) {
		for (int y = 0; y < map.getHeight(); y++) {
			sf::RectangleShape shape;
			shape.setSize(sf::Vector2f(cellWidth, cellHeight));
			shape.setPosition(sf::Vector2f(_region.left + (float)x * cellWidth, _region.top + (float)y * cellHeight));

			switch (map[x][y]) {
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

	for (int x = 0; x <= map.getWidth(); x++) {
		sf::Vertex v[] = {
			sf::Vertex({ _region.left + x * cellWidth, _region.top }),
			sf::Vertex({ _region.left + x * cellWidth, _region.top + _region.height })
		};

		target.draw(v, 2, sf::PrimitiveType::Lines);
	}

	for (int y = 0; y <= map.getHeight(); y++) {
		sf::Vertex v[] = {
			sf::Vertex({ _region.left, _region.top + y * cellHeight }),
			sf::Vertex({ _region.left + _region.width, _region.top + y * cellHeight })
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