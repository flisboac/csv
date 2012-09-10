#include <stdlib.h>
#include <stdio.h>
#include <String.h>

#define DEFAULT_ALLOC realloc
#define DEFAULT_STRING_CAP 2
#define DEFAULT_COLSEP ','
#define DEFAULT_LINESEP '\n'

struct CsvString_t {

	size_t len;
	size_t cap;
	char* ptr;
};

struct Csv_t {

	CsvString *headers; /* [cols] */
	CsvString **values; /* [lines, cols] */
	size_t cols;
	size_t lines;
	char colsep;
	char linesep;
	Csv_alloc_f alloc;
};

typedef struct {

	FILE *file;
	const char *filename;
} OpenFileState;

typedef struct {

	char *str;
	size_t sz;
	size_t pos;
} OpenStringState;


static int
CsvString_init(Csv* C, CsvString* S) {
	int status = 1;

	S->len = 0;
	S->cap = DEFAULT_STRING_CAP;
	S->ptr = C->alloc(NULL, sizeof(char) * S->cap);
	if (!S->ptr)
		status = 0;
	else
		S->ptr[0] = '\0';

	return status;
}


static int
CsvString_isvalid(Csv *C, CsvString *S) {

	return S && S->ptr;
}


static void
CsvString_quit(Csv* C, CsvString *S) {

	if (CsvString_isvalid(C, S))
		C->alloc(S->ptr, 0);
}


static void
CsvString_swapcontent(Csv *C, CsvString *S, CsvString *O) {

	CsvString T;

	T.len = S->len;
	T.cap = S->cap;
	T.ptr = S->ptr;

	S->len = O->len;
	S->cap = O->cap;
	S->ptr = O->ptr;

	O->len = T.len;
	O->cap = T.cap;
	O->ptr = T.ptr;
}


static int
CsvString_ensurecap(Csv *C, CsvString *S, size_t cap) {

	int status = 0, newcap;
	char *ptr;

	if (CsvString_isvalid(C, S)) {

		if (S->cap < cap + 1) {
			newcap = S->cap;
			while (newcap < cap + 1) newcap *= 2;
			ptr = C->alloc(S->ptr, sizeof(char) * newcap);

			if (ptr) {
				S->ptr = ptr;
				S->cap = newcap;
				status = 1;
			}

		} else
			status = 1;
	}

	return status;
}


static int
CsvString_append(Csv* C, CsvString *S, char* c, size_t sz) {

	int status = 0, i;

	if (CsvString_isvalid(C, S) && c) {
		if (sz == 0) sz = strlen(c);

		if (sz > 0 && CsvString_ensurecap(C, S, S->len + sz)) {
			for (i = 0; i < sz; i++)
				S->ptr[S->len + i] = c[i];
			S->ptr[S->len + i] = '\0';
			S->len += sz;
		}
	}

	return status;
}


static int
CsvString_appendchar(Csv* C, CsvString *S, char c) {

	return CsvString_append(C, S, &c, 1);
}


static int
openstring(OpenStringState* state, char* c, size_t *sz) {

}


static int
openfile(OpenFileState *state, char *c, size_t *sz) {


}


/* ========================================================================== */


Csv*
Csv_openfile(const char* filename) {

	Csv *C;
	OpenFileState state;

	if (filename) {
		state->file = NULL;
		state->filename = filename;
		C = Csv_open(openfile, &state, NULL, DEFAULT_COLSEP, DEFAULT_LINESEP);
	}

	return C;
}


Csv*
Csv_openstring(const char* str, size_t sz); {

	Csv *C;
	OpenStringState state;

	if (str) {
		state->str = str;
		state->sz = sz;
		state->pos = 0;

		if (state->sz == 0)
			state->sz = strlen(str);

		C = Csv_open(openstring, &state, NULL, DEFAULT_COLSEP, DEFAULT_LINESEP);
	}

	return C;
}


Csv*
Csv_openbuffer(Csv_stream_f buf, void* bufstate) {
	return (Csv_open(buf, bufstate, NULL, DEFAULT_COLSEP, DEFAULT_LINESEP));
}


Csv*
Csv_open(Csv_buffer_f buf, void* bufstate, Csv_alloc_f alloc, char colsep, char linesep) {

	int status = CSV_OK;
	int stop = 0, col = 0;

	size_t ch_sz = 0;
	char ch;

	CsvString str;
	Csv *C = NULL;

	if (!alloc) alloc = DEFAULT_ALLOC;

	if (buf) {

		C = alloc(NULL, sizeof(Csv));
		if (!C) return NULL;

		C->headers = NULL;
		C->values = NULL;
		C->cols = 0;
		C->lines = 0;
		C->colsep = colsep;
		C->linesep = linesep;

		if (!CsvString_init(C, S)) {
			alloc(C, 0);
			return NULL;
		}

		while (!stop || status == CSV_OK) {

			ch_sz = 1;
			status = buf(bufstate, &c, &ch_sz);

			switch (status) {
				/* One character may have been returned. */
				case CSV_OK: {

					if (c == C->colsep) {
						/* Defines columns */
						
					} else if (c == C->linesep) {
						/* Defines lines */

						if (C->cols == 0) {
							/* Define columns */

						} else {
							/* Define lines. Extra columns are ignored. */

						}

					} else
						if (!CsvString_appendchar(C, S, ch))
							stop = 1;
					break;

				/* No character is returned. */
				} case CSV_EOF: {

					break;

				/* Any other value is an error. */
				} default: {

					break;
				}
			}
		}

		if (stop) {
			Csv_close(C);
		}
	}

	return C;
}


void
Csv_close(Csv* C) {
	int i, j;

	if (C) {

		if (Csv_hasheaders(C)) {
			for (i = 0; i < C->cols; i++)
				CsvString_quit(C, &(C->headers[i]));
			C->alloc(C->headers, 0);
		}

		if (Csv_hasvalues(C))
			for (i = 0; i < C->lines; i++) {
				for (j = 0; j < C->cols; i++)
					CsvString_quit(C, &(C->values[i][j]));
				C->alloc(C->values[i], 0);
			}

		C->alloc(C, 0);
	}
}


int Csv_hasvalues(Csv* C) {

	int status = 0;
	if (C && C->values && C->lines > 0 && C->cols > 0) status = 1;
	return status;
}


int
Csv_hasheaders(Csv* C) {

	int status = 0;
	if (C && C->headers && C->cols > 0) status = 1;
	return status;
}


int
Csv_isempty(Csv* C) {

	return !Csv_hasvalues(C) && !Csv_hasheaders(C);
}


const char*
Csv_get(Csv* C, size_t line, size_t col) {
	return Csv_getstring(C, line, col);
}


const char*
Csv_getheader(Csv* C, size_t col) {
	return Csv_getheaderstring(C, col);
}


const CsvString*
Csv_getstring(Csv *C, size_t line, size_t col) {

}


const CsvString*
Csv_getheaderstring(Csv *C, size_t line, size_t col) {

}


size_t
Csv_numlines(Csv* C) {

}


size_t
Csv_numcols(Csv* C) {

}


void
Csv_collect(Csv *C, Csv_collect_f fn, void *fnstate) {

}
