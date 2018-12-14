#include "NeuralNetwork.h"
#include "Map.h"

#define POPULATION 50
#define HALF_POPULATION 25

float random(float min = 0.0f, float max = 1.0f) {
	return min + ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * (max - min));
}

NeuralNetwork::NeuralNetwork(std::vector<int> _layerSizes) {
	//Cannot create a network with only 1 layer
	if (_layerSizes.size() <= 1) {
		throw std::exception("Need at least 2 or more layer sizes to make a NeuralNetwork.");
	}

	int signalCount = 0;

	for (int i = 0; i < _layerSizes.size(); i++) {
		int layerSize = _layerSizes[i];

		//Check to make sure that its not the last layer,
		//because no signals are created on the output nodes.
		if (i < _layerSizes.size() - 1) {
			//Multiply the count of nodes on the current layer
			//against the count of nodes on the next layer to get
			//the total signals between the layers.
			signalCount += layerSize * _layerSizes[i + 1];
		}

		//Push back a new vector of floats representing the layer neurons.
		m_nodes.push_back(std::vector<float>(layerSize));
	}

	int totalNodes = 0;

	for (int& i : _layerSizes)
		totalNodes += i;

	//Init the biases.
	m_biases = std::vector<float>(totalNodes);
	//Randomize the biases.
	shuffleBiases();
	//Initalise the signals.
	m_signals = std::vector<float>(signalCount);

}

std::vector<float> NeuralNetwork::run(std::vector<float> _inputNodes) {
	//Signal offset is used for offsetting the signals from layer to layer.
	int signalOffset = 0;

	//Set the input node data.
	m_nodes[0] = _inputNodes;

	int biasOffset = 0;

	//Start at 1 because the input layer does not use the activation function.
	for (int i = 1; i < m_nodes.size(); i++) {
		std::vector<float>& prevLayer = m_nodes[i - 1];
		std::vector<float>& layer = m_nodes[i];

		for (float& val : layer)
			val = 0.0f;

		//Add all the weighted values.
		//Iterate over all the current layer and then add the weighted value to every
		//connecting node.
		for (int prevNodeIndex = 0; prevNodeIndex < prevLayer.size(); prevNodeIndex++) {
			for (int nodeIndex = 0; nodeIndex < layer.size(); nodeIndex++) {
				//The current node index is the current node multiplied by the next layers
				//node count with the next layer's currently operating node.
				int signalIndex = (prevNodeIndex * layer.size()) + nodeIndex;

				//Multiply the previous layers output value with the current signal.
				layer[nodeIndex] += prevLayer[prevNodeIndex] * m_signals[signalIndex + signalOffset];
			}
		}

		//Activate the values.
		for (int nodeIndex = 0; nodeIndex < layer.size(); nodeIndex++) {
			float& node = layer[nodeIndex];
			//Sigmoid function between 
			node = 1.0f / (1.0f + exp(-node + m_biases[biasOffset + nodeIndex]));
			//node = node / (1.0f + abs(node));
			//node = sin(node);
		}

		biasOffset += layer.size();

		//Add the signal offset.
		//This would be much better if I used a matrix here as I can quickly multiply all
		//the weights with the outputs in one go, however because of the 
		signalOffset += prevLayer.size() * layer.size();
	}

	return m_nodes[m_nodes.size() - 1];
}

std::vector<sf::Vector2i> NeuralNetwork::getPath(Map& _map, bool* _foundPath, float* _fitness) {
	//Lambda function.
	auto positive = [](float value) {
		return value > 0.5f;
	};

	float fitness = 1.0f;
	bool foundPath = false;

	//Calculate the max iterations based on the map size.
	int iterations = 0;
	int maxIterations = _map.getWidth() * _map.getHeight() * 2;

	sf::Vector2i currentPos = _map.getStart();

	std::vector<sf::Vector2i> path;
	path.push_back(_map.getStart());

	//Iterate until path is found or iterations hits max iterations.
	while (!foundPath && iterations < maxIterations) {
		//Update the relative position in the neural network.
		float dx = static_cast<float>(currentPos.x - _map.getEnd().x);
		float dy = static_cast<float>(currentPos.y - _map.getEnd().y);
		float dup = static_cast<float>(_map.stepCast(currentPos.x, currentPos.y, 0, -1));
		float ddown = static_cast<float>(_map.stepCast(currentPos.x, currentPos.y, 0, 1));
		float dleft = static_cast<float>(_map.stepCast(currentPos.x, currentPos.y, -1, 0));
		float dright = static_cast<float>(_map.stepCast(currentPos.x, currentPos.y, 1, 0));

		//Parse the results into the neural network.
		std::vector<float> results = run({ dx, dy, dup, ddown, dleft, dright });

		bool up = positive(results[0]);
		bool down = positive(results[1]);
		bool left = positive(results[2]);
		bool right = positive(results[3]);

		//
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
		else if (lastPos == currentPos) {
			fitness -= 0.1f;
		}

		iterations++;

		if (currentPos == _map.getEnd())
			foundPath = true;
	}

	if (foundPath && _foundPath != nullptr)
		*_foundPath = foundPath;

	if (_fitness != nullptr)
		*_fitness = fitness < 0.0f ? 0.0f : fitness;

	if (!foundPath)
		path.clear();

	return path;
}

void NeuralNetwork::train(Map& _map) {
	std::vector<std::vector<float>> chromosomes;
	std::vector<float> fitnesses;

	//First fill the chromosomes with 4 parents.
	while (chromosomes.size() < POPULATION) {
		std::vector<float> rand;

		for (int i = 0; i < getSignalCount(); i++)
			rand.push_back(random(-5.0f, 5.0f));

		chromosomes.push_back(rand);
	}

	std::vector<float> fittest;

	int generations = 0;
	int maxGenerations = 100000;
	bool foundPath = false;

	for (int gen = 0; gen < maxGenerations; gen++) {
		generations++;


		std::vector<std::vector<float>> children;
		fitnesses.clear();

		#pragma omp for
		for (int f = 0; f < POPULATION; f++) {
			setSignals(chromosomes[f]);

			float fit = 0.0f;
			bool validPath = false;

			getPath(_map, &validPath, &fit);

			#pragma omp critical
			{
				fitnesses.push_back(fit);
			}

			#pragma omp critical
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

	printf("NeuralNetwork: Train | Generations: %i\n", generations);

	if (foundPath) {
		setSignals(fittest);
		std::cout << "NeuralNetwork: Train | Sucessfully found a path." << std::endl;
	}
	else {
		std::cout << "NeuralNetwork: Train | Could not find a path. Potentially bad luck. Push F3 to train again." << std::endl;
	}
}

void NeuralNetwork::shuffleBiases() {
	for (float& bias : m_biases) {
		bias = random(-2.5, 2.5);
	}
}
