#include "mapheat.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

GimpParamDef pdb_params[] =
{
	/* parametros obrigatórios */
	{GIMP_PDB_INT32, "run_mode",       "GimpRunModeType"},
	{GIMP_PDB_IMAGE, "image_id",       "(unused)"},
	{GIMP_PDB_DRAWABLE, "drawable_id", "Drawable to affect"},

	/*
	 * outros parametros
	 * Os mesmos parametros que constam abaixo 
	 * devem constar aqui tb.
	 */
};

const gint pdb_nparams = sizeof(pdb_params) / sizeof(GimpParamDef);

GimpPlugInInfo PLUG_IN_INFO = 
{
	NULL,	/* init_proc */ 
	NULL,	/* quit_proc */ 
	query,	/* query_proc */ 
	run,	/* run_proc */ 
};

MAIN()

static void query(void)
{
	gimp_install_procedure("Create Height Field", BLURB, HELP, AUTHOR, COPYRIGHT, DATA, "<Image>/Filters/Map tools/Create height field", IMAGE_TYPES, GIMP_PLUGIN, pdb_nparams, 0, pdb_params, NULL); 
	gimp_install_procedure("Mean", BLURB, HELP, AUTHOR, COPYRIGHT, DATA, "<Image>/Filters/Map tools/Mean", IMAGE_TYPES, GIMP_PLUGIN, pdb_nparams, 0, pdb_params, NULL); 
}

