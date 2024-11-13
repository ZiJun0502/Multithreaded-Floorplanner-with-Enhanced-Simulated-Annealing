#ifndef CONTOUR_H
#define CONTOUR_H
#include "Block.h"
#define INT_MAX 2147482647
class LineSegment {
public:
    int x0, y0, x1, y1;
    LineSegment (int x0, int y0, int x1, int y1):
        x0(x0), y0(y0), x1(x1), y1(y1) {}
    LineSegment (std::pair<int,int> p0, std::pair<int,int> p1):
        x0(p0.first), y0(p0.second), x1(p1.first), y1(p1.second) {}
    LineSegment(const LineSegment &other) {
        x0 = other.x0;
        y0 = other.y0;
        x1 = other.x1;
        y1 = other.y1;
    }
    LineSegment& operator=(const LineSegment &other) {
        if (this == &other) {
            return *this;  // Check for self-assignment
        }

        // Copy the values
        x0 = other.x0;
        y0 = other.y0;
        x1 = other.x1;
        y1 = other.y1;

        return *this;
    }
    ~LineSegment () {
    }
};
class Contour {
public:
    std::map<int, LineSegment*> contour;
    Contour () {
        contour[0] = new LineSegment(0, 0, INT_MAX, 0);
    }
    Contour(Contour &other) {
        for (const auto& entry : other.contour) {
            contour[entry.first] = new LineSegment(*entry.second);  // Deep copy of LineSegment
        }
    }
    Contour& operator=(const Contour &other) {
        if (this == &other) {
            return *this;  // Check for self-assignment
        }

        // Clean up current resources
        for (auto& entry : contour) {
            delete entry.second;  // Free memory of current LineSegments
        }
        contour.clear();  // Clear the map

        // Deep copy from other
        for (const auto& entry : other.contour) {
            contour[entry.first] = new LineSegment(*entry.second);  // Deep copy each LineSegment
        }

        return *this;
    }
    ~Contour() {
        for (auto& entry : contour) {
            delete entry.second;
        }
        logger.debug("Destroyed contour\n");
    }
    void reset_contour() {
        for (auto& pair : contour) {
            delete pair.second; // Free the LineSegment memory
        }
        contour.clear(); // Clear the map
        contour[0] = new LineSegment(0, 0, INT_MAX, 0);
    }
    // Find segments within the specified range
    std::vector<LineSegment*> find_segments_in_range(int rangeStart, int rangeEnd) {
        std::vector<LineSegment*> result;

        // Use the lower_bound to find starting position
        auto lower = --contour.upper_bound(rangeStart);
        auto upper = contour.lower_bound(rangeEnd);

        // Iterate through the segments in the range
        for (auto it = lower; it != upper; ++it) {
            LineSegment *line = it->second;
            // Check if the segment intersects with the range
            if (line->x0 <= rangeEnd && line->x1 >= rangeStart) {
                result.push_back(line);
            }
        }

        return result;
    }
    void merge_lineseg(LineSegment *a, LineSegment *b) {
        // printf("Merging line1: (%4d, %4d), (%4d, %4d)\n", a->x0, a->y0, a->x1, a->y1);
        // printf("        line2: (%4d, %4d), (%4d, %4d)\n", b->x0, b->y0, b->x1, b->y1);
        // a should be to the left of b
        if (a->x0 == b->x1) std::swap(a, b);
        if (!(a->x1 == b->x0 && a->y1 == b->y0)) return;
        a->x1 = b->x1;
        a->y1 = b->y1;
        this->contour.erase(b->x0);
        delete b;
    }
    void try_merge_left(LineSegment *line) {
        auto it = this->contour.find(line->x0); 
        if (it != this->contour.begin()) {
            --it;
            this->merge_lineseg(line, it->second);
        }
    }
    int get_y_coord(Block *blk) {
        // logger.debug("Getting y coord for: %s\n", blk->name.c_str());
        std::vector<LineSegment*> lines = this->find_segments_in_range(blk->x, blk->x + blk->w);
        int y = -1;
        for (LineSegment *line : lines) {
            y = std::max(y, line->y0);
            // std::cout << "In range: " << line->x0 << ' ' << line->x1 << '\n';
        }
        int contour_y = y + blk->h;
        // update contour
        for (LineSegment *line : lines) {
            // whole block is within the line segment
            if (line->x0 < blk->x && line->x1 > blk->x + blk->w) {
                LineSegment *new_line0 = new LineSegment(blk->x, contour_y, blk->x + blk->w, contour_y),
                            *new_line1 = new LineSegment(blk->x + blk->w, line->y0, line->x1, line->y0);
                line->x1 = blk->x;
                this->contour[blk->x] = new_line0;
                this->contour[blk->x + blk->w] = new_line1;
            // block is overlaping at the right side of line
            } else if (line->x0 < blk->x) {
                int x1 = line->x1;
                line->x1 = blk->x;
                LineSegment* new_line = new LineSegment(blk->x, contour_y, x1, contour_y);
                this->contour[blk->x] = new_line;
            // block is overlaping at the left side of line
            } else if (line->x1 > blk->x + blk->w) {
                int x0 = line->x0;
                line->x0 = blk->x + blk->w;
                LineSegment* new_line = new LineSegment(x0, contour_y, blk->x + blk->w, contour_y);
                this->contour[blk->x + blk->w] = line;
                this->contour[x0] = new_line;
                this->try_merge_left(new_line);
            // line segment is within block
            } else {
                line->y0 = line->y1 = contour_y;
                this->try_merge_left(line);
            }
        }
        return y;
    }
};

#endif