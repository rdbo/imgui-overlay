# Dear ImGUI Overlay
X11 + GLFW + Dear ImGUI Overlay made by rdbo  
Based on https://github.com/rdbo/glfw-overlay

# How to use?
- In `main.c`, there is a `window_callback` function that determines which window to put the overlay on. Use the information provided (window ID, window Name, process ID) to chose.
- Still in `main.c`, there is a `render_callback` function that is run on the overlay's main loop. You can use it for rendering and other stuff.

## REMINDER
This was a test project, more of a PoC than anything 'professional'. I recommend making your own code rather than using this.
