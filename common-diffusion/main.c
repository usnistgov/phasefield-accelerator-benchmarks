/**********************************************************************************
 This file is part of Phase-field Accelerator Benchmarks, written by Trevor Keller
 and available from https://github.com/usnistgov/phasefield-accelerator-benchmarks.

 This software was developed at the National Institute of Standards and Technology
 by employees of the Federal Government in the course of their official duties.
 Pursuant to title 17 section 105 of the United States Code this software is not
 subject to copyright protection and is in the public domain. NIST assumes no
 responsibility whatsoever for the use of this software by other parties, and makes
 no guarantees, expressed or implied, about its quality, reliability, or any other
 characteristic. We would appreciate acknowledgement if the software is used.

 This software can be redistributed and/or modified freely provided that any
 derivative works bear some notice that they are derived from it, and any modified
 versions bear some notice that they have been modified.

 Questions/comments to Trevor Keller (trevor.keller@nist.gov)
 **********************************************************************************/

/**
 \file  main.c
 \brief Implementation of semi-infinite diffusion equation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "mesh.h"
#include "numerics.h"
#include "output.h"
#include "timer.h"
#include "boundaries.h"
#include "discretization.h"

/**
 \brief Run simulation using input parameters specified on the command line

 Program will write a series of PNG image files to visualize scalar composition
 field, plus a final CSV raw data file and CSV runtime log tabulating the
 iteration counter (\a iter), elapsed simulation time (\a sim_time), system free
 energy (\a energy), error relative to analytical solution (\a wrss), time spent
 performing convolution (\a conv_time), time spent updating fields (\a step_time),
 time spent writing to disk (\a IO_time), time spent generating analytical values
 (\a soln_time), and total elapsed (\a run_time).
*/
int main(int argc, char* argv[])
{
	FILE * output;

	/* declare mesh size and resolution */
	fp_t **conc_old, **conc_new, **conc_lap, **mask_lap;
	int nx=512, ny=512, nm=3, code=53;
	fp_t dx=0.5, dy=0.5, h=0.5;
	fp_t bc[2][2];

	/* declare materials and numerical parameters */
	fp_t D=0.00625, linStab=0.1, dt=1., elapsed=0., rss=0.;
	int step=0, steps=100000, checks=10000;
	double start_time=0.;
	struct Stopwatch sw = {0., 0., 0., 0.};

	StartTimer();

	param_parser(argc, argv, &nx, &ny, &nm, &code, &dx, &dy, &D, &linStab, &steps, &checks);

	h = (dx > dy) ? dy : dx;
	dt = (linStab * h * h) / (4.0 * D);

	/* initialize memory */
	make_arrays(&conc_old, &conc_new, &conc_lap, &mask_lap, nx, ny, nm);
	set_mask(dx, dy, code, mask_lap, nm);
	set_boundaries(bc);

	start_time = GetTimer();
	apply_initial_conditions(conc_old, nx, ny, nm, bc);
	sw.step = GetTimer() - start_time;

	/* write initial condition data */
	start_time = GetTimer();
	write_png(conc_old, nx, ny, 0);

	/* prepare to log comparison to analytical solution */
	output = fopen("runlog.csv", "w");
	if (output == NULL) {
		printf("Error: unable to %s for output. Check permissions.\n", "runlog.csv");
		exit(-1);
	}
	sw.file = GetTimer() - start_time;

	fprintf(output, "iter,sim_time,wrss,conv_time,step_time,IO_time,soln_time,run_time\n");
	fprintf(output, "%i,%f,%f,%f,%f,%f,%f,%f\n", step, elapsed, rss, sw.conv, sw.step, sw.file, sw.soln, GetTimer());

	/* do the work */
	for (step = 1; step < steps+1; step++) {
		print_progress(step-1, steps);

		solve_diffusion_equation(conc_old, conc_new, conc_lap, mask_lap,
		                         nx, ny, nm, bc, D, dt, &elapsed, &sw);

		if (step % checks == 0) {
			start_time = GetTimer();
			write_png(conc_new, nx, ny, step);
			sw.file += GetTimer() - start_time;
		}

		if (step % 100 == 0) {
			start_time = GetTimer();
			check_solution(conc_new, conc_lap, nx, ny, dx, dy, nm, elapsed, D, bc, &rss);
			sw.soln += GetTimer() - start_time;

			fprintf(output, "%i,%f,%f,%f,%f,%f,%f,%f\n", step, elapsed, rss, sw.conv, sw.step, sw.file, sw.soln, GetTimer());
		}

		swap_pointers(&conc_old, &conc_new);
	}

	write_csv(conc_old, nx, ny, dx, dy, steps);

	/* clean up */
	fclose(output);
	free_arrays(conc_old, conc_new, conc_lap, mask_lap);

	return 0;
}