#include "BSTree.h"


bool compareByArea(Block* a, Block* b) {
    return a->area > b->area; 
}
BSTree::BSTree() {
    root = nullptr;
    dummy = new BSTreeNode(-1);
}
BSTree::BSTree(int outline_w, int outline_h, std::vector<Block> blocks): BSTree() {
    this->size = blocks.size();
    this->outline_w = outline_w;
    this->outline_h = outline_h;
    this->w = this->h = 0;
    this->blocks = blocks;
    this->root = this->create_tree();
    this->dummy->left = this->root;
    this->root->parent = this->dummy;
}
BSTree::BSTree(const BSTree& other): BSTree() {
    // // Copy over the attributes from the returned tree copy
    this->contour = other.contour;
    this->blocks = other.blocks;
    this->outline_h = other.outline_h;
    this->outline_w = other.outline_w;
    this->h = other.h;
    this->w = other.w;
    this->size = other.size;
    this->root = _copy(other.root);
    this->dummy->left = root, this->root->parent = this->dummy;
}
BSTree& BSTree::operator=(const BSTree& other) {
    if (this == &other) {
        return *this;  // Handle self-assignment
    }
    // // Perform the copy operation
    this->contour = other.contour;
    this->blocks = other.blocks;
    this->outline_h = other.outline_h;
    this->outline_w = other.outline_w;
    this->h = other.h;
    this->w = other.w;
    this->size = other.size;
    this->block_to_node.clear();
    if (root) {
        this->__destroy_tree(root);
    }
    this->space_one_nodes.clear();
    this->space_two_nodes.clear();
    this->root = _copy(other.root);
    this->dummy->left = root, this->root->parent = this->dummy;
    logger.debug("assignment finish, address: %d\n", this);
    return *this;
}
BSTree::~BSTree() {
    this->__destroy_tree(this->dummy);
}
void BSTree::__destroy_tree(BSTreeNode *node) {
    if (!node) return;
    // if (node->id == -1) {
    //     std::cout << "Dummy" << '\n';
    // } else {
    //     std::cout << node->block->name << '\n';
    // }
    __destroy_tree(node->left);
    __destroy_tree(node->right);
    delete node;
}
// Recursively create the left subtree
// If the right boundary is exceeded, build right subtree
// Allow the total length to be up to 2 times the outline width.
BSTreeNode* BSTree::__create_tree(BSTreeNode *node, bool is_left_child) {
    if (ind >= this->blocks.size()) return nullptr;
    Block *cur_block = &(this->blocks[ind]);
    int random_number = intRand();
    // randoms.push_back(random_number);
    if (random_number & 1) {
        this->rotate_module(cur_block);
    }
    int x = node == nullptr ? 0 : (is_left_child ? node->block->x + node->block->w : node->block->x);
    if (x + cur_block->w > (int) ((4) * this->outline_w)) return nullptr;
    cur_block->x = x;
    cur_block->y = this->contour.get_y_coord(cur_block);
    BSTreeNode *new_node = new BSTreeNode(cur_block);
    ind++;
    this->block_to_node[cur_block] = new_node;
    new_node->parent = node;
    // std::cout << "======Contour======\n";
    // printf("Inserting Block: w: %d, h: %d, (x, y): (%d, %d)\n", cur_block->w, cur_block->h, cur_block->x, cur_block->y);
    // for (auto &i : this->contour.contour) {
    //     auto line = i.second;
    //     // std::cout << line->x0 << ' ' << line->y0 << ' ' << line->x1 << ' ' << line->y1 << '\n';
    //     printf("Line (%4d, %4d) to (%4d, %4d)\n", line->x0, line->y0, line->x1, line->y1);
    // }
    // std::cout << "===================\n";
    // within right bound, 
    new_node->left = __create_tree(new_node, true);
    new_node->right = __create_tree(new_node, false);
    if (!new_node->left && !new_node->right) {
        this->space_two_nodes.insert(new_node);
    }else if ((!new_node->left && new_node->right) || (new_node->left && !new_node->right)) {
        this->space_one_nodes.insert(new_node);
    }
    this->w = std::max(this->w, new_node->block->x + new_node->block->w);
    this->h = std::max(this->h, new_node->block->y + new_node->block->h);
    return new_node;
}
BSTreeNode* BSTree::create_tree() {
    if (!this->blocks.size()) return nullptr;

    logger.debug("Num_blocks: %d\n", this->blocks.size());

    this->ind = 0;
    root = this->__create_tree(nullptr, false);
    logger.debug("Tree created\n");
    // this->print2D();
    return root;
}
BSTreeNode* BSTree::_copy(BSTreeNode *node) {
    if (!node) {
        printf("Error root is null\n");
    }
    // note that the block instances is different for each tree
    BSTreeNode *new_node = new BSTreeNode(node);
    new_node->block = &(this->blocks[node->block->id]);
    if (node->left) {
        new_node->left = _copy(node->left);
        new_node->left->parent = new_node;
    }
    if (node->right) {
        new_node->right = _copy(node->right);
        new_node->right->parent = new_node;
    }
    this->block_to_node[new_node->block] = new_node;
    if (!new_node->right && !new_node->left)  {
        this->space_two_nodes.insert(new_node);
    } else if ((new_node->left && !new_node->right) || !new_node->left && new_node->right) {
        this->space_one_nodes.insert(new_node);
    } 
    return new_node;
}
// BSTree* BSTree::copy() {
//     BSTree *tree_copy = new BSTree();
//     logger.debug("copying attributes\n");
//     tree_copy->contour = this->contour;
//     tree_copy->blocks = this->blocks;
//     tree_copy->outline_h = this->outline_h, tree_copy->outline_w = this->outline_w;
//     tree_copy->h = this->h, tree_copy->w = this->w;
//     tree_copy->size = this->size;
//     logger.debug("copying tree structure, size: %d\n", tree_copy->size);
//     tree_copy->root = this->_copy(this->root, tree_copy);
//     logger.debug("setting dummy\n");
//     tree_copy->dummy->left = tree_copy->root;
//     tree_copy->root->parent = tree_copy->dummy;

