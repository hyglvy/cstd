

/* Table of constant values */

static dcomplex c_b1 = {0.,0.};
static dcomplex c_b2 = {1.,0.};
static integer c__6 = 6;
static integer c__0 = 0;
static integer c__2 = 2;
static integer c__1 = 1;
static integer c_n1 = -1;

/* Subroutine */ static int zgesvd_(char *jobu, char *jobvt, integer *m, integer *n, 
        dcomplex *a, integer *lda, doublereal *s, dcomplex *u, 
        integer *ldu, dcomplex *vt, integer *ldvt, dcomplex *work, 
        integer *lwork, doublereal *rwork, integer *info, ftnlen jobu_len, 
        ftnlen jobvt_len)
{
    /* System generated locals */
    address a__1[2];
    integer a_dim1, a_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1[2], 
            i__2, i__3, i__4;
    char ch__1[2];

    /* Builtin functions */
    /* Subroutine */ static VOID s_cat(char *, char **, integer *, integer *, ftnlen);
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, ie, ir, iu, blk, ncu;
    static doublereal dum[1], eps;
    static integer nru;
    static dcomplex cdum[1];
    static integer iscl;
    static doublereal anrm;
    static integer ierr, itau, ncvt, nrvt;
     logical lsame_(char *, char *, ftnlen, ftnlen);
    static integer chunk, minmn;
     /* Subroutine */ int zgemm_(char *, char *, integer *, integer *, 
            integer *, dcomplex *, dcomplex *, integer *, 
            dcomplex *, integer *, dcomplex *, dcomplex *, 
            integer *, ftnlen, ftnlen);
    static integer wrkbl, itaup, itauq, mnthr, iwork;
    static logical wntua, wntva, wntun, wntuo, wntvn, wntvo, wntus, wntvs;
     doublereal dlamch_(char *, ftnlen);
     /* Subroutine */ int dlascl_(char *, integer *, integer *, 
            doublereal *, doublereal *, integer *, integer *, doublereal *, 
            integer *, integer *, ftnlen), xerbla_(char *, integer *, ftnlen),
             zgebrd_(integer *, integer *, dcomplex *, integer *, 
            doublereal *, doublereal *, dcomplex *, dcomplex *, 
            dcomplex *, integer *, integer *);
     integer ilaenv_(integer *, char *, char *, integer *, integer *, 
            integer *, integer *, ftnlen, ftnlen);
     doublereal zlange_(char *, integer *, integer *, dcomplex *, 
            integer *, doublereal *, ftnlen);
    static doublereal bignum;
     /* Subroutine */ int zgelqf_(integer *, integer *, dcomplex *,
             integer *, dcomplex *, dcomplex *, integer *, integer *
            ), zlascl_(char *, integer *, integer *, doublereal *, doublereal 
            *, integer *, integer *, dcomplex *, integer *, integer *, 
            ftnlen), zgeqrf_(integer *, integer *, dcomplex *, integer *,
             dcomplex *, dcomplex *, integer *, integer *), zlacpy_(
            char *, integer *, integer *, dcomplex *, integer *, 
            dcomplex *, integer *, ftnlen), zlaset_(char *, integer *, 
            integer *, dcomplex *, dcomplex *, dcomplex *, 
            integer *, ftnlen);
    static integer ldwrkr;
     /* Subroutine */ int zbdsqr_(char *, integer *, integer *, integer 
            *, integer *, doublereal *, doublereal *, dcomplex *, 
            integer *, dcomplex *, integer *, dcomplex *, integer *,
             doublereal *, integer *, ftnlen);
    static integer minwrk, ldwrku, maxwrk;
     /* Subroutine */ int zungbr_(char *, integer *, integer *, integer 
            *, dcomplex *, integer *, dcomplex *, dcomplex *, 
            integer *, integer *, ftnlen);
    static doublereal smlnum;
    static integer irwork;
     /* Subroutine */ int zunmbr_(char *, char *, char *, integer *, 
            integer *, integer *, dcomplex *, integer *, dcomplex *,
             dcomplex *, integer *, dcomplex *, integer *, integer *
            , ftnlen, ftnlen, ftnlen), zunglq_(integer *, integer *, integer *
            , dcomplex *, integer *, dcomplex *, dcomplex *, 
            integer *, integer *);
    static logical lquery, wntuas, wntvas;
     /* Subroutine */ int zungqr_(integer *, integer *, integer *, 
            dcomplex *, integer *, dcomplex *, dcomplex *, 
            integer *, integer *);


/*  -- LAPACK driver routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     October 31, 1999 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZGESVD computes the singular value decomposition (SVD) of a scomplex */
/*  M-by-N matrix A, optionally computing the left and/or right singular */
/*  vectors. The SVD is written */

/*       A = U * SIGMA * conjugate-transpose(V) */

/*  where SIGMA is an M-by-N matrix which is zero except for its */
/*  MIN(m,n) diagonal elements, U is an M-by-M unitary matrix, and */
/*  V is an N-by-N unitary matrix.  The diagonal elements of SIGMA */
/*  are the singular values of A; they are real and non-negative, and */
/*  are returned in descending order.  The first MIN(m,n) columns of */
/*  U and V are the left and right singular vectors of A. */

/*  Note that the routine returns V**H, not V. */

/*  Arguments */
/*  ========= */

/*  JOBU    (input) CHARACTER*1 */
/*          Specifies options for computing all or part of the matrix U: */
/*          = 'A':  all M columns of U are returned in array U: */
/*          = 'S':  the first MIN(m,n) columns of U (the left singular */
/*                  vectors) are returned in the array U; */
/*          = 'O':  the first MIN(m,n) columns of U (the left singular */
/*                  vectors) are overwritten on the array A; */
/*          = 'N':  no columns of U (no left singular vectors) are */
/*                  computed. */

/*  JOBVT   (input) CHARACTER*1 */
/*          Specifies options for computing all or part of the matrix */
/*          V**H: */
/*          = 'A':  all N rows of V**H are returned in the array VT; */
/*          = 'S':  the first MIN(m,n) rows of V**H (the right singular */
/*                  vectors) are returned in the array VT; */
/*          = 'O':  the first MIN(m,n) rows of V**H (the right singular */
/*                  vectors) are overwritten on the array A; */
/*          = 'N':  no rows of V**H (no right singular vectors) are */
/*                  computed. */

/*          JOBVT and JOBU cannot both be 'O'. */

/*  M       (input) INTEGER */
/*          The number of rows of the input matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the input matrix A.  N >= 0. */

/*  A       (input/output) COMPLEX*16 array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, */
/*          if JOBU = 'O',  A is overwritten with the first MIN(m,n) */
/*                          columns of U (the left singular vectors, */
/*                          stored columnwise); */
/*          if JOBVT = 'O', A is overwritten with the first MIN(m,n) */
/*                          rows of V**H (the right singular vectors, */
/*                          stored rowwise); */
/*          if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A */
/*                          are destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= MAX(1,M). */

/*  S       (output) DOUBLE PRECISION array, dimension (MIN(M,N)) */
/*          The singular values of A, sorted so that S(i) >= S(i+1). */

/*  U       (output) COMPLEX*16 array, dimension (LDU,UCOL) */
/*          (LDU,M) if JOBU = 'A' or (LDU,MIN(M,N)) if JOBU = 'S'. */
/*          If JOBU = 'A', U contains the M-by-M unitary matrix U; */
/*          if JOBU = 'S', U contains the first MIN(m,n) columns of U */
/*          (the left singular vectors, stored columnwise); */
/*          if JOBU = 'N' or 'O', U is not referenced. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= 1; if */
/*          JOBU = 'S' or 'A', LDU >= M. */

/*  VT      (output) COMPLEX*16 array, dimension (LDVT,N) */
/*          If JOBVT = 'A', VT contains the N-by-N unitary matrix */
/*          V**H; */
/*          if JOBVT = 'S', VT contains the first MIN(m,n) rows of */
/*          V**H (the right singular vectors, stored rowwise); */
/*          if JOBVT = 'N' or 'O', VT is not referenced. */

/*  LDVT    (input) INTEGER */
/*          The leading dimension of the array VT.  LDVT >= 1; if */
/*          JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= MIN(M,N). */

/*  WORK    (workspace/output) COMPLEX*16 array, dimension (LWORK) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= 1. */
/*          LWORK >=  2*MIN(M,N)+MAX(M,N). */
/*          For good performance, LWORK should generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  RWORK   (workspace) DOUBLE PRECISION array, dimension (5*MIN(M,N)) */
/*          On exit, if INFO > 0, RWORK(1:MIN(M,N)-1) contains the */
/*          unconverged superdiagonal elements of an upper bidiagonal */
/*          matrix B whose diagonal is in S (not necessarily sorted). */
/*          B satisfies A = U * B * VT, so it has the same singular */
/*          values as A, and singular vectors related by U and VT. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if ZBDSQR did not converge, INFO specifies how many */
/*                superdiagonals of an intermediate bidiagonal form B */
/*                did not converge to zero. See the description of RWORK */
/*                above for details. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --s;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    --work;
    --rwork;

    /* Function Body */
    *info = 0;
    minmn = MIN(*m,*n);
/* Writing concatenation */
    i__1[0] = 1, a__1[0] = jobu;
    i__1[1] = 1, a__1[1] = jobvt;
    s_cat(ch__1, a__1, i__1, &c__2, (ftnlen)2);
    mnthr = ilaenv_(&c__6, "ZGESVD", ch__1, m, n, &c__0, &c__0, (ftnlen)6, (
            ftnlen)2);
    wntua = lsame_(jobu, "A", (ftnlen)1, (ftnlen)1);
    wntus = lsame_(jobu, "S", (ftnlen)1, (ftnlen)1);
    wntuas = wntua || wntus;
    wntuo = lsame_(jobu, "O", (ftnlen)1, (ftnlen)1);
    wntun = lsame_(jobu, "N", (ftnlen)1, (ftnlen)1);
    wntva = lsame_(jobvt, "A", (ftnlen)1, (ftnlen)1);
    wntvs = lsame_(jobvt, "S", (ftnlen)1, (ftnlen)1);
    wntvas = wntva || wntvs;
    wntvo = lsame_(jobvt, "O", (ftnlen)1, (ftnlen)1);
    wntvn = lsame_(jobvt, "N", (ftnlen)1, (ftnlen)1);
    minwrk = 1;
    lquery = *lwork == -1;

    if (! (wntua || wntus || wntuo || wntun)) {
        *info = -1;
    } else if (! (wntva || wntvs || wntvo || wntvn) || wntvo && wntuo) {
        *info = -2;
    } else if (*m < 0) {
        *info = -3;
    } else if (*n < 0) {
        *info = -4;
    } else if (*lda < MAX(1,*m)) {
        *info = -6;
    } else if (*ldu < 1 || wntuas && *ldu < *m) {
        *info = -9;
    } else if (*ldvt < 1 || wntva && *ldvt < *n || wntvs && *ldvt < minmn) {
        *info = -11;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       CWorkspace refers to scomplex workspace, and RWorkspace to */
/*       real workspace. NB refers to the optimal block size for the */
/*       immediately following subroutine, as returned by ILAENV.) */

    if (*info == 0 && (*lwork >= 1 || lquery) && *m > 0 && *n > 0) {
        if (*m >= *n) {

/*           Space needed for ZBDSQR is BDSPAC = 5*N */

            if (*m >= mnthr) {
                if (wntun) {

/*                 Path 1 (M much larger than N, JOBU='N') */

                    maxwrk = *n + *n * ilaenv_(&c__1, "ZGEQRF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = maxwrk, i__3 = (*n << 1) + (*n << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    maxwrk = MAX(i__2,i__3);
                    if (wntvo || wntvas) {
/* Computing MAX */
                        i__2 = maxwrk, i__3 = (*n << 1) + (*n - 1) * ilaenv_(&
                                c__1, "ZUNGBR", "P", n, n, n, &c_n1, (ftnlen)
                                6, (ftnlen)1);
                        maxwrk = MAX(i__2,i__3);
                    }
                    minwrk = *n * 3;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntuo && wntvn) {

/*                 Path 2 (M much larger than N, JOBU='O', JOBVT='N') */

                    wrkbl = *n + *n * ilaenv_(&c__1, "ZGEQRF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "ZUNGQR", 
                            " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + *n * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = *n * *n + wrkbl, i__3 = *n * *n + *m * *n;
                    maxwrk = MAX(i__2,i__3);
                    minwrk = (*n << 1) + *m;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntuo && wntvas) {

/*                 Path 3 (M much larger than N, JOBU='O', JOBVT='S' or */
/*                 'A') */

                    wrkbl = *n + *n * ilaenv_(&c__1, "ZGEQRF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "ZUNGQR", 
                            " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + *n * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", n, n, n, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = *n * *n + wrkbl, i__3 = *n * *n + *m * *n;
                    maxwrk = MAX(i__2,i__3);
                    minwrk = (*n << 1) + *m;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntus && wntvn) {

/*                 Path 4 (M much larger than N, JOBU='S', JOBVT='N') */

                    wrkbl = *n + *n * ilaenv_(&c__1, "ZGEQRF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "ZUNGQR", 
                            " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + *n * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = *n * *n + wrkbl;
                    minwrk = (*n << 1) + *m;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntus && wntvo) {

/*                 Path 5 (M much larger than N, JOBU='S', JOBVT='O') */

                    wrkbl = *n + *n * ilaenv_(&c__1, "ZGEQRF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "ZUNGQR", 
                            " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + *n * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", n, n, n, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = (*n << 1) * *n + wrkbl;
                    minwrk = (*n << 1) + *m;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntus && wntvas) {

/*                 Path 6 (M much larger than N, JOBU='S', JOBVT='S' or */
/*                 'A') */

                    wrkbl = *n + *n * ilaenv_(&c__1, "ZGEQRF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "ZUNGQR", 
                            " ", m, n, n, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + *n * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", n, n, n, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = *n * *n + wrkbl;
                    minwrk = (*n << 1) + *m;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntua && wntvn) {

/*                 Path 7 (M much larger than N, JOBU='A', JOBVT='N') */

                    wrkbl = *n + *n * ilaenv_(&c__1, "ZGEQRF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *n + *m * ilaenv_(&c__1, "ZUNGQR", 
                            " ", m, m, n, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + *n * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = *n * *n + wrkbl;
                    minwrk = (*n << 1) + *m;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntua && wntvo) {

/*                 Path 8 (M much larger than N, JOBU='A', JOBVT='O') */

                    wrkbl = *n + *n * ilaenv_(&c__1, "ZGEQRF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *n + *m * ilaenv_(&c__1, "ZUNGQR", 
                            " ", m, m, n, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + *n * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", n, n, n, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = (*n << 1) * *n + wrkbl;
                    minwrk = (*n << 1) + *m;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntua && wntvas) {

/*                 Path 9 (M much larger than N, JOBU='A', JOBVT='S' or */
/*                 'A') */

                    wrkbl = *n + *n * ilaenv_(&c__1, "ZGEQRF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *n + *m * ilaenv_(&c__1, "ZUNGQR", 
                            " ", m, m, n, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", n, n, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + *n * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", n, n, n, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*n << 1) + (*n - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", n, n, n, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = *n * *n + wrkbl;
                    minwrk = (*n << 1) + *m;
                    maxwrk = MAX(minwrk,maxwrk);
                }
            } else {

/*              Path 10 (M at least N, but not much larger) */

                maxwrk = (*n << 1) + (*m + *n) * ilaenv_(&c__1, "ZGEBRD", 
                        " ", m, n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
                if (wntus || wntuo) {
/* Computing MAX */
                    i__2 = maxwrk, i__3 = (*n << 1) + *n * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", m, n, n, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    maxwrk = MAX(i__2,i__3);
                }
                if (wntua) {
/* Computing MAX */
                    i__2 = maxwrk, i__3 = (*n << 1) + *m * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", m, m, n, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    maxwrk = MAX(i__2,i__3);
                }
                if (! wntvn) {
/* Computing MAX */
                    i__2 = maxwrk, i__3 = (*n << 1) + (*n - 1) * ilaenv_(&
                            c__1, "ZUNGBR", "P", n, n, n, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    maxwrk = MAX(i__2,i__3);
                }
                minwrk = (*n << 1) + *m;
                maxwrk = MAX(minwrk,maxwrk);
            }
        } else {

/*           Space needed for ZBDSQR is BDSPAC = 5*M */

            if (*n >= mnthr) {
                if (wntvn) {

/*                 Path 1t(N much larger than M, JOBVT='N') */

                    maxwrk = *m + *m * ilaenv_(&c__1, "ZGELQF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = maxwrk, i__3 = (*m << 1) + (*m << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    maxwrk = MAX(i__2,i__3);
                    if (wntuo || wntuas) {
/* Computing MAX */
                        i__2 = maxwrk, i__3 = (*m << 1) + *m * ilaenv_(&c__1, 
                                "ZUNGBR", "Q", m, m, m, &c_n1, (ftnlen)6, (
                                ftnlen)1);
                        maxwrk = MAX(i__2,i__3);
                    }
                    minwrk = *m * 3;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntvo && wntun) {

/*                 Path 2t(N much larger than M, JOBU='N', JOBVT='O') */

                    wrkbl = *m + *m * ilaenv_(&c__1, "ZGELQF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "ZUNGLQ", 
                            " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", m, m, m, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = *m * *m + wrkbl, i__3 = *m * *m + *m * *n;
                    maxwrk = MAX(i__2,i__3);
                    minwrk = (*m << 1) + *n;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntvo && wntuas) {

/*                 Path 3t(N much larger than M, JOBU='S' or 'A', */
/*                 JOBVT='O') */

                    wrkbl = *m + *m * ilaenv_(&c__1, "ZGELQF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "ZUNGLQ", 
                            " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", m, m, m, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + *m * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = *m * *m + wrkbl, i__3 = *m * *m + *m * *n;
                    maxwrk = MAX(i__2,i__3);
                    minwrk = (*m << 1) + *n;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntvs && wntun) {

/*                 Path 4t(N much larger than M, JOBU='N', JOBVT='S') */

                    wrkbl = *m + *m * ilaenv_(&c__1, "ZGELQF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "ZUNGLQ", 
                            " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", m, m, m, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = *m * *m + wrkbl;
                    minwrk = (*m << 1) + *n;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntvs && wntuo) {

/*                 Path 5t(N much larger than M, JOBU='O', JOBVT='S') */

                    wrkbl = *m + *m * ilaenv_(&c__1, "ZGELQF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "ZUNGLQ", 
                            " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", m, m, m, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + *m * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = (*m << 1) * *m + wrkbl;
                    minwrk = (*m << 1) + *n;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntvs && wntuas) {

/*                 Path 6t(N much larger than M, JOBU='S' or 'A', */
/*                 JOBVT='S') */

                    wrkbl = *m + *m * ilaenv_(&c__1, "ZGELQF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "ZUNGLQ", 
                            " ", m, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", m, m, m, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + *m * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = *m * *m + wrkbl;
                    minwrk = (*m << 1) + *n;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntva && wntun) {

/*                 Path 7t(N much larger than M, JOBU='N', JOBVT='A') */

                    wrkbl = *m + *m * ilaenv_(&c__1, "ZGELQF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *m + *n * ilaenv_(&c__1, "ZUNGLQ", 
                            " ", n, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", m, m, m, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = *m * *m + wrkbl;
                    minwrk = (*m << 1) + *n;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntva && wntuo) {

/*                 Path 8t(N much larger than M, JOBU='O', JOBVT='A') */

                    wrkbl = *m + *m * ilaenv_(&c__1, "ZGELQF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *m + *n * ilaenv_(&c__1, "ZUNGLQ", 
                            " ", n, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", m, m, m, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + *m * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = (*m << 1) * *m + wrkbl;
                    minwrk = (*m << 1) + *n;
                    maxwrk = MAX(minwrk,maxwrk);
                } else if (wntva && wntuas) {

/*                 Path 9t(N much larger than M, JOBU='S' or 'A', */
/*                 JOBVT='A') */

                    wrkbl = *m + *m * ilaenv_(&c__1, "ZGELQF", " ", m, n, &
                            c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *m + *n * ilaenv_(&c__1, "ZUNGLQ", 
                            " ", n, n, m, &c_n1, (ftnlen)6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m << 1) * ilaenv_(&
                            c__1, "ZGEBRD", " ", m, m, &c_n1, &c_n1, (ftnlen)
                            6, (ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + (*m - 1) * ilaenv_(&c__1,
                             "ZUNGBR", "P", m, m, m, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    wrkbl = MAX(i__2,i__3);
/* Computing MAX */
                    i__2 = wrkbl, i__3 = (*m << 1) + *m * ilaenv_(&c__1, 
                            "ZUNGBR", "Q", m, m, m, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    wrkbl = MAX(i__2,i__3);
                    maxwrk = *m * *m + wrkbl;
                    minwrk = (*m << 1) + *n;
                    maxwrk = MAX(minwrk,maxwrk);
                }
            } else {

/*              Path 10t(N greater than M, but not much larger) */

                maxwrk = (*m << 1) + (*m + *n) * ilaenv_(&c__1, "ZGEBRD", 
                        " ", m, n, &c_n1, &c_n1, (ftnlen)6, (ftnlen)1);
                if (wntvs || wntvo) {
/* Computing MAX */
                    i__2 = maxwrk, i__3 = (*m << 1) + *m * ilaenv_(&c__1, 
                            "ZUNGBR", "P", m, n, m, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    maxwrk = MAX(i__2,i__3);
                }
                if (wntva) {
/* Computing MAX */
                    i__2 = maxwrk, i__3 = (*m << 1) + *n * ilaenv_(&c__1, 
                            "ZUNGBR", "P", n, n, m, &c_n1, (ftnlen)6, (ftnlen)
                            1);
                    maxwrk = MAX(i__2,i__3);
                }
                if (! wntun) {
/* Computing MAX */
                    i__2 = maxwrk, i__3 = (*m << 1) + (*m - 1) * ilaenv_(&
                            c__1, "ZUNGBR", "Q", m, m, m, &c_n1, (ftnlen)6, (
                            ftnlen)1);
                    maxwrk = MAX(i__2,i__3);
                }
                minwrk = (*m << 1) + *n;
                maxwrk = MAX(minwrk,maxwrk);
            }
        }
        work[1].r = (doublereal) maxwrk, work[1].i = 0.;
    }

    if (*lwork < minwrk && ! lquery) {
        *info = -13;
    }
    if (*info != 0) {
        i__2 = -(*info);
        xerbla_("ZGESVD", &i__2, (ftnlen)6);
        return 0;
    } else if (lquery) {
        return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
        if (*lwork >= 1) {
            work[1].r = 1., work[1].i = 0.;
        }
        return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P", (ftnlen)1);
    smlnum = sqrt(dlamch_("S", (ftnlen)1)) / eps;
    bignum = 1. / smlnum;

/*     Scale A if MAX element outside range [SMLNUM,BIGNUM] */

    anrm = zlange_("M", m, n, &a[a_offset], lda, dum, (ftnlen)1);
    iscl = 0;
    if (anrm > 0. && anrm < smlnum) {
        iscl = 1;
        zlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda, &
                ierr, (ftnlen)1);
    } else if (anrm > bignum) {
        iscl = 1;
        zlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda, &
                ierr, (ftnlen)1);
    }

    if (*m >= *n) {

/*        A has at least as many rows as columns. If A has sufficiently */
/*        more rows than columns, first reduce using the QR */
/*        decomposition (if sufficient workspace available) */

        if (*m >= mnthr) {

            if (wntun) {

/*              Path 1 (M much larger than N, JOBU='N') */
/*              No left singular vectors to be computed */

                itau = 1;
                iwork = itau + *n;

/*              Compute A=Q*R */
/*              (CWorkspace: need 2*N, prefer N+N*NB) */
/*              (RWorkspace: need 0) */

                i__2 = *lwork - iwork + 1;
                zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork], &
                        i__2, &ierr);

/*              Zero out below R */

                i__2 = *n - 1;
                i__3 = *n - 1;
                zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &a[a_dim1 + 2], lda, 
                        (ftnlen)1);
                ie = 1;
                itauq = 1;
                itaup = itauq + *n;
                iwork = itaup + *n;

/*              Bidiagonalize R in A */
/*              (CWorkspace: need 3*N, prefer 2*N+2*N*NB) */
/*              (RWorkspace: need N) */

                i__2 = *lwork - iwork + 1;
                zgebrd_(n, n, &a[a_offset], lda, &s[1], &rwork[ie], &work[
                        itauq], &work[itaup], &work[iwork], &i__2, &ierr);
                ncvt = 0;
                if (wntvo || wntvas) {

/*                 If right singular vectors desired, generate P'. */
/*                 (CWorkspace: need 3*N-1, prefer 2*N+(N-1)*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zungbr_("P", n, n, n, &a[a_offset], lda, &work[itaup], &
                            work[iwork], &i__2, &ierr, (ftnlen)1);
                    ncvt = *n;
                }
                irwork = ie + *n;

/*              Perform bidiagonal QR iteration, computing right */
/*              singular vectors of A in A if desired */
/*              (CWorkspace: 0) */
/*              (RWorkspace: need BDSPAC) */

                zbdsqr_("U", n, &ncvt, &c__0, &c__0, &s[1], &rwork[ie], &a[
                        a_offset], lda, cdum, &c__1, cdum, &c__1, &rwork[
                        irwork], info, (ftnlen)1);

/*              If right singular vectors desired in VT, copy them there */

                if (wntvas) {
                    zlacpy_("F", n, n, &a[a_offset], lda, &vt[vt_offset], 
                            ldvt, (ftnlen)1);
                }

            } else if (wntuo && wntvn) {

/*              Path 2 (M much larger than N, JOBU='O', JOBVT='N') */
/*              N left singular vectors to be overwritten on A and */
/*              no right singular vectors to be computed */

                if (*lwork >= *n * *n + *n * 3) {

/*                 Sufficient workspace for a fast algorithm */

                    ir = 1;
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *lda * *n;
                    if (*lwork >= MAX(i__2,i__3) + *lda * *n) {

/*                    WORK(IU) is LDA by N, WORK(IR) is LDA by N */

                        ldwrku = *lda;
                        ldwrkr = *lda;
                    } else /* if(complicated condition) */ {
/* Computing MAX */
                        i__2 = wrkbl, i__3 = *lda * *n;
                        if (*lwork >= MAX(i__2,i__3) + *n * *n) {

/*                    WORK(IU) is LDA by N, WORK(IR) is N by N */

                            ldwrku = *lda;
                            ldwrkr = *n;
                        } else {

/*                    WORK(IU) is LDWRKU by N, WORK(IR) is N by N */

                            ldwrku = (*lwork - *n * *n) / *n;
                            ldwrkr = *n;
                        }
                    }
                    itau = ir + ldwrkr * *n;
                    iwork = itau + *n;

/*                 Compute A=Q*R */
/*                 (CWorkspace: need N*N+2*N, prefer N*N+N+N*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
                            , &i__2, &ierr);

/*                 Copy R to WORK(IR) and zero out below it */

                    zlacpy_("U", n, n, &a[a_offset], lda, &work[ir], &ldwrkr, 
                            (ftnlen)1);
                    i__2 = *n - 1;
                    i__3 = *n - 1;
                    zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &work[ir + 1], &
                            ldwrkr, (ftnlen)1);

/*                 Generate Q in A */
/*                 (CWorkspace: need N*N+2*N, prefer N*N+N+N*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zungqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[
                            iwork], &i__2, &ierr);
                    ie = 1;
                    itauq = itau;
                    itaup = itauq + *n;
                    iwork = itaup + *n;

/*                 Bidiagonalize R in WORK(IR) */
/*                 (CWorkspace: need N*N+3*N, prefer N*N+2*N+2*N*NB) */
/*                 (RWorkspace: need N) */

                    i__2 = *lwork - iwork + 1;
                    zgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &rwork[ie], &
                            work[itauq], &work[itaup], &work[iwork], &i__2, &
                            ierr);

/*                 Generate left vectors bidiagonalizing R */
/*                 (CWorkspace: need N*N+3*N, prefer N*N+2*N+N*NB) */
/*                 (RWorkspace: need 0) */

                    i__2 = *lwork - iwork + 1;
                    zungbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq], &
                            work[iwork], &i__2, &ierr, (ftnlen)1);
                    irwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of R in WORK(IR) */
/*                 (CWorkspace: need N*N) */
/*                 (RWorkspace: need BDSPAC) */

                    zbdsqr_("U", n, &c__0, n, &c__0, &s[1], &rwork[ie], cdum, 
                            &c__1, &work[ir], &ldwrkr, cdum, &c__1, &rwork[
                            irwork], info, (ftnlen)1);
                    iu = itauq;

/*                 Multiply Q in A by left singular vectors of R in */
/*                 WORK(IR), storing result in WORK(IU) and copying to A */
/*                 (CWorkspace: need N*N+N, prefer N*N+M*N) */
/*                 (RWorkspace: 0) */

                    i__2 = *m;
                    i__3 = ldwrku;
                    for (i__ = 1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
                             i__3) {
/* Computing MIN */
                        i__4 = *m - i__ + 1;
                        chunk = MIN(i__4,ldwrku);
                        zgemm_("N", "N", &chunk, n, n, &c_b2, &a[i__ + a_dim1]
                                , lda, &work[ir], &ldwrkr, &c_b1, &work[iu], &
                                ldwrku, (ftnlen)1, (ftnlen)1);
                        zlacpy_("F", &chunk, n, &work[iu], &ldwrku, &a[i__ + 
                                a_dim1], lda, (ftnlen)1);
/* L10: */
                    }

                } else {

/*                 Insufficient workspace for a fast algorithm */

                    ie = 1;
                    itauq = 1;
                    itaup = itauq + *n;
                    iwork = itaup + *n;

/*                 Bidiagonalize A */
/*                 (CWorkspace: need 2*N+M, prefer 2*N+(M+N)*NB) */
/*                 (RWorkspace: N) */

                    i__3 = *lwork - iwork + 1;
                    zgebrd_(m, n, &a[a_offset], lda, &s[1], &rwork[ie], &work[
                            itauq], &work[itaup], &work[iwork], &i__3, &ierr);

/*                 Generate left vectors bidiagonalizing A */
/*                 (CWorkspace: need 3*N, prefer 2*N+N*NB) */
/*                 (RWorkspace: 0) */

                    i__3 = *lwork - iwork + 1;
                    zungbr_("Q", m, n, n, &a[a_offset], lda, &work[itauq], &
                            work[iwork], &i__3, &ierr, (ftnlen)1);
                    irwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of A in A */
/*                 (CWorkspace: need 0) */
/*                 (RWorkspace: need BDSPAC) */

                    zbdsqr_("U", n, &c__0, m, &c__0, &s[1], &rwork[ie], cdum, 
                            &c__1, &a[a_offset], lda, cdum, &c__1, &rwork[
                            irwork], info, (ftnlen)1);

                }

            } else if (wntuo && wntvas) {

/*              Path 3 (M much larger than N, JOBU='O', JOBVT='S' or 'A') */
/*              N left singular vectors to be overwritten on A and */
/*              N right singular vectors to be computed in VT */

                if (*lwork >= *n * *n + *n * 3) {

/*                 Sufficient workspace for a fast algorithm */

                    ir = 1;
/* Computing MAX */
                    i__3 = wrkbl, i__2 = *lda * *n;
                    if (*lwork >= MAX(i__3,i__2) + *lda * *n) {

/*                    WORK(IU) is LDA by N and WORK(IR) is LDA by N */

                        ldwrku = *lda;
                        ldwrkr = *lda;
                    } else /* if(complicated condition) */ {
/* Computing MAX */
                        i__3 = wrkbl, i__2 = *lda * *n;
                        if (*lwork >= MAX(i__3,i__2) + *n * *n) {

/*                    WORK(IU) is LDA by N and WORK(IR) is N by N */

                            ldwrku = *lda;
                            ldwrkr = *n;
                        } else {

/*                    WORK(IU) is LDWRKU by N and WORK(IR) is N by N */

                            ldwrku = (*lwork - *n * *n) / *n;
                            ldwrkr = *n;
                        }
                    }
                    itau = ir + ldwrkr * *n;
                    iwork = itau + *n;

/*                 Compute A=Q*R */
/*                 (CWorkspace: need N*N+2*N, prefer N*N+N+N*NB) */
/*                 (RWorkspace: 0) */

                    i__3 = *lwork - iwork + 1;
                    zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
                            , &i__3, &ierr);

/*                 Copy R to VT, zeroing out below it */

                    zlacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], 
                            ldvt, (ftnlen)1);
                    i__3 = *n - 1;
                    i__2 = *n - 1;
                    zlaset_("L", &i__3, &i__2, &c_b1, &c_b1, &vt[vt_dim1 + 2],
                             ldvt, (ftnlen)1);

/*                 Generate Q in A */
/*                 (CWorkspace: need N*N+2*N, prefer N*N+N+N*NB) */
/*                 (RWorkspace: 0) */

                    i__3 = *lwork - iwork + 1;
                    zungqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[
                            iwork], &i__3, &ierr);
                    ie = 1;
                    itauq = itau;
                    itaup = itauq + *n;
                    iwork = itaup + *n;

/*                 Bidiagonalize R in VT, copying result to WORK(IR) */
/*                 (CWorkspace: need N*N+3*N, prefer N*N+2*N+2*N*NB) */
/*                 (RWorkspace: need N) */

                    i__3 = *lwork - iwork + 1;
                    zgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &rwork[ie], &
                            work[itauq], &work[itaup], &work[iwork], &i__3, &
                            ierr);
                    zlacpy_("L", n, n, &vt[vt_offset], ldvt, &work[ir], &
                            ldwrkr, (ftnlen)1);

/*                 Generate left vectors bidiagonalizing R in WORK(IR) */
/*                 (CWorkspace: need N*N+3*N, prefer N*N+2*N+N*NB) */
/*                 (RWorkspace: 0) */

                    i__3 = *lwork - iwork + 1;
                    zungbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq], &
                            work[iwork], &i__3, &ierr, (ftnlen)1);

/*                 Generate right vectors bidiagonalizing R in VT */
/*                 (CWorkspace: need N*N+3*N-1, prefer N*N+2*N+(N-1)*NB) */
/*                 (RWorkspace: 0) */

                    i__3 = *lwork - iwork + 1;
                    zungbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], 
                            &work[iwork], &i__3, &ierr, (ftnlen)1);
                    irwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of R in WORK(IR) and computing right */
/*                 singular vectors of R in VT */
/*                 (CWorkspace: need N*N) */
/*                 (RWorkspace: need BDSPAC) */

                    zbdsqr_("U", n, n, n, &c__0, &s[1], &rwork[ie], &vt[
                            vt_offset], ldvt, &work[ir], &ldwrkr, cdum, &c__1,
                             &rwork[irwork], info, (ftnlen)1);
                    iu = itauq;

/*                 Multiply Q in A by left singular vectors of R in */
/*                 WORK(IR), storing result in WORK(IU) and copying to A */
/*                 (CWorkspace: need N*N+N, prefer N*N+M*N) */
/*                 (RWorkspace: 0) */

                    i__3 = *m;
                    i__2 = ldwrku;
                    for (i__ = 1; i__2 < 0 ? i__ >= i__3 : i__ <= i__3; i__ +=
                             i__2) {
/* Computing MIN */
                        i__4 = *m - i__ + 1;
                        chunk = MIN(i__4,ldwrku);
                        zgemm_("N", "N", &chunk, n, n, &c_b2, &a[i__ + a_dim1]
                                , lda, &work[ir], &ldwrkr, &c_b1, &work[iu], &
                                ldwrku, (ftnlen)1, (ftnlen)1);
                        zlacpy_("F", &chunk, n, &work[iu], &ldwrku, &a[i__ + 
                                a_dim1], lda, (ftnlen)1);
/* L20: */
                    }

                } else {

/*                 Insufficient workspace for a fast algorithm */

                    itau = 1;
                    iwork = itau + *n;

/*                 Compute A=Q*R */
/*                 (CWorkspace: need 2*N, prefer N+N*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
                            , &i__2, &ierr);

/*                 Copy R to VT, zeroing out below it */

                    zlacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], 
                            ldvt, (ftnlen)1);
                    i__2 = *n - 1;
                    i__3 = *n - 1;
                    zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &vt[vt_dim1 + 2],
                             ldvt, (ftnlen)1);

/*                 Generate Q in A */
/*                 (CWorkspace: need 2*N, prefer N+N*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zungqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[
                            iwork], &i__2, &ierr);
                    ie = 1;
                    itauq = itau;
                    itaup = itauq + *n;
                    iwork = itaup + *n;

/*                 Bidiagonalize R in VT */
/*                 (CWorkspace: need 3*N, prefer 2*N+2*N*NB) */
/*                 (RWorkspace: N) */

                    i__2 = *lwork - iwork + 1;
                    zgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &rwork[ie], &
                            work[itauq], &work[itaup], &work[iwork], &i__2, &
                            ierr);

/*                 Multiply Q in A by left vectors bidiagonalizing R */
/*                 (CWorkspace: need 2*N+M, prefer 2*N+M*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zunmbr_("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt, &
                            work[itauq], &a[a_offset], lda, &work[iwork], &
                            i__2, &ierr, (ftnlen)1, (ftnlen)1, (ftnlen)1);

/*                 Generate right vectors bidiagonalizing R in VT */
/*                 (CWorkspace: need 3*N-1, prefer 2*N+(N-1)*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zungbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], 
                            &work[iwork], &i__2, &ierr, (ftnlen)1);
                    irwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of A in A and computing right */
/*                 singular vectors of A in VT */
/*                 (CWorkspace: 0) */
/*                 (RWorkspace: need BDSPAC) */

                    zbdsqr_("U", n, n, m, &c__0, &s[1], &rwork[ie], &vt[
                            vt_offset], ldvt, &a[a_offset], lda, cdum, &c__1, 
                            &rwork[irwork], info, (ftnlen)1);

                }

            } else if (wntus) {

                if (wntvn) {

/*                 Path 4 (M much larger than N, JOBU='S', JOBVT='N') */
/*                 N left singular vectors to be computed in U and */
/*                 no right singular vectors to be computed */

                    if (*lwork >= *n * *n + *n * 3) {

/*                    Sufficient workspace for a fast algorithm */

                        ir = 1;
                        if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IR) is LDA by N */

                            ldwrkr = *lda;
                        } else {

/*                       WORK(IR) is N by N */

                            ldwrkr = *n;
                        }
                        itau = ir + ldwrkr * *n;
                        iwork = itau + *n;

/*                    Compute A=Q*R */
/*                    (CWorkspace: need N*N+2*N, prefer N*N+N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);

/*                    Copy R to WORK(IR), zeroing out below it */

                        zlacpy_("U", n, n, &a[a_offset], lda, &work[ir], &
                                ldwrkr, (ftnlen)1);
                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &work[ir + 1]
                                , &ldwrkr, (ftnlen)1);

/*                    Generate Q in A */
/*                    (CWorkspace: need N*N+2*N, prefer N*N+N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, n, n, &a[a_offset], lda, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IR) */
/*                    (CWorkspace: need N*N+3*N, prefer N*N+2*N+2*N*NB) */
/*                    (RWorkspace: need N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Generate left vectors bidiagonalizing R in WORK(IR) */
/*                    (CWorkspace: need N*N+3*N, prefer N*N+2*N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IR) */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, &c__0, n, &c__0, &s[1], &rwork[ie], 
                                cdum, &c__1, &work[ir], &ldwrkr, cdum, &c__1, 
                                &rwork[irwork], info, (ftnlen)1);

/*                    Multiply Q in A by left singular vectors of R in */
/*                    WORK(IR), storing result in U */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, n, &c_b2, &a[a_offset], lda, &
                                work[ir], &ldwrkr, &c_b1, &u[u_offset], ldu, (
                                ftnlen)1, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (CWorkspace: need 2*N, prefer N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Generate Q in U */
/*                    (CWorkspace: need 2*N, prefer N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, n, n, &u[u_offset], ldu, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Zero out below R in A */

                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &a[a_dim1 + 
                                2], lda, (ftnlen)1);

/*                    Bidiagonalize R in A */
/*                    (CWorkspace: need 3*N, prefer 2*N+2*N*NB) */
/*                    (RWorkspace: need N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &a[a_offset], lda, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply Q in U by left vectors bidiagonalizing R */
/*                    (CWorkspace: need 2*N+M, prefer 2*N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
                                work[itauq], &u[u_offset], ldu, &work[iwork], 
                                &i__2, &ierr, (ftnlen)1, (ftnlen)1, (ftnlen)1)
                                ;
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, &c__0, m, &c__0, &s[1], &rwork[ie], 
                                cdum, &c__1, &u[u_offset], ldu, cdum, &c__1, &
                                rwork[irwork], info, (ftnlen)1);

                    }

                } else if (wntvo) {

/*                 Path 5 (M much larger than N, JOBU='S', JOBVT='O') */
/*                 N left singular vectors to be computed in U and */
/*                 N right singular vectors to be overwritten on A */

                    if (*lwork >= (*n << 1) * *n + *n * 3) {

/*                    Sufficient workspace for a fast algorithm */

                        iu = 1;
                        if (*lwork >= wrkbl + (*lda << 1) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is LDA by N */

                            ldwrku = *lda;
                            ir = iu + ldwrku * *n;
                            ldwrkr = *lda;
                        } else if (*lwork >= wrkbl + (*lda + *n) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is N by N */

                            ldwrku = *lda;
                            ir = iu + ldwrku * *n;
                            ldwrkr = *n;
                        } else {

/*                       WORK(IU) is N by N and WORK(IR) is N by N */

                            ldwrku = *n;
                            ir = iu + ldwrku * *n;
                            ldwrkr = *n;
                        }
                        itau = ir + ldwrkr * *n;
                        iwork = itau + *n;

/*                    Compute A=Q*R */
/*                    (CWorkspace: need 2*N*N+2*N, prefer 2*N*N+N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

                        zlacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
                                ldwrku, (ftnlen)1);
                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &work[iu + 1]
                                , &ldwrku, (ftnlen)1);

/*                    Generate Q in A */
/*                    (CWorkspace: need 2*N*N+2*N, prefer 2*N*N+N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, n, n, &a[a_offset], lda, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to */
/*                    WORK(IR) */
/*                    (CWorkspace: need   2*N*N+3*N, */
/*                                 prefer 2*N*N+2*N+2*N*NB) */
/*                    (RWorkspace: need   N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &work[iu], &ldwrku, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);
                        zlacpy_("U", n, n, &work[iu], &ldwrku, &work[ir], &
                                ldwrkr, (ftnlen)1);

/*                    Generate left bidiagonalizing vectors in WORK(IU) */
/*                    (CWorkspace: need 2*N*N+3*N, prefer 2*N*N+2*N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);

/*                    Generate right bidiagonalizing vectors in WORK(IR) */
/*                    (CWorkspace: need   2*N*N+3*N-1, */
/*                                 prefer 2*N*N+2*N+(N-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", n, n, n, &work[ir], &ldwrkr, &work[itaup]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IU) and computing */
/*                    right singular vectors of R in WORK(IR) */
/*                    (CWorkspace: need 2*N*N) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, n, n, &c__0, &s[1], &rwork[ie], &work[
                                ir], &ldwrkr, &work[iu], &ldwrku, cdum, &c__1,
                                 &rwork[irwork], info, (ftnlen)1);

/*                    Multiply Q in A by left singular vectors of R in */
/*                    WORK(IU), storing result in U */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, n, &c_b2, &a[a_offset], lda, &
                                work[iu], &ldwrku, &c_b1, &u[u_offset], ldu, (
                                ftnlen)1, (ftnlen)1);

/*                    Copy right singular vectors of R to A */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: 0) */

                        zlacpy_("F", n, n, &work[ir], &ldwrkr, &a[a_offset], 
                                lda, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (CWorkspace: need 2*N, prefer N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Generate Q in U */
/*                    (CWorkspace: need 2*N, prefer N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, n, n, &u[u_offset], ldu, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Zero out below R in A */

                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &a[a_dim1 + 
                                2], lda, (ftnlen)1);

/*                    Bidiagonalize R in A */
/*                    (CWorkspace: need 3*N, prefer 2*N+2*N*NB) */
/*                    (RWorkspace: need N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &a[a_offset], lda, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply Q in U by left vectors bidiagonalizing R */
/*                    (CWorkspace: need 2*N+M, prefer 2*N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
                                work[itauq], &u[u_offset], ldu, &work[iwork], 
                                &i__2, &ierr, (ftnlen)1, (ftnlen)1, (ftnlen)1)
                                ;

/*                    Generate right vectors bidiagonalizing R in A */
/*                    (CWorkspace: need 3*N-1, prefer 2*N+(N-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", n, n, n, &a[a_offset], lda, &work[itaup],
                                 &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in A */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, n, m, &c__0, &s[1], &rwork[ie], &a[
                                a_offset], lda, &u[u_offset], ldu, cdum, &
                                c__1, &rwork[irwork], info, (ftnlen)1);

                    }

                } else if (wntvas) {

/*                 Path 6 (M much larger than N, JOBU='S', JOBVT='S' */
/*                         or 'A') */
/*                 N left singular vectors to be computed in U and */
/*                 N right singular vectors to be computed in VT */

                    if (*lwork >= *n * *n + *n * 3) {

/*                    Sufficient workspace for a fast algorithm */

                        iu = 1;
                        if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IU) is LDA by N */

                            ldwrku = *lda;
                        } else {

/*                       WORK(IU) is N by N */

                            ldwrku = *n;
                        }
                        itau = iu + ldwrku * *n;
                        iwork = itau + *n;

/*                    Compute A=Q*R */
/*                    (CWorkspace: need N*N+2*N, prefer N*N+N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

                        zlacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
                                ldwrku, (ftnlen)1);
                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &work[iu + 1]
                                , &ldwrku, (ftnlen)1);

/*                    Generate Q in A */
/*                    (CWorkspace: need N*N+2*N, prefer N*N+N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, n, n, &a[a_offset], lda, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to VT */
/*                    (CWorkspace: need N*N+3*N, prefer N*N+2*N+2*N*NB) */
/*                    (RWorkspace: need N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &work[iu], &ldwrku, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);
                        zlacpy_("U", n, n, &work[iu], &ldwrku, &vt[vt_offset],
                                 ldvt, (ftnlen)1);

/*                    Generate left bidiagonalizing vectors in WORK(IU) */
/*                    (CWorkspace: need N*N+3*N, prefer N*N+2*N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);

/*                    Generate right bidiagonalizing vectors in VT */
/*                    (CWorkspace: need   N*N+3*N-1, */
/*                                 prefer N*N+2*N+(N-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
                                itaup], &work[iwork], &i__2, &ierr, (ftnlen)1)
                                ;
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IU) and computing */
/*                    right singular vectors of R in VT */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, n, n, &c__0, &s[1], &rwork[ie], &vt[
                                vt_offset], ldvt, &work[iu], &ldwrku, cdum, &
                                c__1, &rwork[irwork], info, (ftnlen)1);

/*                    Multiply Q in A by left singular vectors of R in */
/*                    WORK(IU), storing result in U */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, n, &c_b2, &a[a_offset], lda, &
                                work[iu], &ldwrku, &c_b1, &u[u_offset], ldu, (
                                ftnlen)1, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (CWorkspace: need 2*N, prefer N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Generate Q in U */
/*                    (CWorkspace: need 2*N, prefer N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, n, n, &u[u_offset], ldu, &work[itau], &
                                work[iwork], &i__2, &ierr);

/*                    Copy R to VT, zeroing out below it */

                        zlacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);
                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &vt[vt_dim1 
                                + 2], ldvt, (ftnlen)1);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Bidiagonalize R in VT */
/*                    (CWorkspace: need 3*N, prefer 2*N+2*N*NB) */
/*                    (RWorkspace: need N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &rwork[ie],
                                 &work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors */
/*                    in VT */
/*                    (CWorkspace: need 2*N+M, prefer 2*N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt, 
                                &work[itauq], &u[u_offset], ldu, &work[iwork],
                                 &i__2, &ierr, (ftnlen)1, (ftnlen)1, (ftnlen)
                                1);

/*                    Generate right bidiagonalizing vectors in VT */
/*                    (CWorkspace: need 3*N-1, prefer 2*N+(N-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
                                itaup], &work[iwork], &i__2, &ierr, (ftnlen)1)
                                ;
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in VT */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, n, m, &c__0, &s[1], &rwork[ie], &vt[
                                vt_offset], ldvt, &u[u_offset], ldu, cdum, &
                                c__1, &rwork[irwork], info, (ftnlen)1);

                    }

                }

            } else if (wntua) {

                if (wntvn) {

/*                 Path 7 (M much larger than N, JOBU='A', JOBVT='N') */
/*                 M left singular vectors to be computed in U and */
/*                 no right singular vectors to be computed */

/* Computing MAX */
                    i__2 = *n + *m, i__3 = *n * 3;
                    if (*lwork >= *n * *n + MAX(i__2,i__3)) {

/*                    Sufficient workspace for a fast algorithm */

                        ir = 1;
                        if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IR) is LDA by N */

                            ldwrkr = *lda;
                        } else {

/*                       WORK(IR) is N by N */

                            ldwrkr = *n;
                        }
                        itau = ir + ldwrkr * *n;
                        iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (CWorkspace: need N*N+2*N, prefer N*N+N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Copy R to WORK(IR), zeroing out below it */

                        zlacpy_("U", n, n, &a[a_offset], lda, &work[ir], &
                                ldwrkr, (ftnlen)1);
                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &work[ir + 1]
                                , &ldwrkr, (ftnlen)1);

/*                    Generate Q in U */
/*                    (CWorkspace: need N*N+N+M, prefer N*N+N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IR) */
/*                    (CWorkspace: need N*N+3*N, prefer N*N+2*N+2*N*NB) */
/*                    (RWorkspace: need N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in WORK(IR) */
/*                    (CWorkspace: need N*N+3*N, prefer N*N+2*N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IR) */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, &c__0, n, &c__0, &s[1], &rwork[ie], 
                                cdum, &c__1, &work[ir], &ldwrkr, cdum, &c__1, 
                                &rwork[irwork], info, (ftnlen)1);

