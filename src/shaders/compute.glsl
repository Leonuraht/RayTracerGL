#version 460 core
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D image;

struct Camera {
    vec3 pos, targ;
    float FOV;
};

struct Light{
    vec3 pos,dir,intensity,ambient;
};

uniform float delta;
ivec2 img_size = imageSize(image);
vec3 fragcol = vec3(0.f);
uniform Light light;
uniform Camera camera;

void main() {
    ivec2 texelcord = ivec2(gl_GlobalInvocationID.xy);
    fragcol.xy = vec2(texelcord) / img_size;
    fragcol.x *= 1.77;
    imageStore(image, texelcord, vec4(fragcol, 1.f));
}
