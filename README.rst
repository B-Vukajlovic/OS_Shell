Implements a basic shell, which allows users to execute
commands and manage processes. It supports built-in
commands like 'exit', 'cd', and 'pwd', as well as external
commands executed via fork and execvp. The shell can handle
pipelines, command sequences, and process detachment for background
execution.

Example commands
================

.. code:: sh

   ls
   sleep 5

.. code:: sh

   mkdir t
   cd t
   /bin/pwd
   exit 42

.. code:: sh

   ## sequences:
   echo hello; echo world
   exit 0; echo fail

.. code:: sh

   ## pipes:
   ls | grep t
   ls | more
   ls | sleep 5
   sleep 5 | ls
   ls /usr/lib | grep net | cut -d. -f1 | sort -u