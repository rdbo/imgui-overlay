#ifndef OVERLAY_H
#define OVERLAY_H

#include <X11/Xlib.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <unistd.h>

struct window_info {
	int x;
	int y;
	unsigned int width;
	unsigned int height;
	unsigned int border;
	unsigned int depth;
};

int enumerate_windows(Display *display, int (*callback)(Window window, char *window_name, pid_t pid, void *arg), void *arg);
Display *init_overlay(void);
void shutdown_overlay(Display *display);
int run_overlay(Display *display, Window window, int (*main_loop_callback)(struct window_info *winfo, GLFWwindow *glfw_window, void *arg), void *arg);

#endif
