/* GIMP Dustcleaner
 * Copyright (C) 2006-2008  Frank Tao<solotim.cn@gmail.com> (the "Author").
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Author of the
 * Software shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the Author.
 */

#include <string.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "config.h"
#include "main.h"
#include "interface.h"
#include "render.h"
#include "plugin-intl.h"
#include "dustcleaner_gimp_plugin.h"
#include "dust.h"

/*  Constants  */

#define PROCEDURE_NAME   "dustcleaner"
#define DATA_KEY_VALS    "plug-in-dustcleaner-vals"
#define DATA_KEY_UI_VALS "dustcleaner_ui"
#define PARASITE_KEY     "dustcleaner-options"

/*  Local function prototypes  */

static void   query (void);
static void   run   (const gchar      *name,
                     gint              nparams,
                     const GimpParam  *param,
                     gint             *nreturn_vals,
                     GimpParam       **return_vals);


/*  Local variables  */


const PlugInVals default_vals =
{
	.ptr					    = NULL,
	.compensationMap		    = {},
	.dustSize				    = {8, 8},
	.sensitivity			    = 60,
	.scanStep				    = 8,
	.noiseReductionCoreLenght   = 120,
	.noiseReductionCorePower    = 1,
	.hasContourMorphologyEx	    = 1,
	.kernelSize				    = 0,
	.kernelType				    = CV_GAUSSIAN,
	.isSave					    = 0,
	.op						    = DUST_DETECTION,
	.margin					    = 0,
	.segmentationSensitivity    = 50,
	.recoveryStrength		    = 0
};


const PlugInImageVals default_image_vals =
{
    0
};

const PlugInDrawableVals default_drawable_vals =
{
    0
};

const PlugInUIVals default_ui_vals =
{
    TRUE
};

static PlugInVals         vals;
static PlugInImageVals    image_vals;
static PlugInDrawableVals drawable_vals;
static PlugInUIVals       ui_vals;


GimpPlugInInfo PLUG_IN_INFO =
{
    NULL,  /* init_proc  */
    NULL,  /* quit_proc  */
    query, /* query_proc */
    run,   /* run_proc   */
};

MAIN ()

static void
query (void)
{
    gchar *help_path;
    gchar *help_uri;

    static GimpParamDef args[] =
    {
        { GIMP_PDB_INT32,    "run_mode",   "Interactive, non-interactive"    },
        { GIMP_PDB_IMAGE,    "image",      "Input image"                     },
        { GIMP_PDB_DRAWABLE, "drawable",   "Input drawable"                  },
        { GIMP_PDB_INT32,    "dummy",      "dummy1"                          }
    };

    gimp_plugin_domain_register (PLUGIN_NAME, LOCALEDIR);

    help_path = g_build_filename (DATADIR, "help", NULL);
    help_uri = g_filename_to_uri (help_path, NULL, NULL);
    g_free (help_path);

    gimp_plugin_help_register ("http://developer.gimp.org/dustcleaner/help",
                               help_uri);

    gimp_install_procedure (PROCEDURE_NAME,
                            "Filter",
                            "Help",
                            "Frank Tao<solotim.cn@gmail.com>",
                            "Frank Tao<solotim.cn@gmail.com>",
                            "2006-2008",
                            N_("DustCleaner..."),
                            "RGB*",
                            GIMP_PLUGIN,
                            G_N_ELEMENTS (args), 0,
                            args, NULL);

    gimp_plugin_menu_register (PROCEDURE_NAME, "<Image>/Filters/Noise/");
}

static void
run (const gchar      *name,
     gint              n_params,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
    static GimpParam   values[1];
    GimpDrawable      *drawable;
    gint32             image_ID;
    GimpRunMode        run_mode;
    GimpPDBStatusType  status = GIMP_PDB_SUCCESS;

    *nreturn_vals = 1;
    *return_vals  = values;

    /*  Initialize i18n support  */
    bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
    textdomain (GETTEXT_PACKAGE);

    run_mode = param[0].data.d_int32;
    image_ID = param[1].data.d_int32;
    drawable = gimp_drawable_get (param[2].data.d_drawable);

    /*  Initialize with default values  */
    vals          = default_vals;
    image_vals    = default_image_vals;
    drawable_vals = default_drawable_vals;
    ui_vals       = default_ui_vals;

    if (strcmp (name, PROCEDURE_NAME) == 0)
    {
        switch (run_mode)
        {
        case GIMP_RUN_NONINTERACTIVE:
            if (n_params != 4)
            {
                status = GIMP_PDB_CALLING_ERROR;
            }
            else
            {
                // @TODO: figure this out
                vals.sensitivity = param[3].data.d_int32;
            }
            break;

        case GIMP_RUN_INTERACTIVE:
            /* Get options last values if needed */
            gimp_get_data (DATA_KEY_VALS, &vals);
            gimp_get_data (DATA_KEY_UI_VALS, &ui_vals);
            /* Display the dialog */
            gboolean ack = dialog (image_ID,
                                    drawable,
                                    &vals,
                                    &image_vals,
                                    &drawable_vals,
                                    &ui_vals);
            
            if (!ack)
            {
                status = GIMP_PDB_CANCEL;
            }

            break;

        case GIMP_RUN_WITH_LAST_VALS:
            /*  Possibly retrieve data  */
            gimp_get_data (DATA_KEY_VALS, &vals);

            break;

        default:
            break;
        }
    }
    else
    {
        status = GIMP_PDB_CALLING_ERROR;
    }

    if (status == GIMP_PDB_SUCCESS)
    {
        render (image_ID, drawable, &vals, &image_vals, &drawable_vals);

        if (run_mode != GIMP_RUN_NONINTERACTIVE)
            gimp_displays_flush ();

        if (run_mode == GIMP_RUN_INTERACTIVE)
        {
            gimp_set_data (DATA_KEY_VALS,    &vals,    sizeof (vals));
            gimp_set_data (DATA_KEY_UI_VALS, &ui_vals, sizeof (ui_vals));
        }

        gimp_drawable_detach (drawable);
    }

    values[0].type = GIMP_PDB_STATUS;
    values[0].data.d_status = status;
}

