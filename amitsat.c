#include "amitsat.h"
#include <stdio.h>
#include <stdlib.h>
#define bool int
#define true 1
#define false 0

static int sol_val(int clause_val, int* sol);
static void set_sol(int clause_val, int* sol);
static int choose_permute_index(int** clauses, int* lengths, int* sol, bool* resolved, int sol_len, int num_clauses);
static int* CSAT(long** clauses, int* lengths, long* sol, bool* resolved, int sol_len, int num_clauses);


static int sol_val(int clause_val, int* sol) {
    return sol[abs(clause_val) - 1];
}

static void set_sol(int clause_val, int* sol) {
    sol[abs(clause_val) - 1] = clause_val;
}

static int choose_permute_index(int** clauses, int* lengths, int* sol, bool* resolved, int sol_len, int num_clauses) {
    for (int i = 0; i < sol_len; i ++) {
        if (sol[i] == 0) {
            return i;
        }
    }
}

static int* CSAT(long** clauses, int* lengths, long* sol, bool* resolved, int sol_len, int num_clauses) {
    //checking if any clauses have been violated
    bool all_false, all_true;
    int num_found, index, eval, clause_num;
    long* result;
    all_true = true;
    bool* new_res = malloc(sizeof(bool) * num_clauses);
    memcpy(new_res, resolved, sizeof(bool) * num_clauses);
    //printf("started num_clauses:%d\n", num_clauses);
    for (int i = 0; i < num_clauses; i++) {
        all_false = true;
        num_found = 0;
        index = -1;
        clause_num = i;
        if (!new_res[i]){
            all_true = false;
            for (int j = 0; j < lengths[i]; j++) {
                eval = clauses[i][j] * sol_val(clauses[i][j], sol);
                //printf("made it to eval %d %d %d\n", eval, clauses[i][j], sol_val(clauses[i][j], sol));
                if (eval > 0) {
                    new_res[i] = true;
                    result = CSAT(clauses, lengths, sol, new_res, sol_len, num_clauses);
                    free(new_res);
                    return result;
                }
                if (eval == 0) {
                    all_false = false;
                    num_found++;
                    index = j;
                }
            }
            if (!new_res[i]) {
                if (all_false) {
                    free(new_res);
                    return NULL;
                }
                if (num_found == 1) {
                    break;
                }
            }    
        }
    }

    if (all_true) {
        //printf("spot1\n");
        long* new_sol = malloc(sol_len * sizeof(long));
        memcpy(new_sol, sol, sol_len * sizeof(long));
        free(new_res);
        return new_sol;
    }

    // unit and alone clauses
    if (num_found == 1) {
        //printf("spot2\n");
        long* new_sol = malloc(sol_len * sizeof(long));
        memcpy(new_sol, sol, sol_len * sizeof(long));
        set_sol(clauses[clause_num][index], new_sol);
        result = CSAT(clauses, lengths, new_sol, new_res, sol_len, num_clauses);
        free(new_sol);
        free(new_res);
        return result;
    }

    int first_unresolved = choose_permute_index(clauses, lengths, sol, new_res, sol_len, num_clauses);
    long* new_sol = malloc(sol_len * sizeof(long));
    memcpy(new_sol, sol, sol_len * sizeof(long));
    set_sol(first_unresolved + 1, new_sol);
    result = CSAT(clauses, lengths, new_sol, new_res, sol_len, num_clauses);
    if (result != NULL) {
        //printf("spot3\n");
        free(new_sol);
        free(new_res);
        return result;
    } else {
        //printf("spot4\n");
        set_sol(-(first_unresolved + 1), new_sol);
        result = CSAT(clauses, lengths, new_sol, new_res, sol_len, num_clauses);
        free(new_sol);
        free(new_res);
        return result;
    }

}

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
    int* lengths = malloc(length * sizeof(int *));
    PyObject *holder;
    int shrunk = 0;
    int maximum = 0;
    for (int i = 0; i < length; i++) {
        holder = PyList_GetItem(lst, i);
        int clause_size = PyList_Size(holder);
        if (clause_size == 0) {
            shrunk = 1;
            i--;
            length--;
        } else {
            lengths[i] = clause_size;
            clauses[i] = malloc(clause_size * sizeof(int));
            for (int j = 0; j < clause_size; j++) {
                clauses[i][j] = PyLong_AsLong(PyList_GetItem(holder, j));
                if (abs(clauses[i][j]) > maximum) {
                    maximum = abs(clauses[i][j]);
                }
                //printf("%d\n", clauses[i][j]);
            }
            //printf("\n");
        }
    }
    

    if (shrunk == 1) {
        //malloc check here
        clauses = realloc(clauses, (length) * sizeof(long *));
        lengths = realloc(lengths, length * sizeof(int *));
    }

    // solve the problem
    bool* resolved = malloc(sizeof(bool) * length);
    long* sol = malloc(sizeof(long) * (maximum));
    memset(resolved, false, sizeof(bool) * length);
    memset(sol, 0, sizeof(long) * (maximum));

    long* answer = CSAT(clauses, lengths, sol, resolved, maximum, length);

    for (int i = 0; i < length; i++) {
        free(clauses[i]);
    } 
    free(clauses);
    free(lengths);
    free(resolved);
    free(sol);

    PyObject* RV;
    if (answer != NULL) {
        RV = PyList_New(maximum);
        for (int i = 0; i < maximum; i++) {
            //printf("%d\n", answer[i]);
            if (answer[i] == 0) {
                answer[i] = i + 1;
            }
            PyList_SetItem(RV, i, PyLong_FromLong(answer[i]));
        }
    } else {
        RV = PyList_New(0);
    }

    return RV;


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