/*                    Multiply Q in U by left singular vectors of R in */
/*                    WORK(IR), storing result in A */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, n, &c_b2, &u[u_offset], ldu, &
                                work[ir], &ldwrkr, &c_b1, &a[a_offset], lda, (
                                ftnlen)1, (ftnlen)1);

/*                    Copy left singular vectors of A from A to U */

                        zlacpy_("F", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (CWorkspace: need 2*N, prefer N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Generate Q in U */
/*                    (CWorkspace: need N+M, prefer N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Zero out below R in A */

                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &a[a_dim1 + 
                                2], lda, (ftnlen)1);

/*                    Bidiagonalize R in A */
/*                    (CWorkspace: need 3*N, prefer 2*N+2*N*NB) */
/*                    (RWorkspace: need N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &a[a_offset], lda, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors */
/*                    in A */
/*                    (CWorkspace: need 2*N+M, prefer 2*N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
                                work[itauq], &u[u_offset], ldu, &work[iwork], 
                                &i__2, &ierr, (ftnlen)1, (ftnlen)1, (ftnlen)1)
                                ;
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, &c__0, m, &c__0, &s[1], &rwork[ie], 
                                cdum, &c__1, &u[u_offset], ldu, cdum, &c__1, &
                                rwork[irwork], info, (ftnlen)1);

                    }

                } else if (wntvo) {

/*                 Path 8 (M much larger than N, JOBU='A', JOBVT='O') */
/*                 M left singular vectors to be computed in U and */
/*                 N right singular vectors to be overwritten on A */

/* Computing MAX */
                    i__2 = *n + *m, i__3 = *n * 3;
                    if (*lwork >= (*n << 1) * *n + MAX(i__2,i__3)) {

/*                    Sufficient workspace for a fast algorithm */

                        iu = 1;
                        if (*lwork >= wrkbl + (*lda << 1) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is LDA by N */

                            ldwrku = *lda;
                            ir = iu + ldwrku * *n;
                            ldwrkr = *lda;
                        } else if (*lwork >= wrkbl + (*lda + *n) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is N by N */

                            ldwrku = *lda;
                            ir = iu + ldwrku * *n;
                            ldwrkr = *n;
                        } else {

/*                       WORK(IU) is N by N and WORK(IR) is N by N */

                            ldwrku = *n;
                            ir = iu + ldwrku * *n;
                            ldwrkr = *n;
                        }
                        itau = ir + ldwrkr * *n;
                        iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (CWorkspace: need 2*N*N+2*N, prefer 2*N*N+N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Generate Q in U */
/*                    (CWorkspace: need 2*N*N+N+M, prefer 2*N*N+N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
                                work[iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

                        zlacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
                                ldwrku, (ftnlen)1);
                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &work[iu + 1]
                                , &ldwrku, (ftnlen)1);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to */
/*                    WORK(IR) */
/*                    (CWorkspace: need   2*N*N+3*N, */
/*                                 prefer 2*N*N+2*N+2*N*NB) */
/*                    (RWorkspace: need   N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &work[iu], &ldwrku, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);
                        zlacpy_("U", n, n, &work[iu], &ldwrku, &work[ir], &
                                ldwrkr, (ftnlen)1);

/*                    Generate left bidiagonalizing vectors in WORK(IU) */
/*                    (CWorkspace: need 2*N*N+3*N, prefer 2*N*N+2*N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);

/*                    Generate right bidiagonalizing vectors in WORK(IR) */
/*                    (CWorkspace: need   2*N*N+3*N-1, */
/*                                 prefer 2*N*N+2*N+(N-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", n, n, n, &work[ir], &ldwrkr, &work[itaup]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IU) and computing */
/*                    right singular vectors of R in WORK(IR) */
/*                    (CWorkspace: need 2*N*N) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, n, n, &c__0, &s[1], &rwork[ie], &work[
                                ir], &ldwrkr, &work[iu], &ldwrku, cdum, &c__1,
                                 &rwork[irwork], info, (ftnlen)1);

/*                    Multiply Q in U by left singular vectors of R in */
/*                    WORK(IU), storing result in A */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, n, &c_b2, &u[u_offset], ldu, &
                                work[iu], &ldwrku, &c_b1, &a[a_offset], lda, (
                                ftnlen)1, (ftnlen)1);

/*                    Copy left singular vectors of A from A to U */

                        zlacpy_("F", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Copy right singular vectors of R from WORK(IR) to A */

                        zlacpy_("F", n, n, &work[ir], &ldwrkr, &a[a_offset], 
                                lda, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (CWorkspace: need 2*N, prefer N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Generate Q in U */
/*                    (CWorkspace: need N+M, prefer N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Zero out below R in A */

                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &a[a_dim1 + 
                                2], lda, (ftnlen)1);

/*                    Bidiagonalize R in A */
/*                    (CWorkspace: need 3*N, prefer 2*N+2*N*NB) */
/*                    (RWorkspace: need N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &a[a_offset], lda, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors */
/*                    in A */
/*                    (CWorkspace: need 2*N+M, prefer 2*N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
                                work[itauq], &u[u_offset], ldu, &work[iwork], 
                                &i__2, &ierr, (ftnlen)1, (ftnlen)1, (ftnlen)1)
                                ;

/*                    Generate right bidiagonalizing vectors in A */
/*                    (CWorkspace: need 3*N-1, prefer 2*N+(N-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", n, n, n, &a[a_offset], lda, &work[itaup],
                                 &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in A */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, n, m, &c__0, &s[1], &rwork[ie], &a[
                                a_offset], lda, &u[u_offset], ldu, cdum, &
                                c__1, &rwork[irwork], info, (ftnlen)1);

                    }

                } else if (wntvas) {

/*                 Path 9 (M much larger than N, JOBU='A', JOBVT='S' */
/*                         or 'A') */
/*                 M left singular vectors to be computed in U and */
/*                 N right singular vectors to be computed in VT */

/* Computing MAX */
                    i__2 = *n + *m, i__3 = *n * 3;
                    if (*lwork >= *n * *n + MAX(i__2,i__3)) {

/*                    Sufficient workspace for a fast algorithm */

                        iu = 1;
                        if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IU) is LDA by N */

                            ldwrku = *lda;
                        } else {

/*                       WORK(IU) is N by N */

                            ldwrku = *n;
                        }
                        itau = iu + ldwrku * *n;
                        iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (CWorkspace: need N*N+2*N, prefer N*N+N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Generate Q in U */
/*                    (CWorkspace: need N*N+N+M, prefer N*N+N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
                                work[iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

                        zlacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
                                ldwrku, (ftnlen)1);
                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &work[iu + 1]
                                , &ldwrku, (ftnlen)1);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to VT */
/*                    (CWorkspace: need N*N+3*N, prefer N*N+2*N+2*N*NB) */
/*                    (RWorkspace: need N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &work[iu], &ldwrku, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);
                        zlacpy_("U", n, n, &work[iu], &ldwrku, &vt[vt_offset],
                                 ldvt, (ftnlen)1);

/*                    Generate left bidiagonalizing vectors in WORK(IU) */
/*                    (CWorkspace: need N*N+3*N, prefer N*N+2*N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);

/*                    Generate right bidiagonalizing vectors in VT */
/*                    (CWorkspace: need   N*N+3*N-1, */
/*                                 prefer N*N+2*N+(N-1)*NB) */
/*                    (RWorkspace: need   0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
                                itaup], &work[iwork], &i__2, &ierr, (ftnlen)1)
                                ;
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IU) and computing */
/*                    right singular vectors of R in VT */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, n, n, &c__0, &s[1], &rwork[ie], &vt[
                                vt_offset], ldvt, &work[iu], &ldwrku, cdum, &
                                c__1, &rwork[irwork], info, (ftnlen)1);

/*                    Multiply Q in U by left singular vectors of R in */
/*                    WORK(IU), storing result in A */
/*                    (CWorkspace: need N*N) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, n, &c_b2, &u[u_offset], ldu, &
                                work[iu], &ldwrku, &c_b1, &a[a_offset], lda, (
                                ftnlen)1, (ftnlen)1);

/*                    Copy left singular vectors of A from A to U */

                        zlacpy_("F", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (CWorkspace: need 2*N, prefer N+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Generate Q in U */
/*                    (CWorkspace: need N+M, prefer N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
                                work[iwork], &i__2, &ierr);

/*                    Copy R from A to VT, zeroing out below it */

                        zlacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);
                        i__2 = *n - 1;
                        i__3 = *n - 1;
                        zlaset_("L", &i__2, &i__3, &c_b1, &c_b1, &vt[vt_dim1 
                                + 2], ldvt, (ftnlen)1);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *n;
                        iwork = itaup + *n;

/*                    Bidiagonalize R in VT */
/*                    (CWorkspace: need 3*N, prefer 2*N+2*N*NB) */
/*                    (RWorkspace: need N) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &rwork[ie],
                                 &work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors */
/*                    in VT */
/*                    (CWorkspace: need 2*N+M, prefer 2*N+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt, 
                                &work[itauq], &u[u_offset], ldu, &work[iwork],
                                 &i__2, &ierr, (ftnlen)1, (ftnlen)1, (ftnlen)
                                1);

/*                    Generate right bidiagonalizing vectors in VT */
/*                    (CWorkspace: need 3*N-1, prefer 2*N+(N-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
                                itaup], &work[iwork], &i__2, &ierr, (ftnlen)1)
                                ;
                        irwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in VT */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", n, n, m, &c__0, &s[1], &rwork[ie], &vt[
                                vt_offset], ldvt, &u[u_offset], ldu, cdum, &
                                c__1, &rwork[irwork], info, (ftnlen)1);

                    }

                }

            }

        } else {

/*           M .LT. MNTHR */

/*           Path 10 (M at least N, but not much larger) */
/*           Reduce to bidiagonal form without QR decomposition */

            ie = 1;
            itauq = 1;
            itaup = itauq + *n;
            iwork = itaup + *n;

/*           Bidiagonalize A */
/*           (CWorkspace: need 2*N+M, prefer 2*N+(M+N)*NB) */
/*           (RWorkspace: need N) */

            i__2 = *lwork - iwork + 1;
            zgebrd_(m, n, &a[a_offset], lda, &s[1], &rwork[ie], &work[itauq], 
                    &work[itaup], &work[iwork], &i__2, &ierr);
            if (wntuas) {

/*              If left singular vectors desired in U, copy result to U */
/*              and generate left bidiagonalizing vectors in U */
/*              (CWorkspace: need 2*N+NCU, prefer 2*N+NCU*NB) */
/*              (RWorkspace: 0) */

                zlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], ldu, (
                        ftnlen)1);
                if (wntus) {
                    ncu = *n;
                }
                if (wntua) {
                    ncu = *m;
                }
                i__2 = *lwork - iwork + 1;
                zungbr_("Q", m, &ncu, n, &u[u_offset], ldu, &work[itauq], &
                        work[iwork], &i__2, &ierr, (ftnlen)1);
            }
            if (wntvas) {

/*              If right singular vectors desired in VT, copy result to */
/*              VT and generate right bidiagonalizing vectors in VT */
/*              (CWorkspace: need 3*N-1, prefer 2*N+(N-1)*NB) */
/*              (RWorkspace: 0) */

                zlacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], ldvt, (
                        ftnlen)1);
                i__2 = *lwork - iwork + 1;
                zungbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], &
                        work[iwork], &i__2, &ierr, (ftnlen)1);
            }
            if (wntuo) {

/*              If left singular vectors desired in A, generate left */
/*              bidiagonalizing vectors in A */
/*              (CWorkspace: need 3*N, prefer 2*N+N*NB) */
/*              (RWorkspace: 0) */

                i__2 = *lwork - iwork + 1;
                zungbr_("Q", m, n, n, &a[a_offset], lda, &work[itauq], &work[
                        iwork], &i__2, &ierr, (ftnlen)1);
            }
            if (wntvo) {

/*              If right singular vectors desired in A, generate right */
/*              bidiagonalizing vectors in A */
/*              (CWorkspace: need 3*N-1, prefer 2*N+(N-1)*NB) */
/*              (RWorkspace: 0) */

                i__2 = *lwork - iwork + 1;
                zungbr_("P", n, n, n, &a[a_offset], lda, &work[itaup], &work[
                        iwork], &i__2, &ierr, (ftnlen)1);
            }
            irwork = ie + *n;
            if (wntuas || wntuo) {
                nru = *m;
            }
            if (wntun) {
                nru = 0;
            }
            if (wntvas || wntvo) {
                ncvt = *n;
            }
            if (wntvn) {
                ncvt = 0;
            }
            if (! wntuo && ! wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in U and computing right singular */
/*              vectors in VT */
/*              (CWorkspace: 0) */
/*              (RWorkspace: need BDSPAC) */

                zbdsqr_("U", n, &ncvt, &nru, &c__0, &s[1], &rwork[ie], &vt[
                        vt_offset], ldvt, &u[u_offset], ldu, cdum, &c__1, &
                        rwork[irwork], info, (ftnlen)1);
            } else if (! wntuo && wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in U and computing right singular */
/*              vectors in A */
/*              (CWorkspace: 0) */
/*              (RWorkspace: need BDSPAC) */

                zbdsqr_("U", n, &ncvt, &nru, &c__0, &s[1], &rwork[ie], &a[
                        a_offset], lda, &u[u_offset], ldu, cdum, &c__1, &
                        rwork[irwork], info, (ftnlen)1);
            } else {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in A and computing right singular */
/*              vectors in VT */
/*              (CWorkspace: 0) */
/*              (RWorkspace: need BDSPAC) */

                zbdsqr_("U", n, &ncvt, &nru, &c__0, &s[1], &rwork[ie], &vt[
                        vt_offset], ldvt, &a[a_offset], lda, cdum, &c__1, &
                        rwork[irwork], info, (ftnlen)1);
            }

        }

    } else {

/*        A has more columns than rows. If A has sufficiently more */
/*        columns than rows, first reduce using the LQ decomposition (if */
/*        sufficient workspace available) */

        if (*n >= mnthr) {

            if (wntvn) {

/*              Path 1t(N much larger than M, JOBVT='N') */
/*              No right singular vectors to be computed */

                itau = 1;
                iwork = itau + *m;

/*              Compute A=L*Q */
/*              (CWorkspace: need 2*M, prefer M+M*NB) */
/*              (RWorkspace: 0) */

                i__2 = *lwork - iwork + 1;
                zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork], &
                        i__2, &ierr);

/*              Zero out above L */

                i__2 = *m - 1;
                i__3 = *m - 1;
                zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &a[(a_dim1 << 1) + 1]
                        , lda, (ftnlen)1);
                ie = 1;
                itauq = 1;
                itaup = itauq + *m;
                iwork = itaup + *m;

