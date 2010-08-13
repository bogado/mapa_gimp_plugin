#include <libgimp/gimp.h>
#include <gtk/gtk.h>

/*
 * estes dados definem o plugin e a sua ginterface 
 */

#define NOME "Map Maker" /* nome do plug-in */
#define BLURB "Creates a height field from levels sets." /* nome comprido do plug-in */
#define HELP "Help" /* help do plug-in */
#define AUTHOR "Victor Bogado da Silva Lins (victor@bogado.net)" /* autor do plugin */
#define COPYRIGHT "(c) 2001, Victor bogado da Silva Lins." /* copywrigth notice */
#define DATA "June 20 2001"

	/* parametros para o gimp */
#define PATH "<Image>/Filters/Test/Map make"
#define IMAGE_TYPES "RGBA, GRAYA" 

#if ! defined(__GNUC__)
#	define inline
#endif 

extern GimpParamDef pdb_params[];

/*
 * tipo do parametro
 */

typedef int param_type;

enum { VIZ8, VIZ4 };

/*
 * Os dados abaixo são fixos 
 */

static void query(void);
static void run(const gchar *, gint, const GimpParam *, gint *, GimpParam **);

gint32 execute_plugin(GimpDrawable*, const char *, param_type *);

/* 
 * Estes dados são as informações do plugin
 */ 

extern GimpPlugInInfo PLUG_IN_INFO;

extern const gint pdb_nparams;

typedef guchar cor[4];

void super_grow(param_type *, guchar *, guchar *, gint, gint, guchar);
int grow(param_type *, guchar *, guchar *, guint, guint, guchar);
int find_iso(param_type *, guchar *, guchar*, guint, guint, guchar);
int find_border(guchar *, guchar *, guint, guint, guchar);
void make_height_field(param_type *, guchar *, guchar *, guint, guint, guchar);

inline void getcor(guchar *, gint, cor);
inline int cmp_cor(cor, cor, guchar);
inline void plot(guchar *, cor, gint , guchar);
inline int plot2(guchar *, cor, cor, gint , guchar);
inline int plot3(guchar *, cor, cor, cor, gint , guchar);
inline void carimba(guchar *, cor, gint , gchar );

/* GUI */
int show_dialog(param_type *);

/* callback for ok */
void okay(GtkButton *, gpointer);
