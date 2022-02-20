#include "amitsat.h"
#include <stdio.h>

static PyObject *solve(PyObject* self, PyObject* args) {
    PyObject* lst;
    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &lst)) {
        printf("Parsing Failed\n");
        return NULL;
    }
    if (!PyList_Check(lst)) {
        printf("Input Must be a List\n");
        return NULL;
    }
    int length = PyList_Size(lst);
    int real_length = 0;
    if (length == 0) {
        return PyList_New(0);
    }
    long **clauses = malloc(length * sizeof(long *));
    PyObject *holder;
    int shrunk = 0;
    for (int i = 0; i < length; i++) {
        holder = PyList_GetItem(lst, i);
        int clause_size = PyList_Size(holder);
        if (clause_size == 0) {
            shrunk = 1;
            i--;
            length--;
        } else {
            clauses[i] = malloc(clause_size * sizeof(int));
            for (int j = 0; j < clause_size; j++) {
                clauses[i][j] = PyLong_AsLong(PyList_GetItem(holder, j));
                printf("%d\n", clauses[i][j]);
            }
            printf("\n");
        }
        Py_DECREF(holder);
    }
    Py_DECREF(lst);

    if (shrunk == 1) {
        //malloc check here
        clauses = realloc(clauses, (length) * sizeof(long *));
    }

    for (int i = 0; i < length; i++) {
        free(clauses[i]);
    } 
    free(clauses);
    return PyUnicode_FromString("Poggies");
    // solve the problem


}

static struct PyMethodDef methods[] = {
    {"solve", (PyCFunction)solve, METH_VARARGS},
    {NULL, NULL, NULL}
};

static struct PyModuleDef module =
{
    PyModuleDef_HEAD_INIT,
    "amisat",
    NULL,
    -1,
    methods
};

PyMODINIT_FUNC PyInit_amitsat(void) {
    return PyModule_Create(&module);
}