//     return tree_copy;
// }
void BSTree::reset_floorplan_state() {
    this->contour.reset_contour();
    this->w = this->h = 0;
}
void BSTree::generate_floorplan() {
    logger.debug("Reseting contour\n");
    reset_floorplan_state();
    logger.debug("Getting root coordinates\n");
    root->block->x = 0;
    root->block->y = this->contour.get_y_coord(root->block);
    logger.debug("Getting coordinates\n");
    // printf("Getting coordinates\n");
    get_coord(root);
    // printf("Coord got\n");
}
inline long long calculate_overlap_area(int x0_1, int y0_1, int x1_1, int y1_1,
                           int x0_2, int y0_2, int x1_2, int y1_2) {
    int overlap_x0 = std::max(x0_1, x0_2);
    int overlap_y0 = std::max(y0_1, y0_2);
    int overlap_x1 = std::min(x1_1, x1_2);
    int overlap_y1 = std::min(y1_1, y1_2);

    int overlap_width = overlap_x1 - overlap_x0;
    int overlap_height = overlap_y1 - overlap_y0;

    if (overlap_width > 0 && overlap_height > 0) {
        return (long long) overlap_width * overlap_height;
    }
    
    return 0;
}
long long BSTree::get_area_outside_outline() {
    long long area = 0;
    for (Block blk : this->blocks) {
        int x0 = blk.x, x1 = blk.x + blk.w,
            y0 = blk.y, y1 = blk.y + blk.h;
        long long blk_area = (long long) blk.w * blk.h;
        area += blk_area - calculate_overlap_area(x0, y0, x1, y1, 0, 0, outline_w, outline_h);
    }
    return area;
}
void BSTree::get_coord(BSTreeNode *node) {
    BSTreeNode *left_child = node->left, *right_child = node->right;
    if (left_child) {
        left_child->block->x = node->block->x + node->block->w;
        left_child->block->y = this->contour.get_y_coord(left_child->block);
        this->w = std::max(this->w, left_child->block->x + left_child->block->w);
        this->h = std::max(this->h, left_child->block->y + left_child->block->h);
        get_coord(left_child);
    } 
    // std::cout << "Getting right child\n";
    if (right_child) {
        right_child->block->x = node->block->x;
        right_child->block->y = this->contour.get_y_coord(right_child->block);
        this->w = std::max(this->w, right_child->block->x + right_child->block->w);
        this->h = std::max(this->h, right_child->block->y + right_child->block->h);
        get_coord(right_child);
    }
}

