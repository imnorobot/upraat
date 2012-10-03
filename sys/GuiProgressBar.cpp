/* GuiProgressBar.cpp
 *
 * Copyright (C) 1993-2012 Paul Boersma, 2008 Stefan de Konink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "GuiP.h"

Thing_implement (GuiProgressBar, GuiControl, 0);

#if gtk
	static void _guiGtkProgressBar_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiProgressBar);
		Melder_free (me);
	}
#elif cocoa
#elif motif
	static void _guiMotifProgressBar_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiProgressBar);
		forget (me);
	}
#endif

GuiProgressBar GuiProgressBar_create (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags)
{
	GuiProgressBar me = Thing_new (GuiProgressBar);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		my d_widget = gtk_progress_bar_new ();
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
	#elif cocoa
	#elif motif
		my d_widget = XmCreateScale (parent -> d_widget, "scale", NULL, 0);
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		XtVaSetValues (my d_widget, XmNorientation, XmHORIZONTAL,
			XmNminimum, 0, XmNmaximum, 10000, XmNvalue, 0,
			#if ! defined (macintosh)
				XmNscaleHeight, 20,
			#endif
			#ifdef macintosh
				//XmNscaleWidth, 340,
			#endif
			NULL);
	#endif

	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkProgressBar_destroyCallback), me);
	#elif cocoa
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifProgressBar_destroyCallback, me);
	#endif

	return me;
}

GuiProgressBar GuiProgressBar_createShown (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags)
{
	GuiProgressBar me = GuiProgressBar_create (parent, left, right, top, bottom, flags);
	my f_show ();
	return me;
}

void structGuiProgressBar :: f_setValue (double value) {
	#if gtk
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (d_widget), value);
	#elif cocoa
	#elif motif
		XmScaleSetValue (d_widget, round (value * 10000));
	#endif
}

/* End of file GuiProgressBar.cpp */
