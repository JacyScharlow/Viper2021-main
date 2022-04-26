/*
** RPI Game Architecture Engine
** FINAL
**
** By: Jacy Scharlow
*/
// **********************************************************************

class f_render
{
public:
	f_render(void* win);
	~f_render();

	void update(struct ga_frame_params* params);
	void draw();

private:
	void* _window;

	class ga_constant_color_material* _default_material;

public:
	bool shadow;
};