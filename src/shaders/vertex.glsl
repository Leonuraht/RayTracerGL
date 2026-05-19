#version 460 core

out vec2 Texcord;

void main() {
    float x = (float((gl_VertexID << 1) & 2) * 0.5f);
    float y = float(((gl_VertexID & 4) >> 2 )| int((gl_VertexID & 3) == 0));
    Texcord = vec2(x, y);
    gl_Position = vec4(Texcord*2.f - 1.f, 0.f, 1.f);
}
