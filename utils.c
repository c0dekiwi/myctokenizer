#ifndef UTILS_C_
#define UTILS_C_

#include "utils.h"

char* readentirefile(const char *filepath) {
	FILE *f;
	int rd;
	char buf[4096+1];
	char *file;
	size_t fsize;

	struct stat statbuf;
	rd = stat(filepath, &statbuf);
	if (rd < 0) {
		perror("stat");
		return nul;
	} // handle errors
	fsize = statbuf.st_size;
	printf("File size: %zu\n", fsize);

	file = malloc(fsize);
	if (!file) {
		fprintf(stderr, "out of memory\n");
		return nul;
	}

	f = fopen(filepath, "r");
	if (!f) {
		perror("fopen");
		free(file);
		return nul;
	}

	fsize = 0;
	do {
		rd = fread(buf, sizeof(*buf), 4096, f);
		if (rd < 0) { break; } // handle errors
		if (rd) {
			memcpy(&file[fsize], buf, rd);
			fsize += rd;
			file[fsize] = 0;
		}
	} while (rd);

	fclose(f);
	return file;
}

#endif
