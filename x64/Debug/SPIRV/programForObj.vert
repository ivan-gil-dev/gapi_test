#version 440
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 v_Pos;
layout(location = 1) in vec3 v_Color;
layout(location = 2) in vec2 v_UV;
layout(location = 3) in vec3 v_Normal;

layout(location = 0) out vec3 f_Color;
layout(location = 1) out vec2 f_UV;
layout(location = 2) out vec3 f_Normal;
layout(location = 3) out vec3 f_FragPos;    

layout(set = 0, binding = 0) uniform Mvp{
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
}mvp;
    

void main(){

    f_FragPos = vec3( mvp.model * vec4( v_Pos, 1.0f ) );
    gl_Position = mvp.projection * mvp.view * vec4( f_FragPos, 1.0f );
    f_Color = v_Color;
    f_UV = v_UV;
    f_Normal = v_Normal;
}
