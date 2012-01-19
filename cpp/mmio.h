/* 
*   Matrix Market I/O library for ANSI C
*
*   See http://math.nist.gov/MatrixMarket for details.
*
*
*/

#ifndef MM_IO_H
#define MM_IO_H

#include <stdio.h>
#include <stdlib.h>

#define MM_MAX_LINE_LENGTH 1025
#define MatrixMarketBanner "%%MatrixMarket"
#define MM_MAX_TOKEN_LENGTH 64

typedef char MM_typecode[4];

char *mm_typecode_to_str(MM_typecode matcode);
char const *mm_error_to_str(int errcode);

int mm_read_banner(FILE *f, MM_typecode *matcode);
int mm_read_matrix_coordinate_size(FILE *f, int *M, int *N, int *nz);
int mm_read_tensor_coordinate_size(FILE *f, int *L, int *M, int *N, int *nz);
int mm_read_tensor_compressed_size(FILE *f, int *L, int *M, int *N, int *nz, char *orientation, int *size);
int mm_read_tensor_compressed_slice_size(FILE *f, int *L, int *M, int *N, int *nz, char *orientation, int *rn);
int mm_read_vector_array_size(FILE *f, int *N);
int mm_read_matrix_array_size(FILE *f, int *M, int *N);
int mm_read_tensor_array_size(FILE *f, int *L, int *M, int *N);

int mm_write_banner(FILE *f, MM_typecode matcode);
int mm_write_matrix_coordinate_size(FILE *f, int M, int N, int nz);
int mm_write_tensor_coordinate_size(FILE *f, int L, int M, int N, int nz);
int mm_write_tensor_compressed_size(FILE *f, int L, int M, int N, int nz, char const *orientation, int size);
int mm_write_tensor_compressed_slice_size(FILE *f, int L, int M, int N, int nz, char const *orientation, int rn);
int mm_write_tensor_jds_size(FILE *f, int L, int M, int N, int nz, char const *orientation, int kn, int tn);
int mm_write_vector_array_size(FILE *f, int N);
int mm_write_matrix_array_size(FILE *f, int M, int N);
int mm_write_tensor_array_size(FILE *f, int L, int M, int N);


/********************* MM_typecode query fucntions ***************************/

#define mm_is_vector(typecode)	   ((typecode)[0]=='V')
#define mm_is_matrix(typecode)	   ((typecode)[0]=='M')
#define mm_is_tensor(typecode)	   ((typecode)[0]=='T')

#define mm_is_sparse(typecode)	   ((typecode)[1]=='C')
#define mm_is_coordinate(typecode) ((typecode)[1]=='C')
#define mm_is_dense(typecode)	   ((typecode)[1]=='A')
#define mm_is_array(typecode)	   ((typecode)[1]=='A')
#define mm_is_compressed(typecode) ((typecode)[1]=='O')
#define mm_is_gundersen(typecode)  ((typecode)[1]=='G')
#define mm_is_slice(typecode)      ((typecode)[1]=='S')
#define mm_is_ekmr(typecode)       ((typecode)[1]=='K')
#define mm_is_zzekmr(typecode)     ((typecode)[1]=='Z')
#define mm_is_jds(typecode)        ((typecode)[1]=='J')

#define mm_is_complex(typecode)	   ((typecode)[2]=='C')
#define mm_is_real(typecode)	   ((typecode)[2]=='R')
#define mm_is_pattern(typecode)	   ((typecode)[2]=='P')
#define mm_is_integer(typecode)    ((typecode)[2]=='I')

#define mm_is_symmetric(typecode)  ((typecode)[3]=='S')
#define mm_is_general(typecode)	   ((typecode)[3]=='G')
#define mm_is_skew(typecode)	   ((typecode)[3]=='K')
#define mm_is_hermitian(typecode)  ((typecode)[3]=='H')

int mm_is_valid(MM_typecode matcode);		/* too complex for a macro */


/********************* MM_typecode modify fucntions ***************************/

#define mm_set_vector(typecode)     ((*typecode)[0]='V')
#define mm_set_matrix(typecode)     ((*typecode)[0]='M')
#define mm_set_tensor(typecode)	    ((*typecode)[0]='T')

