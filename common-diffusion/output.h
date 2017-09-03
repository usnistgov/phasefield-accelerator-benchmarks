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

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include "type.h"

/**
 \file  output.h
 \brief Declaration of output function prototypes for diffusion benchmarks
*/

/**
 \brief Read parameters from file specified on the command line
*/
void param_parser(int argc, char* argv[], int* nx, int* ny, int* nm, int* code, fp_t* dx, fp_t* dy, fp_t* D, fp_t* linStab, int* steps, int* checks);

/**
 \brief Prints timestamps and a 20-point progress bar to stdout

 Call inside the timestepping loop, near the top, e.g.
 \code
 for (int step=0; step<steps; step++) {
 	print_progress(step, steps);
 	take_a_step();
 	elapsed += dt;
 }
 \endcode
*/
void print_progress(const int step, const int steps);

/**
 \brief Writes scalar composition field to diffusion.???????.csv
*/
void write_csv(fp_t** conc, int nx, int ny, fp_t dx, fp_t dy, int step);

/**
 \brief Writes scalar composition field to diffusion.???????.png
*/
void write_png(fp_t** conc, int nx, int ny, int step);

#endif /* _OUTPUT_H_ */