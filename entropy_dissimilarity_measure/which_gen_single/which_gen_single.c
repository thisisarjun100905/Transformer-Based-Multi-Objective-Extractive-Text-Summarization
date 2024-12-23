/****************************************************************************
**
** Copyright (C) 2017
** Indian Institute of Technology, Roorkee (www.iitr.ac.in)
** The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Entropy based Termination Criterion for 
** Multi-objective Evolutionary Algorithms
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/
#include "global.h"

int main(int argc, char *argv[]) {

    FILE *infile;
    int rows;
    double *data = NULL;
    int i;
    int n_iter, n_decimals;
    gsl_matrix* dataset;
    int ncols = 0, nruns = 0;
    int *cumsizes = NULL;

    double gen;
    double accuracy;
    double entropy_mean_double, temp1;
    double entropy_std_double, temp2;

    double entropy, sum, sumsq;

    int entropy_mean_int, previous_entropy_mean_int;
    int entropy_std_int, previous_entropy_std_int;
    int counter1;
    int counter2;

    double mean, std;
    int generation, f_eval;

    int opt; /* it's actually going to hold a char */
    int longopt_index;

    /* see the man page for getopt_long for an explanation of these fields */
    static struct option long_options[] = {
    		{"n_iter",		required_argument,	NULL,	's'},
    		{"n_decimals",	required_argument,	NULL,	'd'},
    		{"help",		no_argument,		NULL,	'h'},
    		{"version",		no_argument,		NULL,	'V'},
    		{NULL, 0, NULL, 0} /* marks end of list */
    };

    n_iter=20; n_decimals=2;

    while (0 < (opt = getopt_long(argc, argv, "s:d:Vvh", long_options, &longopt_index))) {

        switch (opt) {

        case 's': /* number of successive iterations */
        	n_iter=atoi(optarg);
        	break;
        case 'd': /* decimal accuracy */
        	n_decimals=atoi(optarg);
        	break;
        case 'V':
        case 'v':/* --version */
            version();
            exit(EXIT_SUCCESS);

        case '?':
            /* getopt prints an error message right here */
            fprintf(stderr, "Try `%s --help' for more information.\n", program_invocation_short_name);
            exit(EXIT_FAILURE);
        case 'h':
            usage();
            exit(EXIT_SUCCESS);

        default: /* should never happen */
            abort();
        }
    }

    if (optind < argc)
        for (; optind < argc; optind++)
            if (strncmp(argv[optind],"-", strlen("-")+1)) {
                infile = fopen(argv[optind],"r");
                if (infile == NULL) {
                    fprintf (stderr, "%s: %s: %s\n", program_invocation_short_name, argv[optind], strerror(errno));
                    exit(EXIT_FAILURE);
                }
                read_input(infile, argv[optind], &data, &ncols, &cumsizes, &nruns);
                fclose(infile);
            } else
                read_input(stdin, "stdin", &data, &ncols, &cumsizes, &nruns);
    else
        read_input(stdin, "stdin", &data, &ncols, &cumsizes, &nruns);


    rows = cumsizes[nruns-1];

    dataset = perform_conversion(data, ncols, rows);

    /*************/
    /* Algorithm */
    /*************/

    double* mean_t;
    double* std_t;

    mean_t = (double*) malloc(rows*sizeof(double));
    std_t = (double*) malloc(rows*sizeof(double));

    mean=0.0;
    std=0.0;
    generation=0;
    f_eval=0;
    accuracy=pow(10,n_decimals);
    counter1=0;
    counter2=0;
    entropy=0.0;
    entropy_mean_int=0;
    entropy_std_int=0;
    sum=0.0;
    sumsq=0.0;
    cout<<"t";
    for(i=0;i<rows;i++) {

    	gen=(double)i+1.0;
    	entropy=gsl_matrix_get(dataset,i,0);
    	sum+=entropy;
    	sumsq+=entropy*entropy;

    	entropy_mean_double=sum/gen;
    	entropy_std_double=sqrt( (sumsq/gen) - pow(sum/gen,2.0) );

    	mean_t[i] = entropy_mean_double;
    	std_t[i] = entropy_std_double;

    	previous_entropy_mean_int = entropy_mean_int;
    	previous_entropy_std_int = entropy_std_int;

    	/* multiply by accuracy and retrieve the integer part
    	 * ex: consider n=3.1454 and accuracy of 2 then 1) n=314.54 and 2) n=314 */
    	modf(entropy_mean_double*accuracy, &temp1);
    	modf(entropy_std_double*accuracy, &temp2);
    	entropy_mean_int = (int)temp1;
    	entropy_std_int = (int)temp2;

    	//printf("m: %f\n",entropy_mean_double);
    	//printf("s: %f\n",entropy_std_double);

    	if(previous_entropy_mean_int == entropy_mean_int) {
    		counter1++;
    	}
    	else {
    		counter1=0;
    		counter2=0;
    	}

    	if(previous_entropy_std_int == entropy_std_int) {
    		counter2++;
    	}
    	else {
    		counter1=0;
    		counter2=0;
    	}

    	/* both mean and standard deviation */
    	if( (counter1>=n_iter) && (counter2>=n_iter) ) {
    		generation=(int)gen-1;
    		f_eval=i+1-1;
    		entropy=gsl_matrix_get(dataset,i-1,0);
    		//mean=entropy_mean_double;
    		//std=entropy_std_double;
    		mean=mean_t[i-1];
    		std=std_t[i-1];

    		break;
    	}
    }

    if(i!=rows) {
    	printf("generation: %d\n",generation);
    	printf("function_eval: %d\n",f_eval);
    	printf("entropy: %f\n",entropy);
    	printf("mean: %f\n",mean);
    	printf("std: %f\n",std);
    }
    else {
    	printf("it has not converged\n");
    }

    /*************/
    /*************/
    /*************/

    free(mean_t);
    free(std_t);
    gsl_matrix_free(dataset);
    free(data);

    exit(EXIT_SUCCESS);

}

gsl_matrix* perform_conversion(const double* data, int nobj, int rows) {

    gsl_matrix* objectives;
    int n,r,k;
    k=0;
    objectives=gsl_matrix_alloc(rows,nobj);
    for(r=0; r<rows; r++) {
        for(n=0;n<nobj; n++, k++)
            gsl_matrix_set(objectives,r,n,data[k]);
    }
    return objectives;
}

void usage(void) {
    printf("\nUsage: %s [OPTIONS] [FILE...]\n\n", program_invocation_short_name);
    printf("Generation counter for entropy ref files\n\n");

    printf(	"Options:\n"
            " -h, --help        print this summary and exit.                  \n"
            " --version     	print version number and exit.                \n\n");
    return;
}

void version(void) {

    printf("%s version %s \n\n", program_invocation_short_name, VERSION);
    printf(	"Copyright (C) 2010  \nJoao Antonio Duro (j.a.duro@cranfield.ac.uk) and "
            "\nDhish Saxena (d.saxena@cranfield.ac.uk)\n\n"
            "This is free software, and you are welcome to redistribute it under certain\n"
            "conditions.  See the GNU General Public License for details. There is NO   \n"
            "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

