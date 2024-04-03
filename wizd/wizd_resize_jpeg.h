/****************************************************/
/* Quelques definitions utiles partout              */
/* general.h                                        */
/*                                                  */
/* Ecrit par : Daniel Lacroix (all rights reserved) */
/*                                                  */
/****************************************************/

#ifndef __GENERAL_H__
#define __GENERAL_H__

#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  !FALSE
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef int8
#define int8  char
#endif
#ifndef int16
#define int16 short
#endif
#ifndef int32
#define int32 int
#endif

#ifndef uint8
#define uint8  unsigned int8
#endif
#ifndef uint16
#define uint16 unsigned int16
#endif
#ifndef uint32
#define uint32 unsigned int32
#endif

#ifndef boolean
#define boolean int
#endif

#ifndef pointer
#define pointer void *
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN3
#define MIN3(a,b,c) MIN(MIN(a,b),MIN(b,c))
#endif
#ifndef MAX3
#define MAX3(a,b,c) MAX(MAX(a,b),MAX(b,c))
#endif

#ifndef ABS
#define ABS(a)   ((a >= 0)?(a):-(a))
#endif

#endif /* __GENERAL_H__ */
#ifndef __IMAGE_H__
#define __IMAGE_H__

/* definition d'un pixel d'une image */
typedef uint32 pix;
/* Les differentes couleurs RGB   */
/* sont positionnees comme suit : */
/* rouge 0xFF000000               */
/* vert  0x00FF0000               */
/* bleu  0x0000FF00               */
/* trans 0x000000FF               */

/* definition d'une image */
typedef struct {
  /* dimensions de l'image */
  int32 width,height;
  /* buffer en mode 32 bits (quelque soit la valeur reel de l'ecran) */
  pix    *buf;
} image;

/* renvoi les dimensions de l'image */
void image_size(image *pimage, int32 *width, int32 *height);

/* cree une nouvelle image */
image *image_new(int32 width, int32 height);

/* libere une image */
void image_free(image *pimage);

/* copy l'image psrc dans pdest, les deux images */
/* doivent avoir la meme taille                  */
void image_copy(image *pdest, image *psrc);

/* (x,y) = position dans pdest ou est colle psrc    */
void image_paste(image *pdest, image *psrc, int32 x, int32 y);

/* (x,y) = position dans pdest ou est incruste psrc */
/* et col la couleur d'incruste de psrc             */
void image_incruste(image *pdest, image *psrc, int32 x, int32 y, pix col);

/* (x,y) = position dans pdest ou est incruste psrc */
/* et col la couleur d'incruste de psrc. rapport l' */
/* intensite de psrc sur 256 par rapport a pdest    */
void image_transparent(image *pdest, image *psrc, int32 x, int32 y, pix col,
  uint16 rapport);

/* converti l'image psrc en noir et blanc dans pdest */
void image_gris(image *pdest, image *psrc, int32 x, int32 y);

/* (x,y) = position dans pdest ou est colle psrc    */
void image_paste_with_alpha(image *pdest, image *psrc, int32 x, int32 y);

/* (x,y) = position dans pdest ou psrc est utilise comme alpha */
void image_set_alpha_with_grey(image *pdest, image *psrc, int32 x, int32 y);

#endif /* __IMAGE_H__ */

#ifndef __IMAGE_FROM_JPEG_H__
#define __IMAGE_FROM_JPEG_H__

#define MAX_JPEG_FILE_SIZE 10000000


/* Renvoi l'image ou NULL si impossible */
image *image_new_from_jpeg(char *file_name);

/* Renvoi l'image ou NULL si impossible */
image *image_new_from_jpeg_stream(FILE *file);

#define JPEG_QUALITY 255

/* Sauvegarde l'image pimage dans le fichier file      */
/* sous le format JPEG. Return -1 = si ERREUR, 0 sinon */
int image_save_to_jpeg(image *pimage, char *file,int out_jpeg_quality);

/* Sauvegarde l'image pimage dans le flux stream       */
/* sous le format JPEG. Return -1 = si ERREUR, 0 sinon */
int image_save_to_jpeg_stream(image *pimage, FILE *stream,int out_jpeg_quality);

#endif /* __IMAGE_FROM_JPEG_H__ */

