// example program that requires intra-procedural code partition
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct passwd {
	char *pw_id;
	char *pw_dir;
	char *pw_shell;
};

struct passwd *pw_locate(const char *id) {
	FILE *fp = fopen("passwd", "r");
	if (fp == NULL) {
		fprintf(stderr, "Unable to open passwd!\n");
		return NULL;
	}
	char buf[256];
	struct passwd *pw = (struct passwd *)malloc(sizeof(struct passwd));
	if (pw == NULL) {
		fprintf(stderr, "Out of memory!\n");
		return NULL;
	}
	while (!feof(fp)) {
		if (fgets(buf, sizeof(buf), fp) == NULL) {
			fprintf(stderr, "Error reading passwd!\n");
			break;
		}
		char *p = strtok(buf, " \t");
		if (p)
			pw->pw_id = strdup(p);
		else
			continue;
		if (strcmp(pw->pw_id, id) == 0) {
			p = strtok(NULL, " \t");
			if (p)
				pw->pw_dir = strdup(p);
			else
				break;
			p = strtok(NULL, " \t");
			if (p)
				pw->pw_shell = strdup(p);
			else
				break;
			return pw;
		}
	}
	free(pw);
	return NULL; 
}

void pw_update(const struct passwd *pw) {
}

int main(int argc, char *argv[]) {
	const struct passwd __attribute__((annotate("sensitive"))) *pw;
	struct passwd newpw;
	char *fields[3];
	
	if (argc != 4) {
		printf("Usage: %s uid home_dir shell\n", argv[0]);
		exit(0);
	}

	int i;
	for (i = 1; i < argc; i++) {
		fields[i - 1] = strdup(argv[i]);
	}
	pw = pw_locate(fields[0]);
	if (pw == NULL) {
		fprintf(stderr, "Error!\n");
		return 0;
	}
	newpw = *pw;
	newpw.pw_dir = fields[1];
	newpw.pw_shell = fields[2];
	pw_update(&newpw);
	return 0;
}
