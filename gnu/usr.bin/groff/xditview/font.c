/*
 * font.c
 *
 * map dvi fonts to X fonts
 */

#ifndef lint
static char rcsid[] = "$Id: font.c,v 1.2 1993/08/02 17:46:00 mycroft Exp $";
#endif /* not lint */

#include <X11/Xos.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <ctype.h>
#include "DviP.h"
#include "XFontName.h"

static DisposeFontSizes();

static char *
savestr (s)
	char	*s;
{
	char	*n;

	if (!s)
		return 0;
	n = XtMalloc (strlen (s) + 1);
	if (n)
		strcpy (n, s);
	return n;
}

static DviFontList *
LookupFontByPosition (dw, position)
	DviWidget	dw;
	int		position;
{
	DviFontList	*f;

	for (f = dw->dvi.fonts; f; f = f->next)
		if (f->dvi_number == position)
			break;
	return f;
}

int
MaxFontPosition (dw)
	DviWidget dw;
{
	DviFontList	*f;
	int n = -1;

	for (f = dw->dvi.fonts; f; f = f->next)
		if (f->dvi_number > n)
			n = f->dvi_number;
	return n;
}

static DviFontSizeList *
LookupFontSizeBySize (dw, f, size)
	DviWidget	dw;
	DviFontList	*f;
	int		size;
{
	DviFontSizeList	*fs, *best = 0, *smallest = 0;
	int		bestsize = 0;
	XFontName	fontName;
	unsigned int    fontNameAttributes;
	char	    	fontNameString[2048];
	int		decipointsize;
	
	if (f->scalable) {
		decipointsize = (10*size)/dw->dvi.sizescale;
		for (best = f->sizes; best; best = best->next)
			if (best->size == decipointsize)
				return best;
		best = (DviFontSizeList *) XtMalloc(sizeof *best);
		best->next = f->sizes;
		best->size = decipointsize;
		f->sizes = best;
		XParseFontName (f->x_name, &fontName, &fontNameAttributes);
		fontNameAttributes &= ~(FontNamePixelSize|FontNameAverageWidth);
		fontNameAttributes |= FontNameResolutionX;
		fontNameAttributes |= FontNameResolutionY;
		fontNameAttributes |= FontNamePointSize;
		fontName.ResolutionX = dw->dvi.display_resolution;
		fontName.ResolutionY = dw->dvi.display_resolution;
		fontName.PointSize = decipointsize;
		XFormatFontName (&fontName, fontNameAttributes, fontNameString);
		best->x_name = savestr (fontNameString);
		best->doesnt_exist = 0;
		best->font = 0;
		return best;
	}
	for (fs = f->sizes; fs; fs=fs->next) {
		if (dw->dvi.sizescale*fs->size <= 10*size
		    && fs->size >= bestsize) {
			best = fs;
			bestsize = fs->size;
		}
		if (smallest == 0 || fs->size < smallest->size)
			smallest = fs;
	}
	return best ? best : smallest;
}

static char *
SkipFontNameElement (n)
	char	*n;
{
	while (*n != '-')
		if (!*++n)
			return 0;
	return n+1;
}

# define SizePosition		8
# define EncodingPosition	13

static
ConvertFontNameToSize (n)
	char	*n;
{
	int	i, size;

	for (i = 0; i < SizePosition; i++) {
		n = SkipFontNameElement (n);
		if (!n)
			return -1;
	}
	size = atoi (n);
	return size;
}

static char *
ConvertFontNameToEncoding (n)
	char	*n;
{
        int i;
	for (i = 0; i < EncodingPosition; i++) {
		n = SkipFontNameElement (n);
		if (!n)
			return 0;
	}
	return n;
}

