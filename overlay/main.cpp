#include "overlay.h"
#include <stdio.h>
#include <string.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>
#include <X11/keysym.h>
#include <time.h>

bool get_key_state(Display *display, KeySym key)
{
	char keymap[32];
	XQueryKeymap(display, keymap);
	KeyCode kc = XKeysymToKeycode(display, key);
	return (bool)(!!(keymap[kc >> 3] & (1 << (kc & 7))));
}

int render_callback(struct window_info *winfo, GLFWwindow *glfw_window, void *arg)
{
	static bool init = false;
	static bool show_window = false;
	static KeySym const menu_key = XK_Insert;
	static time_t show_menu_time = time(NULL);

	Display *display = (Display *)arg;
	if (!init) {
		ImGuiContext *ctx = ImGui::CreateContext();
		ImGui::SetCurrentContext(ctx);
		ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
		ImGui_ImplOpenGL2_Init();
		init = true;
	}

	time_t now = time(NULL);
	/* 1 sec delay between menu toggles */
	if (get_key_state(display, menu_key) && now - show_menu_time > 1) {
		show_window = !show_window;
		show_menu_time = now;
	}

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	if (show_window && glfwGetWindowAttrib(glfw_window, GLFW_MOUSE_PASSTHROUGH) == GLFW_TRUE)
		glfwSetWindowAttrib(glfw_window, GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
	else if (!show_window && glfwGetWindowAttrib(glfw_window, GLFW_MOUSE_PASSTHROUGH) == GLFW_FALSE)
		glfwSetWindowAttrib(glfw_window, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
	
	if (show_window) {
		static bool checkbox_state;
		static char text_buf[100];
		ImGui::Begin("Test");
		ImGui::Button("My Button");
		ImGui::Checkbox("My Checkbox", &checkbox_state);
		ImGui::InputText("My Text Input", text_buf, sizeof(text_buf));
		ImGui::End();
	}

	ImGui::Render();
	int display_w, display_h;
        glfwGetFramebufferSize(glfw_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	
	return 0;
}

int window_callback(Window window, char *window_name, pid_t pid, void *arg)
{
	printf("[*] Window ID: %lu\n", (unsigned long)window);
	printf("[*] Window Name: %s\n", window_name);
	printf("[*] Window PID: %d\n", pid);
	printf("====================\n");

	if (!strcmp(window_name, "Counter-Strike Source - OpenGL")) {
		Display *display = (Display *)arg;
		printf("[*] Game Window Found\n");
		printf("[*] Process ID: %d\n", pid);
		printf("====================\n");
		run_overlay(display, window, render_callback, display);
		return -1;
	} else if (!strcmp(window_name, "steam")) {
		enumerate_windows((Display *)arg, window_callback, arg);
	}

	return 0;
}

int main()
{
	Display *display;

	display = init_overlay();
	if (!display)
		return -1;
	enumerate_windows(display, window_callback, (void *)display);
	shutdown_overlay(display);
	return 0;
}