/*              Bidiagonalize L in A */
/*              (CWorkspace: need 3*M, prefer 2*M+2*M*NB) */
/*              (RWorkspace: need M) */

                i__2 = *lwork - iwork + 1;
                zgebrd_(m, m, &a[a_offset], lda, &s[1], &rwork[ie], &work[
                        itauq], &work[itaup], &work[iwork], &i__2, &ierr);
                if (wntuo || wntuas) {

/*                 If left singular vectors desired, generate Q */
/*                 (CWorkspace: need 3*M, prefer 2*M+M*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zungbr_("Q", m, m, m, &a[a_offset], lda, &work[itauq], &
                            work[iwork], &i__2, &ierr, (ftnlen)1);
                }
                irwork = ie + *m;
                nru = 0;
                if (wntuo || wntuas) {
                    nru = *m;
                }

/*              Perform bidiagonal QR iteration, computing left singular */
/*              vectors of A in A if desired */
/*              (CWorkspace: 0) */
/*              (RWorkspace: need BDSPAC) */

                zbdsqr_("U", m, &c__0, &nru, &c__0, &s[1], &rwork[ie], cdum, &
                        c__1, &a[a_offset], lda, cdum, &c__1, &rwork[irwork], 
                        info, (ftnlen)1);

/*              If left singular vectors desired in U, copy them there */

                if (wntuas) {
                    zlacpy_("F", m, m, &a[a_offset], lda, &u[u_offset], ldu, (
                            ftnlen)1);
                }

            } else if (wntvo && wntun) {

/*              Path 2t(N much larger than M, JOBU='N', JOBVT='O') */
/*              M right singular vectors to be overwritten on A and */
/*              no left singular vectors to be computed */

                if (*lwork >= *m * *m + *m * 3) {

/*                 Sufficient workspace for a fast algorithm */

                    ir = 1;
/* Computing MAX */
                    i__2 = wrkbl, i__3 = *lda * *n;
                    if (*lwork >= MAX(i__2,i__3) + *lda * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is LDA by M */

                        ldwrku = *lda;
                        chunk = *n;
                        ldwrkr = *lda;
                    } else /* if(complicated condition) */ {
/* Computing MAX */
                        i__2 = wrkbl, i__3 = *lda * *n;
                        if (*lwork >= MAX(i__2,i__3) + *m * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is M by M */

                            ldwrku = *lda;
                            chunk = *n;
                            ldwrkr = *m;
                        } else {

/*                    WORK(IU) is M by CHUNK and WORK(IR) is M by M */

                            ldwrku = *m;
                            chunk = (*lwork - *m * *m) / *m;
                            ldwrkr = *m;
                        }
                    }
                    itau = ir + ldwrkr * *m;
                    iwork = itau + *m;

/*                 Compute A=L*Q */
/*                 (CWorkspace: need M*M+2*M, prefer M*M+M+M*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
                            , &i__2, &ierr);

/*                 Copy L to WORK(IR) and zero out above it */

                    zlacpy_("L", m, m, &a[a_offset], lda, &work[ir], &ldwrkr, 
                            (ftnlen)1);
                    i__2 = *m - 1;
                    i__3 = *m - 1;
                    zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &work[ir + 
                            ldwrkr], &ldwrkr, (ftnlen)1);

/*                 Generate Q in A */
/*                 (CWorkspace: need M*M+2*M, prefer M*M+M+M*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zunglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[
                            iwork], &i__2, &ierr);
                    ie = 1;
                    itauq = itau;
                    itaup = itauq + *m;
                    iwork = itaup + *m;

/*                 Bidiagonalize L in WORK(IR) */
/*                 (CWorkspace: need M*M+3*M, prefer M*M+2*M+2*M*NB) */
/*                 (RWorkspace: need M) */

                    i__2 = *lwork - iwork + 1;
                    zgebrd_(m, m, &work[ir], &ldwrkr, &s[1], &rwork[ie], &
                            work[itauq], &work[itaup], &work[iwork], &i__2, &
                            ierr);

/*                 Generate right vectors bidiagonalizing L */
/*                 (CWorkspace: need M*M+3*M-1, prefer M*M+2*M+(M-1)*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zungbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup], &
                            work[iwork], &i__2, &ierr, (ftnlen)1);
                    irwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing right */
/*                 singular vectors of L in WORK(IR) */
/*                 (CWorkspace: need M*M) */
/*                 (RWorkspace: need BDSPAC) */

                    zbdsqr_("U", m, m, &c__0, &c__0, &s[1], &rwork[ie], &work[
                            ir], &ldwrkr, cdum, &c__1, cdum, &c__1, &rwork[
                            irwork], info, (ftnlen)1);
                    iu = itauq;

/*                 Multiply right singular vectors of L in WORK(IR) by Q */
/*                 in A, storing result in WORK(IU) and copying to A */
/*                 (CWorkspace: need M*M+M, prefer M*M+M*N) */
/*                 (RWorkspace: 0) */

                    i__2 = *n;
                    i__3 = chunk;
                    for (i__ = 1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
                             i__3) {
/* Computing MIN */
                        i__4 = *n - i__ + 1;
                        blk = MIN(i__4,chunk);
                        zgemm_("N", "N", m, &blk, m, &c_b2, &work[ir], &
                                ldwrkr, &a[i__ * a_dim1 + 1], lda, &c_b1, &
                                work[iu], &ldwrku, (ftnlen)1, (ftnlen)1);
                        zlacpy_("F", m, &blk, &work[iu], &ldwrku, &a[i__ * 
                                a_dim1 + 1], lda, (ftnlen)1);
/* L30: */
                    }

                } else {

/*                 Insufficient workspace for a fast algorithm */

                    ie = 1;
                    itauq = 1;
                    itaup = itauq + *m;
                    iwork = itaup + *m;

/*                 Bidiagonalize A */
/*                 (CWorkspace: need 2*M+N, prefer 2*M+(M+N)*NB) */
/*                 (RWorkspace: need M) */

                    i__3 = *lwork - iwork + 1;
                    zgebrd_(m, n, &a[a_offset], lda, &s[1], &rwork[ie], &work[
                            itauq], &work[itaup], &work[iwork], &i__3, &ierr);

/*                 Generate right vectors bidiagonalizing A */
/*                 (CWorkspace: need 3*M, prefer 2*M+M*NB) */
/*                 (RWorkspace: 0) */

                    i__3 = *lwork - iwork + 1;
                    zungbr_("P", m, n, m, &a[a_offset], lda, &work[itaup], &
                            work[iwork], &i__3, &ierr, (ftnlen)1);
                    irwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing right */
/*                 singular vectors of A in A */
/*                 (CWorkspace: 0) */
/*                 (RWorkspace: need BDSPAC) */

                    zbdsqr_("L", m, n, &c__0, &c__0, &s[1], &rwork[ie], &a[
                            a_offset], lda, cdum, &c__1, cdum, &c__1, &rwork[
                            irwork], info, (ftnlen)1);

                }

            } else if (wntvo && wntuas) {

/*              Path 3t(N much larger than M, JOBU='S' or 'A', JOBVT='O') */
/*              M right singular vectors to be overwritten on A and */
/*              M left singular vectors to be computed in U */

                if (*lwork >= *m * *m + *m * 3) {

/*                 Sufficient workspace for a fast algorithm */

                    ir = 1;
/* Computing MAX */
                    i__3 = wrkbl, i__2 = *lda * *n;
                    if (*lwork >= MAX(i__3,i__2) + *lda * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is LDA by M */

                        ldwrku = *lda;
                        chunk = *n;
                        ldwrkr = *lda;
                    } else /* if(complicated condition) */ {
/* Computing MAX */
                        i__3 = wrkbl, i__2 = *lda * *n;
                        if (*lwork >= MAX(i__3,i__2) + *m * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is M by M */

                            ldwrku = *lda;
                            chunk = *n;
                            ldwrkr = *m;
                        } else {

/*                    WORK(IU) is M by CHUNK and WORK(IR) is M by M */

                            ldwrku = *m;
                            chunk = (*lwork - *m * *m) / *m;
                            ldwrkr = *m;
                        }
                    }
                    itau = ir + ldwrkr * *m;
                    iwork = itau + *m;

/*                 Compute A=L*Q */
/*                 (CWorkspace: need M*M+2*M, prefer M*M+M+M*NB) */
/*                 (RWorkspace: 0) */

                    i__3 = *lwork - iwork + 1;
                    zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
                            , &i__3, &ierr);

/*                 Copy L to U, zeroing about above it */

                    zlacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], ldu, (
                            ftnlen)1);
                    i__3 = *m - 1;
                    i__2 = *m - 1;
                    zlaset_("U", &i__3, &i__2, &c_b1, &c_b1, &u[(u_dim1 << 1) 
                            + 1], ldu, (ftnlen)1);