#ifndef __IMAGE_TRANS_H__
#define __IMAGE_TRANS_H__

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest en prenant        */
/* toujours le point le plus proche.               */
void image_resize_nearest(image *pdest, image *psrc, int32 width, int32 height);

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest. On determine les */
/* points par interpolation bilinaire.             */
void image_resize_bilinear(image *pdest, image *psrc, int32 width, int32 height);

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest. On determine les */
/* points par interpolation avec une DCT 2D des 64 */
/* points les plus pres.                           */
void image_resize_dct(image *pdest, image *psrc, int32 width, int32 height);

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest. On determine les */
/* points par interpolation avec une BSpline de    */
/* Catmull-Rom cubique.                            */
void image_resize_catmull_rom(image *pdest, image *psrc, int32 width, int32 height);

/* Redimentionne l'image psrc et place le resultat          */
/* dans pdest. pdest doit être 2 fois plus grande que psrc. */
void image_resize_tv(image *pdest, image *psrc);

/* Redimentionne l'image psrc et place le resultat          */
/* dans pdest. pdest doit être 2 fois plus grande que psrc. */
void image_resize_tv2(image *pdest, image *psrc);

/* Redimentionne l'image psrc et place le resultat          */
/* dans pdest. pdest doit être 2 fois plus grande que psrc. */
void image_resize_most(image *pdest, image *psrc);

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest en fusionnant les */
/* points. L'image résultat doit être plus petite  */
/* que l'image d'origine.                          */
void image_downsize(image *pdest, image *psrc,
  int32 width, int32 height);

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest en fusionnant les */
/* points. L'image résultat doit être plus petite  */
/* que l'image d'origine. L'algo est plus simple,  */
/* moins exact mais génère moins de flou.          */
void image_downsize_rought(image *pdest, image *psrc,
  int32 width, int32 height);

#endif /* __IMAGE_TRANS_H__ */

#ifndef __IMAGE_DRAW_FUNC_H__
#define __IMAGE_DRAW_FUNC_H__

/* les definitions des differentes utilisation du canal      */
/* alpha qui suivent NE DOIVENT PAS etre utilisees ailleurs  */
/* elle en interne a ne pas ecrirent 3 fonctions en fonction */
/* de l'utilisation de alpha mais une seul parametree.       */
typedef enum {
  ALPHA_REPLACE,
  ALPHA_KEEP,
  ALPHA_USE
} AlphaType;

/* dessine un point aux coordonnees (x,y) de couleur col  */
/* la transparence resultante dans pimage de celle de col */
void put_pix_alpha_replace(image *pimage, int32 x, int32 y, pix col);
/* dessine un point aux coordonnees (x,y) de couleur col     */
/* la transparence resultante dans pimage de celle de pimage */
void put_pix_alpha_keep   (image *pimage, int32 x, int32 y, pix col);
/* dessine un point aux coordonnees (x,y) de couleur col     */
/* la transparence resultante dans pimage de celle de pimage */
/* la transparence de col est utilise pour calculer le       */
/* nouveau point resultant de la superposition de col et du  */
/* point de pimage.                                          */
void put_pix_alpha_use    (image *pimage, int32 x, int32 y, pix col);

/* renvoi le point aux coordonnees (x,y) */
pix get_pix(image *pimage, int32 x, int32 y);

/* dessine une ligne entre (x1,y1) et (x2,y2) de couleur col */
/* utilisation en interne seulement, utilisez plutot les     */
/* fonctions qui suivent.                                    */
void line(image *pimage, int32 x1, int32 y1, int32 x2, int32 y2, pix col,
  AlphaType alpha_type);
/* dessine une ligne entre (x1,y1) et (x2,y2) de couleur col */
/* la transparence resultante dans pimage de celle de col    */
/* void line_alpha_replace(image *pimage, int32 x1, int32 y1,*/
/*   int32 x2, int32 y2, pix col);                           */
#define line_alpha_replace(pimage,x1,y1,x2,y2,col) \
 line(pimage,x1,y1,x2,y2,col,ALPHA_REPLACE)
