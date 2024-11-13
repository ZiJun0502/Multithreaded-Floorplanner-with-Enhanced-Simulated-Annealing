# Multithreaded B* Tree Floorplanner
This project aims to provide a high-performance implementation of B*-tree, with enhanced simulated annealing.

Techniques used:
1. Reheat SA and continue running when the solution improves.
2. Dynamic adjustment of initial temperature.
3. Separate multithreaded approach to run SA, gathering the best solution at the end.

Followings are the papers I've referred to:
1. [B*-trees: a new representation for non-slicing floorplans](https://ieeexplore.ieee.org/document/855354)
2. [Computing the Initial Temperature of Simulated Annealing](https://www.researchgate.net/publication/227061666_Computing_the_Initial_Temperature_of_Simulated_Annealing)
3. [An improved Simulated Annealing algorithm based on ancient metallurgy techniques](https://www.sciencedirect.com/science/article/abs/pii/S1568494619305423)

## How to Run
```bash
make
./floorplanner [alpha value] input.block input.nets output.txt
```
The alpha value is provided to adjust the cost ratio between HPWL and area:
$$
Cost = \alpha\cdot Area + (1-\alpha)\cdot HPWL
$$
.block and .nets files can be placed in a folder named `folder_name`, then place this folder inside the testcases directory. After that, you can run:
```bash
./run `folder_name`
```
## Visualization
The Python script `draw_block_layout.py` can be used to visualize the output of a test case. Running a test case will generate a text file named `draw.txt`, which contains the data for visualization. To create the visualization image, run:
```bash
python draw_block_layout.py draw.txt draw.png
```