#include "maze.hpp"
#include "calc.hpp"
#include <iostream>

Maze::Node::Node()
    : visited(false)
{}

Maze::Maze(const glm::ivec2 &size)
    : size(size), nodes(size.x * size.y)
{
    glm::ivec2 start;
    start.x = Calc::randrange(0, size.x);
    start.y = Calc::randrange(0, size.y);

    this->visit_node(start);
}

void Maze::visit_node(const glm::ivec2 &position)
{
    std::cout << position.x << ", " << position.y << '\n';
    Node &node = this->nodes[position.y * this->size.x + position.x];
    node.visited = true;

    std::vector<glm::ivec2> directions =
    {
        glm::ivec2(1, 0),
        glm::ivec2(0, 1),
        glm::ivec2(-1, 0),
        glm::ivec2(0, -1),
    };

    while (directions.size() > 0)
    {
        int direction_index = Calc::randrange(0, directions.size());
        glm::ivec2 direction = directions[direction_index];

        directions.erase(directions.begin() + direction_index);

        glm::ivec2 neighbor_position = position + 2 * direction;

        if (neighbor_position.x < 0 || neighbor_position.x >= this->size.x ||
                neighbor_position.y < 0 || neighbor_position.y >= this->size.y)
        {
            continue;
        }

        Node &neighbor = this->nodes[
            neighbor_position.y * this->size.x + neighbor_position.x];

        if (neighbor.visited)
        {
            continue;
        }

        glm::ivec2 in_between_position = position + direction;
        this->nodes[in_between_position.y * this->size.x + in_between_position.x].visited = true;

        visit_node(neighbor_position);
    }
}

const std::vector<Maze::Node> &Maze::get_nodes()
{
    return this->nodes;
}
