#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#define EPS 1e-9

double calc_norm(gsl_matrix* M, gsl_vector_int* types, gsl_vector* ansnum) {
    int n = types->size;
    int m = gsl_vector_int_max(types) + 1;

    gsl_vector* typesum = gsl_vector_calloc(m);
    gsl_matrix* W = gsl_matrix_calloc(m, n);
    gsl_matrix* L = gsl_matrix_calloc(m, m);
    gsl_matrix* WT = gsl_matrix_calloc(n, m);
    gsl_matrix* M_copy = gsl_matrix_alloc(n, n);
    gsl_matrix_memcpy(M_copy, M);

    for (int i = 0; i < n; i++) {
        double val = gsl_vector_get(ansnum, i);
        gsl_vector_set(typesum, gsl_vector_int_get(types, i), gsl_vector_get(typesum, gsl_vector_int_get(types, i)) + val * val);
    }

    for (int i = 0; i < n; i++) {
        double denom = gsl_vector_get(typesum, gsl_vector_int_get(types, i));
        if (denom != 0) {
            gsl_matrix_set(W, gsl_vector_int_get(types, i), i, sqrt(gsl_vector_get(ansnum, i) * gsl_vector_get(ansnum, i) / denom));
        }
    }

    gsl_matrix_transpose_memcpy(WT, W);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, W, M, 0.0, L);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, L, W, 0.0, M);

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < i; j++) {
            gsl_matrix_set(L, i, j, 0);
        }
    }

    gsl_matrix_sub(M_copy, M);
    double norm = gsl_matrix_frobenius_norm(M_copy);

    gsl_vector_free(typesum);
    gsl_matrix_free(W);
    gsl_matrix_free(L);
    gsl_matrix_free(WT);
    gsl_matrix_free(M_copy);

    return norm;
}

gsl_vector_int* calc_order(gsl_vector_int* types, gsl_vector* ansnum) {
    int n = types->size;
    gsl_vector_int* mytypes = gsl_vector_int_alloc(n);
    gsl_vector_int_memcpy(mytypes, types);
    gsl_vector_int* res = gsl_vector_int_calloc(n);

    for (int i = 0; i < n; i++) {
        int cur = 0;
        for (int j = 0; j < n; j++) {
            if ((gsl_vector_int_get(mytypes, j) < gsl_vector_int_get(mytypes, cur)) || ((gsl_vector_int_get(mytypes, j) == gsl_vector_int_get(mytypes, cur)) && (gsl_vector_get(ansnum, j) > gsl_vector_get(ansnum, cur)))) {
                cur = j;
            }
        }
        gsl_vector_int_set(mytypes, cur, n + 1);
        gsl_vector_int_set(res, i, cur);
    }

    gsl_vector_int_free(mytypes);

    return res;
}

void answer_rank_default(gsl_matrix* M, gsl_vector* ansnum, char* normalize, gsl_vector_int** order_out) {
    int n = M->size1;
    if (normalize != NULL && strcmp(normalize, "all") == 0) {
        double sum = gsl_matrix_sum(M);
        gsl_matrix_scale(M, 1.0 / sum);
    }
    if (ansnum == NULL) {
        ansnum = gsl_vector_calloc(n);
        for (int i = 0; i < n; i++) {
            gsl_vector_set(ansnum, i, gsl_matrix_get(M, i, i));
        }
    }
    double* opt_uppersum = calloc((1 << n), sizeof(double));
    int* opt_last = calloc((1 << n), sizeof(int));
    gsl_vector_int* order = gsl_vector_int_calloc(n);

    for (int i = 1; i < (1 << n); i++) {
        opt_uppersum[i] = -1;
        for (int j = n - 1; j >= 0; j--) {
            if ((i >> j) & 1 == 1) {
                int k = i ^ (1 << j);
                double uppersum = opt_uppersum[k];
                while (k != 0) {
                    uppersum += gsl_matrix_get(M, j, __builtin_ctz(k)) * gsl_matrix_get(M, j, __builtin_ctz(k));
                    k -= k & -k;
                }
                if (uppersum > opt_uppersum[i] + EPS || ((uppersum > opt_uppersum[i] - EPS) && gsl_vector_get(ansnum, j) > gsl_vector_get(ansnum, opt_last[i]))) {
                    opt_uppersum[i] = uppersum;
                    opt_last[i] = j;
                }
            }
        }
    }

    int k = (1 << n) - 1;
    int idx = 0;
    while (k != 0) {
        gsl_vector_int_set(order, idx++, opt_last[k]);
        k = k ^ (1 << opt_last[k]);
    }

    gsl_vector_int* types = calc_order(order, ansnum);
    double norm = calc_norm(M, types, ansnum);

    printf("Norm: %lf\n", norm);

    *order_out = order;

    free(opt_uppersum);
    free(opt_last);
    gsl_vector_int_free(types);
}

