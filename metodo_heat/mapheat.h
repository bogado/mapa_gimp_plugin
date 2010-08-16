#include <libgimp/gimp.h>
#include <gtk/gtk.h>

/*
 * estes dados definem o plugin e a sua ginterface 
 */

#define NOME "Map heat" /* nome do plug-in */
#define BLURB "Creates a height field from levels sets. Using the heat equation" /* nome comprido do plug-in */
#define HELP "Help" /* help do plug-in */
#define AUTHOR "Victor Bogado da Silva Lins (victor@bogado.net)" /* autor do plugin */
#define COPYRIGHT "(c) 2001, Victor bogado da Silva Lins." /* copywrigth notice */
#define DATA "August 14 2001"

	/* parametros para o gimp */
#define IMAGE_TYPES "RGBA, GRAYA" 

#if ! defined(__GNUC__)
#	define inline
#endif 

/* constantes de manipulação de erro    */
/* em todos os casos BIGERR > THRESHOLD */
#define BIGERR 0x7fff
#define THRESHOLD 2

extern GimpParamDef pdb_params[];

/*
 * Os dados abaixo são fixos 
 */

static void query(void);
static void run(gchar *, gint, GimpParam *, gint *, GimpParam **);

gint32 execute_plugin(GimpDrawable *, char *);

/* 
 * Estes dados são as informações do plugin
 */ 

extern GimpPlugInInfo PLUG_IN_INFO;

extern const gint pdb_nparams;

typedef guchar cor[4];

typedef struct { 
	guchar *buf;
	guint w,h;
	guchar bpp;
} DrawBuf;

DrawBuf *new_drawbuf(guint, guint, guchar);
void free_drawbuf(DrawBuf *);
void copy_drawbuf(DrawBuf *, DrawBuf *);
void clean_alpha(DrawBuf *);

/* esta macro calcula o tamanho do buffer. */
#define BUFSIZE(d) (sizeof(guchar) * (d)->w * (d)->h * (d)->bpp)
/* macro para encontrar a posição (x,y) do buffer */
#define BUFOFFSET(d,x,y) (((x) + (y) * (d)->w) * (d)->bpp)
/* macro para pegar o canal de alpha de um buffer usando um offset */
#define BUFALPHA_OFF(d,off) (*((d)->buf + (off) + (d)->bpp - 1))
/* macro para pegar o canal de alpha de um buffer usando coordenadas */
#define BUFALPHA(d,x,y) BUFALPHA_OFF((d),BUFOFFSET((d),(x),(y)))
/* macro que testa se um pixel é transparente */
#define ISTRANS(d,x,y) (BUFALPHA((d),(x),(y)) == 0xFF)

/* Calcula toda a interação */
void heat(DrawBuf *, DrawBuf *);

/* Calcula uma iteração e retorna o maior erro */
guint mean(DrawBuf *, DrawBuf *);

/* Diminui a resolução de um DrawBuf (Respeitando as normas) */
void shrink(DrawBuf *, DrawBuf *, int);

/* Restora o tamanho original do DrawBuf (de novo respeita as normas :-)) */
void restore(DrawBuf *, DrawBuf *, int);

