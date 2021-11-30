 #version 440 core
layout(location = 0) in vec3 f_Color; 
layout(location = 1) in vec2 f_UV;
layout(location = 2) in vec3 f_Normal;
layout(location = 3) in vec3 f_FragPos;

layout(binding = 0) uniform ColorStruct {vec3 color;} colorStruct;

layout(binding = 1) uniform sampler2D sampler1;

layout(location = 0) out vec4 fragColor;

void main() 
{ 
    vec3 color = vec3(texture( sampler1, f_UV ));
    fragColor = vec4( color, 1.0f ); 
}   