DviFontSizeList *
InstallFontSizes (dw, x_name, scalablep)
	DviWidget	dw;
	char		*x_name;
	Boolean		*scalablep;
{
	char	fontNameString[2048];
	char	**fonts;
	int	i, count;
	int	size;
	DviFontSizeList	*sizes, *new;
	XFontName	fontName;
	unsigned int	fontNameAttributes;

	*scalablep = FALSE;
	if (!XParseFontName (x_name, &fontName, &fontNameAttributes))
		return 0;
	fontNameAttributes &= ~(FontNamePixelSize|FontNamePointSize
				|FontNameAverageWidth);
	fontNameAttributes |= FontNameResolutionX;
	fontNameAttributes |= FontNameResolutionY;
	fontName.ResolutionX = dw->dvi.display_resolution;
	fontName.ResolutionY = dw->dvi.display_resolution;
	XFormatFontName (&fontName, fontNameAttributes, fontNameString);
	fonts = XListFonts (XtDisplay (dw), fontNameString, 10000000, &count);
	sizes = 0;
	for (i = 0; i < count; i++) {
		size = ConvertFontNameToSize (fonts[i]);
		if (size == 0) {
			DisposeFontSizes (dw, sizes);
			sizes = 0;
			*scalablep = TRUE;
			break;
		}
		if (size != -1) {
			new = (DviFontSizeList *) XtMalloc (sizeof *new);
			new->next = sizes;
			new->size = size;
			new->x_name = savestr (fonts[i]);
			new->doesnt_exist = 0;
			new->font = 0;
			sizes = new;
		}
	}
	XFreeFontNames (fonts);
	return sizes;
}

static
DisposeFontSizes (dw, fs)
	DviWidget	dw;
	DviFontSizeList	*fs;
{
	DviFontSizeList	*next;

	for (; fs; fs=next) {
		next = fs->next;
		if (fs->x_name)
			XtFree (fs->x_name);
		if (fs->font) {
			XUnloadFont (XtDisplay (dw), fs->font->fid);
			XFree ((char *)fs->font);
		}
		XtFree ((char *) fs);
	}
}

static DviFontList *
InstallFont (dw, position, dvi_name, x_name)
	DviWidget	dw;
	int		position;
	char		*dvi_name;
	char		*x_name;
{
	DviFontList	*f;
	char		*encoding;

	if ((f = LookupFontByPosition (dw, position)) != NULL) {
		/*
		 * ignore gratuitous font loading
		 */
		if (!strcmp (f->dvi_name, dvi_name) &&
		    !strcmp (f->x_name, x_name))
			return f;

		DisposeFontSizes (dw, f->sizes);
		if (f->dvi_name)
			XtFree (f->dvi_name);
		if (f->x_name)
			XtFree (f->x_name);
		f->device_font = 0;
	} else {
		f = (DviFontList *) XtMalloc (sizeof (*f));
		f->next = dw->dvi.fonts;
		dw->dvi.fonts = f;
	}
	f->initialized = FALSE;
	f->dvi_name = savestr (dvi_name);
	f->device_font = device_find_font (dw->dvi.device, dvi_name);
	f->x_name = savestr (x_name);
	f->dvi_number = position;
	f->sizes = 0;
	f->scalable = FALSE;
	if (f->x_name) {
		encoding = ConvertFontNameToEncoding (f->x_name);
		f->char_map = DviFindMap (encoding);
	} else
		f->char_map = 0;
	/* 
	 * force requery of fonts
	 */
	dw->dvi.font = 0;
	dw->dvi.font_number = -1;
	dw->dvi.cache.font = 0;
	dw->dvi.cache.font_number = -1;
	dw->dvi.device_font = 0;
	dw->dvi.device_font_number = -1;
	return f;
}

ForgetFonts (dw)
	DviWidget dw;
{
	DviFontList *f = dw->dvi.fonts;
	
	while (f) {
		DviFontList *tem = f;

		if (f->sizes)
			DisposeFontSizes (dw, f->sizes);
		if (f->dvi_name)
			XtFree (f->dvi_name);
		if (f->x_name)
			XtFree (f->x_name);
		f = f->next;
		XtFree ((char *) tem);
	}
	
	/* 
	 * force requery of fonts
	 */
	dw->dvi.font = 0;
	dw->dvi.font_number = -1;
	dw->dvi.cache.font = 0;
	dw->dvi.cache.font_number = -1;
	dw->dvi.device_font = 0;
	dw->dvi.device_font_number = -1;
	dw->dvi.fonts = 0;
}


