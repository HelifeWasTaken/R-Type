#!/usr/bin/env python3
import random

def random_points(offset_x, offset_y, width, height, nb):
    global points_list
    min_x = offset_x
    min_y = offset_y
    max_x = min_x + width
    max_y = min_y + height
    for i in range(nb):
        print("[", random.uniform(min_x, max_x), ",", random.uniform(min_y, max_y), "],")

random_points(3550, 50, 384, 200, 10)

