#include "NeuralNetwork.h"

float random2(float min = 0.0f, float max = 1.0f) {
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

	m_biases = std::vector<float>(totalNodes);

	for (float& bias : m_biases) {
		bias = random2(-2.5, 2.5);
	}

	//Initalise the signals
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
