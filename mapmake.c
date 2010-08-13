#include "mapmake.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char progress[] = "Mapping...    ";

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
	{GIMP_PDB_INT32, "teste", "dados de teste"}
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
	gimp_install_procedure(NOME, BLURB, HELP, AUTHOR, COPYRIGHT, DATA, PATH, IMAGE_TYPES, GIMP_PLUGIN, pdb_nparams, 0, pdb_params, NULL); /* Por enquanto sem retornar nada */
	gimp_install_procedure("Grow", BLURB, HELP, AUTHOR, COPYRIGHT, DATA, "<Image>/Filters/Test/Grow", IMAGE_TYPES, GIMP_PLUGIN, pdb_nparams, 0, pdb_params, NULL); 
	gimp_install_procedure("Super-Grow", BLURB, HELP, AUTHOR, COPYRIGHT, DATA, "<Image>/Filters/Test/Super Grow", IMAGE_TYPES, GIMP_PLUGIN, pdb_nparams, 0, pdb_params, NULL); 
	gimp_install_procedure("Add Isoline", BLURB, HELP, AUTHOR, COPYRIGHT, DATA, "<Image>/Filters/Test/Add Isoline", IMAGE_TYPES, GIMP_PLUGIN, pdb_nparams, 0, pdb_params, NULL); 
}