static char *
MapDviNameToXName (dw, dvi_name)
	DviWidget	dw;
	char		*dvi_name;
{
	DviFontMap	*fm;
	
	for (fm = dw->dvi.font_map; fm; fm=fm->next)
		if (!strcmp (fm->dvi_name, dvi_name))
			return fm->x_name;
	return 0;
}

#if 0
static char *
MapXNameToDviName (dw, x_name)
	DviWidget	dw;
	char		*x_name;
{
	DviFontMap	*fm;
	
	for (fm = dw->dvi.font_map; fm; fm=fm->next)
		if (!strcmp (fm->x_name, x_name))
			return fm->dvi_name;
	return 0;
}
#endif

ParseFontMap (dw)
	DviWidget	dw;
{
	char		dvi_name[1024];
	char		x_name[2048];
	char		*m, *s;
	DviFontMap	*fm, *new;

	if (dw->dvi.font_map)
		DestroyFontMap (dw->dvi.font_map);
	fm = 0;
	m = dw->dvi.font_map_string;
	while (*m) {
		s = m;
		while (*m && !isspace (*m))
			++m;
		strncpy (dvi_name, s, m-s);
		dvi_name[m-s] = '\0';
		while (isspace (*m))
			++m;
		s = m;
		while (*m && *m != '\n')
			++m;
		strncpy (x_name, s, m-s);
		x_name[m-s] = '\0';
		new = (DviFontMap *) XtMalloc (sizeof *new);
		new->x_name = savestr (x_name);
		new->dvi_name = savestr (dvi_name);
		new->next = fm;
		fm = new;
		++m;
	}
	dw->dvi.font_map = fm;
}

DestroyFontMap (font_map)
	DviFontMap	*font_map;
{
	DviFontMap	*next;

	for (; font_map; font_map = next) {
		next = font_map->next;
		if (font_map->x_name)
			XtFree (font_map->x_name);
		if (font_map->dvi_name)
			XtFree (font_map->dvi_name);
		XtFree ((char *) font_map);
	}
}

/* ARGSUSED */

SetFontPosition (dw, position, dvi_name, extra)
	DviWidget	dw;
	int		position;
	char		*dvi_name;
	char		*extra;	/* unused */
{
	char	*x_name;

	x_name = MapDviNameToXName (dw, dvi_name);
	if (x_name)
		(void) InstallFont (dw, position, dvi_name, x_name);
}

XFontStruct *
QueryFont (dw, position, size)
	DviWidget	dw;
	int		position;
	int		size;
{
	DviFontList	*f;
	DviFontSizeList	*fs;

	f = LookupFontByPosition (dw, position);
	if (!f)
		return dw->dvi.default_font;
	if (!f->initialized) {
		f->sizes = InstallFontSizes (dw, f->x_name, &f->scalable);
		f->initialized = TRUE;
	}
	fs = LookupFontSizeBySize (dw, f, size);
	if (!fs)
		return dw->dvi.default_font;
	if (!fs->font) {
		if (fs->x_name)
			fs->font = XLoadQueryFont (XtDisplay (dw), fs->x_name);
		if (!fs->font)
			fs->font = dw->dvi.default_font;
	}
	return fs->font;
}

DeviceFont *
QueryDeviceFont (dw, position)
	DviWidget	dw;
	int		position;
{
	DviFontList	*f;

	f = LookupFontByPosition (dw, position);
	if (!f)
		return 0;
	return f->device_font;
}

DviCharNameMap *
QueryFontMap (dw, position)
	DviWidget	dw;
	int		position;
{
	DviFontList	*f;

	f = LookupFontByPosition (dw, position);
	if (f)
	    return f->char_map;
	else
	    return 0;
}

#if 0
LoadFont (dw, position, size)
	DviWidget	dw;
	int		position;
	int		size;
{
	XFontStruct	*font;

	font = QueryFont (dw, position, size);
	dw->dvi.font_number = position;
	dw->dvi.font_size = size;
	dw->dvi.font = font;
	XSetFont (XtDisplay (dw), dw->dvi.normal_GC, font->fid);
	return;
}
#endif

/*
Local Variables:
c-indent-level: 8
c-continued-statement-offset: 8
c-brace-offset: -8
c-argdecl-indent: 8
c-label-offset: -8
c-tab-always-indent: nil
End:
*/
