# Lab 6: Debugging

## Tasks
- Read and run the first example and observe the error. Then build in debug mode, and run the debugger to find the problem.
- Read the second example, and come up with an assertion statement on `sum` that catches the bug.

## Description
This lab aims to practice two debugging techniques: 1) Using a debugger 2) Adding assertion statements.
Students also learn that they should enable debug symbols during the build process to make deubgger running possible.

The first example has a possible NULL pointer access for `prev` when inserting a value less than all items.
The students are expected to run the debugger to trace the segmentation fault, identify the problem and fix it.

The second example has a function bug in insertion where the newly-inserted node is incorrectly
connected to the one after the expected node, effectively causing an item to be dropped.
This is supposed to be difficult to be identified by manually debugging, thus we suggest adding assertions.
Again, as assertions for insertion operation is generally difficult, we suggest adding assertions over a
side property, sum of the items. Sum is calculated in two ways so the unintended item drop is detected.

Instructions and motivations behind each approach is expected to be discussed followed with a demonstration
of using the tools (e.g., `cgdb`).