/*                 Generate Q in A */
/*                 (CWorkspace: need M*M+2*M, prefer M*M+M+M*NB) */
/*                 (RWorkspace: 0) */

                    i__3 = *lwork - iwork + 1;
                    zunglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[
                            iwork], &i__3, &ierr);
                    ie = 1;
                    itauq = itau;
                    itaup = itauq + *m;
                    iwork = itaup + *m;

/*                 Bidiagonalize L in U, copying result to WORK(IR) */
/*                 (CWorkspace: need M*M+3*M, prefer M*M+2*M+2*M*NB) */
/*                 (RWorkspace: need M) */

                    i__3 = *lwork - iwork + 1;
                    zgebrd_(m, m, &u[u_offset], ldu, &s[1], &rwork[ie], &work[
                            itauq], &work[itaup], &work[iwork], &i__3, &ierr);
                    zlacpy_("U", m, m, &u[u_offset], ldu, &work[ir], &ldwrkr, 
                            (ftnlen)1);

/*                 Generate right vectors bidiagonalizing L in WORK(IR) */
/*                 (CWorkspace: need M*M+3*M-1, prefer M*M+2*M+(M-1)*NB) */
/*                 (RWorkspace: 0) */

                    i__3 = *lwork - iwork + 1;
                    zungbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup], &
                            work[iwork], &i__3, &ierr, (ftnlen)1);