static void run(const gchar G_GNUC_UNUSED *nome, gint nparams, const GimpParam * params, gint *nretvals, GimpParam ** retparams)
{
	static GimpParam ret[1];
        GimpPDBStatusType* status;
	gint drawable;

	param_type p = VIZ4;

	*nretvals = 1;
	*retparams = ret;

	ret[0].type = GIMP_PDB_STATUS;
	status = (GimpPDBStatusType*) &(ret[0].data.d_status);

	*status = GIMP_PDB_CALLING_ERROR;

	//drawable = gimp_drawable_get(params[2].data.d_drawable);
	
	if (!gimp_get_data(nome, &p))
		p = VIZ4;

	switch (params[0].data.d_int32)
	{
		case GIMP_RUN_INTERACTIVE:
			if (!show_dialog(&p))
			{
				ret[0].data.d_status = GIMP_PDB_CANCEL;
				return;
			}
			break;
		case GIMP_RUN_WITH_LAST_VALS:
			break;
		case GIMP_RUN_NONINTERACTIVE:
			if (nparams != 4)
			{
				/* retorna o erro de chamadoa para o gimp */
				ret[0].data.d_status = GIMP_PDB_CALLING_ERROR;
				return;
			}

			p = params[3].data.d_int8;
			break;
		default:
			g_error("programa no lugar errado");
			ret[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
			return;
	}
	gimp_set_data(nome, &p, sizeof(param_type));

	*status = execute_plugin(drawable, nome, &p);

	ret[0].data.d_status = GIMP_PDB_SUCCESS;
	return;
}

gint32 execute_plugin(gint d, const char *nome, param_type *p)
{
//	gint i;
	guchar *img, *org;
	gint x,y,x2,y2,w,h;
//	guchar *work;
	GimpPixelRgn region, rgn_org;
	GimpDrawable *drawable = gimp_drawable_get(d);
	gint bpp = drawable->bpp;

	gimp_drawable_mask_bounds(d, &x, &y, &x2, &y2);
	w = x2 - x;
	h = y2 - y;

	img = malloc(w * h * bpp);
	org = malloc(w * h * bpp);
//	work = malloc(d.width * d.height * bpp);

	gimp_pixel_rgn_init(&region, drawable, x, y, w, h, TRUE, TRUE);
	//este é buffer para gravar a imagem modificada.
	gimp_pixel_rgn_init(&rgn_org, drawable, x, y, w, h, FALSE, FALSE);
	// Esta é a imagem em si 

	gimp_pixel_rgn_get_rect(&region, img, x, y, w, h);
	gimp_pixel_rgn_get_rect(&rgn_org, org, x, y, w, h);

	switch (*nome) {
		case 'S':
			super_grow(p, org, img, w, h, bpp);
			break;
		case 'G':
			memcpy(img, org, w * h * bpp);
			grow(p, org, img, w, h, bpp);
			break;
		case 'A':
			find_iso(p, org, img, w, h, bpp);
			break;
		default:
			make_height_field(p, org, img, w, h, bpp);
			break;
			
	}

	gimp_pixel_rgn_set_rect(&region, img, x, y, w, h);

	/* finish the process */
	gimp_drawable_flush (drawable);
	gimp_drawable_merge_shadow (d, TRUE);
	gimp_drawable_update (d, x, y, w, h);
	gimp_displays_flush();
	gimp_drawable_detach(drawable);

	free(img);
	free(org);

	return GIMP_PDB_SUCCESS;	
}

void make_height_field(param_type *p, guchar *org, guchar *new, guint w, guint h, guchar bpp)
{
	guchar *tmp, *tmp_old;
	int r =1;

	tmp = malloc(w * h * bpp);
	tmp_old = malloc(w * h * bpp);

	memcpy(tmp_old, org, w * h * bpp);
	while (r != 0)
	{
		guchar *t = tmp;
		tmp = tmp_old;
		tmp_old = t;

		r = find_iso(p, tmp, tmp_old, w, h, bpp);
		sprintf(progress, "Mapping %03d%%", 100 - (100*r)/(w*h*bpp));
	}

	super_grow(p, tmp, new, w, h, bpp);

	free(tmp);
	free(tmp_old);
}

int find_iso(param_type *p, guchar *org, guchar *new, guint w, guint h, guchar bpp)
{
	guchar *tmp;
	int r;

	tmp = malloc(w * h * bpp); 

	if (tmp == NULL)
		exit(-1);

	super_grow(p, org, tmp, w, h, bpp);

	memcpy(new, org, w * h * bpp);

	r = find_border(tmp, new, w, h, bpp);

	free(tmp);

	printf("findiso result = %d\n" , r);
	return r;
}

void super_grow(param_type *p, guchar *org, guchar *new, gint w, gint h, guchar bpp)
{
	guint cnt = 1,cnt2 = 0, old_cnt = 0;
	guchar *tmp;
	guchar *tmp_old;

	tmp = malloc(w * h * bpp);
	tmp_old = malloc(w * h * bpp);

	gimp_progress_init(progress);

	if (tmp == NULL || tmp_old == NULL)
	{
		g_error("no memory!");
	}

	/* inicializo o temporário com o a imagem */
	memcpy(tmp, org, w * h * bpp);
	while (cnt != old_cnt)
	{
		/* troca os temporários. */
		guchar *t;
		t = tmp;
		tmp = tmp_old;
		tmp_old = t;

		gimp_progress_update(1 - (double) cnt/(w*h*bpp));

		cnt2++;
		old_cnt = cnt;
		cnt = 0;
		/* zera o tmp (isto é necessário?)*/
		//memset(tmp, 0, w * h * (bpp));

		cnt = grow(p, tmp_old, tmp, w, h, bpp);
	}

	memcpy(new, tmp, w * h * bpp);

	/* libera a memoria */
	free(tmp);
	free(tmp_old);
}

int find_border(guchar *org, guchar *new, guint w, guint h, guchar bpp)
{
	guint i, n, idx, r = 0;
	cor c_h, c_v, c;

	for (i = 0; i < w; i++)
	{
		for (n = 0; n < h; n++)
		{
			idx = (n * w + i) * bpp;

			getcor(org, idx, c);

			/* Mais */
			if (i < w - 1)
				getcor(org, idx + bpp, c_h);
			else
				getcor(c, 0, c_h);

			if (n < h - 1)
				getcor(org, idx + bpp * w, c_v);
			else
				getcor(c, 0, c_v);

			if (new[idx + bpp -1] > 127)
				continue;

			if (cmp_cor(c_v, c_h, bpp))
			{
				if (!cmp_cor(c_v, c, bpp))
					r += plot2(new, c_v, c, idx, bpp);
			} else if (cmp_cor(c_v, c, bpp))
				r += plot2(new, c_h, c, idx, bpp);
			else if (cmp_cor(c_h, c, bpp))
				r += plot2(new, c_v, c, idx, bpp);
			else
				r += plot3(new, c_v, c_h, c, idx, bpp);
		}
	}
	return r;
}

int grow(param_type *p, guchar *tmp_old, guchar *tmp, guint w, guint h, guchar bpp)
{
	guint n, i, idx;
	guint cnt = 0;

	for (n = 0; n < w; n++)
	{
		for (i = 0; i < h; i++)
		{
			cor atual;

			idx = (i * w + n) * bpp;
			getcor(tmp_old, idx, atual);

			/* se não é transparente ... */
			if (atual[bpp -1] > 127)
			{
				/* carimba o espaço temporário se necessário */

				/* centro */
				carimba(tmp, atual, idx, bpp);

				/* diagonais */
				if (*p == VIZ8)
				{
					if (i < h - 1 && n < w - 1)
						carimba(tmp, atual, idx + bpp * (w + 1), bpp);
					if (i > 0 && n < w - 1)
						carimba(tmp, atual, idx - bpp * (w - 1), bpp);
					if (i < h - 1 && n > 0)
						carimba(tmp, atual, idx + bpp * (w - 1), bpp);
					if (i > 0 && n > 0)
						carimba(tmp, atual, idx - bpp * (w + 1), bpp); 
				}

				/* Mais */
 				if (n < w - 1)
					carimba(tmp, atual, idx + bpp, bpp);
				if (n > 0)
					carimba(tmp, atual, idx - bpp, bpp);

				if (i < h - 1)
					carimba(tmp, atual, idx + w * bpp, bpp);
				if (i > 0)
					carimba(tmp, atual, idx - w * bpp, bpp);
			} else
			{
				cnt++;
			}
		}
	}
	return cnt;
}

inline void getcor(guchar *org, gint idx, cor c)
{
	c[0] = org[idx];
	c[1] = org[idx + 1];
	c[2] = org[idx + 2];
	c[3] = org[idx + 3];
}

inline void carimba(guchar *dst1, cor c, gint idx, gchar bpp)
{
	cor c2;

	getcor(dst1, idx, c2);

	if (c2[bpp - 1] <= 127)
		plot(dst1, c, idx, bpp);
	/* ou o espaço reposta */
}

inline int plot3(guchar *tmp, cor c1, cor c2, cor c3, gint idx, guchar bpp)
{
	int i, r = 0;

	if (tmp[idx + bpp -1] > 127)
		return 0;

	for (i = 0; i < bpp; i++)
	{
		if (c1[i] != c2[i] || c2[i] != c3[i])
			r = 1;

		tmp[idx + i] = (c1[i] + c2[i] + c3[i])/3;
	}

	return r;
}

inline int plot2(guchar *tmp, cor c1, cor c2, gint idx, guchar bpp)
{
	int i, r = 0;

	if (tmp[idx + bpp -1] > 127)
		return 0;

	for (i = 0; i < bpp; i++)
	{
		if (c1[i] != c2[i])
			r = 1;

		tmp[idx + i] = (c1[i] + c2[i])/2;
	}

	return r;
}

inline void plot(guchar *tmp, cor c, gint idx, guchar bpp)
{
	int i;

	for (i = 0; i < bpp; i++)
		tmp[idx + i] = c[i];
}

inline int cmp_cor(cor c1, cor c2, guchar bpp)
{
	int i;

	for (i = 0; i < bpp; i++)
		if (c1[i] != c2[i])
			return 0;

	return 1;
}

