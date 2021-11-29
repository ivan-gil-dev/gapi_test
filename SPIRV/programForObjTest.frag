 #version 440 core
layout(location = 0) in vec3 f_Color; 
layout(location = 1) in vec2 f_UV;
layout(location = 2) in vec3 f_Normal;
layout(location = 3) in vec3 f_FragPos;

layout(binding = 0) uniform ColorStruct {vec3 color;} colorStruct;

layout(location = 0) out vec4 fragColor;

void main() 
{ 
    fragColor = vec4( colorStruct.color, 1.0f ); 
}   