/* dessine une ligne entre (x1,y1) et (x2,y2) de couleur col */
/* la transparence resultante dans pimage de celle de pimage */
/* void line_alpha_keep   (image *pimage, int32 x1, int32 y1,*/
/*   int32 x2, int32 y2, pix col);                           */
#define line_alpha_keep(pimage,x1,y1,x2,y2,col) \
 line(pimage,x1,y1,x2,y2,col,ALPHA_KEEP)
/* dessine une ligne entre (x1,y1) et (x2,y2) de couleur col */
/* la transparence resultante dans pimage de celle de pimage */
/* la transparence de col est utilise pour calculer le       */
/* nouveau point resultant de la superposition de col et du  */
/* point de pimage.                                          */
/* void line_alpha_use    (image *pimage, int32 x1, int32 y1,*/
/*   int32 x2, int32 y2, pix col);                           */
#define line_alpha_use(pimage,x1,y1,x2,y2,col) \
 line(pimage,x1,y1,x2,y2,col,ALPHA_USE)

/* dessine une ligne avec antialiasee entre */
/* (x1,y1) et (x2,y2) de couleur col        */
void line_aliased(image *pimage, int32 x1, int32 y1, int32 x2, int32 y2,
  pix col);

/* dessine une ligne horizontale entre (x1,y) et (x2,y) de couleur col */
void hline(image *pimage, int32 x1, int32 x2, int32 y, pix col,
  AlphaType alpha_type);
/* dessine une ligne horizontale entre (x1,y) et (x2,y) de couleur col */
/* la transparence resultante dans pimage de celle de col              */
/* void hline_alpha_replace(image *pimage, int32 x1, int32 x2, int32 y,*/
/* pix col);                                                           */
#define hline_alpha_replace(pimage,x1,x2,y,col) \
 hline(pimage,x1,x2,y,col,ALPHA_REPLACE)
/* dessine une ligne horizontale entre (x1,y) et (x2,y) de couleur col */
/* la transparence resultante dans pimage de celle de pimage           */
/* void hline_alpha_keep(image *pimage, int32 x1, int32 x2, int32 y,   */
/*   pix col);                                                         */
#define hline_alpha_keep(pimage,x1,x2,y,col) \
 hline(pimage,x1,x2,y,col,ALPHA_KEEP)
/* dessine une ligne horizontale entre (x1,y) et (x2,y) de couleur col */
/* la transparence resultante dans pimage de celle de pimage           */
/* la transparence de col est utilise pour calculer le                 */
/* nouveau point resultant de la superposition de col et du            */
/* point de pimage.                                                    */
/* void hline_alpha_use(image *pimage, int32 x1, int32 x2, int32 y,    */
/*   pix col);                                                         */
#define hline_alpha_use(pimage,x1,x2,y,col) \
 hline(pimage,x1,x2,y,col,ALPHA_USE)

/* dessine une ligne verticale entre (x,y1) et (x,y2) de couleur col */
void vline(image *pimage, int32 x, int32 y1, int32 y2, pix col,
  AlphaType alpha_type);
/* dessine une ligne verticale entre (x,y1) et (x,y2) de couleur col */
/* la transparence resultante dans pimage de celle de col            */
/* void vline_alpha_replace(image *pimage, int32 x, int32 y1,        */
/*   int32 y2, pix col);                                             */
#define vline_alpha_replace(pimage,x,y1,y2,col) \
 vline(pimage,x,y1,y2,col,ALPHA_REPLACE)
/* dessine une ligne verticale entre (x,y1) et (x,y2) de couleur col */
/* la transparence resultante dans pimage de celle de pimage         */
/* void vline_alpha_keep(image *pimage, int32 x, int32 y1, int32 y2, */
/*   pix col);                                                       */
#define vline_alpha_keep(pimage,x,y1,y2,col) \
 vline(pimage,x,y1,y2,col,ALPHA_KEEP)
/* dessine une ligne verticale entre (x,y1) et (x,y2) de couleur col */
/* la transparence resultante dans pimage de celle de pimage         */
/* la transparence de col est utilise pour calculer le               */
/* nouveau point resultant de la superposition de col et du          */
/* point de pimage.                                                  */
/* void vline_alpha_use(image *pimage, int32 x, int32 y1, int32 y2,  */
/*   pix col);                                                       */
#define vline_alpha_use(pimage,x,y1,y2,col) \
 vline(pimage,x,y1,y2,col,ALPHA_USE)

