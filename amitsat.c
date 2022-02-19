#include "amitsat.h"
#include <stdio.h>

PyObject *solve(PyObject* lst) {
    int length = PyList_Size(lst);
    if (length == 0) {
        return PyList_New(0);
    }
    long **clauses = malloc(length * sizeof(long *));
    PyObject *holder;
    for (int i = 0; i < length; i++) {
        holder = PyList_GetItem(lst, i);
        int clause_size = PyList_Size(holder);
        if (clause_size == 0) {
            //malloc check here
            clauses = realloc(clauses, (--length) * sizeof(long *))
            i--;
        } else {
            //malloc check here
            clauses[i] = malloc(clause_size * sizeof(int));
            for (int j = 0; j < clause_size; j++) {
                clauses[i][j] = PyLong_AsLong(PyList_GetItem(holder, j));
                printf("%d\n", clauses[i][j]);
            }
            printf("\n");
        }
    }
    return 0
    // solve the problem


}