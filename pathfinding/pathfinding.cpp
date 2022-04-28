#include <iostream>
#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <array>
#include <random>

// type shortenings
using board_subt = std::vector<sf::Sprite>;
using board_t = std::vector<board_subt>;
using pos = std::pair<int, int>;

// variables
const auto WIDTH = 20;
const auto HEIGHT = 20;
const auto SCALE = 3;
const bool diagonals = true;
int obstacles_n = 200;
int delay_ms = 10;

std::array<sf::Texture, 6> colours;
auto& root_colour = colours[0];
auto& dest_colour = colours[1];
auto& checked_colour = colours[2];
auto& route_colour = colours[3];
auto& obst_colour = colours[5];
auto& empty_colour = colours[4];

// board
board_t board(HEIGHT, board_subt(WIDTH));
sf::RenderWindow window(sf::VideoMode(WIDTH * (SCALE * 10 + 5) - 5, HEIGHT * (SCALE * 10 + 5) - 5), "most mostowski");

// vector containing the visited points
std::vector<pos> visited;

// operator+= that pushes back the contents of one vector to another
std::vector<pos>& operator+=(std::vector<pos>& lhs, std::vector<pos> const& rhs) {
	lhs.insert(lhs.end(), rhs.begin(), rhs.end());
	return lhs;
}

void initBoard() {
	for (auto i = 0; i < HEIGHT; i++) {
		for (auto j = 0; j < WIDTH; j++) {
			board[i][j].setTexture(empty_colour);
			board[i][j].setScale(SCALE, SCALE);
			board[i][j].setPosition(i * (SCALE * 10 + 5), j * (SCALE * 10 + 5));
		}
	}
}

// prints board
void printBoard() {
	for (auto& i : board) {
		for (auto& j : i)
			window.draw(j);
	}
}

// sets points
void setPoints(pos const& root, pos const& dest) {
	auto const& [rx, ry] = root;
	auto const& [dx, dy] = dest;
	board[ry][rx].setTexture(root_colour);
	board[dy][dx].setTexture(dest_colour);
}

// sets obstacles
void setObstacles(const std::vector<pos>& obstacles) {
	for (auto const& [x, y] : obstacles)
		board[y][x].setTexture(obst_colour);
}

// checks if a point is within board borders
bool isWithin(pos const& point) {
	return (point.first >= 0 and point.first < WIDTH and point.second >= 0 and point.second < HEIGHT);
}

// checks if a point has been visited
bool isVisited(pos const& point) {
	for (auto const& i : visited)
		if (point == i) return true;
	return false;
}

// checks if a point is NOT an obstacle
bool notObstacle(pos const& point) {
	return (board[point.second][point.first].getTexture() != &obst_colour);
}

/* finds neighbours of a point:
* N N N
* N C N
* N N N
* // C - point
* // N - potential neighbours
*/ 
std::vector<pos> findNeighbours(pos centre) {
	std::vector<pos> neighbours;
	if (diagonals) {
		centre.first--; // goes to the top left corner of the point
		centre.second--;
		for (auto i = 0; i < 2; i++) { // goes from top left corner to top right corner so checks the entire top row
			centre.first++; // centre.first++ increases the x coordinate of the potential neighbour
			if (isWithin(centre) and !isVisited(centre) and notObstacle(centre)) neighbours.push_back(centre);
		}
		for (auto i = 0; i < 2; i++) { // checks the entire right columns
			centre.second++;
			if (isWithin(centre) and !isVisited(centre) and notObstacle(centre)) neighbours.push_back(centre);
		}
		for (auto i = 0; i < 2; i++) { // checks the bottom row
			centre.first--;
			if (isWithin(centre) and !isVisited(centre) and notObstacle(centre)) neighbours.push_back(centre);
		}
		for (auto i = 0; i < 2; i++) { // checks the left column
			centre.second--;
			if (isWithin(centre) and !isVisited(centre) and notObstacle(centre)) neighbours.push_back(centre);
		}
	}
	else {
		centre.second--;
		if (isWithin(centre) and !isVisited(centre) and notObstacle(centre)) neighbours.push_back(centre);
		centre.second++;
		centre.first++;
		if (isWithin(centre) and !isVisited(centre) and notObstacle(centre)) neighbours.push_back(centre);
		centre.second++;
		centre.first--;
		if (isWithin(centre) and !isVisited(centre) and notObstacle(centre)) neighbours.push_back(centre);
		centre.second--;
		centre.first--;
		if (isWithin(centre) and !isVisited(centre) and notObstacle(centre)) neighbours.push_back(centre);
	}
	return neighbours;
}

