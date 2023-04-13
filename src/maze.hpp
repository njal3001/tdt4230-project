#pragma once
#include <vector>
#include <glm/vec2.hpp>

class Maze
{
public:
    struct Node
    {
        bool visited;

        Node();
    };

private:
    glm::ivec2 size;
    std::vector<Node> nodes;

public:
    Maze(const glm::ivec2 &size);

    const std::vector<Node> &get_nodes();

private:
    void visit_node(const glm::ivec2 &position);
};
