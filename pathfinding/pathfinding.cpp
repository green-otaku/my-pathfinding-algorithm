#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>
#pragma warning(disable: 26812)

// type shortenings
using board_subt = std::vector<char>;
using board_t = std::vector<board_subt>;
using pos = std::pair<int, int>;

// board sizes
const auto WIDTH = 20;
const auto HEIGHT = 10;

// board
board_t board(HEIGHT, board_subt(WIDTH, ' '));

// vector containing the visited points
std::vector<pos> visited;

// operator+= that pushes back the contents of one vector to another
std::vector<pos>& operator+=(std::vector<pos>& lhs, std::vector<pos> const& rhs) {
	lhs.insert(lhs.end(), rhs.begin(), rhs.end());
	return lhs;
}

// prints board
void printBoard() {
	for (auto i = 0; i < WIDTH + 2; i++) std::cout << '#';
	std::cout << '\n';
	for (auto& i : board) {
		std::cout << '#';
		for (auto& j : i)
			std::cout << j;
		std::cout << "#\n";
	}
	for (auto i = 0; i < WIDTH + 2; i++) std::cout << '#';
}

// sets points
void setPoints(pos const& root, pos const& dest) {
	auto const& [rx, ry] = root;
	auto const& [dx, dy] = dest;
	board[ry][rx] = 'o';
	board[dy][dx] = 'x';
}

// sets obstacles
void setObstacles(const std::vector<pos>& obstacles) {
	for (auto const& [x, y] : obstacles)
		board[y][x] = '#';
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
	return (board[point.second][point.first] != '#');
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
	centre.first--; // goes to the top left corner of the point
	centre.second--;
	for (auto i = 0; i < 2; i++) { // goes from top left corner to top right corner so checks the entire top row
		pos point(centre.first++, centre.second); // centre.first++ increases the x coordinate of the potential neighbour
		if (isWithin(point) and !isVisited(point) and notObstacle(point)) neighbours.push_back(point);
	}
	for (auto i = 0; i < 2; i++) { // checks the entire right columns
		pos point(centre.first, centre.second++);
		if (isWithin(point) and !isVisited(point) and notObstacle(point)) neighbours.push_back(point);
	}
	for (auto i = 0; i < 2; i++) { // checks the bottom row
		pos point(centre.first--, centre.second);
		if (isWithin(point) and !isVisited(point) and notObstacle(point)) neighbours.push_back(point);
	}
	for (auto i = 0; i < 2; i++) { // checks the left column
		pos point(centre.first, centre.second--);
		if (isWithin(point) and !isVisited(point) and notObstacle(point)) neighbours.push_back(point);
	}
	return neighbours;
}

bool foundDest(const pos& point, const pos& dest) {
	return (point == dest);
}

int main() {
	pos root = { 0, 0 };
	pos dest = { 17, 3 };
	visited.push_back(root);
	std::vector<pos> obstacles = { /*{2, 2}, {7, 3}, {14, 7}, {19, 8}, {2, 6}*/ };
	setPoints(root, dest);
	setObstacles(obstacles);
	std::vector<pos> points = { root }; // points that we will be going over
	std::vector<std::pair<pos, pos>> routes = {}; // connections between points that will be later used to find the route
	std::vector<pos> route; // the final route
	int it = 0; // iterator going over each element in points
	auto cur = points[0]; // current point
	bool destFound = false;
	while (true) {
		system("cls");
		auto neighbours = findNeighbours(cur); // finds neighbours for the current point
		for (auto const& [x, y] : neighbours) {
			routes.push_back({ cur, pos(x, y) }); // adds the connection to routes
			visited.push_back(pos(x, y)); // adds the point to visited points
			if (foundDest(pos(x, y), dest)) destFound = true; // if destination found end loop
			board[y][x] = '+';
		}
		if (destFound) break;
		points += neighbours; // adds neighbours to points
		cur = points[it++]; // checks next point
		printBoard();
		if (cur == pos{ -1, -1 }) break; // idk
	}
	// finds route
	auto next = dest; // start from the end
	while (next != pos{ 0, 0 }) { // get route until back to beginning
		auto entry = std::find_if(routes.begin(), routes.end(), [&](std::pair<pos, pos> const& p) { // find the connection, so find the point which found the destination
			return (p.second == next); // and then the point which found the point which found the destination, etc.
		});
		route.push_back(next); // add the point to the route
		next = entry->first; // go to the next point
	}
	std::reverse(route.begin(), route.end()); // reverses the route so from dest to root we have root to dest
	if (route.front() == pos(0, 0)) { // if route is complete
		for (auto const& [x, y] : route) {
			board[y][x] = 'H';
		}
		printBoard();
	}
	else std::cout << "no route found";
}