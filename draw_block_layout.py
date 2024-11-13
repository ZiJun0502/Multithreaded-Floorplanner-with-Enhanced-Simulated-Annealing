import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import math

def draw_block(ax, x0, y0, x1, y1, id):
    width = x1 - x0
    height = y1 - y0
    if id > 0:
        color = "#FFCCCC"
    else:
        color = "#BBBBBB"

    ax.add_patch(
        patches.Rectangle( 
            (x0, y0),
            width,
            height,
            fill=True,
            edgecolor="#000000",
            facecolor=color,
            alpha=1.0  # original was 0.3
        )
    )

# Read the draw.txt file
txt_name = sys.argv[1] 
png_name = sys.argv[2] 

with open(txt_name, 'r') as fread:
    f = fread.read().split("\n")

# First line: width and height of the entire window
window_width, window_height = map(int, f[0].split(" "))

# Second line: outline dimensions (outline_w, outline_h)
outline_w, outline_h = map(int, f[1].split(" "))

# Third line: number of blocks
num_blocks = int(f[2])

# Set up the plot
fig = plt.figure(figsize=(16,12))
ax = fig.add_subplot(111)
ax.set_xbound(0, window_width)
ax.set_ybound(0, window_height)

max_w = max(outline_w, window_width)
max_h = max(outline_h, window_height)
ax.set_xlim(0, max_w)
ax.set_ylim(0, max_h)
ax.set_aspect('equal')

# Draw a notable outline rectangle
ax.add_patch(
    patches.Rectangle(
        (0, 0),  # Starting from origin
        outline_w,
        outline_h,
        fill=False,
        edgecolor="#FF0000",  # Red outline for visibility
        linewidth=2,
        linestyle='--'  # Dashed line to make it notable
    )
)
# Loop over the blocks
for i in range(3, 3 + num_blocks):
    block_data = f[i].split(" ")
    block_name = block_data[0]
    x0, y0, x1, y1 = map(int, block_data[1:])
    draw_block(ax, x0, y0, x1, y1, i - 2)  # Use i-2 as id for coloring

# Save the plot to a PNG file
plt.savefig(png_name)
