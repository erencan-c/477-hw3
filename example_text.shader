#shader vertex
#version 330 compatibility

layout(location = 2) in vec4 vertex; 
varying vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = gl_ProjectionMatrix * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
};

#shader fragment
#version 330 compatibility

varying vec2 TexCoords;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(text, TexCoords).r);
    gl_FragColor = vec4(textColor, 1.0) * sampled;
}

