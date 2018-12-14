#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <Windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <chrono>

#include "NeuralNetwork.h"
#include "Map.h"

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<float> Duration;

void drawGrid(sf::RenderTarget& target, Map& map, sf::FloatRect _region, std::vector<sf::Vector2i> _path);

int main() {
	srand(time(NULL));

	//Print instructions.
	printf("FOCUS ON THE GAME WINDOW! Keys do not work otherwise.");
	printf("Press [1] To load a map.\n");
	printf("Press [2] To run the A* algorithm.\n");
	printf("Press [3] To train the NeuralNetwork.\n");
	printf("Press [4] To run the NeuralNetwork which produces a path.\n");
	printf("Press [5] To shuffle the biases in the NeuralNetwork (You will need to retrain [3]).\n\n");

	sf::RenderWindow window(sf::VideoMode(640, 480), "AI Pathfinding");

	NeuralNetwork network({ 6, 6, 4 });
	Map* map = nullptr;
	std::vector<sf::Vector2i> path;

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed) {
				//Open a file.
				if (event.key.code == sf::Keyboard::Num1) {
					//Make use of the Win32 file explorer to allow the ability to select a file.
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
						delete map;
						map = new Map(filename);
						path.clear();
					}
				}
				//Shuffle biases
				else if (event.key.code == sf::Keyboard::Num5) {
					printf("NeuralNetwork: Biases have been shuffled.\n");
					network.shuffleBiases();
				}
				else if (map != nullptr) {
						//Run a* pathfinding.
					if (event.key.code == sf::Keyboard::Num2) {
						auto start = Time::now();

						path = map->getPath();

						auto difference = Time::now() - start;

						if (path.size() != 0) {
							printf("A-Star: Search | Successfully found a path through the map. Time Elapsed: %fs\n", std::chrono::duration_cast<Duration>(difference).count());
						}

					}
					//Train neural network.
					else if (event.key.code == sf::Keyboard::Num3) {
						printf("NeuralNetwork: Train | Training NeuralNetwork...\n");
						//Run this in parallel as training can take quite along time.

						auto start = Time::now();
						network.train(*map);
						auto difference = Time::now() - start;
						printf("NeuralNetwork: Train | Finished training NeuralNetwork. Time Elapsed: %fs\n", std::chrono::duration_cast<Duration>(difference).count());
					}
					//Run neural network.
					else if (event.key.code == sf::Keyboard::Num4) {
						printf("NeuralNetwork: Search | Processing path...\n");
						path = network.getPath(*map);

						if (path.size() != 0) {
							printf("NeuralNetwork: Search | Successfully found a path.\n");
						}
						else {
							printf("NeuralNetwork: Search | Could not find path.\n");
						}
					}
				}
				else {
					printf("ERROR: There is no map loaded. Press 1 to load a map.\n");
				}
			}
		}

		window.clear();

		sf::Vector2u size = window.getSize();
		window.setView(sf::View(sf::Vector2f(size.x / 2, size.y / 2), sf::Vector2f(size.x, size.y)));
		window.clear();

		if (map != nullptr) {
			//Calculate a region that i can display the grid.
			sf::FloatRect region;

			float sqSizeX = 0.0f;
			float sqSizeY = 0.0f;

			if (size.x < size.y) {
				sqSizeX = (float)size.x * 0.7f;
				sqSizeY = (float)size.x * 0.7f;

				if (map->getWidth() < map->getHeight()) {
					sqSizeX *= (float)map->getWidth() / (float)map->getHeight();
				}
				else {
					sqSizeY *= (float)map->getHeight() / (float)map->getWidth();
				}
			}
			else {
				sqSizeX = (float)size.y * 0.7f;
				sqSizeY = (float)size.y * 0.7f;

				if (map->getWidth() < map->getHeight()) {
					sqSizeX *= (float)map->getWidth() / (float)map->getHeight();
				}
				else {
					sqSizeY *= (float)map->getHeight() / (float)map->getWidth();
				}
			}


			region.width = sqSizeX;
			region.height = sqSizeY;
			region.left = (size.x - sqSizeX) / 2;
			region.top = (size.y - sqSizeY) / 2;

			//Draw the grid.
			drawGrid(window, *map, region, path);
		}

		window.display();
	}
}

void drawGrid(sf::RenderTarget& target, Map& map, sf::FloatRect _region, std::vector<sf::Vector2i> _path) {
	float cellWidth = _region.width / (float)map.getWidth();
	float cellHeight = _region.height / (float)map.getHeight();

	//Draw the map data.
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

	//Draw grid lines.
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

	//Draw the path.
	for (sf::Vector2i node : _path) {
		sf::RectangleShape shape;
		shape.setSize(sf::Vector2f(cellWidth, cellHeight));
		shape.setPosition(sf::Vector2f(_region.left + (float)node.x * cellWidth, _region.top + (float)node.y * cellHeight));
		shape.setFillColor(sf::Color(0, 0, 255, 100));
		target.draw(shape);
	}
}