#version 460 core

out vec4 fragcol;
in vec2 Texcord;
uniform sampler2D text0;

void main(){
    fragcol = vec4(texture(text0,Texcord).rgb,1.f);
}