/*                 Generate left vectors bidiagonalizing L in U */
/*                 (CWorkspace: need M*M+3*M, prefer M*M+2*M+M*NB) */
/*                 (RWorkspace: 0) */

                    i__3 = *lwork - iwork + 1;
                    zungbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &
                            work[iwork], &i__3, &ierr, (ftnlen)1);
                    irwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of L in U, and computing right */
/*                 singular vectors of L in WORK(IR) */
/*                 (CWorkspace: need M*M) */
/*                 (RWorkspace: need BDSPAC) */

                    zbdsqr_("U", m, m, m, &c__0, &s[1], &rwork[ie], &work[ir],
                             &ldwrkr, &u[u_offset], ldu, cdum, &c__1, &rwork[
                            irwork], info, (ftnlen)1);
                    iu = itauq;

/*                 Multiply right singular vectors of L in WORK(IR) by Q */
/*                 in A, storing result in WORK(IU) and copying to A */
/*                 (CWorkspace: need M*M+M, prefer M*M+M*N)) */
/*                 (RWorkspace: 0) */

                    i__3 = *n;
                    i__2 = chunk;
                    for (i__ = 1; i__2 < 0 ? i__ >= i__3 : i__ <= i__3; i__ +=
                             i__2) {
/* Computing MIN */
                        i__4 = *n - i__ + 1;
                        blk = MIN(i__4,chunk);
                        zgemm_("N", "N", m, &blk, m, &c_b2, &work[ir], &
                                ldwrkr, &a[i__ * a_dim1 + 1], lda, &c_b1, &
                                work[iu], &ldwrku, (ftnlen)1, (ftnlen)1);
                        zlacpy_("F", m, &blk, &work[iu], &ldwrku, &a[i__ * 
                                a_dim1 + 1], lda, (ftnlen)1);
/* L40: */
                    }

                } else {

/*                 Insufficient workspace for a fast algorithm */

                    itau = 1;
                    iwork = itau + *m;

/*                 Compute A=L*Q */
/*                 (CWorkspace: need 2*M, prefer M+M*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
                            , &i__2, &ierr);

/*                 Copy L to U, zeroing out above it */

                    zlacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], ldu, (
                            ftnlen)1);
                    i__2 = *m - 1;
                    i__3 = *m - 1;
                    zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &u[(u_dim1 << 1) 
                            + 1], ldu, (ftnlen)1);

