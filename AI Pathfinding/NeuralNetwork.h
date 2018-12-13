#pragma once

#include <vector>

class NeuralNetwork
{
private:
	std::vector<std::vector<float>> m_nodes;
	std::vector<float> m_signals;
public:
	NeuralNetwork(std::vector<int> _layerSizes);

	inline std::vector<float> getSignals() { m_signals; }
	inline void setSignals(std::vector<float> _signals) { m_signals = _signals; }
	inline int getSignalCount() { return m_signals.size(); }

	std::vector<float> run(std::vector<float> _inputNodes);
	void train(std::vector<float> _expectedAnswer);
};