#define mm_set_coordinate(typecode) ((*typecode)[1]='C')
#define mm_set_array(typecode)	    ((*typecode)[1]='A')
#define mm_set_dense(typecode)	    mm_set_array(typecode)
#define mm_set_sparse(typecode)	    mm_set_coordinate(typecode)
#define mm_set_compressed(typecode) ((*typecode)[1]='O')
#define mm_set_gundersen(typecode)  ((*typecode)[1]='G')
#define mm_set_slice(typecode)      ((*typecode)[1]='S')
#define mm_set_ekmr(typecode)       ((*typecode)[1]='K')
#define mm_set_zzekmr(typecode)     ((*typecode)[1]='Z')
#define mm_set_jds(typecode)        ((*typecode)[1]='J')

#define mm_set_complex(typecode)    ((*typecode)[2]='C')
#define mm_set_real(typecode)	    ((*typecode)[2]='R')
#define mm_set_pattern(typecode)    ((*typecode)[2]='P')
#define mm_set_integer(typecode)    ((*typecode)[2]='I')

#define mm_set_symmetric(typecode)  ((*typecode)[3]='S')
#define mm_set_general(typecode)    ((*typecode)[3]='G')
#define mm_set_skew(typecode)	    ((*typecode)[3]='K')
#define mm_set_hermitian(typecode)  ((*typecode)[3]='H')

#define mm_clear_typecode(typecode) ((*typecode)[0]=(*typecode)[1]=(*typecode)[2]=' ',(*typecode)[3]='G')
#define mm_initialize_typecode(typecode) mm_clear_typecode(typecode)


/********************* Matrix Market error codes ***************************/


#define MM_COULD_NOT_READ_FILE	11
#define MM_PREMATURE_EOF		12
#define MM_NOT_MTX				13
#define MM_NO_HEADER			14
#define MM_UNSUPPORTED_TYPE		15
#define MM_LINE_TOO_LONG		16
#define MM_COULD_NOT_WRITE_FILE	17

/******************** Matrix Market internal definitions ********************

   MM_matrix_typecode: 4-character sequence

				    ojbect 		sparse/   	data        storage 
						  		dense     	type        scheme

   string position:	 [0]        [1]			[2]         [3]

   Matrix typecode:  M(atrix)  C(oord)		R(eal)   	G(eneral)
						        A(array)	C(omplex)   H(ermitian)
											P(attern)   S(ymmetric)
								    		I(nteger)	K(kew)

 ***********************************************************************/

#define MM_VECTOR_STR		"vector"
#define MM_MTX_STR		"mtx"
#define MM_TENSOR_STR		"tensor"

#define MM_ARRAY_STR	        "array"
#define MM_DENSE_STR	        "array"
#define MM_COORDINATE_STR       "coordinate"
#define MM_SPARSE_STR	        "coordinate"
#define MM_COMPRESSED_STR	"compressed"
#define MM_GUNDERSEN_STR	"gundersen"
#define MM_SLICE_STR	        "slice"
#define MM_EKMR_STR	        "ekmr"
#define MM_ZZPKMR_STR	        "zzekmr"

#define MM_COMPLEX_STR	        "complex"
#define MM_REAL_STR		"real"
#define MM_INT_STR		"integer"
#define MM_GENERAL_STR          "general"
#define MM_SYMM_STR		"symmetric"
#define MM_HERM_STR		"hermitian"
#define MM_SKEW_STR		"skew-symmetric"
#define MM_PATTERN_STR          "pattern"


/*  high level routines */

int mm_write_matrix_coordinate(char fname[], int M, int N, int nz, int I[], int J[],
		 double val[], MM_typecode matcode);
int mm_read_matrix_coordinate_data(FILE *f, int M, int N, int nz, int I[], int J[],
		double val[], MM_typecode matcode);
int mm_read_matrix_coordinate_entry(FILE *f, int *I, int *J, double *real, double *img,
			MM_typecode matcode);

int mm_read_unsymmetric_sparse(const char *fname, int *M_, int *N_, int *nz_,
                double **val_, int **I_, int **J_);



#endif
