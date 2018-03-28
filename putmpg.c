/* putmpg.c, block and motion vector encoding routines                      */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */

#include <stdio.h>
#include "global.h"

/* generate variable length codes for an intra-coded block (6.2.6, 6.3.17) */
void 
simpeg_encode_putintrablk(simpeg_encode_context * context,
                          short int *blk, int cc)
{
  int n, dct_diff, run, signed_level;

  /* DC coefficient (7.2.1) */
  dct_diff = blk[0] - context->dc_dct_pred[cc]; /* difference to previous block */
  context->dc_dct_pred[cc] = blk[0];

  if (cc==0)
    simpeg_encode_putDClum(context, dct_diff);
  else
    simpeg_encode_putDCchrom(context, dct_diff);

  /* AC coefficients (7.2.2) */
  run = 0;
  for (n=1; n<64; n++)
  {
    /* use appropriate entropy scanning pattern */
    signed_level = blk[(context->altscan ? context->alternate_scan : context->zig_zag_scan)[n]];
    if (signed_level!=0)
    {
      simpeg_encode_putAC(context, run,signed_level,context->intravlc);
      run = 0;
    }
    else
      run++; /* count zero coefficients */
  }

  /* End of Block -- normative block punctuation */
  if (context->intravlc)
    simpeg_encode_putbits(context,6,4); /* 0110 (Table B-15) */
  else
    simpeg_encode_putbits(context,2,2); /* 10 (Table B-14) */
}

/* generate variable length codes for a non-intra-coded block (6.2.6, 6.3.17) */
void 
simpeg_encode_putnonintrablk(simpeg_encode_context * context, short int *blk)
{
  int n, run, signed_level, first;

  run = 0;
  first = 1;

  for (n=0; n<64; n++)
  {
    /* use appropriate entropy scanning pattern */
    signed_level = blk[(context->altscan ? context->alternate_scan : context->zig_zag_scan)[n]];

    if (signed_level!=0)
    {
      if (first)
      {
        /* first coefficient in non-intra block */
        simpeg_encode_putACfirst(context,run,signed_level);
        first = 0;
      }
      else
        simpeg_encode_putAC(context,run,signed_level,0);

      run = 0;
    }
    else
      run++; /* count zero coefficients */
  }

  /* End of Block -- normative block punctuation  */
  simpeg_encode_putbits(context,2,2);
}

/* generate variable length code for a motion vector component (7.6.3.1) */
void 
simpeg_encode_putmv(simpeg_encode_context * context, int dmv, int f_code)
{
  int r_size, f, vmin, vmax, dv, temp, motion_code, motion_residual;

  r_size = f_code - 1; /* number of fixed length code ('residual') bits */
  f = 1<<r_size;
  vmin = -16*f; /* lower range limit */
  vmax = 16*f - 1; /* upper range limit */
  dv = 32*f;

  /* fold vector difference into [vmin...vmax] */
  if (dmv>vmax)
    dmv-= dv;
  else if (dmv<vmin)
    dmv+= dv;

  /* check value */
  if (dmv<vmin || dmv>vmax)
    if (!context->quiet)
      SimpegWrite_warning(context,"invalid motion vector");

  /* split dmv into motion_code and motion_residual */
  temp = ((dmv<0) ? -dmv : dmv) + f - 1;
  motion_code = temp>>r_size;
  if (dmv<0)
    motion_code = -motion_code;
  motion_residual = temp & (f-1);

  simpeg_encode_putmotioncode(context,motion_code); /* variable length code */

  if (r_size!=0 && motion_code!=0)
    simpeg_encode_putbits(context, motion_residual,r_size); /* fixed length code */
}