/*                 Generate Q in A */
/*                 (CWorkspace: need 2*M, prefer M+M*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zunglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[
                            iwork], &i__2, &ierr);
                    ie = 1;
                    itauq = itau;
                    itaup = itauq + *m;
                    iwork = itaup + *m;

/*                 Bidiagonalize L in U */
/*                 (CWorkspace: need 3*M, prefer 2*M+2*M*NB) */
/*                 (RWorkspace: need M) */

                    i__2 = *lwork - iwork + 1;
                    zgebrd_(m, m, &u[u_offset], ldu, &s[1], &rwork[ie], &work[
                            itauq], &work[itaup], &work[iwork], &i__2, &ierr);

/*                 Multiply right vectors bidiagonalizing L by Q in A */
/*                 (CWorkspace: need 2*M+N, prefer 2*M+N*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zunmbr_("P", "L", "C", m, n, m, &u[u_offset], ldu, &work[
                            itaup], &a[a_offset], lda, &work[iwork], &i__2, &
                            ierr, (ftnlen)1, (ftnlen)1, (ftnlen)1);

/*                 Generate left vectors bidiagonalizing L in U */
/*                 (CWorkspace: need 3*M, prefer 2*M+M*NB) */
/*                 (RWorkspace: 0) */

                    i__2 = *lwork - iwork + 1;
                    zungbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &
                            work[iwork], &i__2, &ierr, (ftnlen)1);
                    irwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of A in U and computing right */
/*                 singular vectors of A in A */
/*                 (CWorkspace: 0) */
/*                 (RWorkspace: need BDSPAC) */

                    zbdsqr_("U", m, n, m, &c__0, &s[1], &rwork[ie], &a[
                            a_offset], lda, &u[u_offset], ldu, cdum, &c__1, &
                            rwork[irwork], info, (ftnlen)1);

                }

            } else if (wntvs) {

                if (wntun) {

/*                 Path 4t(N much larger than M, JOBU='N', JOBVT='S') */
/*                 M right singular vectors to be computed in VT and */
/*                 no left singular vectors to be computed */

                    if (*lwork >= *m * *m + *m * 3) {

/*                    Sufficient workspace for a fast algorithm */

                        ir = 1;
                        if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IR) is LDA by M */

                            ldwrkr = *lda;
                        } else {

/*                       WORK(IR) is M by M */

                            ldwrkr = *m;
                        }
                        itau = ir + ldwrkr * *m;
                        iwork = itau + *m;

/*                    Compute A=L*Q */
/*                    (CWorkspace: need M*M+2*M, prefer M*M+M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);

/*                    Copy L to WORK(IR), zeroing out above it */

                        zlacpy_("L", m, m, &a[a_offset], lda, &work[ir], &
                                ldwrkr, (ftnlen)1);
                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &work[ir + 
                                ldwrkr], &ldwrkr, (ftnlen)1);

/*                    Generate Q in A */
/*                    (CWorkspace: need M*M+2*M, prefer M*M+M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(m, n, m, &a[a_offset], lda, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IR) */
/*                    (CWorkspace: need M*M+3*M, prefer M*M+2*M+2*M*NB) */
/*                    (RWorkspace: need M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &work[ir], &ldwrkr, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Generate right vectors bidiagonalizing L in */
/*                    WORK(IR) */
/*                    (CWorkspace: need M*M+3*M, prefer M*M+2*M+(M-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right */
/*                    singular vectors of L in WORK(IR) */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, m, &c__0, &c__0, &s[1], &rwork[ie], &
                                work[ir], &ldwrkr, cdum, &c__1, cdum, &c__1, &
                                rwork[irwork], info, (ftnlen)1);

/*                    Multiply right singular vectors of L in WORK(IR) by */
/*                    Q in A, storing result in VT */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, m, &c_b2, &work[ir], &ldwrkr, &
                                a[a_offset], lda, &c_b1, &vt[vt_offset], ldvt,
                                 (ftnlen)1, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *m;

