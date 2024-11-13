#ifndef BSTREE_H
#define BSTREE_H

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include "Globals.h"
#include "Block.h"
#include "Contour.h"
class BSTreeNode {
public:
    int id;
    // Store width and height here to track rotation without modifying Block attributes
    Block *block;
    BSTreeNode *left, *right, *parent;
    BSTreeNode(int id):
        id(id), block(nullptr), left(nullptr), right(nullptr), parent(nullptr) {}
    BSTreeNode(Block *block):
        id(block->id), block(block), left(nullptr), right(nullptr), parent(nullptr) {}
    // for tree copying operation
    BSTreeNode(BSTreeNode *node):
        id(node->block->id), block(nullptr), left(nullptr), right(nullptr), parent(nullptr) {}
    ~BSTreeNode() {}
};
class BSTree {
private:
    Contour contour;
    // actual width and height of floorplan
    BSTreeNode* create_tree();
    BSTreeNode* __create_tree(BSTreeNode *node, bool is_left_child);
    void __destroy_tree(BSTreeNode *node);
    int get_y_coord(Block* blk);

    void __print_tree(BSTreeNode *node) const; // For debugging and visualization

public:
    BSTreeNode *root, *dummy;  // Root of the B* tree
    int outline_w, outline_h;
    int w, h;
    unsigned int size, COUNT, ind;
    int last_perturbation;
    Block *last_perturb_a, *last_perturb_b;
    std::vector<Block> blocks;
    std::unordered_map<Block*, BSTreeNode*> block_to_node;
    // nodes with one or zero child
    std::unordered_set<BSTreeNode*> space_one_nodes, space_two_nodes;
    BSTree();
    BSTree(const BSTree& other);
    BSTree(int outline_w, int outline_h, std::vector<Block> blocks); 
    BSTree& operator=(const BSTree& other);
    // BSTree(BSTree &other); 
    ~BSTree(); 
    // Operations
    BSTreeNode* _copy(BSTreeNode *node);
    // BSTree* copy();
    void reset_floorplan_state();
    void generate_floorplan();
    long long get_area_outside_outline();
    bool is_in_outline();
    void get_coord(BSTreeNode *node);
    void detach_node(BSTreeNode *node);
    BSTreeNode* sample_space_node();
    // Perturbations
    BSTree* find_neighbor();
    int perturb();
    void rotate_module(Block *block);
    void random_move_module(Block *block);
    void swap_module(Block *block, Block *blockB);
    // int size() const {return this->_size;}
    void _get_tree_size(BSTreeNode *node, int &cnt) {
        if (!node) return;
        cnt++;
        _get_tree_size(node->left, cnt);
        _get_tree_size(node->right, cnt);
    }
    int get_tree_size() {
        int cnt = 0;
        _get_tree_size(root, cnt);
        return cnt;
    }
    void _print_memaddr(FILE *file, BSTreeNode *node) const {
        fprintf(file, "(node: %p, block: %p)\n", (void*)node, (void*)node->block);
        if (node->left) _print_memaddr(file, node->left);
        if (node->right) _print_memaddr(file, node->right);
    }
    void print_memaddr(const char* filename) const {
        // Open the file for writing
        FILE* file = fopen(filename, "w");
        if (!file) {
            perror("Error opening file");
            return;
        }

        // Write tree nodes
        fprintf(file, "Tree node: ");
        _print_memaddr(file, dummy);
        fprintf(file, "\n");

        // Write block to node map
        fprintf(file, "Block to node map: ");
        for (const auto& i : block_to_node) {
            fprintf(file, "(blk: %p, node: %p),\n", i.first, (void*)i.second);
        }
        fprintf(file, "\n");

        // Write space nodes
        fprintf(file, "Space nodes: ");
        for (BSTreeNode* i : space_one_nodes) {
            fprintf(file, "%p\n", (void*)i);
        }
        for (BSTreeNode* i : space_two_nodes) {
            fprintf(file, "%p\n", (void*)i);
        }
        fprintf(file, "\n");

        // Write block array
        fprintf(file, "Block array: start: %p, end: %p\n", (void*)&blocks[0], (void*)&blocks[blocks.size() - 1]);

        // Write contour map
        fprintf(file, "Contour map: ");
        for (const auto& i : contour.contour) {
            fprintf(file, "(left_bound: %d, line_ptr: %p),\n", i.first, (void*)i.second);
        }
        fprintf(file, "\n");

        // Close the file after writing
        fclose(file);
    }
    void print_tree() const; // For debugging and visualization
    void print2DUtil(BSTreeNode* root, int space) {
        // Base case
        if (root == nullptr)
            return;
    
        // Increase distance between levels
        space += COUNT;
    
        // Process right child first
        print2DUtil(root->right, space);
    
        // Print current node after space
        // count
        std::cout << "\n";
        for (int i = COUNT; i < space; i++)
            std::cout << " ";
        std::cout << root->block->name << '\n';
    
        // Process left child
        print2DUtil(root->left, space);
    }
    
    void print2D() {
        COUNT = 3;
        print2DUtil(root, 0);
    }
    bool isIdentical(BSTreeNode* r1, BSTreeNode* r2) {
        // If both trees are empty, they are identical
        if (r1 == nullptr && r2 == nullptr)
            return true;

        // If only one tree is empty, they are not identical
        if (r1 == nullptr || r2 == nullptr) {
            if (!r1)
                printf("r1 is null\n");
            if (!r2)
                printf("r2 is null\n");
            return false;
        }

        // Check if the root data is the same and
        // recursively check for the left and right subtrees
        bool equal = (r1->block->name == r2->block->name) && (r1->block->w == r2->block->w);
        // if (equal) {
        //     printf("Equal! BlockA: %s, BlockB: %s\n", r1->block->name.c_str(), r2->block->name.c_str());
        // } else {
        //     printf("Not Equal! BlockA: %s, h: %d, w: %d, BlockB: %s, h: %d, w: %d\n", 
        //         r1->block->name.c_str(), r1->block->h, r1->block->w, 
        //         r2->block->name.c_str(), r2->block->h, r2->block->w);
        // }
        return equal && isIdentical(r1->left, r2->left) &&
            isIdentical(r1->right, r2->right);
    }
};

#endif  // BSTREE_H