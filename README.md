This project was created to help deal with my procrastination. Its a scheduling program where the user enters tasks. And then each day the tasks are ordered with the least amount of time spent having the highest priority and the task with the most time spent on it having the lowest priority.

the task times are then reset every week.

Compile
------------
gcc -o schedule schedule.c

How to use:
---------------
command: tasks - outputs all the tasks to the screen
command: add - once typed will prompt user to enter name of task they want to add.
command: delete - once typed will prompt user to enter name of task they want to delete.
command: exit - exits the program.
command: start - starts the scheduler


Issues (needed to be fixed)
--------------
-only a certain number of tasks are allowed, as hanging occurs in the array.

-if program hangs when starting schedule it seems to happen because user presses 'n' if the time of
the task is 0. A temp fix is to do just start the task and after a second type 'y' for finish.
This stops the hanging from occuring. if you didn't want to start the task.

However the array can also hang randomly, so if so, try restarting the program several times.and like
i said it can hang after a certain number of tasks.

**These issues will be fixed shortly**

