/*
	DustCleaner is a GIMP plugin to detect and remove the dust spots 
	in digital image.
	Copyright (C) 2006-2007  Frank Tao<solotim.cn@gmail.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
	02110-1301, USA        	
*/
#ifdef __DUSTCLEANER_UI_H__
#define __DUSTCLEANER_UI_H__


#include <gtk/gtk.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#include "plugin-intl.h"

gboolean show_dustcleaner_dialog (GimpDrawable *drawable, TPara *para);
GtkWidget* create_dialog (GimpDrawable *drawable);

#endif
