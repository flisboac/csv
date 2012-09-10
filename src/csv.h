#include <stddef.h>

#define CSV_EOS   (-1)
#define CSV_OK    ( 0)
#define CSV_ERROR ( 1)

typedef struct CsvString_t {

	size_t len;
	size_t cap;
	char* data;
} CsvString;

typedef struct Csv_t Csv;

/**
 * Function signature that's used to allocate memory. 
 * 
 * Its usage is the exact same as stdlib's @c realloc.
 */
typedef void* (*Csv_alloc_f)(void* ptr, size_t sz);

/**
 * Function signature used to read data from an abstract source.
 * 
 * The function receives its internal state, if any, through the @c state
 * parameter (it can, for example, hold a string pointer, or a file, that's
 * used in the read process). 
 * 
 * @c c is a standard char string, where read data will be written to.
 * The string is considered to be a mere vector, and not a strict "C" string.
 * This means that NULL-termination is effectively ignored.
 * 
 * @c c is considered to have at least @c *sz characters in allocated size.
 * @c sz must not be @c NULL, and in this case, @c *sz chars will be written
 * to @c c. If @c c is @c NULL, the stream must ignore the next @c *sz
 * characters.
 * 
 * If @c *sz equals 0, it means that no further usage for this
 * stream is expected. This can also mean closing the stream in other contexts,
 * like for example, closing the file instantly, or leaving that action for
 * a later time.
 *
 * The return value is meant to be used in trivial boolean checks.
 * A return of 1 means success, 0 means failure and -1 means end-of-stream.
 * Please refrain from returning any different values.
 */
typedef int (*Csv_stream_f)(void* state, char* c, size_t *sz);

/**
 * Function signature used in value iteration.
 *
 * For headers' iteration, line is always 0 and has no meaning.
 * For values' iteration, line starts counting from 0.
 */
typedef void (*Csv_collect_f)(void *ptr, size_t line, size_t column, const CsvString* str);

Csv* Csv_openfile(const char* filename);
Csv* Csv_openstring(const char* str, size_t sz);
Csv* Csv_openbuffer(Csv_stream_f buf, void* bufstate);
Csv* Csv_open(Csv_buffer_f buf, void* bufstate, Csv_alloc_f alloc, char colsep, char linesep);
void Csv_close(Csv* C);

int Csv_isempty(Csv* C);
int Csv_hasvalues(Csv* C);
int Csv_hasheaders(Csv* C);
const char* Csv_get(Csv* C, size_t line, size_t col);
const char* Csv_getheader(Csv* C, size_t col);
const CsvString* Csv_getstring(Csv *C, size_t line, size_t col);
const CsvString* Csv_getheaderstring(Csv *C, size_t col);
size_t Csv_numlines(Csv* C);
size_t Csv_numcols(Csv* C);
void Csv_collect(Csv *C, Csv_collect_f fn, void *fnstate);
