#! /usr/bin/env python

import cairo

from subprocess import run
from more_itertools import flatten

samples = []

output = run("./wav", capture_output=True).stdout.decode().split("\n")

samples = []

for line in output:
    if line.strip() != "":
        samples.append(list(map(lambda x: int(x), line.strip().split("\t"))))

xrange = len(samples)
yrange = max(flatten(samples))

xres = 10000
yres = 1000

scrsize_x = 1500
scrsize_y = 1000

print(xrange)
print(yrange)

print(xres)
print(yres)

print(scrsize_x)
print(scrsize_y)

yrange = yrange // yres
xrange = xrange // xres

print(xrange)
print(yrange)

with cairo.SVGSurface("test.svg", scrsize_x, scrsize_y) as surface:
    context = cairo.Context(surface)
    sx = int(scrsize_x / xrange)
    sy = int(scrsize_y / yrange)
    context.scale(sx, sy)
    context.set_source_rgb(0, 0, 0)
    context.set_line_width(0.1)
    context.move_to(0, 0)
    for x in range(xrange):
        sample = samples[x * xres][0]
        sample = int(sample / (yres * 2))
        context.line_to(x, ((sy / 2) - sample))
    context.stroke()
