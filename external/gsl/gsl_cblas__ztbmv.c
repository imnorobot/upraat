#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_ztbmv (const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
             const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
             const int N, const int K, const void *A, const int lda, void *X,
             const int incX)
{
#define BASE double
#include "gsl_cblas__source_tbmv_c.h"
#undef BASE
}
