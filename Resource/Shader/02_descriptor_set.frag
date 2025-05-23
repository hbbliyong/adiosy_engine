#version 450

layout(location=1) in vec2 v_TexCoord;
layout(set=0,binding=2) uniform sampler2D u_texture0;
layout(set=0,binding=3) uniform sampler2D u_texture1;

layout(location=0) out vec4 fragColor;
void main(){
    fragColor = mix(texture(u_texture0,v_TexCoord),texture(u_texture1,v_TexCoord),0.4);
}
