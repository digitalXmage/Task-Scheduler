This project was created to help deal with my procrastination. Its a scheduling program where the user enters tasks. And then each day the tasks are ordered with the least amount of time spent having the highest priority and the task with the most time spent on it having the lowest priority.

the task times are then reset every week.

Compile & run Instructions (Make sure that both files are in the same directory)
------------
gcc -o schedule schedule.c

chmod u+x clear.sh

gcc ./schedule

How to use:
---------------
command: tasks - outputs all the tasks to the screen

command: add - once typed will prompt user to enter name of task they want to add.

command: delete - once typed will prompt user to enter name of task they want to delete.

command: exit - exits the program.

command: start - starts the scheduler

command: clear - clears the terminal