/* dessine un rectangle vide entre (x1,y1) et (x2,y2) de couleur col */
void rectangle(image *pimage, int32 x1, int32 y1, int32 x2, int32 y2,
  pix col, AlphaType alpha_type);
/* dessine un rectangle vide entre (x1,y1) et (x2,y2) de couleur col */
/* la transparence resultante dans pimage de celle de col            */
/* void rectangle_alpha_replace(image *pimage, int32 x1, int32 y1,   */
/*   int32 x2, int32 y2, pix col);                                   */
#define rectangle_alpha_replace(pimage,x1,y1,x2,y2,col) \
 rectangle(pimage,x1,y1,x2,y2,col,ALPHA_REPLACE)
/* dessine un rectangle vide entre (x1,y1) et (x2,y2) de couleur col */
/* la transparence resultante dans pimage de celle de pimage         */
/* void rectangle_alpha_keep(image *pimage, int32 x1, int32 y1,      */
/*   int32 x2, int32 y2, pix col);                                   */
#define rectangle_alpha_keep(pimage,x1,y1,x2,y2,col) \
 rectangle(pimage,x1,y1,x2,y2,col,ALPHA_KEEP)
/* dessine un rectangle vide entre (x1,y1) et (x2,y2) de couleur col */
/* la transparence resultante dans pimage de celle de pimage         */
/* la transparence de col est utilise pour calculer le               */
/* nouveau point resultant de la superposition de col et du          */
/* point de pimage.                                                  */
/* void rectangle_alpha_use(image *pimage, int32 x1, int32 y1,       */
/*   int32 x2, int32 y2, pix col);                                   */
#define rectangle_alpha_use(pimage,x1,y1,x2,y2,col) \
 rectangle(pimage,x1,y1,x2,y2,col,ALPHA_USE)

/* dessine un rectangle rempli */
void rectangle_fill(image *pimage, int32 x1, int32 y1, int32 x2, int32 y2,
  pix col, AlphaType alpha_type);
/* dessine un rectangle rempli                            */
/* la transparence resultante dans pimage de celle de col */
/* void rectangle_fill_alpha_replace(image *pimage,       */
/*   int32 x1, int32 y1, int32 x2, int32 y2, pix col);    */
#define rectangle_fill_alpha_replace(pimage,x1,y1,x2,y2,col) \
 rectangle_fill(pimage,x1,y1,x2,y2,col,ALPHA_REPLACE)
/* dessine un rectangle rempli                               */
/* la transparence resultante dans pimage de celle de pimage */
/* void rectangle_fill_alpha_keep(image *pimage,             */
/*   int32 x1, int32 y1, int32 x2, int32 y2, pix col);       */
#define rectangle_fill_alpha_keep(pimage,x1,y1,x2,y2,col) \
 rectangle_fill(pimage,x1,y1,x2,y2,col,ALPHA_KEEP)
/* dessine un rectangle rempli                               */
/* la transparence resultante dans pimage de celle de pimage */
/* la transparence de col est utilise pour calculer le       */
/* nouveau point resultant de la superposition de col et du  */
/* point de pimage.                                          */
/* void rectangle_fill_alpha_use(image *pimage,              */
/*   int32 x1, int32 y1, int32 x2, int32 y2, pix col);       */
#define rectangle_fill_alpha_use(pimage,x1,y1,x2,y2,col) \
 rectangle_fill(pimage,x1,y1,x2,y2,col,ALPHA_USE)

/* repaint le fond avec col. meme le canal alpha est remplace */
void clear_img(image *pimage,pix col);

/* dessine une ellipse de centre (cx,cy) et de rayon */
/* horizontal rx, vertical ry                        */
void ellipse(image *pimage, int32 cx, int32 cy, uint32 rx, uint32 ry,
  pix col, AlphaType alpha_type);
/* dessine une ellipse de centre (cx,cy) et de rayon horizontal  */
/* rx, vertical ry                                               */
/* la transparence resultante dans pimage de celle de col        */
/* void ellipse_alpha_replace(image *pimage, int32 cx, int32 cy, */
/*   uint32 rx, uint32 ry, pix col);                             */
#define ellipse_alpha_replace(pimage,cx,cy,rx,ry,col) \
 ellipse(pimage,cx,cy,rx,ry,col,ALPHA_REPLACE)