/*                    Compute A=L*Q */
/*                    (CWorkspace: need 2*M, prefer M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);

/*                    Copy result to VT */

                        zlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

/*                    Generate Q in VT */
/*                    (CWorkspace: need 2*M, prefer M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(m, n, m, &vt[vt_offset], ldvt, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Zero out above L in A */

                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &a[(a_dim1 <<
                                 1) + 1], lda, (ftnlen)1);

/*                    Bidiagonalize L in A */
/*                    (CWorkspace: need 3*M, prefer 2*M+2*M*NB) */
/*                    (RWorkspace: need M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &a[a_offset], lda, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply right vectors bidiagonalizing L by Q in VT */
/*                    (CWorkspace: need 2*M+N, prefer 2*M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("P", "L", "C", m, n, m, &a[a_offset], lda, &
                                work[itaup], &vt[vt_offset], ldvt, &work[
                                iwork], &i__2, &ierr, (ftnlen)1, (ftnlen)1, (
                                ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right */
/*                    singular vectors of A in VT */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, n, &c__0, &c__0, &s[1], &rwork[ie], &
                                vt[vt_offset], ldvt, cdum, &c__1, cdum, &c__1,
                                 &rwork[irwork], info, (ftnlen)1);

                    }

                } else if (wntuo) {

/*                 Path 5t(N much larger than M, JOBU='O', JOBVT='S') */
/*                 M right singular vectors to be computed in VT and */
/*                 M left singular vectors to be overwritten on A */

                    if (*lwork >= (*m << 1) * *m + *m * 3) {

/*                    Sufficient workspace for a fast algorithm */

                        iu = 1;
                        if (*lwork >= wrkbl + (*lda << 1) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is LDA by M */

                            ldwrku = *lda;
                            ir = iu + ldwrku * *m;
                            ldwrkr = *lda;
                        } else if (*lwork >= wrkbl + (*lda + *m) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is M by M */

                            ldwrku = *lda;
                            ir = iu + ldwrku * *m;
                            ldwrkr = *m;
                        } else {

/*                       WORK(IU) is M by M and WORK(IR) is M by M */

                            ldwrku = *m;
                            ir = iu + ldwrku * *m;
                            ldwrkr = *m;
                        }
                        itau = ir + ldwrkr * *m;
                        iwork = itau + *m;

/*                    Compute A=L*Q */
/*                    (CWorkspace: need 2*M*M+2*M, prefer 2*M*M+M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out below it */

                        zlacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
                                ldwrku, (ftnlen)1);
                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &work[iu + 
                                ldwrku], &ldwrku, (ftnlen)1);

/*                    Generate Q in A */
/*                    (CWorkspace: need 2*M*M+2*M, prefer 2*M*M+M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(m, n, m, &a[a_offset], lda, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to */
/*                    WORK(IR) */
/*                    (CWorkspace: need   2*M*M+3*M, */
/*                                 prefer 2*M*M+2*M+2*M*NB) */
/*                    (RWorkspace: need   M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &work[iu], &ldwrku, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);
                        zlacpy_("L", m, m, &work[iu], &ldwrku, &work[ir], &
                                ldwrkr, (ftnlen)1);

/*                    Generate right bidiagonalizing vectors in WORK(IU) */
/*                    (CWorkspace: need   2*M*M+3*M-1, */
/*                                 prefer 2*M*M+2*M+(M-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);

/*                    Generate left bidiagonalizing vectors in WORK(IR) */
/*                    (CWorkspace: need 2*M*M+3*M, prefer 2*M*M+2*M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", m, m, m, &work[ir], &ldwrkr, &work[itauq]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of L in WORK(IR) and computing */
/*                    right singular vectors of L in WORK(IU) */
/*                    (CWorkspace: need 2*M*M) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, m, m, &c__0, &s[1], &rwork[ie], &work[
                                iu], &ldwrku, &work[ir], &ldwrkr, cdum, &c__1,
                                 &rwork[irwork], info, (ftnlen)1);

/*                    Multiply right singular vectors of L in WORK(IU) by */
/*                    Q in A, storing result in VT */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, m, &c_b2, &work[iu], &ldwrku, &
                                a[a_offset], lda, &c_b1, &vt[vt_offset], ldvt,
                                 (ftnlen)1, (ftnlen)1);

/*                    Copy left singular vectors of L to A */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: 0) */

                        zlacpy_("F", m, m, &work[ir], &ldwrkr, &a[a_offset], 
                                lda, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (CWorkspace: need 2*M, prefer M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

/*                    Generate Q in VT */
/*                    (CWorkspace: need 2*M, prefer M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(m, n, m, &vt[vt_offset], ldvt, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Zero out above L in A */

                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &a[(a_dim1 <<
                                 1) + 1], lda, (ftnlen)1);

/*                    Bidiagonalize L in A */
/*                    (CWorkspace: need 3*M, prefer 2*M+2*M*NB) */
/*                    (RWorkspace: need M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &a[a_offset], lda, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply right vectors bidiagonalizing L by Q in VT */
/*                    (CWorkspace: need 2*M+N, prefer 2*M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("P", "L", "C", m, n, m, &a[a_offset], lda, &
                                work[itaup], &vt[vt_offset], ldvt, &work[
                                iwork], &i__2, &ierr, (ftnlen)1, (ftnlen)1, (
                                ftnlen)1);

/*                    Generate left bidiagonalizing vectors of L in A */
/*                    (CWorkspace: need 3*M, prefer 2*M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", m, m, m, &a[a_offset], lda, &work[itauq],
                                 &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in A and computing right */
/*                    singular vectors of A in VT */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, n, m, &c__0, &s[1], &rwork[ie], &vt[
                                vt_offset], ldvt, &a[a_offset], lda, cdum, &
                                c__1, &rwork[irwork], info, (ftnlen)1);

                    }

                } else if (wntuas) {

/*                 Path 6t(N much larger than M, JOBU='S' or 'A', */
/*                         JOBVT='S') */
/*                 M right singular vectors to be computed in VT and */
/*                 M left singular vectors to be computed in U */

                    if (*lwork >= *m * *m + *m * 3) {

/*                    Sufficient workspace for a fast algorithm */

                        iu = 1;
                        if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IU) is LDA by N */

                            ldwrku = *lda;
                        } else {

/*                       WORK(IU) is LDA by M */

                            ldwrku = *m;
                        }
                        itau = iu + ldwrku * *m;
                        iwork = itau + *m;

/*                    Compute A=L*Q */
/*                    (CWorkspace: need M*M+2*M, prefer M*M+M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out above it */

                        zlacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
                                ldwrku, (ftnlen)1);
                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &work[iu + 
                                ldwrku], &ldwrku, (ftnlen)1);

/*                    Generate Q in A */
/*                    (CWorkspace: need M*M+2*M, prefer M*M+M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(m, n, m, &a[a_offset], lda, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to U */
/*                    (CWorkspace: need M*M+3*M, prefer M*M+2*M+2*M*NB) */
/*                    (RWorkspace: need M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &work[iu], &ldwrku, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);
                        zlacpy_("L", m, m, &work[iu], &ldwrku, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Generate right bidiagonalizing vectors in WORK(IU) */
/*                    (CWorkspace: need   M*M+3*M-1, */
/*                                 prefer M*M+2*M+(M-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);

/*                    Generate left bidiagonalizing vectors in U */
/*                    (CWorkspace: need M*M+3*M, prefer M*M+2*M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
                                 &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of L in U and computing right */
/*                    singular vectors of L in WORK(IU) */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, m, m, &c__0, &s[1], &rwork[ie], &work[
                                iu], &ldwrku, &u[u_offset], ldu, cdum, &c__1, 
                                &rwork[irwork], info, (ftnlen)1);

/*                    Multiply right singular vectors of L in WORK(IU) by */
/*                    Q in A, storing result in VT */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, m, &c_b2, &work[iu], &ldwrku, &
                                a[a_offset], lda, &c_b1, &vt[vt_offset], ldvt,
                                 (ftnlen)1, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (CWorkspace: need 2*M, prefer M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

/*                    Generate Q in VT */
/*                    (CWorkspace: need 2*M, prefer M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(m, n, m, &vt[vt_offset], ldvt, &work[itau], &
                                work[iwork], &i__2, &ierr);

/*                    Copy L to U, zeroing out above it */

                        zlacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);
                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &u[(u_dim1 <<
                                 1) + 1], ldu, (ftnlen)1);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Bidiagonalize L in U */
/*                    (CWorkspace: need 3*M, prefer 2*M+2*M*NB) */
/*                    (RWorkspace: need M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &u[u_offset], ldu, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in U by Q */
/*                    in VT */
/*                    (CWorkspace: need 2*M+N, prefer 2*M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("P", "L", "C", m, n, m, &u[u_offset], ldu, &
                                work[itaup], &vt[vt_offset], ldvt, &work[
                                iwork], &i__2, &ierr, (ftnlen)1, (ftnlen)1, (
                                ftnlen)1);

/*                    Generate left bidiagonalizing vectors in U */
/*                    (CWorkspace: need 3*M, prefer 2*M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
                                 &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in VT */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, n, m, &c__0, &s[1], &rwork[ie], &vt[
                                vt_offset], ldvt, &u[u_offset], ldu, cdum, &
                                c__1, &rwork[irwork], info, (ftnlen)1);

                    }

                }

            } else if (wntva) {

                if (wntun) {

/*                 Path 7t(N much larger than M, JOBU='N', JOBVT='A') */
/*                 N right singular vectors to be computed in VT and */
/*                 no left singular vectors to be computed */

/* Computing MAX */
                    i__2 = *n + *m, i__3 = *m * 3;
                    if (*lwork >= *m * *m + MAX(i__2,i__3)) {

/*                    Sufficient workspace for a fast algorithm */

                        ir = 1;
                        if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IR) is LDA by M */

                            ldwrkr = *lda;
                        } else {

/*                       WORK(IR) is M by M */

                            ldwrkr = *m;
                        }
                        itau = ir + ldwrkr * *m;
                        iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (CWorkspace: need M*M+2*M, prefer M*M+M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

/*                    Copy L to WORK(IR), zeroing out above it */

                        zlacpy_("L", m, m, &a[a_offset], lda, &work[ir], &
                                ldwrkr, (ftnlen)1);
                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &work[ir + 
                                ldwrkr], &ldwrkr, (ftnlen)1);

/*                    Generate Q in VT */
/*                    (CWorkspace: need M*M+M+N, prefer M*M+M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IR) */
/*                    (CWorkspace: need M*M+3*M, prefer M*M+2*M+2*M*NB) */
/*                    (RWorkspace: need M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &work[ir], &ldwrkr, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in WORK(IR) */
/*                    (CWorkspace: need   M*M+3*M-1, */
/*                                 prefer M*M+2*M+(M-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right */
/*                    singular vectors of L in WORK(IR) */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, m, &c__0, &c__0, &s[1], &rwork[ie], &
                                work[ir], &ldwrkr, cdum, &c__1, cdum, &c__1, &
                                rwork[irwork], info, (ftnlen)1);

/*                    Multiply right singular vectors of L in WORK(IR) by */
/*                    Q in VT, storing result in A */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, m, &c_b2, &work[ir], &ldwrkr, &
                                vt[vt_offset], ldvt, &c_b1, &a[a_offset], lda,
                                 (ftnlen)1, (ftnlen)1);

/*                    Copy right singular vectors of A from A to VT */

                        zlacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (CWorkspace: need 2*M, prefer M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

/*                    Generate Q in VT */
/*                    (CWorkspace: need M+N, prefer M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Zero out above L in A */

                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &a[(a_dim1 <<
                                 1) + 1], lda, (ftnlen)1);

/*                    Bidiagonalize L in A */
/*                    (CWorkspace: need 3*M, prefer 2*M+2*M*NB) */
/*                    (RWorkspace: need M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &a[a_offset], lda, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in A by Q */
/*                    in VT */
/*                    (CWorkspace: need 2*M+N, prefer 2*M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("P", "L", "C", m, n, m, &a[a_offset], lda, &
                                work[itaup], &vt[vt_offset], ldvt, &work[
                                iwork], &i__2, &ierr, (ftnlen)1, (ftnlen)1, (
                                ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right */
