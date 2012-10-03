/* GuiDrawingArea.cpp
 *
 * Copyright (C) 1993-2012 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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
#if gtk
	#include "gdk/gdkkeysyms.h"
#endif

Thing_implement (GuiDrawingArea, GuiControl, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_drawingarea \
		Melder_assert (widget -> widgetClass == xmDrawingAreaWidgetClass); \
		GuiDrawingArea me = (GuiDrawingArea) widget -> userData
#else
	#define iam_drawingarea \
		GuiDrawingArea me = (GuiDrawingArea) _GuiObject_getUserData (widget)
#endif

#if gtk
	static void _GuiGtkDrawingArea_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiDrawingArea);
		forget (me);
	}
//	static void _GuiGtkDrawingArea_activateCallback (GtkAction *action, gpointer void_me) {
		//iam (GuiDrawingArea);
		// TODO: compliled niet
		/*
		struct structGuiDrawingAreaEvent event = { widget, 0 };
		if (my activateCallback != NULL) {
			my activateCallback (my activateBoss, & event);
		}*/
//	}
	static gboolean _GuiGtkDrawingArea_exposeCallback (GuiObject widget, GdkEventExpose *expose, gpointer void_me) {
		trace ("begin");
		iam (GuiDrawingArea);
		Melder_assert (me);
		// TODO: that helps agains the damaged regions outside the rect where the
		// Graphics drawing is done, but where does that margin come from in the
		// first place?? Additionally this causes even more flickering
		//gdk_window_clear_area(widget->window, expose->area.x, expose->area.y, expose->area.width, expose->area.height);
		if (my d_exposeCallback) {
			struct structGuiDrawingAreaExposeEvent event = { me, 0 };
			event. x = expose -> area. x;
			event. y = expose -> area. y;
			event. width = expose -> area. width;
			event. height = expose -> area. height;
			try {
				//GdkRectangle rect = { event. x, event. y, event. width, event. height };
				//gdk_window_begin_paint_rect ((GTK_WIDGET (widget)) -> window, & rect);
				trace ("send the expose callback");
				my d_exposeCallback (my d_exposeBoss, & event);
				trace ("the expose callback finished");
				//gdk_window_end_paint ((GTK_WIDGET (widget)) -> window);
			} catch (MelderError) {
				Melder_flushError ("Redrawing not completed");
			}
			trace ("the expose callback handled drawing");
			return TRUE;
		}
		trace ("GTK will handle redrawing");
		return FALSE;
	}
	static gboolean _GuiGtkDrawingArea_clickCallback (GuiObject widget, GdkEvent *e, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my d_clickCallback) {
			struct structGuiDrawingAreaClickEvent event = { me, 0 };
			trace ("event type %ld", (long) e -> type);
			switch (e -> type) {
				case GDK_BUTTON_PRESS:
					event. type = BUTTON_PRESS;
					event. button = ((GdkEventButton *) e) -> button;
					break;
				case GDK_BUTTON_RELEASE:
					event. type = BUTTON_RELEASE;
					event. button = ((GdkEventButton *) e) -> button;
					break;
				case GDK_MOTION_NOTIFY:
					event. type = MOTION_NOTIFY;
					event. button =
						((GdkEventMotion *) e) -> state & GDK_BUTTON1_MASK ? 1 :
						((GdkEventMotion *) e) -> state & GDK_BUTTON2_MASK ? 2 :
						((GdkEventMotion *) e) -> state & GDK_BUTTON3_MASK ? 3 :
						((GdkEventMotion *) e) -> state & GDK_BUTTON4_MASK ? 4 :
						((GdkEventMotion *) e) -> state & GDK_BUTTON5_MASK ? 5 : 0;
					break;
				default:
					// Do NOTHING
					return FALSE;
			}
			event. x = ((GdkEventButton *) e) -> x;
			event. y = ((GdkEventButton *) e) -> y;
			event. shiftKeyPressed = (((GdkEventButton *) e) -> state & GDK_SHIFT_MASK) != 0;
			if (e -> type == GDK_BUTTON_PRESS || 1) {
				try {
					my d_clickCallback (my d_clickBoss, & event);
				} catch (MelderError) {
					Melder_flushError ("Mouse click not completely handled.");
				}
				return TRUE;
			}
		}
		return FALSE;
	}
	static gboolean _GuiGtkDrawingArea_keyCallback (GuiObject widget, GdkEvent *gevent, gpointer void_me) {
		iam (GuiDrawingArea);
		trace ("begin");
		if (my d_keyCallback && gevent -> type == GDK_KEY_PRESS) {
			struct structGuiDrawingAreaKeyEvent event = { me, 0 };
			GdkEventKey *gkeyEvent = (GdkEventKey *) gevent;
			event. key = gkeyEvent -> keyval;
			/*
			 * Translate with the help of /usr/include/gtk-2.0/gdk/gdkkeysyms.h
			 */
			if (event. key == GDK_Escape) event. key = 27;
			if (event. key == GDK_Left)   event. key = 0x2190;
			if (event. key == GDK_Up)     event. key = 0x2191;
			if (event. key == GDK_Right)  event. key = 0x2192;
			if (event. key == GDK_Down)   event. key = 0x2193;
			event. shiftKeyPressed = (gkeyEvent -> state & GDK_SHIFT_MASK) != 0;
			event. commandKeyPressed = (gkeyEvent -> state & GDK_CONTROL_MASK) != 0;
			event. optionKeyPressed = (gkeyEvent -> state & GDK_MOD1_MASK) != 0;
			event. extraControlKeyPressed = false;
			try {
				my d_keyCallback (my d_keyBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Key press not completely handled.");
			}
			/*
			 * FIXME: here we should empty the type-ahead buffer
			 */
			return TRUE;
		}
		return FALSE;   // if the drawing area has no keyCallback, the system will send the key press to a text field.
	}
	static gboolean _GuiGtkDrawingArea_resizeCallback (GuiObject widget, GtkAllocation *allocation, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my d_resizeCallback) {
			struct structGuiDrawingAreaResizeEvent event = { me, 0 };
			trace ("drawingArea resized to %d x %d.", (int) allocation -> width, (int) allocation -> height);
			event. width = allocation -> width;
			event. height = allocation -> height;
			//g_debug("%d %d", allocation->width, allocation->height);
			try {
				my d_resizeCallback (my d_resizeBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Window resizing not completely handled.");
			}
			return TRUE;
		}
		return FALSE;
	}
#elif cocoa
#elif win
	void _GuiWinDrawingArea_destroy (GuiObject widget) {
		iam_drawingarea;
		DestroyWindow (widget -> window);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinDrawingArea_update (GuiObject widget) {
		iam_drawingarea;
		PAINTSTRUCT paintStruct;
		BeginPaint (widget -> window, & paintStruct);
		if (my d_exposeCallback) {
			struct structGuiDrawingAreaExposeEvent event = { me };
			try {
				my d_exposeCallback (my d_exposeBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Redrawing not completed");
			}
		}
		EndPaint (widget -> window, & paintStruct);
	}
	void _GuiWinDrawingArea_handleClick (GuiObject widget, int x, int y) {
		iam_drawingarea;
		if (my d_clickCallback) {
			struct structGuiDrawingAreaClickEvent event = { me, 0 };
			event. x = x;
			event. y = y;
			event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;
			try {
				my d_clickCallback (my d_clickBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Mouse click not completely handled.");
			}
		}
	}
	void _GuiWinDrawingArea_handleKey (GuiObject widget, TCHAR kar) {   // TODO: event?
		iam_drawingarea;
		if (my d_keyCallback) {
			struct structGuiDrawingAreaKeyEvent event = { me, 0 };
			event. key = kar;
			if (event. key == VK_LEFT)  event. key = 0x2190;
			if (event. key == VK_RIGHT) event. key = 0x2192;
			if (event. key == VK_UP)    event. key = 0x2191;
			if (event. key == VK_DOWN)  event. key = 0x2193;
			event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;   // TODO: event -> key?
			try {
				my d_keyCallback (my d_keyBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Key press not completely handled.");
			}
		}
	}
	void _GuiWinDrawingArea_shellResize (GuiObject widget) {
		iam_drawingarea;
		if (my d_resizeCallback) {
			struct structGuiDrawingAreaResizeEvent event = { me };
			event. width = widget -> width;
			event. height = widget -> height;
			try {
				my d_resizeCallback (my d_resizeBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Window resizing not completely handled.");
			}
		}
	}
#elif mac
	void _GuiMacDrawingArea_destroy (GuiObject widget) {
		iam_drawingarea;
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiMacDrawingArea_update (GuiObject widget) {
		iam_drawingarea;
		if (my d_exposeCallback) {
			struct structGuiDrawingAreaExposeEvent event = { me };
			_GuiMac_clipOnParent (widget);
			try {
				my d_exposeCallback (my d_exposeBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Redrawing not completed");
			}
			GuiMac_clipOff ();
		}
	}
	void _GuiMacDrawingArea_handleClick (GuiObject widget, EventRecord *macEvent) {
		iam_drawingarea;
		if (my d_clickCallback) {
			struct structGuiDrawingAreaClickEvent event = { me, 0 };
			event. x = macEvent -> where. h;
			event. y = macEvent -> where. v;
			event. shiftKeyPressed = (macEvent -> modifiers & shiftKey) != 0;
			event. commandKeyPressed = (macEvent -> modifiers & cmdKey) != 0;
			event. optionKeyPressed = (macEvent -> modifiers & optionKey) != 0;
			event. extraControlKeyPressed = (macEvent -> modifiers & controlKey) != 0;
			try {
				my d_clickCallback (my d_clickBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Mouse click not completely handled.");
			}
		}
	}
	bool _GuiMacDrawingArea_tryToHandleKey (GuiObject widget, EventRecord *macEvent) {
		iam_drawingarea;
		if (my d_keyCallback) {
			struct structGuiDrawingAreaKeyEvent event = { me, 0 };
			event. key = macEvent -> message & charCodeMask;
			if (event. key == 28) event. key = 0x2190;
			if (event. key == 29) event. key = 0x2192;
			if (event. key == 30) event. key = 0x2191;
			if (event. key == 31) event. key = 0x2193;
			event. shiftKeyPressed = (macEvent -> modifiers & shiftKey) != 0;
			event. commandKeyPressed = (macEvent -> modifiers & cmdKey) != 0;
			event. optionKeyPressed = (macEvent -> modifiers & optionKey) != 0;
			event. extraControlKeyPressed = (macEvent -> modifiers & controlKey) != 0;
			try {
				my d_keyCallback (my d_keyBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Key press not completely handled.");
			}
			return true;
		}
		return false;
	}
	void _GuiMacDrawingArea_shellResize (GuiObject widget) {
		iam_drawingarea;
		if (my d_resizeCallback) {
			struct structGuiDrawingAreaResizeEvent event = { me, 0 };
			event. width = widget -> width;
			event. height = widget -> height;
			try {
				my d_resizeCallback (my d_resizeBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Window resizing not completely handled.");
			}
		}
	}
#endif

#if gtk
static gboolean _guiGtkDrawingArea_swipeCallback (GuiObject w, GdkEventScroll *event, gpointer void_me) {
	iam (GuiDrawingArea);
	if (my d_horizontalScrollBar) {
		double hv = gtk_range_get_value (GTK_RANGE (my d_horizontalScrollBar -> d_widget));
		double hi = gtk_range_get_adjustment (GTK_RANGE (my d_horizontalScrollBar -> d_widget)) -> step_increment;
		switch (event -> direction) {
			case GDK_SCROLL_LEFT:
				gtk_range_set_value (GTK_RANGE (my d_horizontalScrollBar -> d_widget), hv - hi);
				break;
			case GDK_SCROLL_RIGHT:
				gtk_range_set_value (GTK_RANGE (my d_horizontalScrollBar -> d_widget), hv + hi);
				break;
		}
	}
	if (my d_verticalScrollBar) {
		double vv = gtk_range_get_value (GTK_RANGE (my d_verticalScrollBar -> d_widget));
		double vi = gtk_range_get_adjustment (GTK_RANGE (my d_verticalScrollBar -> d_widget)) -> step_increment;
		switch (event -> direction) {
			case GDK_SCROLL_UP:
				gtk_range_set_value (GTK_RANGE (my d_verticalScrollBar -> d_widget), vv - vi);
				break;
			case GDK_SCROLL_DOWN:
				gtk_range_set_value (GTK_RANGE (my d_verticalScrollBar -> d_widget), vv + vi);
				break;
		}
	}
	return TRUE;
}
#endif

void structGuiDrawingArea :: f_setSwipable (GuiScrollBar horizontalScrollBar, GuiScrollBar verticalScrollBar) {
	d_horizontalScrollBar = horizontalScrollBar;
	d_verticalScrollBar = verticalScrollBar;
	#if gtk
		g_signal_connect (G_OBJECT (d_widget), "scroll-event", G_CALLBACK (_guiGtkDrawingArea_swipeCallback), this);
	#endif
}

GuiDrawingArea GuiDrawingArea_create (GuiForm parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback)  (void *boss, GuiDrawingAreaClickEvent  event),
	void (*keyCallback)    (void *boss, GuiDrawingAreaKeyEvent    event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags)
{
	GuiDrawingArea me = Thing_new (GuiDrawingArea);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_exposeCallback = exposeCallback;
	my d_exposeBoss = boss;
	my d_clickCallback = clickCallback;
	my d_clickBoss = boss;
	my d_keyCallback = keyCallback;
	my d_keyBoss = boss;
	my d_resizeCallback = resizeCallback;
	my d_resizeBoss = boss;
	#if gtk
		my d_widget = gtk_drawing_area_new ();
		GdkEventMask mask = (GdkEventMask) (GDK_EXPOSURE_MASK   // receive exposure events
			| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK   // receive click events
			| GDK_BUTTON_MOTION_MASK                            // receive motion notifies when a button is pressed
			| GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK
			| GDK_POINTER_MOTION_HINT_MASK);                    // receive fewer motion notify events (the cb might take time)
		gtk_widget_set_events (GTK_WIDGET (my d_widget), mask);
		g_signal_connect (G_OBJECT (my d_widget), "expose-event",         G_CALLBACK (_GuiGtkDrawingArea_exposeCallback),  me);
		g_signal_connect (G_OBJECT (my d_widget), "destroy",              G_CALLBACK (_GuiGtkDrawingArea_destroyCallback), me);
		g_signal_connect (G_OBJECT (my d_widget), "button-press-event",   G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me);
		g_signal_connect (G_OBJECT (my d_widget), "button-release-event", G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me);
		g_signal_connect (G_OBJECT (my d_widget), "motion-notify-event",  G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me);
		if (parent != NULL) {
			Melder_assert (parent -> d_widget);
			g_signal_connect (G_OBJECT (gtk_widget_get_toplevel (GTK_WIDGET (parent -> d_widget))), "key-press-event",
				G_CALLBACK (_GuiGtkDrawingArea_keyCallback), me);
		}
		g_signal_connect (G_OBJECT (my d_widget), "size-allocate", G_CALLBACK (_GuiGtkDrawingArea_resizeCallback), me);

//		g_signal_connect (GTK_WIDGET (my d_widget), "activate", G_CALLBACK (_GuiGtkDrawingArea_activateCallback), me);

		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		gtk_widget_set_double_buffered (GTK_WIDGET (my d_widget), FALSE);
	#elif cocoa
	#elif win
		my d_widget = _Gui_initializeWidget (xmDrawingAreaWidgetClass, parent -> d_widget, L"drawingArea");
		_GuiObject_setUserData (my d_widget, me);
		my d_widget -> window = CreateWindowEx (0, _GuiWin_getDrawingAreaClassName (), L"drawingArea",
			WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height, my d_widget -> parent -> window, NULL, theGui.instance, NULL);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
	#elif mac
		my d_widget = _Gui_initializeWidget (xmDrawingAreaWidgetClass, parent -> d_widget, L"drawingArea");
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
	#endif
	return me;
}

GuiDrawingArea GuiDrawingArea_createShown (GuiForm parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags)
{
	GuiDrawingArea me = GuiDrawingArea_create (parent, left, right, top, bottom, exposeCallback, clickCallback, keyCallback, resizeCallback, boss, flags);
	my f_show ();
	return me;
}

GuiDrawingArea GuiDrawingArea_create (GuiScrolledWindow parent, int width, int height,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags)
{
	GuiDrawingArea me = Thing_new (GuiDrawingArea);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_exposeCallback = exposeCallback;
	my d_exposeBoss = boss;
	my d_clickCallback = clickCallback;
	my d_clickBoss = boss;
	my d_keyCallback = keyCallback;
	my d_keyBoss = boss;
	my d_resizeCallback = resizeCallback;
	my d_resizeBoss = boss;
	#if gtk
		my d_widget = gtk_drawing_area_new ();
		GdkEventMask mask = (GdkEventMask) (GDK_EXPOSURE_MASK   // receive exposure events
			| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK   // receive click events
			| GDK_BUTTON_MOTION_MASK                            // receive motion notifies when a button is pressed
			| GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK
			| GDK_POINTER_MOTION_HINT_MASK);                    // receive fewer motion notify events (the cb might take time)
		gtk_widget_set_events (GTK_WIDGET (my d_widget), mask);
		g_signal_connect (G_OBJECT (my d_widget), "expose-event",         G_CALLBACK (_GuiGtkDrawingArea_exposeCallback),  me);
		g_signal_connect (G_OBJECT (my d_widget), "destroy",              G_CALLBACK (_GuiGtkDrawingArea_destroyCallback), me);
		g_signal_connect (G_OBJECT (my d_widget), "button-press-event",   G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me);
		g_signal_connect (G_OBJECT (my d_widget), "button-release-event", G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me);
		g_signal_connect (G_OBJECT (my d_widget), "motion-notify-event",  G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me);
		if (parent != NULL) {
			g_signal_connect (G_OBJECT (gtk_widget_get_toplevel (GTK_WIDGET (parent -> d_widget))), "key-press-event",
				G_CALLBACK (_GuiGtkDrawingArea_keyCallback), me);
		}
		g_signal_connect (G_OBJECT (my d_widget), "size-allocate", G_CALLBACK (_GuiGtkDrawingArea_resizeCallback), me);

//		g_signal_connect (GTK_WIDGET (my d_widget), "activate", G_CALLBACK (_GuiGtkDrawingArea_activateCallback), me);

		_GuiObject_setUserData (my d_widget, me);
		my v_positionInScrolledWindow (my d_widget, width, height, parent);
		gtk_widget_set_double_buffered (GTK_WIDGET (my d_widget), FALSE);
	#elif cocoa
	#elif win
		my d_widget = _Gui_initializeWidget (xmDrawingAreaWidgetClass, parent -> d_widget, L"drawingArea");
		_GuiObject_setUserData (my d_widget, me);
		my d_widget -> window = CreateWindowEx (0, _GuiWin_getDrawingAreaClassName (), L"drawingArea",
			WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
			0, 0, my d_widget -> width, my d_widget -> height, my d_widget -> parent -> window, NULL, theGui.instance, NULL);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		my v_positionInScrolledWindow (my d_widget, width, height, parent);
	#elif mac
		my d_widget = _Gui_initializeWidget (xmDrawingAreaWidgetClass, parent -> d_widget, L"drawingArea");
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInScrolledWindow (my d_widget, width, height, parent);
	#endif
	return me;
}

GuiDrawingArea GuiDrawingArea_createShown (GuiScrolledWindow parent, int width, int height,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags)
{
	GuiDrawingArea me = GuiDrawingArea_create (parent, width, height, exposeCallback, clickCallback, keyCallback, resizeCallback, boss, flags);
	my f_show ();
	return me;
}

void structGuiDrawingArea :: f_setExposeCallback (void (*callback) (void *boss, GuiDrawingAreaExposeEvent event), void *boss) {
	d_exposeCallback = callback;
	d_exposeBoss = boss;
}

void structGuiDrawingArea :: f_setClickCallback (void (*callback) (void *boss, GuiDrawingAreaClickEvent event), void *boss) {
	d_clickCallback = callback;
	d_clickBoss = boss;
}

void structGuiDrawingArea :: f_setResizeCallback (void (*callback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss) {
	d_resizeCallback = callback;
	d_resizeBoss = boss;
}

/* End of file GuiDrawingArea.cpp */
