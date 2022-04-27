#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>
#pragma warning(disable: 26812)

using board_subt = std::vector<char>;
using board_t = std::vector<board_subt>;
using pos = std::pair<int, int>;

const auto WIDTH = 20;
const auto HEIGHT = 10;

board_t board(HEIGHT, board_subt(WIDTH, ' '));

std::vector<pos> visited;

std::vector<pos>& operator+=(std::vector<pos>& lhs, std::vector<pos> const& rhs) {
	lhs.insert(lhs.end(), rhs.begin(), rhs.end());
	return lhs;
}

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

void setPoints(pos const& root, pos const& dest) {
	auto const& [rx, ry] = root;
	auto const& [dx, dy] = dest;
	board[ry][rx] = 'o';
	board[dy][dx] = 'x';
}

void setObstacles(const std::vector<pos>& obstacles) {
	for (auto const& [x, y] : obstacles)
		board[y][x] = '#';
}

bool isWithin(pos const& point) {
	return (point.first >= 0 and point.first < WIDTH and point.second >= 0 and point.second < HEIGHT);
}

bool isVisited(pos const& point) {
	for (auto const& i : visited)
		if (point == i) return true;
	return false;
}

bool notObstacle(pos const& point) {
	return (board[point.second][point.first] != '#');
}

std::vector<pos> findNeighbours(pos centre) {
	std::vector<pos> neighbours;
	// check all corners
	centre.first--;
	centre.second--;
	for (auto i = 0; i < 2; i++) {
		pos point(centre.first++, centre.second);
		if (isWithin(point) and !isVisited(point) and notObstacle(point)) neighbours.push_back(point);
	}
	for (auto i = 0; i < 2; i++) {
		pos point(centre.first, centre.second++);
		if (isWithin(point) and !isVisited(point) and notObstacle(point)) neighbours.push_back(point);
	}
	for (auto i = 0; i < 2; i++) {
		pos point(centre.first--, centre.second);
		if (isWithin(point) and !isVisited(point) and notObstacle(point)) neighbours.push_back(point);
	}
	for (auto i = 0; i < 2; i++) {
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
	std::vector<pos> points = { root };
	std::vector<std::pair<pos, pos>> routes = {}; // last one is dest if found
	std::vector<pos> route;
	int it = 0;
	auto cur = points[0];
	bool destFound = false;
	while (true) {
		system("cls");
		auto neighbours = findNeighbours(cur);
		for (auto const& [x, y] : neighbours) {
			routes.push_back({ cur, pos(x, y) });
			visited.push_back(pos(x, y));
			if (foundDest(pos(x, y), dest)) destFound = true;
			board[y][x] = '+';
		}
		if (destFound) break;
		points += neighbours;
		cur = points[it++];
		printBoard();
		if (cur == pos{ -1, -1 }) break;
	}
	for (auto& i : routes) {
		auto const& [x1, y1] = i.first;
		auto const& [x2, y2] = i.second;
		std::cout << x1 << ',' << y1 << ' ' << x2 << ',' << y2 << '\n';
	}
	auto next = dest;
	while (next != pos{ 0, 0 }) {
		auto entry = std::find_if(routes.begin(), routes.end(), [&](std::pair<pos, pos> const& p) {
			return (p.second == next);
		});
		route.push_back(next);
		next = entry->first;
	}
	route.push_back(next);
	if (route.size()) {
		for (auto const& [x, y] : route) {
			board[y][x] = 'H';
		}
		printBoard();
	}
	else std::cout << "no route found";
}