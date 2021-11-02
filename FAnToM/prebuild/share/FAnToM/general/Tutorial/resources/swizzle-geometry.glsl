#version 330 core
#extension GL_ARB_geometry_shader4 : enable

//von FAnToM definierte uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

//Ausgaben Vertex-Shader = Input Geometry-Shader
layout( lines ) in; 

//Ausgaben Geometry-Shader = Input Fragment-Shader
layout( line_strip, max_vertices = 4 ) out; 

void main()
{
	//urspruengliche Linien
    for (int i=0; i<2; ++i)
    {
		gl_Position = proj * view * model * gl_in[ i ].gl_Position;
		EmitVertex(); // Vertex geschrieben, nächsten Vertex anfangen
    }
    EndPrimitive(); //Primitive beendet -> Ausgabe
    
    //duplizierte Linien mit vertauschten x- und y-Koordinaten
    for (int i=0; i<2; ++i)
    {		
		vec4 temp = gl_in[ i ].gl_Position;
		temp.xy = temp.yx;
		gl_Position = proj * view * model * temp;
		EmitVertex(); // Vertex geschrieben, nächsten Vertex anfangen
	}
	EndPrimitive(); //Primitive beendet -> Ausgabe
 }
