// 
// random_histogram_example.c
//
// This example tests the random number generators for different
// distributions.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include "liquid.h"

#define OUTPUT_FILENAME "random_histogram_example.m"


// print usage/help message
void usage()
{
    printf("random_histogram_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  N     : number of trials\n");
    printf("  n     : number of histogram bins\n");
    printf("  d     : distribution: {uniform, normal, exp, weib, gamma, nak, rice}\n");
    printf("  e     : eta    NORMAL: mean\n");
    printf("  s     : sigma  NORMAL: standard deviation\n");
    printf("  l     : lambda EXPONENTIAL: decay factor\n");
    printf("  a     : alpha  WEIBULL: shape\n");
    printf("  b     : beta   WEIBULL: spread\n");
    printf("  g     : gamma  WEIBULL: threshold\n");
    printf("  A     : alpha  GAMMA: shape\n");
    printf("  B     : beta   GAMMA: spread\n");
    printf("  m     : m      NAKAGAMI: shape\n");
    printf("  o     : omega  NAKAGAMI: spread\n");
    printf("  K     : K      RICE-K: spread\n");
    printf("  O     : omega  RICE-K: spread\n");
}

int main(int argc, char*argv[])
{
    unsigned long int num_trials = 100000; // number of trials
    unsigned int num_bins = 20;
    enum {
        UNIFORM=0,
        NORMAL,
        EXPONENTIAL,
        WEIBULL,
        GAMMA,
        NAKAGAMIM,
        RICEK
    } distribution=0;

    // distribution parameters
    float eta = 0.0f;       // NORMAL: mean
    float sigma = 1.0f;     // NORMAL: standard deviation
    float lambda = 3.0f;    // EXPONENTIAL: decay factor
    float alphaw = 1.0f;    // WEIBULL: shape
    float betaw = 1.0f;     // WEIBULL: spread
    float gammaw = 1.0f;    // WEIBULL: threshold
    float alphag = 4.5f;    // GAMMA: shape
    float betag = 1.0f;     // GAMMA: spread
    float m = 4.5f;         // NAKAGAMI: shape factor
    float omeganak = 1.0f;  // NAKAGMAI: spread factor
    float K = 4.0f;         // RICE-K: K-factor (shape)
    float omegarice = 1.0f; // RICE-K: spread factor

    int dopt;
    while ((dopt = getopt(argc,argv,"uhn:d:e:s:l:a:b:g:A:B:m:o:K:O:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':
            usage();
            return 0;
        case 'N': num_trials = atoi(optarg); break;
        case 'n': num_bins = atoi(optarg); break;
        case 'd':
            if      (strcmp(optarg,"uniform")==0)   distribution = UNIFORM;
            else if (strcmp(optarg,"normal")==0)    distribution = NORMAL;
            else if (strcmp(optarg,"exp")==0)       distribution = EXPONENTIAL;
            else if (strcmp(optarg,"weib")==0)      distribution = WEIBULL;
            else if (strcmp(optarg,"gamma")==0)     distribution = GAMMA;
            else if (strcmp(optarg,"nak")==0)       distribution = NAKAGAMIM;
            else if (strcmp(optarg,"rice")==0)      distribution = RICEK;
            else {
                fprintf(stderr,"error: %s, unknown/unsupported distribution '%s'\n", argv[0], optarg);
                exit(1);
            }
        case 'e': eta       = atof(optarg); break;
        case 's': sigma     = atof(optarg); break;
        case 'l': lambda    = atof(optarg); break;
        case 'a': alphaw    = atof(optarg); break;
        case 'b': betaw     = atof(optarg); break;
        case 'g': gammaw    = atof(optarg); break;
        case 'A': alphag    = atof(optarg); break;
        case 'B': betag     = atof(optarg); break;
        case 'm': m         = atof(optarg); break;
        case 'o': omeganak  = atof(optarg); break;
        case 'K': K         = atof(optarg); break;
        case 'O': omegarice = atof(optarg); break;
        default:
            fprintf(stderr,"error: %s, unknown/unsupported option\n", argv[0]);
            exit(1);
        }
    }

    float xmin = 0.0f;
    float xmax = 1.0f;

    unsigned long int i;

    if (distribution == UNIFORM) {
        xmin =  0.0f;
        xmax =  1.0f;
    } else if (distribution == NORMAL) {
        xmin = eta - 4.0f*sigma;
        xmax = eta + 4.0f*sigma;
    } else if (distribution == EXPONENTIAL) {
        xmin = 0.0f;
        xmax = 7.0f / lambda;
    } else if (distribution == WEIBULL) {
        xmin = gammaw;
        xmax = gammaw + 6.0f;
    } else if (distribution == GAMMA) {
        xmin = 0.0f;
        xmax = 14.0f;
    } else if (distribution == NAKAGAMIM) {
        xmin = 0.0f;
        xmax = 2.0f * sqrtf(omeganak);
    } else if (distribution == RICEK) {
        xmin = 0.0f;
        xmax = 2.3f * sqrtf(omegarice);
    } else {
        fprintf(stderr, "error: %s, unknown/unsupported distribution\n", argv[0]);
        exit(1);
    }

    //
    //float xspan = xmax - xmin;
    float bin_width = (xmax - xmin) / (num_bins);

    // initialize histogram
    unsigned int hist[num_bins];
    for (i=0; i<num_bins; i++)
        hist[i] = 0;

    // generate random variables
    float x = 0.0f;
    for (i=0; i<num_trials; i++) {
        switch (distribution) {
        case UNIFORM:     x = randf(); break;
        case NORMAL:      x = sigma*randnf() + eta; break;
        case EXPONENTIAL: x = randexpf(lambda); break;
        case WEIBULL:     x = randweibf(alphaw,betaw,gammaw); break;
        case GAMMA:       x = randgammaf(alphag,betag); break;
        case NAKAGAMIM:   x = randnakmf(m,omeganak); break;
        case RICEK:       x = randricekf(K,omegarice); break;
        default:
            fprintf(stderr,"error: %s, unknown/unsupported distribution\n", argv[0]);
            exit(1);
        }

        // compute bin index
        unsigned int index;
        float ihat = num_bins * (x - xmin) / (xmax - xmin);
        if (ihat < 0.0f)
            index = 0;
        else
            index = (unsigned int)ihat;
        
        if (index >= num_bins)
            index = num_bins-1;

        hist[index]++;
    }

    // compute expected distribution
    unsigned int num_steps = 100;
    float xstep = (xmax - xmin) / (num_steps - 1);
    float f[num_steps];
    float F[num_steps];
    for (i=0; i<num_steps; i++) {
        x = xmin + i*xstep;
        switch (distribution) {
        case UNIFORM:
            f[i] = randf_pdf(x);
            F[i] = randf_cdf(x);
            break;
        case NORMAL:
            f[i] = randnf_pdf(x,eta,sigma);
            F[i] = randnf_cdf(x,eta,sigma);
            break;
        case EXPONENTIAL:
            f[i] = randexpf_pdf(x,lambda);
            F[i] = randexpf_cdf(x,lambda);
            break;
        case WEIBULL:
            f[i] = randweibf_pdf(x,alphaw,betaw,gammaw);
            F[i] = randweibf_cdf(x,alphaw,betaw,gammaw);
            break;
        case GAMMA:
            f[i] = randgammaf_pdf(x,alphag,betag);
            F[i] = randgammaf_cdf(x,alphag,betag);
            break;
        case NAKAGAMIM:
            f[i] = randnakmf_pdf(x,m,omeganak);
            F[i] = randnakmf_cdf(x,m,omeganak);
            break;
        case RICEK:
            f[i] = randricekf_pdf(x,K,omegarice);
            F[i] = randricekf_cdf(x,K,omegarice);
            break;
        default:
            fprintf(stderr,"error: %s, unknown/unsupported distribution\n", argv[0]);
            exit(1);
        }
    }

    // open output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"xmin = %12.4e;\n", xmin);
    fprintf(fid,"xmax = %12.4e;\n", xmax);
    fprintf(fid,"num_bins = %u;\n", num_bins);
    fprintf(fid,"xspan = xmax - xmin;\n");

    float F_hat = 0.0f;
    for (i=0; i<num_bins; i++) {
        x = xmin + ((float)i + 0.5f)*bin_width;
        float h = (float)(hist[i]) / (num_trials * bin_width);
        fprintf(fid,"xh(%3lu) = %12.4e; h(%3lu) = %12.4e;\n", i+1, x, i+1, h);

        x = xmin + ((float)i + 1.0f)*bin_width;
        F_hat += h;
        fprintf(fid,"xH(%3lu) = %12.4e; H(%3lu) = %12.4e;\n", i+1, x, i+1, F_hat);
    }
    fprintf(fid,"H = H/H(end);\n");

    for (i=0; i<num_steps; i++) {
        x = xmin + i*xstep;
        fprintf(fid,"xf(%3lu) = %12.4e; f(%3lu) = %12.4e; F(%3lu) = %12.4e;\n", i+1, x, i+1, f[i], i+1, F[i]);
    }

    // plot results
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(xh,h,'x', xf,f,'-');\n");
    fprintf(fid,"xlabel('x');\n");
    fprintf(fid,"ylabel('f_x(x)');\n");
    fprintf(fid,"axis([(xmin-0.1*xspan) (xmax+0.1*xspan) 0 1.1*max([h f])]);\n");
    fprintf(fid,"legend('histogram','true PDF',1);\n");

    // plot results
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(xH,H,'x', xf,F,'-');\n");
    fprintf(fid,"xlabel('x');\n");
    fprintf(fid,"ylabel('f_x(x)');\n");
    //fprintf(fid,"axis([(xmin-0.1*xspan) (xmax+0.1*xspan) 0 1]);\n");
    fprintf(fid,"legend('histogram','true CDF',0);\n");

    fclose(fid);
    printf("results written to %s.\n",OUTPUT_FILENAME);


    printf("done.\n");
    return 0;
}

