#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {
constexpr int kWidth = 20;
constexpr int kHeight = 20;

enum class Direction { Stop, Left, Right, Up, Down };

struct Game {
    int x{};
    int y{};
    int fruitX{};
    int fruitY{};
    int score{};
    int tailLength{};
    Direction dir{Direction::Stop};
    std::vector<std::pair<int, int>> tail;
    bool gameOver{false};
};

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

bool occupiesSnake(const Game& g, int px, int py) {
    if (g.x == px && g.y == py) return true;
    return std::any_of(g.tail.begin(), g.tail.end(),
                       [&](const auto& seg) { return seg.first == px && seg.second == py; });
}

void spawnFruit(Game& g) {
    while (true) {
        int fx = std::rand() % kWidth;
        int fy = std::rand() % kHeight;
        if (!occupiesSnake(g, fx, fy)) {
            g.fruitX = fx;
            g.fruitY = fy;
            return;
        }
    }
}

void setup(Game& g) {
    g.dir = Direction::Stop;
    g.x = kWidth / 2;
    g.y = kHeight / 2;
    g.score = 0;
    g.tailLength = 0;
    g.tail.clear();
    g.gameOver = false;
    spawnFruit(g);
}

void draw(const Game& g) {
    clearScreen();
    std::cout << "Score: " << g.score << "\n";

    for (int i = 0; i < kWidth + 2; i++) std::cout << '#';
    std::cout << "\n";

    for (int row = 0; row < kHeight; row++) {
        for (int col = 0; col < kWidth; col++) {
            if (col == 0) std::cout << '#';

            if (row == g.y && col == g.x) {
                std::cout << 'O';
            } else if (row == g.fruitY && col == g.fruitX) {
                std::cout << 'F';
            } else {
                bool printed = false;
                for (const auto& seg : g.tail) {
                    if (seg.first == col && seg.second == row) {
                        std::cout << 'o';
                        printed = true;
                        break;
                    }
                }
                if (!printed) std::cout << ' ';
            }

            if (col == kWidth - 1) std::cout << '#';
        }
        std::cout << "\n";
    }

    for (int i = 0; i < kWidth + 2; i++) std::cout << '#';
    std::cout << "\n";
}

void input(Game& g) {
    std::cout << "Enter direction (W/A/S/D) or Q to quit: ";
    std::string s;
    if (!(std::cin >> s)) {
        g.gameOver = true;
        return;
    }

    char c = static_cast<char>(std::tolower(static_cast<unsigned char>(s[0])));

    if (c == 'q') {
        g.gameOver = true;
        return;
    }

    // Keep it simple: allow direction changes freely.
    if (c == 'w') g.dir = Direction::Up;
    else if (c == 's') g.dir = Direction::Down;
    else if (c == 'a') g.dir = Direction::Left;
    else if (c == 'd') g.dir = Direction::Right;
}

void logic(Game& g) {
    if (g.dir == Direction::Stop) return;

    // Move tail: insert old head position at the front.
    if (g.tailLength > 0) {
        g.tail.insert(g.tail.begin(), {g.x, g.y});
        if (static_cast<int>(g.tail.size()) > g.tailLength) {
            g.tail.pop_back();
        }
    }

    // Move head.
    switch (g.dir) {
        case Direction::Left:  g.x--; break;
        case Direction::Right: g.x++; break;
        case Direction::Up:    g.y--; break;
        case Direction::Down:  g.y++; break;
        default: break;
    }

    // Wall collision.
    if (g.x < 0 || g.x >= kWidth || g.y < 0 || g.y >= kHeight) {
        std::cout << "Game Over! You hit the wall.\n";
        g.gameOver = true;
        return;
    }

    // Self collision.
    for (const auto& seg : g.tail) {
        if (seg.first == g.x && seg.second == g.y) {
            std::cout << "Game Over! You ran into yourself.\n";
            g.gameOver = true;
            return;
        }
    }

    // Eat fruit.
    if (g.x == g.fruitX && g.y == g.fruitY) {
        g.score += 10;
        g.tailLength++;
        spawnFruit(g);
    }
}
}  // namespace

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Game g;
    setup(g);

    while (!g.gameOver) {
        draw(g);
        input(g);
        logic(g);
    }

    std::cout << "Final Score: " << g.score << "\n";
    return 0;
}
