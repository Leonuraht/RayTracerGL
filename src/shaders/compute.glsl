#version 460 core
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D image;

struct Camera {
    vec3 pos, dir, targ, up, right;
    float FOV, asp_ratio;
};

struct Light {
    vec3 pos, dir, intensity, ambient, col;
};

struct Object {
    float radius;
    vec3 center;
    vec3 color;
};

Object obj[3] = Object[3](
        Object(0.1f, vec3(0.f, 0.f, -1.f), vec3(1.f, 0.4f, 0.5f)),
        Object(0.2f, vec3(0.f, -1.f, 2.f), vec3(0.5f, 0.2f, 0.3f)),
        Object(0.3f, vec3(-3.f, 0.f, -4.f), vec3(0.6f, 0.7f, 0.4f))
    );
ivec2 img_size = imageSize(image);

uniform Light dirlight;
uniform Camera camera;
uniform float delta;

void main() {
    vec3 fragcol = vec3(0.f);
    vec3 UV_pixel = vec3(0.f);
    ivec2 texelcord = ivec2(gl_GlobalInvocationID.xy);
    UV_pixel.xy = ((vec2(texelcord) / img_size) * 2.f - 1.f) * camera.FOV;
    UV_pixel.x *= 1.77;
    vec3 ray_dir = normalize(normalize(camera.dir) + UV_pixel.x * camera.right + UV_pixel.y * camera.up);
    float closest_dis = 1e20f;
    for (int i = 0; i < 3; i++) {
        vec3 omp = vec3(camera.pos - obj[i].center);
        float b = dot(ray_dir, omp);
        float c = dot(omp, omp) - obj[i].radius * obj[i].radius;
        float Disrc = b * b - c;
        if (Disrc >= 0.f) {
            float t = -b - sqrt(Disrc);
            if (t > 0.f) {
                if (t < closest_dis) {
                    vec3 light_dir = normalize(dirlight.dir);
                    vec3 hit_point = omp + ray_dir * t;
                    vec3 normal = normalize(hit_point);
                    float inten = max(dot(normal, -light_dir), 0.f);
                    float spex = pow(max(dot(normalize(-light_dir - ray_dir), normal), 0.f), 32) * inten;
                    fragcol = obj[i].color * inten + vec3(0.5f) * spex;
                    closest_dis = t;
                }
            }
        }
    }
    fragcol = pow(fragcol, vec3(1 / 2.2));
    imageStore(image, texelcord, vec4(fragcol, 1.f));
}
