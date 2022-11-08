
	My implementation of cat and grep linux utilities featuring regex.h

I've built this project for educational purposes while studying at School 21. Both cat and grep support many flags, including combinations of them with rules of placement based on the ones that are used in built-in analogs. Leaks utility made it easy to make sure that no memory leakage is possible.

Grep implementation allows for more than one expression being included in search.
(usage: grep -e [pattern] -f [file] -e[pattern] ... [file...])