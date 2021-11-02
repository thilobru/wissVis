#version 330 core

out vec4 out_color;

void main()
{
	//Yeah, yeah, yeah ... we just color the pixel
	// this example is showing off geometry shaders, not fragments!
	//Shade to blue!
	out_color = vec4(0.0,1.0,1.0,1.0);
}