/* dessine une ellipse de centre (cx,cy) et de rayon         */
/* horizontal rx, vertical ry                                */
/* la transparence resultante dans pimage de celle de pimage */
/* void ellipse_alpha_keep(image *pimage, int32 cx, int32 cy,*/
/*   uint32 rx, uint32 ry, pix col);                         */
#define ellipse_alpha_keep(pimage,cx,cy,rx,ry,col) \
 ellipse(pimage,cx,cy,rx,ry,col,ALPHA_KEEP)
/* dessine une ellipse de centre (cx,cy) et de rayon         */
/* horizontal rx, vertical ry                                */
/* la transparence resultante dans pimage de celle de pimage */
/* la transparence de col est utilise pour calculer le       */
/* nouveau point resultant de la superposition de col et du  */
/* point de pimage.                                          */
/* void ellipse_alpha_use(image *pimage, int32 cx, int32 cy, */
/*   uint32 rx, uint32 ry, pix col);                         */
#define ellipse_alpha_use(pimage,cx,cy,rx,ry,col) \
 ellipse(pimage,cx,cy,rx,ry,col,ALPHA_USE)

/* Dessine une ellipse rempli de centre (cx,cy) et de */
/* rayon horizontal rx, vertical ry                   */
void ellipse_fill(image *pimage, int32 cx, int32 cy,
  uint32 rx, uint32 ry, pix col, AlphaType alpha_type);
/* Dessine une ellipse rempli de centre (cx,cy) et de     */
/* rayon horizontal rx, vertical ry                       */
/* la transparence resultante dans pimage de celle de col */
/* void ellipse_fill_alpha_replace(image *pimage,         */
/*   int32 cx, int32 cy, uint32 rx, uint32 ry, pix col);  */
#define ellipse_fill_alpha_replace(pimage,cx,cy,rx,ry,col) \
 ellipse_fill(pimage,cx,cy,rx,ry,col,ALPHA_REPLACE)
/* Dessine une ellipse rempli de centre (cx,cy) et de        */
/* rayon horizontal rx, vertical ry                          */
/* la transparence resultante dans pimage de celle de pimage */
/* void ellipse_fill_alpha_keep(image *pimage,               */
/*   int32 cx, int32 cy, uint32 rx, uint32 ry, pix col);     */
#define ellipse_fill_alpha_keep(pimage,cx,cy,rx,ry,col) \
 ellipse_fill(pimage,cx,cy,rx,ry,col,ALPHA_KEEP)
/* Dessine une ellipse rempli de centre (cx,cy) et de        */
/* rayon horizontal rx, vertical ry                          */
/* la transparence resultante dans pimage de celle de pimage */
/* la transparence de col est utilise pour calculer le       */
/* nouveau point resultant de la superposition de col et du  */
/* point de pimage.                                          */
/* void ellipse_fill_alpha_use(image *pimage, int32 cx,      */
/*   int32 cy, uint32 rx, uint32 ry, pix col);               */
#define ellipse_fill_alpha_use(pimage,cx,cy,rx,ry,col) \
 ellipse_fill(pimage,cx,cy,rx,ry,col,ALPHA_USE)

/* dessine une ligne horizontale entre (x1,y) de couleur col1    */
/* et (x2,y) de couleur col2 avec un degrade lineaire de couleur */
void hline_shade(image *pimage, int32 x1, int32 x2, int32 y,
  pix col1, pix col2, AlphaType alpha_type);
/* dessine une ligne horizontale entre (x1,y) de couleur col1    */
/* et (x2,y) de couleur col2 avec un degrade lineaire de couleur */
/* la transparence resultante dans pimage de celle du degrade    */
/* de col1 et col2                                               */
/* void hline_shade_alpha_replace(image *pimage,                 */
/*   int32 x1, int32 x2, int32 y, pix col1, pix col2);           */
#define hline_shade_alpha_replace(pimage,x1,x2,y,col1,col2) \
 hline_shade(pimage,x1,x2,y,col1,col2,ALPHA_REPLACE)
