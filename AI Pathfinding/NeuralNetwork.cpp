#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork(std::vector<int> _layerSizes) {
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

		m_nodes.push_back(std::vector<float>(layerSize));
	}

	//Initalise the signals
	m_signals = std::vector<float>(signalCount);
}

std::vector<float> NeuralNetwork::run(std::vector<float> _inputNodes) {
	int signalOffset = 0;

	m_nodes[0] = _inputNodes;

	//Start at 1 because the input layer does not use the activation function.
	for (int i = 1; i < m_nodes.size(); i++) {
		std::vector<float>& prevLayer = m_nodes[i - 1];
		std::vector<float>& layer = m_nodes[i];

		//Add all the weighted values.
		for (int prevNodeIndex = 0; prevNodeIndex < prevLayer.size(); prevNodeIndex++) {
			for (int nodeIndex = 0; nodeIndex < layer.size(); nodeIndex++) {
				int signalIndex = (prevNodeIndex * layer.size()) + nodeIndex;

				layer[nodeIndex] = prevLayer[prevNodeIndex] * m_signals[signalIndex + signalOffset];
			}
		}

		//Activate the values.
		for (int nodeIndex = 0; nodeIndex < layer.size(); nodeIndex++) {
			float& node = layer[nodeIndex];

			node = node / (1.0f + abs(node));
		}

		signalOffset += prevLayer.size() * layer.size();
	}

	return m_nodes[m_nodes.size() - 1];
}

void NeuralNetwork::train(std::vector<float> _expectedAnswer) {
	float alpha = 0.01f;
	std::vector<float> deltas;
	std::vector<float> newSignals(m_signals.size());
	float finalDelta = 0.0f;

	for (int i = 0; i < _expectedAnswer.size(); i++) {
		float& answer = _expectedAnswer[i];
		float& result = m_nodes[m_nodes.size() - 1][i];
		float error = answer - result;
		float delta = result * (1 - result) * error;

		deltas.push_back(delta);
		finalDelta += delta;
	}

	int signalOffset = m_signals.size() - 1;

	for (int i = m_nodes.size() - 1; i > 0; i--) {
		std::vector<float>& prevLayer = m_nodes[i - 1];
		std::vector<float>& layer = m_nodes[i];

		signalOffset -= prevLayer.size() * layer.size();

		std::vector<float> newDeltas(prevLayer.size());

		for (int nodeIndex = 0; nodeIndex < layer.size(); nodeIndex++) {
			for (int prevNodeIndex = 0; prevNodeIndex < prevLayer.size(); prevNodeIndex++) {
				int signalIndex = (prevNodeIndex * layer.size()) + nodeIndex;

				newSignals[signalIndex] = m_signals[signalIndex] + alpha * prevLayer[prevNodeIndex] * deltas[nodeIndex];



				float& answer = layer[nodeIndex];
				float& result = prevLayer[prevNodeIndex];

				float error = answer - result;
				float newDelta = result * (1 - result) * error;
				newDeltas.push_back(newDelta);

			}
		}
	}


}
