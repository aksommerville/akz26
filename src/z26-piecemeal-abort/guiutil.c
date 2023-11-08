/*
** guiutil.c --gui utility functions
*/

#include "z26.h"


/* Utility function for use by any menu item that's a toggle */

void set_yesno_string(char *str, int yesno) {
	if(yesno)
		sprintf(str, "Yes");
	else
		sprintf(str, "No");
}


/* Convert an SDL event to a gui_action. Using this ensures that
	our various GUI screens will have the same key/joystick/etc.
	bindings. If we ever decided to e.g. allow GUI key remapping or
	support the mouse in the GUI, we'd only have to change this one
	routine. */
gui_action gui_navigation(SDL_Event *evp) {
	if(evp->type == SDL_QUIT) {
		return GUI_EXIT_GUI;
	}
	
	if(evp->type == SDL_JOYAXISMOTION) {
		/* allow any joystick to control UI, not just first one.
			Also, any axes will work, not just 0 and 1. */
		if(evp->jaxis.axis % 2 == 0) { // even numbered axes are left/right
			/**
				// this makes the GUI hard to control with the stick,
				// disabled.
			if(evp->jaxis.value < -1024)
				return GUI_ACTIVATE_SEC;
			else if(evp->jaxis.value > 1024)
				return GUI_ACTIVATE_PRI;
			else
			*/
				return GUI_NO_ACTION;
		} else { // odd numbered axes are up/down
			if(evp->jaxis.value < -1024)
				return GUI_UP;
			else if(evp->jaxis.value > 1024)
				return GUI_DOWN;
			else
				return GUI_NO_ACTION;
		}
	}

	if(evp->type == SDL_JOYBUTTONDOWN) {
		if(evp->jbutton.button % 3 == 0) {
			return GUI_ACTIVATE_PRI;
		} else if(evp->jbutton.button % 3 == 1) {
			return GUI_ACTIVATE_SEC;
		} else {
			return GUI_EXIT_GUI;
		}
	}

	if(evp->type == SDL_KEYDOWN) {
//		printf("PgUp: %d\n", SDLK_PAGEUP);
//		printf("-%d-", evp->key.keysym.sym);
		switch(evp->key.keysym.sym) {
			case SDLK_RETURN:
//			case SDLK_SPACE:
			case SDLK_RIGHT:
			case SDLK_KP_6:
			case SDLK_KP_ENTER:
				return GUI_ACTIVATE_PRI;

			case SDLK_LEFT:
			case SDLK_KP_4:
				return GUI_ACTIVATE_SEC;

			case SDLK_UP:
			case SDLK_KP_8:
				return GUI_UP;

			case SDLK_TAB:
			case SDLK_DOWN:
			case SDLK_KP_2:
				return GUI_DOWN;

			case SDLK_BACKSPACE:
			case SDLK_ESCAPE:
				return GUI_EXIT_GUI;

			case SDLK_PAGEUP:
			case SDLK_KP_9:
				return GUI_PAGEUP;

			case SDLK_PAGEDOWN:
			case SDLK_KP_3:
				return GUI_PAGEDOWN;

			case SDLK_HOME:
			case SDLK_KP_7:
				return GUI_HOME;

			case SDLK_END:
			case SDLK_KP_1:
				return GUI_END;
				
			case SDLK_BACKSLASH:
				return GUI_RANDOM;
				
			default:
				return GUI_NO_ACTION;
		}
	}
	return GUI_NO_ACTION;	/* all other events */
}

int set_gui_height(gui_entry *menu) {
	int height = 0;
	
	for(; menu->label != NULL; menu++)
		height++;
		
	return(height);	
}

int gui_handle_action(int action, int *ptr_exit_gui, gui_entry *current_gui_items, int gui_current)
{
	int gui_height;
	gui_entry g;
	
	gui_height = set_gui_height(current_gui_items);
	g = current_gui_items[gui_current];
	switch(action) {

		case GUI_EXIT_GUI:
			*ptr_exit_gui = 1;
			break;

		case GUI_UP:
			gui_current--;
			if(gui_current < 0) gui_current = gui_height - 1;
			g = current_gui_items[gui_current];
			if(g.handler == NULL) gui_current--;
			if(gui_current < 0) gui_current = gui_height - 1;
			break;

		case GUI_DOWN:
			gui_current++;
			if(gui_current == gui_height) gui_current = 0;
			g = current_gui_items[gui_current];
			if(g.handler == NULL) gui_current++;
			if(gui_current == gui_height) gui_current = 0;
			break;

		case GUI_PAGEUP:
		case GUI_HOME:
			gui_current = 0;
			break;

		case GUI_PAGEDOWN:
		case GUI_END:
			gui_current = gui_height - 1;
			break;

		case GUI_ACTIVATE_PRI:
			if(g.handler != NULL && !g.disabled)
				g.handler();
			break;

		case GUI_ACTIVATE_SEC:
			if(g.sec_handler != NULL && !g.disabled)
				g.sec_handler();
			break;

		default:
			break;
	}
	return(gui_current);
}

/* Draw the GUI items. Called by gui() and sub-menus. Highlights the currently-selected item. */

void draw_gui(gui_entry *entries, int cur) {
	int i, x, y;
	int gui_height;

	gui_height = set_gui_height(entries);

	clrscr();
	
	y = 4;
//	y = 2 + (MaxLines/FONT_HEIGHT - set_gui_height(entries))/2;
	x = (tiawidth/FONT_WIDTH - GUI_WIDTH)/2;

	draw_msg_at(1, 1);

	for(i=0; i<gui_height; i++) {
		if(entries[i].data == NULL)
			sprintf(msg, "%s", entries[i].label);
		else
			sprintf(msg, entries[i].label, entries[i].data);

		//	fprintf(stderr, "%d %d\n", x, y);
		if(i==cur && entries[i].disabled)
			grey_hilite_msg_at(x, y+i);
		else if(i==cur)
			hilite_msg_at(x, y+i);
		else if(entries[i].disabled)
			grey_msg_at(x, y+i);
		else
			draw_msg_at(x, y+i);
	}
	gui_CopyScreen();
}


/**
** z26 is Copyright 1997-2019 by John Saeger and contributors.  
** z26 is released subject to the terms and conditions of the
** GNU General Public License Version 2 (GPL).  z26 comes with no warranty.
** Please see COPYING.TXT for details.
*/
