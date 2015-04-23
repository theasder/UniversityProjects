#!/usr/bin/python

import sys

def overlap(rect1, rect2):
    (x1, y1, width1, height1) = rect1
    (x2, y2, width2, height2) = rect2

    ov_x = max(x1, x2)
    ov_y = max(y1, y1)
    ov_x_end = min(x1 + width1, x2 + width2)
    ov_y_end = min(y1 + height1, y2 + height2)

    ov_area = (ov_x_end - ov_x + 1) * (ov_y_end - ov_y + 1)

    if (ov_area > 0 and
        ov_area / float(min(width1 * height1, width2 * height2)) > 0.5):
        return True

    return False

def read_path(filename):
    lines = open(filename, 'r').readlines()
    rects = []
    for line in lines:
        rect = []
        for i in line.split(' '):
            rect.append(int(i))
        rects.append(rect)
    return rects


if len(sys.argv) != 3:
    print 'Usage: %s <ground_truth.txt> <program_output.txt>' % sys.argv[0]
    sys.exit(0)

gt = read_path(sys.argv[1])
result = read_path(sys.argv[2])

if len(result) == 1 and overlap(gt[-1], result[0]):
    print 'BASE OK'
    sys.exit(0)

if len(result) != len(gt):
    print 'FAIL'

for i in range(0, len(result)):
    if not overlap(gt[i], result[i]):
        print 'FAIL'
        sys.exit(0)

print 'BONUS OK'
