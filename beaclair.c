From tron@visgraf.impa.br Thu Jun 21 20:25:20 2001
Return-Path: <tron@lorenz.visgraf.impa.br>
Received: from localhost (victor.bogado [10.0.0.1]) by victor.bogado
	(8.11.2/8.8.7) with ESMTP id f5LNPJX01580 for <bogado@localhost>; Thu, 21
	Jun 2001 20:25:20 -0300
Envelope-to: victor@bogado.net
Delivery-date: Thu, 21 Jun 2001 17:17:27 -0400
Received: from bogado.net by localhost with POP3 (fetchmail-5.8.2) for
	bogado@localhost (single-drop); Thu, 21 Jun 2001 20:25:20 -0300 (BRT)
Received: from external.visgraf.impa.br ([147.65.1.2]
	helo=Fourier.visgraf.impa.br) by octagon.interserver.net with esmtp (Exim
	3.20 #1) id 15DBpM-0000zE-00 for victor@bogado.net; Thu, 21 Jun 2001
	17:17:24 -0400
Received: from lorenz.visgraf.impa.br (IDENT:root@Lorenz [147.65.6.44]) by
	Fourier.visgraf.impa.br (8.11.2/8.11.2) with ESMTP id f5LLH8b26091 for
	<victor@bogado.net>; Thu, 21 Jun 2001 18:17:13 -0300 (EST)
Received: (from tron@localhost) by lorenz.visgraf.impa.br (8.11.2/8.8.7) id
	f5LFIXm06554 for victor@bogado.net; Thu, 21 Jun 2001 18:18:33 +0300
Date: Thu, 21 Jun 2001 18:18:33 +0300
From: Roberto de Beauclair Seixas <tron@visgraf.impa.br>
Message-Id: <200106211518.f5LFIXm06554@lorenz.visgraf.impa.br>
To: victor@bogado.net
Content-Type: text/plain; CHARSET=US-ASCII
Subject: No Subject
Mime-Version: 1.0

/*
** dilat.c - tron@impa.br
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define noVIZ8		
#define IDX(x,y)	(((w) * (y)) + (x))
FILE *fimg, *fout;
/* unsigned char *img, *in, *out; */
unsigned char *itmp;
int *img, *in, *out;

void SaveImg(int *p, int w, int h, char *fn);
int MaxCorViz(int x, int y, int w, int h);

int main(int argc, char **argv)
{
 int  NVEZES = 1;
 int w, h, i, j, n = 0, k;
 char str[82];
 int c;

 if (argc < 2)
 {
    fprintf(stderr,"\n\nERROR: You Must Specify a Datafile.\n\n");
    return(-1);
 }
 if ((fimg = fopen(argv[1], "rb")) == NULL)
 {
    fprintf(stderr,"\n\nERROR: Opening Datafile.\n\n");
    return(-2);
 }
 if (argc == 3)
    sscanf(argv[2],"%d", &NVEZES);

 /* Check P5 - Grayscale Raw */
 fgets(str, 80, fimg);
 if ((str[0] != 'P') && (str[1] != '5'))
 {    
    fprintf(stderr,"\n\nERROR: Invalid Data Format! Use PGM Raw.\n\n");
    return(-3);
 }

 fgets(str, 80, fimg);
 while (str[0] == '#')
    fgets(str, 80, fimg);
 sscanf(str,"%d %d", &w, &h);
 fgets(str, 80, fimg);

 itmp  = (unsigned char *) calloc(w*h, sizeof(unsigned char));
 in  = (int *) calloc(w*h, sizeof(int));
 assert (itmp);
 fread(itmp, sizeof(unsigned char), w*h, fimg); 
 fclose(fimg);
 for (i = 0; i < w*h; i++)
  	in[i] = itmp[i];
 
 img = (int *) calloc(w*h, sizeof(int));
 assert (img);
 memcpy(img, in, w*h*sizeof(int));

 out = (int *) calloc(w*h, sizeof(int));
 assert (out);
 memcpy(out, img, w*h*sizeof(int)); 

 /*
 ** loop de geracao das iso-linhas intermediarias
 */
 for (k = 0; k < NVEZES; k++)
  {
    /* salva imagem intermediaria origem */
    sprintf(str,"%02d_a.out", k);
    SaveImg(img, w, h, str);

    /* determina a quantidade de pixels do background */
    for (i = 0; i < w; i++)
     for (j = 0; j < h; j++)
      if (out[IDX(i,j)] == 0) ++n;

    do
    {
     /* Loop  de dilatacao */
     for (i = 0; i < w; i++)
      for (j = 0; j < h; j++)
      {
#if 1
       c = img[IDX(i,j)];
#else
       c = MaxCorViz(i, j, w, h);
#endif
       if (c != 0) 
       {
          if ((i < w-1) && (out[IDX(i+1,j)] == 0))   
             { out[IDX(i+1,j)] = c; --n; }
          if ((i > 0) && (out[IDX(i-1,j)] == 0))   
             { out[IDX(i-1,j)] = c; --n; }
          if ((j < h-1) && (out[IDX(i,j+1)] == 0))   
             { out[IDX(i,j+1)] = c; --n; }
          if ((j > 0) && (out[IDX(i,j-1)] == 0))   
             { out[IDX(i,j-1)] = c; --n; }
#ifdef VIZ8
          if ((i < w-1) && (j < h-1) && (out[IDX(i+1,j+1)] == 0)) 
             { out[IDX(i+1,j+1)] = c; --n; }
          if ((i > 0) && (j > 0) && (out[IDX(i-1,j-1)] == 0)) 
             { out[IDX(i-1,j-1)] = c; --n; }
          if ((i < w-1) && (j > 0) && (out[IDX(i+1,j-1)] == 0)) 
             { out[IDX(i+1,j-1)] = c; --n; }
          if ((i > 0) && (j < h-1) && (out[IDX(i-1,j+1)] == 0)) 
             { out[IDX(i-1,j+1)] = c; --n; }
#endif
       }
      }

    fprintf(stderr,"img %d of %d: %d pixels to go               \r", 
      			k+1, NVEZES, n);
    /* Atualiza a imagem origem */
    memcpy(img, out, w*h*sizeof(int));

    } while (n>0); 

    /* salva imagem intermediaria */
    sprintf(str,"%02d_b.out", k);
    SaveImg(out, w, h, str);

    fprintf(stderr,"\n");

    /* Loop de "edge detection" */
    for (i = 0; i < w; i++)
      for (j = 0; j < h; j++)
      {
       c = out[IDX(i,j)];
       if (c != 0) 
       {
	  int S= (i>0)   ? out[IDX(i-1,j)] : c;
	  int N= (i<w-1) ? out[IDX(i+1,j)] : c;
	  int W= (j>0)   ? out[IDX(i,j-1)] : c;
	  int E= (j<h-1) ? out[IDX(i,j+1)] : c;
	  if ((c==S || S == 0) && 
	      (c==N || N == 0) && 
	      (c==W || W == 0) && 
	      (c==E || E == 0) )
	     out[IDX(i,j)] = 0;
       }
      }

#if 0
    /* salva imagem intermediaria */
    sprintf(str,"%02d_c.out", k);
    SaveImg(out, w, h, str);
#endif

    /* Salva a imagem para a simplificacao das bordas */
    memcpy(img, out, w*h*sizeof(int));
    /* faz o tratamento da intersecao das bordas duplas */
    for (i = 0; i < w; i++)
      for (j = 0; j < h; j++)
      {
	int a = 0, b = 0;
        c = img[IDX(i,j)];
        if (c != 0)
	{
          if ((i < w-1) && ((c = img[IDX(i+1,j)]) != 0))   
             { if (a == 0) a = c; else if ((a != c) && (b == 0)) b = c; }
          if ((i > 0) && ((c = img[IDX(i-1,j)]) != 0))   
             { if (a == 0) a = c; else if ((a != c) && (b == 0)) b = c; }
          if ((j < h-1) && ((c = img[IDX(i,j+1)]) != 0))   
             { if (a == 0) a = c; else if ((a != c) && (b == 0)) b = c; }
          if ((j > 0) && ((c = img[IDX(i,j-1)]) != 0))   
             { if (a == 0) a = c; else if ((a != c) && (b == 0)) b = c; }
          if ((i < w-1) && (j < h-1) && ((c = img[IDX(i+1,j+1)]) != 0))   
             { if (a == 0) a = c; else if ((a != c) && (b == 0)) b = c; }
          if ((i > 0) && (j > 0) && ((c = img[IDX(i-1,j-1)]) != 0))   
             { if (a == 0) a = c; else if ((a != c) && (b == 0)) b = c; }
          if ((i > 0) && (j < h-1) && ((c = img[IDX(i-1,j+1)]) != 0))   
             { if (a == 0) a = c; else if ((a != c) && (b == 0)) b = c; }
          if ((j > 0) && (i < w-1) && ((c = img[IDX(i+1,j-1)]) != 0))   
             { if (a == 0) a = c; else if ((a != c) && (b == 0)) b = c; }
	  out[IDX(i,j)] = (a + b) / 2;
	}
      }
#if 0
    /* salva imagem intermediaria */
    sprintf(str,"%02d_d.out", k);
    SaveImg(out, w, h, str);
#endif

    /* soma a imagem original com a imagem de resultado */
    for (i = 0; i < w; i++)
      for (j = 0; j < h; j++)
      {
        c = in[IDX(i,j)];
        if (c != 0)
           out[IDX(i,j)] = c;
      }
		    
#if 0
    /* salva imagem intermediaria */
    sprintf(str,"%02d_e.out", k);
    SaveImg(out, w, h, str);
#endif

    /* a imagem intermediaria de resultado para a ser a imagem origem */
    memcpy(img, out, w*h*sizeof(int));
    memcpy(in, out, w*h*sizeof(int));
  }

 /* salva imagem intermediaria */
 sprintf(str,"final.out");
 SaveImg(out, w, h, str);

 free(itmp);
 free(in);
 free(out);
 free(img);

 return 0;
}


/*
** Salva uma imagem 
*/
void SaveImg(int *p, int w, int h, char *fn)
{
   int max, i;
   unsigned char *img;

   img = (unsigned char *) calloc (w*h,sizeof(unsigned char));
   assert (img);
   /* determina o intervalo de valores */
   max = p[0];
   for (i = 1; i < w*h; i++)
      if (p[i] > max) 
       	max = p[i];
   /* faz a reducao para 8 bits para gerar a imagem */
   for (i = 0; i < w*h; i++)
    	img[i] = (p[i] * 255) / max;

   /* grava a imagem */
   if ((fout = fopen(fn, "wb")) == NULL)
   {
      fprintf(stderr,"\n\nERROR: Writing Intermediate Results.\n\n");
      assert(0);
   }  
   fprintf(fout,"P5\n# dilat - tron@impa.br\n %d %d\n255\n", w, h);
   fwrite(img, sizeof(unsigned char), w*h, fout); 
   fclose(fout);
   free(img);
}


/*
** Retorna a "maior cor" dos vizinhos
*/
int MaxCorViz(int x, int y, int w, int h)
{
   int c, n;

   c = img[IDX(x,y)];
   if ((x < w-1) && (c < (n = img[IDX(x+1,y)])))	c = n;
   if ((x > 0) && (c < (n = img[IDX(x-1,y)])))		c = n;
   if ((y < h-1) && (c < (n = img[IDX(x,y+1)])))	c = n;
   if ((y > 0) && (c < (n = img[IDX(x,y-1)])))		c = n;
#ifdef VIZ8
   if ((x < w-1) && (y < h-1) && (c < (n = img[IDX(x+1,y+1)])))	c = n;
   if ((x > 0) && (y > 0) && (c < (n = img[IDX(x-1,y-1)])))	c = n;
   if ((x > 0) && (y < h-1) && (c < (n = img[IDX(x-1,y+1)])))	c = n;
   if ((x < w-1) && (y > 0) && (c < (n = img[IDX(x+1,y-1)])))	c = n;
#endif

   return (c);
}

