#include "overlay.h"
#include <stdio.h>
#include <stdlib.h>

static int enum_check = 0;

int _enumerate_windows(Display *display, Window root_window, int (*callback)(Window window, char *window_name, pid_t pid, void *arg), void *arg)
{
	Window *window_list;
	Atom property;
	unsigned int window_count;
	unsigned int i;
	char *window_name;
	unsigned char *pid_data;
	int check;
	/* Variables to pass in X functions (unused otherwise) */
	Window window_tmp;
	Atom atom_tmp;
	unsigned long ul_tmp;
	int i_tmp;

	if (enum_check)
		return enum_check;

	if (XQueryTree(display, root_window, &window_tmp, &window_tmp, &window_list, &window_count) < Success)
		return -1;
	
	property = XInternAtom(display, "_NET_WM_PID", True); /* Window PID property */

	for (i = 0; i < window_count; ++i) {
		window_name = (char *)NULL;
		pid_data = (unsigned char *)NULL;

		XGetWindowProperty(
			display, window_list[i], property, 0, BUFSIZ,
			False, AnyPropertyType, &atom_tmp,
			&i_tmp, &ul_tmp, &ul_tmp, &pid_data
		);

		check = XFetchName(display, window_list[i], &window_name);

		if (check >= Success && window_name && pid_data)
			enum_check = callback(window_list[i], window_name, *(pid_t *)pid_data, arg);

		XFree(pid_data);
		XFree(window_name);

		if (enum_check)
			break;
		
		enum_check = _enumerate_windows(display, window_list[i], callback, arg);
	}

	XFree(window_list);

	return enum_check;
}

int enumerate_windows(Display *display, int (*callback)(Window window, char *window_name, pid_t pid, void *arg), void *arg)
{
	Window root_window;

	root_window = RootWindow(display, DefaultScreen(display));
	_enumerate_windows(display, root_window, callback, arg);
	enum_check = 0;
	return 0;
}

Display *init_overlay(void)
{
	Display *display = (Display *)NULL;

	if (!glfwInit())
		return display;
	
	display = XOpenDisplay(NULL);
	if (!display)
		shutdown_overlay(display);

	return display;
}

void shutdown_overlay(Display *display)
{
	glfwTerminate();
	if (display)
		XCloseDisplay(display);
}

int get_window_info(Display *display, Window window, struct window_info *geom)
{
	Window root_window;
	Window child;
	XWindowAttributes window_attr;
	int x;
	int y;

	root_window = RootWindow(display, DefaultScreen(display));
	if (XTranslateCoordinates(display, window, root_window, 0, 0, &x, &y, &child) < Success)
		return -1;
	
	if (XGetWindowAttributes(display, window, &window_attr) < Success)
		return -1;
	
	geom->border = window_attr.border_width;
	geom->depth = window_attr.depth;
	geom->x = x;
	geom->y = y;
	geom->width = window_attr.width;
	geom->height = window_attr.height;

	return 0;
}

int run_overlay(Display *display, Window window, int (*main_loop_callback)(struct window_info *winfo, GLFWwindow *glfw_window, void *arg), void *arg)
{
	GLFWwindow *glfw_window;
	struct window_info winfo;

	if (get_window_info(display, window, &winfo))
		return -1;
	
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	/*
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	*/

	glfw_window = glfwCreateWindow(winfo.width, winfo.height, "Overlay", NULL, NULL);

	glfwSetWindowAttrib(glfw_window, GLFW_DECORATED, GLFW_FALSE);
	glfwSetWindowAttrib(glfw_window, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
	glfwSetWindowAttrib(glfw_window, GLFW_FLOATING, GLFW_TRUE);

	glfwMakeContextCurrent(glfw_window);
	glfwShowWindow(glfw_window);
	glfwSwapInterval(1);
	
	while (!get_window_info(display, window, &winfo) && !glfwWindowShouldClose(glfw_window)) {
		glfwPollEvents();
		glfwSetWindowPos(glfw_window, winfo.x, winfo.y);
		glfwSetWindowSize(glfw_window, winfo.width, winfo.height);

		if (main_loop_callback(&winfo, glfw_window, arg))
			break;
		
		glfwSwapBuffers(glfw_window);
	}

	glfwDestroyWindow(glfw_window);

	return 0;
}