/*                    singular vectors of A in VT */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, n, &c__0, &c__0, &s[1], &rwork[ie], &
                                vt[vt_offset], ldvt, cdum, &c__1, cdum, &c__1,
                                 &rwork[irwork], info, (ftnlen)1);

                    }

                } else if (wntuo) {

/*                 Path 8t(N much larger than M, JOBU='O', JOBVT='A') */
/*                 N right singular vectors to be computed in VT and */
/*                 M left singular vectors to be overwritten on A */

/* Computing MAX */
                    i__2 = *n + *m, i__3 = *m * 3;
                    if (*lwork >= (*m << 1) * *m + MAX(i__2,i__3)) {

/*                    Sufficient workspace for a fast algorithm */

                        iu = 1;
                        if (*lwork >= wrkbl + (*lda << 1) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is LDA by M */

                            ldwrku = *lda;
                            ir = iu + ldwrku * *m;
                            ldwrkr = *lda;
                        } else if (*lwork >= wrkbl + (*lda + *m) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is M by M */

                            ldwrku = *lda;
                            ir = iu + ldwrku * *m;
                            ldwrkr = *m;
                        } else {

/*                       WORK(IU) is M by M and WORK(IR) is M by M */

                            ldwrku = *m;
                            ir = iu + ldwrku * *m;
                            ldwrkr = *m;
                        }
                        itau = ir + ldwrkr * *m;
                        iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (CWorkspace: need 2*M*M+2*M, prefer 2*M*M+M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

/*                    Generate Q in VT */
/*                    (CWorkspace: need 2*M*M+M+N, prefer 2*M*M+M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
                                work[iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out above it */

                        zlacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
                                ldwrku, (ftnlen)1);
                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &work[iu + 
                                ldwrku], &ldwrku, (ftnlen)1);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to */
/*                    WORK(IR) */
/*                    (CWorkspace: need   2*M*M+3*M, */
/*                                 prefer 2*M*M+2*M+2*M*NB) */
/*                    (RWorkspace: need   M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &work[iu], &ldwrku, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);
                        zlacpy_("L", m, m, &work[iu], &ldwrku, &work[ir], &
                                ldwrkr, (ftnlen)1);

/*                    Generate right bidiagonalizing vectors in WORK(IU) */
/*                    (CWorkspace: need   2*M*M+3*M-1, */
/*                                 prefer 2*M*M+2*M+(M-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);

/*                    Generate left bidiagonalizing vectors in WORK(IR) */
/*                    (CWorkspace: need 2*M*M+3*M, prefer 2*M*M+2*M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", m, m, m, &work[ir], &ldwrkr, &work[itauq]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of L in WORK(IR) and computing */
/*                    right singular vectors of L in WORK(IU) */
/*                    (CWorkspace: need 2*M*M) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, m, m, &c__0, &s[1], &rwork[ie], &work[
                                iu], &ldwrku, &work[ir], &ldwrkr, cdum, &c__1,
                                 &rwork[irwork], info, (ftnlen)1);

/*                    Multiply right singular vectors of L in WORK(IU) by */
/*                    Q in VT, storing result in A */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, m, &c_b2, &work[iu], &ldwrku, &
                                vt[vt_offset], ldvt, &c_b1, &a[a_offset], lda,
                                 (ftnlen)1, (ftnlen)1);

/*                    Copy right singular vectors of A from A to VT */

                        zlacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

/*                    Copy left singular vectors of A from WORK(IR) to A */

                        zlacpy_("F", m, m, &work[ir], &ldwrkr, &a[a_offset], 
                                lda, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (CWorkspace: need 2*M, prefer M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

/*                    Generate Q in VT */
/*                    (CWorkspace: need M+N, prefer M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
                                work[iwork], &i__2, &ierr);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Zero out above L in A */

                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &a[(a_dim1 <<
                                 1) + 1], lda, (ftnlen)1);

/*                    Bidiagonalize L in A */
/*                    (CWorkspace: need 3*M, prefer 2*M+2*M*NB) */
/*                    (RWorkspace: need M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &a[a_offset], lda, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in A by Q */
/*                    in VT */
/*                    (CWorkspace: need 2*M+N, prefer 2*M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("P", "L", "C", m, n, m, &a[a_offset], lda, &
                                work[itaup], &vt[vt_offset], ldvt, &work[
                                iwork], &i__2, &ierr, (ftnlen)1, (ftnlen)1, (
                                ftnlen)1);

/*                    Generate left bidiagonalizing vectors in A */
/*                    (CWorkspace: need 3*M, prefer 2*M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", m, m, m, &a[a_offset], lda, &work[itauq],
                                 &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in A and computing right */
/*                    singular vectors of A in VT */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, n, m, &c__0, &s[1], &rwork[ie], &vt[
                                vt_offset], ldvt, &a[a_offset], lda, cdum, &
                                c__1, &rwork[irwork], info, (ftnlen)1);

                    }

                } else if (wntuas) {

/*                 Path 9t(N much larger than M, JOBU='S' or 'A', */
/*                         JOBVT='A') */
/*                 N right singular vectors to be computed in VT and */
/*                 M left singular vectors to be computed in U */

/* Computing MAX */
                    i__2 = *n + *m, i__3 = *m * 3;
                    if (*lwork >= *m * *m + MAX(i__2,i__3)) {

/*                    Sufficient workspace for a fast algorithm */

                        iu = 1;
                        if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IU) is LDA by M */

                            ldwrku = *lda;
                        } else {

/*                       WORK(IU) is M by M */

                            ldwrku = *m;
                        }
                        itau = iu + ldwrku * *m;
                        iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (CWorkspace: need M*M+2*M, prefer M*M+M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

/*                    Generate Q in VT */
/*                    (CWorkspace: need M*M+M+N, prefer M*M+M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
                                work[iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out above it */

                        zlacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
                                ldwrku, (ftnlen)1);
                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &work[iu + 
                                ldwrku], &ldwrku, (ftnlen)1);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to U */
/*                    (CWorkspace: need M*M+3*M, prefer M*M+2*M+2*M*NB) */
/*                    (RWorkspace: need M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &work[iu], &ldwrku, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);
                        zlacpy_("L", m, m, &work[iu], &ldwrku, &u[u_offset], 
                                ldu, (ftnlen)1);

/*                    Generate right bidiagonalizing vectors in WORK(IU) */
/*                    (CWorkspace: need M*M+3*M, prefer M*M+2*M+(M-1)*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
                                , &work[iwork], &i__2, &ierr, (ftnlen)1);

/*                    Generate left bidiagonalizing vectors in U */
/*                    (CWorkspace: need M*M+3*M, prefer M*M+2*M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
                                 &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of L in U and computing right */
/*                    singular vectors of L in WORK(IU) */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, m, m, &c__0, &s[1], &rwork[ie], &work[
                                iu], &ldwrku, &u[u_offset], ldu, cdum, &c__1, 
                                &rwork[irwork], info, (ftnlen)1);

/*                    Multiply right singular vectors of L in WORK(IU) by */
/*                    Q in VT, storing result in A */
/*                    (CWorkspace: need M*M) */
/*                    (RWorkspace: 0) */

                        zgemm_("N", "N", m, n, m, &c_b2, &work[iu], &ldwrku, &
                                vt[vt_offset], ldvt, &c_b1, &a[a_offset], lda,
                                 (ftnlen)1, (ftnlen)1);

/*                    Copy right singular vectors of A from A to VT */

                        zlacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

                    } else {

/*                    Insufficient workspace for a fast algorithm */

                        itau = 1;
                        iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (CWorkspace: need 2*M, prefer M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
                                iwork], &i__2, &ierr);
                        zlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], 
                                ldvt, (ftnlen)1);

/*                    Generate Q in VT */
/*                    (CWorkspace: need M+N, prefer M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
                                work[iwork], &i__2, &ierr);

/*                    Copy L to U, zeroing out above it */

                        zlacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], 
                                ldu, (ftnlen)1);
                        i__2 = *m - 1;
                        i__3 = *m - 1;
                        zlaset_("U", &i__2, &i__3, &c_b1, &c_b1, &u[(u_dim1 <<
                                 1) + 1], ldu, (ftnlen)1);
                        ie = 1;
                        itauq = itau;
                        itaup = itauq + *m;
                        iwork = itaup + *m;

/*                    Bidiagonalize L in U */
/*                    (CWorkspace: need 3*M, prefer 2*M+2*M*NB) */
/*                    (RWorkspace: need M) */

                        i__2 = *lwork - iwork + 1;
                        zgebrd_(m, m, &u[u_offset], ldu, &s[1], &rwork[ie], &
                                work[itauq], &work[itaup], &work[iwork], &
                                i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in U by Q */
/*                    in VT */
/*                    (CWorkspace: need 2*M+N, prefer 2*M+N*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zunmbr_("P", "L", "C", m, n, m, &u[u_offset], ldu, &
                                work[itaup], &vt[vt_offset], ldvt, &work[
                                iwork], &i__2, &ierr, (ftnlen)1, (ftnlen)1, (
                                ftnlen)1);

/*                    Generate left bidiagonalizing vectors in U */
/*                    (CWorkspace: need 3*M, prefer 2*M+M*NB) */
/*                    (RWorkspace: 0) */

                        i__2 = *lwork - iwork + 1;
                        zungbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
                                 &work[iwork], &i__2, &ierr, (ftnlen)1);
                        irwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in VT */
/*                    (CWorkspace: 0) */
/*                    (RWorkspace: need BDSPAC) */

                        zbdsqr_("U", m, n, m, &c__0, &s[1], &rwork[ie], &vt[
                                vt_offset], ldvt, &u[u_offset], ldu, cdum, &
                                c__1, &rwork[irwork], info, (ftnlen)1);

                    }

                }

            }

        } else {

/*           N .LT. MNTHR */

/*           Path 10t(N greater than M, but not much larger) */
/*           Reduce to bidiagonal form without LQ decomposition */

            ie = 1;
            itauq = 1;
            itaup = itauq + *m;
            iwork = itaup + *m;

/*           Bidiagonalize A */
/*           (CWorkspace: need 2*M+N, prefer 2*M+(M+N)*NB) */
/*           (RWorkspace: M) */

            i__2 = *lwork - iwork + 1;
            zgebrd_(m, n, &a[a_offset], lda, &s[1], &rwork[ie], &work[itauq], 
                    &work[itaup], &work[iwork], &i__2, &ierr);
            if (wntuas) {

/*              If left singular vectors desired in U, copy result to U */
/*              and generate left bidiagonalizing vectors in U */
/*              (CWorkspace: need 3*M-1, prefer 2*M+(M-1)*NB) */
/*              (RWorkspace: 0) */

                zlacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], ldu, (
                        ftnlen)1);
                i__2 = *lwork - iwork + 1;
                zungbr_("Q", m, m, n, &u[u_offset], ldu, &work[itauq], &work[
                        iwork], &i__2, &ierr, (ftnlen)1);
            }
            if (wntvas) {

/*              If right singular vectors desired in VT, copy result to */
/*              VT and generate right bidiagonalizing vectors in VT */
/*              (CWorkspace: need 2*M+NRVT, prefer 2*M+NRVT*NB) */
/*              (RWorkspace: 0) */

                zlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt, (
                        ftnlen)1);
                if (wntva) {
                    nrvt = *n;
                }
                if (wntvs) {
                    nrvt = *m;
                }
                i__2 = *lwork - iwork + 1;
                zungbr_("P", &nrvt, n, m, &vt[vt_offset], ldvt, &work[itaup], 
                        &work[iwork], &i__2, &ierr, (ftnlen)1);
            }
            if (wntuo) {

/*              If left singular vectors desired in A, generate left */
/*              bidiagonalizing vectors in A */
/*              (CWorkspace: need 3*M-1, prefer 2*M+(M-1)*NB) */
/*              (RWorkspace: 0) */

                i__2 = *lwork - iwork + 1;
                zungbr_("Q", m, m, n, &a[a_offset], lda, &work[itauq], &work[
                        iwork], &i__2, &ierr, (ftnlen)1);
            }
            if (wntvo) {

/*              If right singular vectors desired in A, generate right */
/*              bidiagonalizing vectors in A */
/*              (CWorkspace: need 3*M, prefer 2*M+M*NB) */
/*              (RWorkspace: 0) */

                i__2 = *lwork - iwork + 1;
                zungbr_("P", m, n, m, &a[a_offset], lda, &work[itaup], &work[
                        iwork], &i__2, &ierr, (ftnlen)1);
            }
            irwork = ie + *m;
            if (wntuas || wntuo) {
                nru = *m;
            }
            if (wntun) {
                nru = 0;
            }
            if (wntvas || wntvo) {
                ncvt = *n;
            }
            if (wntvn) {
                ncvt = 0;
            }
            if (! wntuo && ! wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in U and computing right singular */
/*              vectors in VT */
/*              (CWorkspace: 0) */
/*              (RWorkspace: need BDSPAC) */

                zbdsqr_("L", m, &ncvt, &nru, &c__0, &s[1], &rwork[ie], &vt[
                        vt_offset], ldvt, &u[u_offset], ldu, cdum, &c__1, &
                        rwork[irwork], info, (ftnlen)1);
            } else if (! wntuo && wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in U and computing right singular */
/*              vectors in A */
/*              (CWorkspace: 0) */
/*              (RWorkspace: need BDSPAC) */

                zbdsqr_("L", m, &ncvt, &nru, &c__0, &s[1], &rwork[ie], &a[
                        a_offset], lda, &u[u_offset], ldu, cdum, &c__1, &
                        rwork[irwork], info, (ftnlen)1);
            } else {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in A and computing right singular */
/*              vectors in VT */
/*              (CWorkspace: 0) */
/*              (RWorkspace: need BDSPAC) */

                zbdsqr_("L", m, &ncvt, &nru, &c__0, &s[1], &rwork[ie], &vt[
                        vt_offset], ldvt, &a[a_offset], lda, cdum, &c__1, &
                        rwork[irwork], info, (ftnlen)1);
            }

        }

    }

/*     Undo scaling if necessary */

    if (iscl == 1) {
        if (anrm > bignum) {
            dlascl_("G", &c__0, &c__0, &bignum, &anrm, &minmn, &c__1, &s[1], &
                    minmn, &ierr, (ftnlen)1);
        }
        if (*info != 0 && anrm > bignum) {
            i__2 = minmn - 1;
            dlascl_("G", &c__0, &c__0, &bignum, &anrm, &i__2, &c__1, &rwork[
                    ie], &minmn, &ierr, (ftnlen)1);
        }
        if (anrm < smlnum) {
            dlascl_("G", &c__0, &c__0, &smlnum, &anrm, &minmn, &c__1, &s[1], &
                    minmn, &ierr, (ftnlen)1);
        }
        if (*info != 0 && anrm < smlnum) {
            i__2 = minmn - 1;
            dlascl_("G", &c__0, &c__0, &smlnum, &anrm, &i__2, &c__1, &rwork[
                    ie], &minmn, &ierr, (ftnlen)1);
        }
    }

/*     Return optimal workspace in WORK(1) */

    work[1].r = (doublereal) maxwrk, work[1].i = 0.;

    return 0;

/*     End of ZGESVD */

} /* zgesvd_ */
