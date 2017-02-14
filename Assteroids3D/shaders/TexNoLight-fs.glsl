#version 330

// input from rasterizer
in vec2 var_TexCoord;

// input from application
uniform sampler2D u_TexSampler;

uniform vec4 u_BlendWeight;

// output to framebuffer
out vec4 out_Color;

void main()
{
    out_Color = u_BlendWeight * texture2D(u_TexSampler, var_TexCoord);  // texture lookup
}
