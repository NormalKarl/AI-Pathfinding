#ifndef _H_NEURALNETWORK
#define _H_NEURALNETWORK

#include <vector>

//A NeuralNetwork represents a forward-feeding neural network.
class NeuralNetwork
{
private:
	//An array of array's containing the values for each node.
	std::vector<std::vector<float>> m_nodes;
	//A 1D array of signals matching all the weights between the nodes.
	std::vector<float> m_signals;

	std::vector<float> m_biases;
public:
	//Creates a Neural Network with all the nodes and signals connected.
	//Signals are a 1D array for the purposes of a chromosome.
	//_layerSizes is an array of integers with each integer representing the
	//layer's node count.
	NeuralNetwork(std::vector<int> _layerSizes);

	//Returns all the weights in the neural network in a 1D array
	//for use as a chromosome.
	inline std::vector<float> getSignals() { m_signals; }
	//Sets all the weights in the array.
	//WARNING: Must be the correct length of weights
	inline void setSignals(std::vector<float> _signals) { m_signals = _signals; }
	//Returns the current signal count.
	inline int getSignalCount() { return m_signals.size(); }
	//Runs the neural network with the currently held signals.
	//_inputNodes are parse and set in the neural network.
	std::vector<float> run(std::vector<float> _inputNodes);
};

#endif