/* dessine une ligne horizontale entre (x1,y) de couleur col1    */
/* et (x2,y) de couleur col2 avec un degrade lineaire de couleur */
/* la transparence resultante dans pimage de celle de pimage     */
/* void hline_shade_alpha_keep(image *pimage, int32 x1, int32 x2,*/
/*   int32 y, pix col1, pix col2);                               */
#define hline_shade_alpha_keep(pimage,x1,x2,y,col1,col2) \
 hline_shade(pimage,x1,x2,y,col1,col2,ALPHA_KEEP)
/* dessine une ligne horizontale entre (x1,y) de couleur col1    */
/* et (x2,y) de couleur col2 avec un degrade lineaire de couleur */
/* la transparence resultante dans pimage de celle de pimage     */
/* le degrade de transparence de col1 et col2 est utilise pour   */
/* calculer le nouveau point resultant de la superposition de    */
/* col1 col2 et du point de pimage.                              */
/* void hline_shade_alpha_use(image *pimage,                     */
/*   int32 x1, int32 x2, int32 y, pix col1, pix col2);           */
#define hline_shade_alpha_use(pimage,x1,x2,y,col1,col2) \
 hline_shade(pimage,x1,x2,y,col1,col2,ALPHA_USE)

/* dessine une ligne verticale entre (x,y1) de couleur col1      */
/* et (x,y2) de couleur col2 avec un degrade lineaire de couleur */
void vline_shade(image *pimage, int32 x, int32 y1, int32 y2,
  pix col1, pix col2, AlphaType alpha_type);
/* dessine une ligne verticale entre (x,y1) de couleur col1      */
/* et (x,y2) de couleur col2 avec un degrade lineaire de couleur */
/* la transparence resultante dans pimage de celle du degrade de */
/* col1 vers col2                                                */
/* void vline_shade_alpha_replace(image *pimage, int32 x,        */
/*   int32 y1, int32 y2, pix col1, pix col2);                    */
#define vline_shade_alpha_replace(pimage,x,y1,y2,col1,col2) \
 vline_shade(pimage,x,y1,y2,col1,col2,ALPHA_REPLACE)
/* dessine une ligne verticale entre (x,y1) de couleur col1      */
/* et (x,y2) de couleur col2 avec un degrade lineaire de couleur */
/* la transparence resultante dans pimage de celle de pimage     */
/* void vline_shade_alpha_keep(image *pimage, int32 x, int32 y1, */
/*   int32 y2, pix col1, pix col2);                              */
#define vline_shade_alpha_keep(pimage,x,y1,y2,col1,col2) \
 vline_shade(pimage,x,y1,y2,col1,col2,ALPHA_KEEP)
/* dessine une ligne verticale entre (x,y1) de couleur col1      */
/* et (x,y2) de couleur col2 avec un degrade lineaire de couleur */
/* la transparence resultante dans pimage de celle de pimage     */
/* le degrade de transparence de col1 et col2 est utilise pour   */
/* calculer le nouveau point resultant de la superposition de    */
/* col1 col2 et du point de pimage.                              */
/* void vline_shade_alpha_use(image *pimage, int32 x,            */
/*   int32 y1, int32 y2, pix col1, pix col2);                    */
#define vline_shade_alpha_use(pimage,x,y1,y2,col1,col2) \
 vline_shade(pimage,x,y1,y2,col1,col2,ALPHA_USE)

/* Dessine un rectangle rempli entre les points (x1,y1) et (x2,y2) */
/* ce rectangle est rempli par un degrade lineaire de couleur      */
/*  definie comme sur le schema ci-dessous                         */
/*  col1 ---- col2                                                 */
/*   |          |                                                  */
/*  col3 ---- col4                                                 */
void rectangle_fill_shade(image *pimage,
  int32 x1, int32 y1, int32 x2, int32 y2,
  pix col1, pix col2, pix col3, pix col4, AlphaType alpha_type);
/* Dessine un rectangle rempli entre les points (x1,y1) et (x2,y2) */
/* ce rectangle est rempli par un degrade lineaire de couleur      */
/*  definie comme sur le schema ci-dessous                         */
/*  col1 ---- col2                                                 */
/*   |          |                                                  */
/*  col3 ---- col4                                                 */
/* la transparence resultante dans pimage de celle du degrade de   */
/* col1, col2, col3 et col4                                        */
/* void rectangle_fill_shade_alpha_replace(image *pimage,          */
/*   int32 x1, int32 y1, int32 x2, int32 y2,                       */
/*   pix col1, pix col2, pix col3, pix col4);                      */
#define rectangle_fill_shade_alpha_replace(pimage,x1,y1,x2,y2,\
col1,col2,col3,col4) \
 rectangle_fill_shade(pimage,x1,y1,x2,y2,col1,col2,col3,col4,ALPHA_REPLACE)