double calc_norm(double **M, int *types, int types_len, double *ansnum, int ansnum_len) {
    int n = types_len;
    int m = max(types, types_len) + 1;
    double *typesum = calloc(m, sizeof(double));
    for (int i = 0; i < n; i++) {
        typesum[types[i]] += ansnum[i] * ansnum[i];
    }
    double **W = malloc(m * sizeof(double *));
    for (int i = 0; i < m; i++) {
        W[i] = calloc(n, sizeof(double));
    }
    for (int i = 0; i < n; i++) {
        W[types[i]][i] = typesum[types[i]] != 0 ? sqrt(ansnum[i] * ansnum[i] / typesum[types[i]]) : 0;
    }
    double **L = matrix_dot_matrix(matrix_dot_matrix(W, m, n, M, n, n), m, n, matrix_transpose(W, m, n), n, m);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < i; j++) {
            L[i][j] = 0;
        }
    }
    double norm = matrix_frobenius_norm(matrix_sub_matrix(M, n, n, matrix_dot_matrix(matrix_dot_matrix(matrix_transpose(W, m, n), n, m, L, m, m), n, m, W, m, n), n, n));
    // remember to free allocated memory
    free(typesum);
    for (int i = 0; i < m; i++) {
        free(W[i]);
    }
    free(W);
    return norm;
}

void calc_order(int *types, int types_len, double *ansnum, int ansnum_len, int *res) {
    int *mytypes = malloc(types_len * sizeof(int));
    memcpy(mytypes, types, types_len * sizeof(int));
    int n = types_len;
    for (int i = 0; i < n; i++) {
        int cur = 0;
        for (int j = 0; j < n; j++) {
            if (mytypes[j] < mytypes[cur] || (mytypes[j] == mytypes[cur] && ansnum[j] > ansnum[cur])) {
                cur = j;
            }
        }
        mytypes[cur] = n + 1;
        res[i] = cur;
    }
    free(mytypes);
}

void permute_matrix(double **matrix, int n, int *permutation, double **res) {
    double **temp = malloc(n * sizeof(double *));
    for (int i = 0; i

void generate_permutations(int n, int ***perms, int *perms_len) {
    // generate all permutations of range(n) using some algorithm
    // update *perms and *perms_len
}

double*** find_optimal_permutations(double **matrix, int n, int *optimal_perms_len) {
    int **perms;
    int perms_len;
    generate_permutations(n, &perms, &perms_len);
    double max_sum_sq = 0;
    int optimal_perms_size = 1;
    int *optimal_perms = malloc(optimal_perms_size * sizeof(int *));
    int optimal_perms_num = 0;
    for (int i = 0; i < perms_len; i++) {
        double **perm_matrix;
        permute_matrix(matrix, n, perms[i], perm_matrix);
        double sum_sq = upper_tri_sum_sq(perm_matrix, n);
        if (sum_sq > max_sum_sq) {
            max_sum_sq = sum_sq;
            optimal_perms_num = 1;
            optimal_perms[0] = perms[i];
        } else if (sum_sq == max_sum_sq) {
            if (optimal_perms_num == optimal_perms_size) {
                optimal_perms_size *= 2;
                optimal_perms = realloc(optimal_perms, optimal_perms_size * sizeof(int *));
            }
            optimal_perms[optimal_perms_num++] = perms[i];
        }
    }
    double ***optimal_matrices = malloc(optimal_perms_num * sizeof(double **));
    for (int i = 0; i < optimal_perms_num; i++) {
        double **optimal_matrix;
        permute_matrix(matrix, n, optimal_perms[i], optimal_matrix);
        optimal_matrices[i] = optimal_matrix;
    }
    *optimal_perms_len = optimal_perms_num;
    // remember to free allocated memory
    free(optimal_perms);
    for (int i = 0; i < perms_len; i++) {
        free(perms[i]);
    }
    free(perms);
    return optimal_matrices;
}

int compare_diag(double ***matrix_list, int list_len, int n) {
    int *indices = malloc(list_len * sizeof(int));
    for (int i = 0; i < list_len; i++) {
        indices[i] = i;
    }
    for (int i = 0; i < n; i++) {
        double max_val = -1;
        for (int j = 0; j < list_len; j++) {
            if (matrix_list[j][i][i] > max_val) {
                max_val = matrix_list[j][i][i];
            }
        }
        int new_list_len = 0;
        double ***new_matrix_list = malloc(list_len * sizeof(double **));
        int *new_indices = malloc(list_len * sizeof(int));
        for (int j = 0; j < list_len; j++) {
            if (matrix_list[j][i][i] >= max_val) {
                new_matrix_list[new_list_len] = matrix_list[j];
                new_indices[new_list_len++] = indices[j];
            }
        }
        free(matrix_list);
        free(indices);
        matrix_list = new_matrix_list;
        indices = new_indices;
        list_len = new_list_len;
        if (list_len == 1) {
            int res = indices[0];
            free(indices);
            return res;
        }
    }
    int res = indices[0];
    free(indices);
    return res;
}