void BSTree::detach_node(BSTreeNode *node) {

    bool is_left_child = (node->parent->left == node);
    BSTreeNode *parent = node->parent;
    if (!node->left && !node->right) {
        // No child
        if (is_left_child) {
            parent->left = nullptr;
        } else {
            parent->right = nullptr;
        }
        if (this->space_one_nodes.count(node->parent)) {
            this->space_one_nodes.erase(node->parent);
            this->space_two_nodes.insert(node->parent);
        } else {
            this->space_one_nodes.insert(node->parent);
        }
        this->space_two_nodes.erase(node);
    } else if ((!node->left && node->right) || (!node->right && node->left)) {
        logger.debug("\tOne child\n");
        BSTreeNode *child = node->left ? node->left : node->right;
        // One child
        if (is_left_child) {
            parent->left = child;
        } else {
            parent->right = child;
        }
        child->parent = parent;
        this->space_one_nodes.erase(node);
        if (node == root) root = child;
    } else {
        logger.debug("\tTwo child\n");
        // Two children: repeatedly swap with its left child until meeting a node with only one child
        auto replace_node_iter = this->space_two_nodes.begin();
        while ((*replace_node_iter)->parent == node) replace_node_iter++;
        BSTreeNode *replace_node = *replace_node_iter;
        // std::cout << replace_node->block->name << '\n';
        this->detach_node(replace_node);
        if (node == root) root = replace_node;
        replace_node->left  = node->left,  node->left->parent  = replace_node;
        replace_node->right = node->right, node->right->parent = replace_node;
        replace_node->parent = node->parent;
        if (is_left_child) {
            parent->left = replace_node;
        } else {
            parent->right = replace_node;
        }
    }
    node->left = node->right = node->parent = nullptr;
}

BSTreeNode* BSTree::sample_space_node() {
    // printf("Sampling space node\n");
    BSTreeNode *sampled_space_node;
    int num_space_nodes = this->space_one_nodes.size() + this->space_two_nodes.size();
    int random_number = intRand(0, num_space_nodes-1);
    // randoms.push_back(random_number);
    // int index = random_number % num_space_nodes;
    // Randomly choose which set to sample
    // std::cout << "Num space nodes: " << num_space_nodes << '\n';
    // std::cout << "Index: " << index << " space_one_nodes_size: " << this->space_one_nodes.size() << '\n';
    auto& chosen_set = (random_number >= this->space_one_nodes.size()) ? this->space_two_nodes : this->space_one_nodes;
    // Insert a random pointer. Get its iterator, and increment it to get the sample.
    // printf("Probing\n");
    auto it = chosen_set.begin();
    // index %= this->space_one_nodes.size();
    std::advance(it, intRand(0, chosen_set.size()-1));
    return *it;
}
// BSTreeNode* BSTree::sample_space_node() {
//     // printf("Sampling space node\n");
//     BSTreeNode *sampled_space_node;
//     int num_space_nodes = this->space_one_nodes.size() + this->space_two_nodes.size();
//     int random_number = intRand();
//     // randoms.push_back(random_number);
//     int index = random_number % num_space_nodes;
//     // Randomly choose which set to sample
//     // std::cout << "Num space nodes: " << num_space_nodes << '\n';
//     // std::cout << "Index: " << index << " space_one_nodes_size: " << this->space_one_nodes.size() << '\n';
//     auto& chosen_set = (index >= this->space_one_nodes.size()) ? this->space_two_nodes : this->space_one_nodes;
//     // Insert a random pointer. Get its iterator, and increment it to get the sample.
//     // printf("Probing\n");
//     BSTreeNode *probe_pointer = (BSTreeNode *)(random_number);
//     auto probe_iterator = chosen_set.insert(probe_pointer).first;
//     auto temp_iterator = probe_iterator;
//     temp_iterator++;
    
