#version 330

// vertex attributes
layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;

// transform
uniform mat4 u_ModelViewProjectionMatrix;

// outputs to rasterizer
out vec4 var_Color;

void main(void)
{
	// output transformed vertex position
	gl_Position = u_ModelViewProjectionMatrix * in_Position;

    // output vertex color to rasterizer
    var_Color = in_Color;
}
