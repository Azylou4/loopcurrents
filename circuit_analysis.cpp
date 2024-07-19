#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <stack>
#define DIMENSION 4
#define LINK std::string
#define INDICES std::pair < int, int >
#define MIN(a, b)((a) <= (b) ? (a) : (b))
#define MAX(a, b)((b) >= (a) ? (b) : (a))

bool is_connected(LINK graph[DIMENSION][DIMENSION], std::set < INDICES >& available);
std::vector < INDICES > find_cycle(LINK graph[DIMENSION][DIMENSION], std::set < INDICES >& available);
int get_R(LINK link);
int get_E(LINK link);

int main() {
    LINK graph[DIMENSION][DIMENSION] = {
      {".", "R1", "-E1-R6", "R2"},
      {"-R2", ".", "R3", "R4"},
      {"E1R6", "-R4", ".", "R5"},
      {"-R1", "-R3", "-R5", "."},
    };
    auto available = std::set < INDICES >();
    auto removed = std::set < INDICES >();


    for (int i = 0; i < DIMENSION - 1; i++) {
        for (int j = i + 1; j < DIMENSION; j++) {
            if (graph[i][j] != ".") {
                available.insert(std::make_pair(i, j));
            }
        }
    }


    bool finished = false;
    while (!finished) {
        finished = true;
        for (int i = 0; i < DIMENSION - 1; i++) {
            for (int j = i + 1; j < DIMENSION; j++) {
                auto pair = std::make_pair(MIN(i, j), MAX(i, j));
                if (available.find(pair) == available.end())
                    continue;
                available.erase(pair);
                removed.insert(pair);
                if (is_connected(graph, available)) {
                    finished = false;
                }
                else {
                    available.insert(pair);
                    removed.erase(pair);
                }
            }
        }
    }
    auto cycles = std::vector < std::vector < INDICES >>();
    for (auto indexes : removed) {
        available.insert(indexes);
        cycles.push_back(find_cycle(graph, available));
        available.erase(indexes);
    }
    auto counters = std::vector < std::vector < int >>(DIMENSION * DIMENSION);
    for (int i = 0; i < cycles.size(); i++) {
        for (auto pair : cycles[i]) {
            int R = get_R(graph[pair.first][pair.second]);
            if (R == 0)
                R = get_E(graph[pair.first][pair.second]);
            int sign = R < 0 ? -1 : 1;
            counters[R * sign].push_back((i + 1) * sign);
        }
    }
    auto equations = std::vector < std::string >();
    for (auto cycle : cycles) {
        std::string output = "0";
        auto Es = std::vector < int >();
        for (auto pair : cycle) {
            int R = get_R(graph[pair.first][pair.second]);
            int E = get_E(graph[pair.first][pair.second]);
            if (E != 0) {
                Es.push_back(E);
            }
            int sign = R < 0 ? -1 : 1;
            for (auto R_ind : counters[R * sign]) {
                output += R_ind > 0 ? " + " : " - ";
                output += "I";
                output += std::to_string(abs(R_ind));
                output += "*R";
                output += std::to_string(abs(R));
            }
        }
        output += " = 0";
        for (auto E : Es) {
            output += (E > 0 ? " + " : " - ");
            output += "E";
            output += std::to_string(abs(E));
        }
        equations.push_back(output);
    }
    for (auto eq : equations) {
        std::cout << eq << std::endl;
    }
    return 0;
}
bool is_connected(LINK graph[DIMENSION][DIMENSION], std::set < INDICES >& available) {
    auto done = std::set < int >();
    auto paths = std::stack < int >();
    paths.push(0);
    while (!paths.empty()) {
        int point = paths.top();
        paths.pop();
        done.insert(point);
        for (int i = 0; i < DIMENSION; i++) {
            bool is_edge = graph[point][i] != ".";
            auto pair = std::make_pair(MIN(point, i), MAX(point, i));
            bool is_available = available.find(pair) != available.end();
            bool isnt_done = done.find(i) == done.end();
            if (is_edge && is_available && isnt_done) {
                paths.push(i);
            }
        }
    }
    return done.size() == DIMENSION;
}
std::vector < INDICES > find_cycle(LINK graph[DIMENSION][DIMENSION], std::set < INDICES >& available) {
    auto output = std::vector < INDICES >();
    auto ways = std::stack < INDICES >();
    ways.push(std::make_pair(0, -1));
    auto done = std::set < int >();
    auto trace = std::stack < INDICES >();
    trace.push(std::make_pair(0, -1));
    while (!ways.empty()) {
        int point = ways.top().first;
        int parent = ways.top().second;
        ways.pop();
        if (done.find(point) != done.end()) {
            break;
        }
        done.insert(point);
        for (int i = 0; i < DIMENSION; i++) {
            bool is_edge = graph[point][i] != ".";
            auto pair = std::make_pair(MIN(point, i), MAX(point, i));
            bool is_available = available.find(pair) != available.end();
            bool isnt_done = i != parent;
            if (is_edge && is_available && isnt_done) {
                ways.push(std::make_pair(i, point));
                trace.push(std::make_pair(i, point));
            }
        }
    }
    int root = trace.top().first;
    INDICES ptr = trace.top();
    trace.pop();
    output.push_back(ptr);
    while (ptr.second != root) {
        while (trace.top().first != ptr.second)
            trace.pop();
        output.push_back(trace.top());
        ptr = trace.top();
    }
    return output;
}
int get_R(LINK link) {
    for (int i = 0; i < link.size(); i++) {
        if (link[i] == 'R') {
            int num = 0;
            for (int j = i + 1; j < link.size(); j++) {
                if (link[j] > '9' || link[j] < '0') {
                    break;
                }
                num *= 10;
                num += link[j] - '0';
            }
            if (i > 0 && link[i - 1] == '-') {
                num = -num;
            }
            return num;
        }
    }
    return 0;
}
int get_E(LINK link) {
    for (int i = 0; i < link.size(); i++) {
        if (link[i] == 'E') {
            int num = 0;
            for (int j = i + 1; j < link.size(); j++) {
                if (link[j] > '9' || link[j] < '0') {
                    break;
                }
                num *= 10;
                num += link[j] - '0';
            }
            if (i > 0 && link[i - 1] == '-') {
                num = -num;
            }
            return num;
        }
    }
    return 0;
}