//     sampled_space_node = (temp_iterator == chosen_set.end()) ? *chosen_set.begin() : *temp_iterator;
//     chosen_set.erase(probe_iterator);
//     return sampled_space_node;
// }
BSTree* BSTree::find_neighbor() {

}
int BSTree::perturb() {
    int random_number1 = intRand(), random_number2 = intRand();
    // randoms.push_back(random_number1);
    // randoms.push_back(random_number2);
    int chosen_perturbation = random_number1 % 3,
        chosen_block_a_id = random_number2 % this->blocks.size();
    // chosen_perturbation = 1;
    logger.debug("Perturbing tree, operation: %d\n", chosen_perturbation);
    Block *block_a = &(this->blocks[chosen_block_a_id]);
    Block *block_b;
    // std::cout << "Chosen operation: " << chosen_perturbation << '\n';
    if (this->block_to_node[block_a] == nullptr) {
        throw std::runtime_error("Error, block to node map is incorrect\n");
    }

    switch(chosen_perturbation) {
        case 0:
            this->rotate_module(block_a);
            break;
        case 1:
            this->random_move_module(block_a);
            break;
        case 2:
            int random_number2 = intRand(),
                chosen_block_b_id = random_number2 % this->blocks.size(); 
            // randoms.push_back(random_number2);
            block_b = &(this->blocks[chosen_block_b_id]);
            // Ensure block_b is different from block_a
            while (block_a == block_b) {
                random_number2 = intRand();
                chosen_block_b_id = random_number2 % this->blocks.size(); 
                // randoms.push_back(random_number2);
                block_b = &(this->blocks[chosen_block_b_id]);
            } 
            this->swap_module(block_a, block_b);
            break;
    }
    last_perturbation = chosen_perturbation;
    last_perturb_a = block_a, last_perturb_b = block_b;
    return chosen_perturbation;
}
void BSTree::rotate_module(Block *block) {
    block->rotate();
}
void BSTree::random_move_module(Block *block) {
    logger.debug("Random move module: %s\n", block->name.c_str());
    // delete and re-insert node
    BSTreeNode *node = this->block_to_node[block];
    // delete
    logger.debug("\tdetaching node\n");
    this->detach_node(node);
    // re-insert, choose a node with a empty child
    logger.debug("\tdetach finish\n");
    BSTreeNode *sampled_space_node = sample_space_node();
    if (!sampled_space_node->left) {
        sampled_space_node->left = node;
    } else if (!sampled_space_node->right) {
        sampled_space_node->right = node;
    } else {
        throw std::runtime_error("ERROR, sampled node does not have space\n"); // Raise an exception
    }
    if (this->space_one_nodes.count(sampled_space_node)) {
        this->space_one_nodes.erase(sampled_space_node);
    } else {
        this->space_two_nodes.erase(sampled_space_node);
        this->space_one_nodes.insert(sampled_space_node);
    }
    this->space_two_nodes.insert(node);
    node->parent = sampled_space_node;
    logger.debug("Random move finish\n");
}
void BSTree::swap_module(Block *block_a, Block *block_b) {
    logger.debug("Swapping module: %s, %s\n", block_a->name.c_str(), block_b->name.c_str());
    BSTreeNode *node_a = this->block_to_node[block_a],
               *node_b = this->block_to_node[block_b];
    this->block_to_node[block_a] = node_b;
    this->block_to_node[block_b] = node_a;
    std::swap(node_a->block, node_b->block);
    logger.debug("Swap finish\n");
}

int cnt = 0;
void BSTree::print_tree() const {
    cnt = 0;
    this->__print_tree(root);
    printf("Tree size: %d\n", cnt);
}
void BSTree::__print_tree(BSTreeNode *node) const {
    if (!node) {
        return;
    }
    this->__print_tree(node->left);
    // std::cout << node->block->name << ' ' << node->block->x << ' ' << node->block->y << '\n';
    printf("Block name: %-6s, width: %4d, height: %4d, (x, y): (%4d, %4d)\n", node->block->name.c_str(), node->block->w, node->block->h, node->block->x, node->block->y);
    cnt++;
    this->__print_tree(node->right);
}