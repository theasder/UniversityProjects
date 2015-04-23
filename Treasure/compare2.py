#!/usr/bin/python2

import sys

def overlap(rect1, rect2, th = 0.5):
    (x1, y1, width1, height1) = rect1
    (x2, y2, width2, height2) = rect2

    ov_x = max(x1, x2)
    ov_y = max(y1, y2)
    ov_x_end = min(x1 + width1, x2 + width2)
    ov_y_end = min(y1 + height1, y2 + height2)

    ov_area = (ov_x_end - ov_x + 1) * (ov_y_end - ov_y + 1)
    un_area = width1 * height1 + width2 * height2 - ov_area

    try:
        if ((ov_x_end - ov_x + 1 > 0) and (ov_y_end - ov_y + 1 > 0) and (ov_area / float(un_area) > th)):
            return True
    except ZeroDivisionError:
        return False

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

gt = None
result = None

try:
    gt = read_path(sys.argv[1])
    result = read_path(sys.argv[2])
except OSError:
    print 'FAIL'
    sys.exit(0)

if len(result) == 0:
    print 'FAIL'
    sys.exit(0)

if not overlap(gt[-1], result[-1]):
    print 'FAIL'
    sys.exit(0)

bonus_fail = False
if len(result) != len(gt):
    bonus_fail = True
else:
    for i in range(0, len(result)):
        if not overlap(gt[i], result[i]):
            bonus_fail = True

if bonus_fail:
    print 'BASE OK'
else:
    print 'BONUS OK'