bool foundDest(const pos& point, const pos& dest) {
	return (point == dest);
}

pos randomisePoint(pos other = { -1, -1 }) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> disx(0, WIDTH - 1);
	std::uniform_int_distribution<> disy(0, HEIGHT - 1);
	pos point(disx(gen), disy(gen));
	if (point == other) return randomisePoint(point);
	else return point;
}

std::vector<pos> randomiseObstacles(pos root, pos dest, int number = -1) {
	std::vector<pos> obstacles;
	if (number == -1) return obstacles;
	obstacles.reserve(number);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> disx(0, WIDTH - 1);
	std::uniform_int_distribution<> disy(0, HEIGHT - 1);
	for (auto i = 0; i < number; i++) {
		auto point = pos{ disx(gen), disy(gen) };
		if (point != root and point != dest) obstacles.push_back(point);
	}
	return obstacles;
}

std::vector<pos> findRoute(pos const& root, pos const& dest) {
	visited.push_back(root);
	std::vector<pos> points = { root }; // points that we will be going over
	std::vector<std::pair<pos, pos>> routes = {}; // connections between points that will be later used to find the route
	std::vector<pos> route; // the final route
	int it = 0; // iterator going over each element in points
	auto cur = points[0]; // current point
	bool destFound = false;
	bool noRoute = false;
	while (true) {
		auto neighbours = findNeighbours(cur); // finds neighbours for the current point
		for (auto const& [x, y] : neighbours) {
			routes.push_back({ cur, pos(x, y) }); // adds the connection to routes
			visited.push_back(pos(x, y)); // adds the point to visited points
			if (foundDest(pos(x, y), dest)) destFound = true; // if destination found end loop
			board[y][x].setTexture(checked_colour);
		}
		if (destFound) break;
		points += neighbours; // adds neighbours to points
		if (it >= points.size()) noRoute = true;
		if (noRoute) break;
		cur = points[it++]; // checks next point
		window.clear();
		printBoard();
		window.display();
		sf::sleep(sf::Time(sf::milliseconds(delay_ms)));
		if (cur == pos{ -1, -1 }) break; // idk
	}
	if (!noRoute) {
		// finds route
		auto next = dest; // start from the end
		while (next != pos{ 0, 0 }) { // get route until back to beginning
			auto entry = std::find_if(routes.begin(), routes.end(), [&](std::pair<pos, pos> const& p) { // find the connection, so find the point which found the destination
				return (p.second == next); // and then the point which found the point which found the destination, etc.
				});
			route.push_back(next); // add the point to the route
			if (entry == routes.end()) break;
			next = entry->first; // go to the next point
		}
		std::reverse(route.begin(), route.end()); // reverses the route so from dest to root we have root to dest
		if (destFound) { // if route is complete
			for (auto i = 1; i < route.size() - 1; i++) {
				auto const& [x, y] = route[i];
				board[y][x].setTexture(route_colour);
				window.clear();
				printBoard();
				setPoints(root, dest);
				window.display();
				sf::sleep(sf::Time(sf::milliseconds(delay_ms)));
			}
		}
	}
	return route;
}

int main() {
	for (auto i = 0; i < 6; i++)
		colours[i].loadFromFile("./colours.png", sf::IntRect(i * 10, 0, 10, 10));
	pos root = randomisePoint();
	pos dest = randomisePoint(root);
	std::vector<pos> obstacles = randomiseObstacles(root, dest, obstacles_n);
	initBoard();
	setObstacles(obstacles);
	setPoints(root, dest);
	bool found = false;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		printBoard();
		window.display();
		std::vector<pos> temp;
		if(!found) temp = findRoute(root, dest);
		if(temp.size()) found = true;
	}
}