/* Dessine un rectangle rempli entre les points (x1,y1) et (x2,y2) */
/* ce rectangle est rempli par un degrade lineaire de couleur      */
/*  definie comme sur le schema ci-dessous                         */
/*  col1 ---- col2                                                 */
/*   |          |                                                  */
/*  col3 ---- col4                                                 */
/* la transparence resultante dans pimage de celle de pimage       */
/* void rectangle_fill_shade_alpha_keep(image *pimage,             */
/*   int32 x1, int32 y1, int32 x2, int32 y2,                       */
/*   pix col1, pix col2, pix col3, pix col4);                      */
#define rectangle_fill_shade_alpha_keep(pimage,x1,y1,x2,y2,\
col1,col2,col3,col4) \
 rectangle_fill_shade(pimage,x1,y1,x2,y2,col1,col2,col3,col4,ALPHA_KEEP)
/* Dessine un rectangle rempli entre les points (x1,y1) et (x2,y2) */
/* ce rectangle est rempli par un degrade lineaire de couleur      */
/*  definie comme sur le schema ci-dessous                         */
/*  col1 ---- col2                                                 */
/*   |          |                                                  */
/*  col3 ---- col4                                                 */
/* la transparence resultante dans pimage de celle de pimage       */
/* le degrade de transparence de col1, col2, col3 et col4 est      */
/* utilise pour calculer le nouveau point resultant de la          */
/* superposition du degrade et du point de pimage.                 */
/* void rectangle_fill_shade_alpha_use(image *pimage,              */
/*   int32 x1, int32 y1, int32 x2, int32 y2,                       */
/*   pix col1, pix col2, pix col3, pix col4);                      */
#define rectangle_fill_shade_alpha_use(pimage,x1,y1,x2,y2,\
col1,col2,col3,col4) \
 rectangle_fill_shade(pimage,x1,y1,x2,y2,col1,col2,col3,col4,ALPHA_USE)

/* remplace la couleur src_col dans pimage par dst_col */
void image_replace_color(image *pimage, pix dst_col, pix src_col);

#endif /* __IMAGE_DRAW_FUNC_H__ */

#ifndef __COLOR_H__
#define __COLOR_H__

#define COL(red,green,blue) (((red)<<24)|((green)<<16)|((blue)<<8)|0xFF)
#define COL_FULL(red,green,blue,alpha) (((red)<<24)|((green)<<16)|((blue)<<8)|(alpha))
#define SET_COL(col,red,green,blue) {col=(red)<<24|(green)<<16|(blue)<<8|0xFF;}
#define SET_COL_RED(col,red)     {col=((col)&0x00FFFFFF)|((red)<<24);  }
#define SET_COL_GREEN(col,green) {col=((col)&0xFF00FFFF)|((green)<<16);}
#define SET_COL_BLUE(col,blue)   {col=((col)&0xFFFF00FF)|((blue)<<8);  }
#define SET_COL_ALPHA(col,alpha) {col=((col)&0xFFFFFF00)|(alpha);      }
#define COL_RED(col)   (col>>24)
#define COL_GREEN(col) ((col>>16)&0xFF)
#define COL_BLUE(col)  ((col>>8)&0xFF)
#define COL_ALPHA(col) (col&0xFF)

#define RED_MASK   0xFF000000
#define GREEN_MASK 0x00FF0000
#define BLUE_MASK  0x0000FF00
#define ALPHA_MASK 0x000000FF

#define RED_SHL   24
#define GREEN_SHL 16
#define BLUE_SHL  8
#define ALPHA_SHL 0

#define WHITE  0xFFFFFFFF
#define BLACK  0x000000FF
#define BLUE   0x0000FFFF
#define GREEN  0x00FF00FF
#define RED    0xFF0000FF
#define ORANGE 0xF08020FF
#define YELLOW 0xFFFF00FF

#endif /* __COLOR_H__ */
