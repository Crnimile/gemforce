#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
typedef struct Gem_O gem;
#include "bboundg_utils.h"
#include "gfon.h"

void gem_print(gem *p_gem) {
	printf("Grade:\t%d\nBbound:\t%f\n\n", p_gem->grade, p_gem->bbound);
}

char gem_color(gem* p_gem) {
	if (p_gem->bbound==0) return 'r';
	else return 'b';
}

#include "print_utils.h"

void worker(int len, int output_options, char* filename)
{
	FILE* table=file_check(filename);      // file is open to read
	if (table==NULL) exit(1);              // if the file is not good we exit
	int i;
	gem* gems=malloc(len*sizeof(gem));     // if not malloc-ed 230k is the limit
	gem** pool=malloc(len*sizeof(gem*));   // if not malloc-ed 690k is the limit
	int* pool_length=malloc(len*sizeof(int));    // if not malloced 400k is the limit (win)
	pool[0]=malloc(sizeof(gem));
	gem_init(gems,1,1);
	gem_init(pool[0],1,1);
	pool_length[0]=1;
	
	int prevmax=pool_from_table(pool, pool_length, len, table);		// pool filling
	fclose(table);				// close
	if (prevmax<len-1) {
		for (i=0;i<=prevmax;++i) free(pool[i]);		// free
		free(pool);				// free
		free(pool_length);	// free
		free(gems);				// free
		if (prevmax>0) printf("Table stops at %d, not %d\n",prevmax+1,len);
		exit(1);
	}
	if (!(output_options & mask_quiet)) gem_print(gems);

	for (i=1;i<len;++i) {
		int j;
		gems[i]=pool[i][0];
		for (j=1;j<pool_length[i];++j) if (gem_better(pool[i][j],gems[i])) {
			gems[i]=pool[i][j];
		}
		
		if (!(output_options & mask_quiet)) {
			printf("Value:\t%d\n",i+1);
			if (output_options & mask_info)
				printf("Growth:\t%f\n", log(gem_power(gems[i]))/log(i+1));
			if (output_options & mask_debug)
				printf("Pool:\t%d\n",pool_length[i]);
			gem_print(gems+i);
		}
	}
	
	if (output_options & mask_quiet) {    // outputs last if we never seen any
		printf("Value:\t%d\n",len);
		printf("Growth:\t%f\n", log(gem_power(gems[len-1]))/log(len));
		if (output_options & mask_debug)
			printf("Pool:\t%d\n",pool_length[len-1]);
		gem_print(gems+len-1);
	}

	gem* gemf=gems+len-1;  // gem that will be displayed

	if (output_options & mask_upto) {
		double best_growth=-INFINITY;
		int best_index=0;
		for (i=0; i<len; ++i) {
			if (log(gem_power(gems[i]))/log(i+1) > best_growth) {
				best_index=i;
				best_growth=log(gem_power(gems[i]))/log(i+1);
			}
		}
		printf("Best gem up to %d:\n\n", len);
		printf("Value:\t%d\n",best_index+1);
		printf("Growth:\t%f\n", best_growth);
		gem_print(gems+best_index);
		gemf = gems+best_index;
	}

	gem* gem_array;
	gem red;
	if (output_options & mask_red) {
		if (len < 2) printf("I could not add red!\n\n");
		else {
			int value=gem_getvalue(gemf);
			gemf = gem_putred(pool[value-1], pool_length[value-1], value, &red, &gem_array);
			printf("Gem with red added:\n\n");
			printf("Value:\t%d\n", value);    // made to work well with -u
			printf("Growth:\t%f\n", log(gem_power(*gemf))/log(value));
			gem_print(gemf);
		}
	}

	if (output_options & mask_parens) {
		printf("Compressed combining scheme:\n");
		print_parens_compressed(gemf);
		printf("\n\n");
	}
	if (output_options & mask_tree) {
		printf("Gem tree:\n");
		print_tree(gemf, "");
		printf("\n");
	}
	if (output_options & mask_table) print_table(gems, len);
	
	if (output_options & mask_equations) {   // it ruins gems, must be last
		printf("Equations:\n");
		print_equations(gemf);
		printf("\n");
	}
	
	for (i=0;i<len;++i) free(pool[i]);		// free
	free(pool);		// free
	free(pool_length);
	free(gems);		// free
	if (output_options & mask_red && len > 1) {
		free(gem_array);
	}
}

int main(int argc, char** argv)
{
	int len;
	char opt;
	int output_options=0;
	char filename[256]="";		// it should be enough

	while ((opt=getopt(argc,argv,"hptecidqurf:"))!=-1) {
		switch(opt) {
			case 'h':
				print_help("hptecidqurf:");
				return 0;
			PTECIDQUR_OPTIONS_BLOCK
			case 'f':
				strcpy(filename,optarg);
				break;
			case '?':
				return 1;
			default:
				break;
		}
	}
	if (optind==argc) {
		printf("No length specified\n");
		return 1;
	}
	if (optind+1==argc) {
		len = atoi(argv[optind]);
	}
	else {
		printf("Too many arguments:\n");
		while (argv[optind]!=NULL) {
			printf("%s ", argv[optind]);
			optind++;
		}
		printf("\n");
		return 1;
	}
	if (len<1) {
		printf("Improper gem number\n");
		return 1;
	}
	file_selection(filename, "table_bbound");
	worker(len, output_options, filename);
	return 0;
}