static void run(const gchar G_GNUC_UNUSED *nome, gint nparams, const GimpParam * params, gint *nretvals, GimpParam ** retparams)
{
	static GimpParam ret[1];
        GimpPDBStatusType* status;
	GimpDrawable* drawable;

	*nretvals = 1;
	*retparams = ret;

	ret[0].type = GIMP_PDB_STATUS;
	status = (GimpPDBStatusType*) &(ret[0].data.d_status);

	*status = GIMP_PDB_CALLING_ERROR;

	drawable = gimp_drawable_get(params[2].data.d_drawable);

	switch (params[0].data.d_int32)
	{
		case GIMP_RUN_INTERACTIVE:
			break;
		case GIMP_RUN_WITH_LAST_VALS:
			break;
		case GIMP_RUN_NONINTERACTIVE:
			break;
		default:
			g_error("programa no lugar errado");
			ret[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
			return;
	}

	*status = execute_plugin(drawable, nome);

	ret[0].data.d_status = GIMP_PDB_SUCCESS;
	return;
}

gint32 execute_plugin(GimpDrawable* d, const char *nome)
{
	DrawBuf *img, *r;

	GimpPixelRgn region, rgn_org;
	gint bpp = d->bpp;
	guint x,y,x2,y2,w,h;

	gimp_drawable_mask_bounds(d->drawable_id, &x, &y, &x2, &y2);
	w = x2 - x;
	h = y2 - y;

	img = new_drawbuf(w, h, bpp); assert(img);
	r = new_drawbuf(w, h, bpp); assert(r);

	/*Imagem de Shadow */
	gimp_pixel_rgn_init(&region, d, x, y, w, h, TRUE, TRUE);
	/* Imagem Original */
	gimp_pixel_rgn_init(&rgn_org, d, x, y, w, h, FALSE, FALSE);

	gimp_pixel_rgn_get_rect(&rgn_org, img->buf, x, y, w, h);

	printf("1-- > %s %d %d %d %d \n", nome, x, y, w, h);
		
	switch (nome[0])
	{
		case 'C':
			heat(img, r);
			break;
		case 'M':
			mean(img, r);
			break;
	}

	clean_alpha(r);

	printf("2-- > %s %d %d %d %d \n", nome, x, y, w, h);

	gimp_pixel_rgn_set_rect(&region, r->buf, x, y, w, h);

	/* finish the process */
	gimp_drawable_flush (d);
	gimp_drawable_merge_shadow (d->drawable_id, TRUE);
	gimp_drawable_update (d->drawable_id, x, y, w, h);
	gimp_displays_flush();
	gimp_drawable_detach(d);

	free_drawbuf(img);
	free_drawbuf(r);

	return GIMP_PDB_SUCCESS;	
}

void heat(DrawBuf *o, DrawBuf *d)
{
	DrawBuf *t1, *t2, *tmp;
	int err = BIGERR;
	int x,y;

	printf ("HEAT \n");

	t1 = new_drawbuf(o->w, o->h, o->bpp); assert(t1);
	t2 = new_drawbuf(o->w, o->h, o->bpp); assert(t2);

	copy_drawbuf(o, t1);
	copy_drawbuf(o, t2);

	while (err > THRESHOLD)
	{
		printf("-%d-\n", err);
		tmp = t1; t1 = t2; t2 = tmp;
		err = mean(t1, t2);
	}

	copy_drawbuf(t2, d);

	free_drawbuf(t1);
	free_drawbuf(t2);
}

void clean_alpha(DrawBuf *d)
{
	int x,y;	
	for (x = 0; x != d->w; x++)
		for (y = 0; y != d->h; y++)
		{
			BUFALPHA(d,x,y) = (BUFALPHA(d,x,y)==255)?255:254; 
		}
}

guint mean(DrawBuf *o, DrawBuf *d)
{
	guint x, y;
	guint err = BIGERR;

	for (x = 0; x < o->w; x++)
		for (y = 0; y < o->h; y++)
		{
			int off;
			off = BUFOFFSET(o, x, y);

			/* se este pixel é visivel então tenho que mante-lo */
			if (ISTRANS(o,x,y))
			{
				int i;

				for (i=0; i < o->bpp; i++)
					*(d->buf + off + i) = *(o->buf + off + i);
			} else /* se não quero a média dos vizinhos */
			{
				int m, n;
				int i;
				int err_t;

				for (i=0; i < o->bpp; i++)
				{
					m = 0;
					n = 0;
					/* não estou na 1a linha */
					if (y != 0)
					{
						m += *(o->buf + BUFOFFSET(o,x,y-1) + i);
						n++;
					}
					/* não estou na ultima linha */
					if (y != o->h -1)
					{
						m += *(o->buf + BUFOFFSET(o,x,y+1) + i);
						n++;
					}

					/* não estou na 1a coluna */
					if (x != 0)
					{
						m += *(o->buf + BUFOFFSET(o,x-1,y) + i);
						n++;
					}
					/* não estou na ultima coluna */
					if (x != o->w -1)
					{
						m += *(o->buf + BUFOFFSET(o,x+1,y) + i);
						n++;
					}

					err_t = abs(*(d->buf + off + i) - m/n);

					if (err_t < err)
					{
						err = err_t;
					}

					*(d->buf + off + i) = m/n;
				}
			}

			/* A informação de alpha deve ser mantida! */
			BUFALPHA_OFF(d,off) = BUFALPHA_OFF(o,off);
		}
}

/* Cria uma nova area de desenho */
DrawBuf *new_drawbuf(guint w, guint h, guchar bpp)
{
	DrawBuf *n;

	n = malloc(sizeof(DrawBuf));
	if (n == NULL)
		return NULL;

	n->w = w;
	n->h = h;
	n->bpp = bpp;

	n->buf = calloc(1, BUFSIZE(n));

	if (n->buf == NULL)
	{
		free(n);
		return NULL;
	}

	return n;
}

/* libera uma area de desenho */
void free_drawbuf(DrawBuf *d)
{
	free(d->buf);
	free(d);
}

/* copia uma area de desenho para outra */
void copy_drawbuf(DrawBuf *o, DrawBuf *d)
{
	/* Caso exista a necessidade altera o tamanho da area destino */
	if (o->h != d->h || o->w != d->w || o->bpp != d->bpp)
	{
		d->w = o->w;
		d->h = o->h;
		d->bpp = o->bpp;
		d->buf = realloc(d->buf, BUFSIZE(d));
	}

	memcpy(d->buf, o->buf, BUFSIZE(d));
}

