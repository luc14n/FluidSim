#pragma once
#include <vector>
#include <memory>
#include <tuple>

class FluidGrid {
public:
     struct Cell {
          float velocity[3]; // x, y, z (z=0 for 2D)
          float pressure;
          int   material_id;
     };

     FluidGrid(int order = 16);
     Cell* find(int x, int y, int z = 0);
     void insert(int x, int y, int z, const Cell& cell);

private:
     struct Node {
          bool isLeaf;
          std::vector<std::tuple<int, int, int>> keys;
          std::vector<std::shared_ptr<Node>> children; // for internal nodes
          std::vector<Cell> values; // for leaf nodes
          std::shared_ptr<Node> next; // for leaf node chaining

          Node(bool leaf) : isLeaf(leaf) {}
     };

     int order;
     std::shared_ptr<Node> root;

     void splitChild(std::shared_ptr<Node> parent, int idx);
     void insertNonFull(std::shared_ptr<Node> node, const std::tuple<int, int, int>& key, const Cell& cell);
     Cell* findInNode(std::shared_ptr<Node> node, const std::tuple<int, int, int>& key);

     int width, height, depth;
     std::vector<Cell> cells;
};