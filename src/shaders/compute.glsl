#version 460 core
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D image;

struct Object {
    float radius;
    vec3 center;
    vec3 color;
};
layout(std430, binding = 1) buffer ScreenObj {
    Object obj[];
};
ivec2 img_size = imageSize(image);

struct Camera {
    vec3 pos, dir, targ, up, right;
    float FOV, asp_ratio;
};

struct Light {
    vec3 pos, dir, intensity, ambient, col;
};

uniform Light dirlight;
uniform Camera camera;
uniform float delta;

uint seed;

uint pcg_hash(uint input_seed) {
    uint state = input_seed * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float rand() {
    seed = pcg_hash(seed);
    return float(seed) / 4294967296.0;
}
void main() {
    ivec2 texelcord = ivec2(gl_GlobalInvocationID.xy);
    seed = uint(texelcord.x + texelcord.y * img_size.x);
    vec3 final_col = vec3(0.f);
    for (int aa = 0; aa < 4; aa++) {
        vec3 UV_pixel = vec3(0.f);
        vec2 jitter = vec2(0.f);
        jitter.x = rand() - 0.5f;
        jitter.y = rand() - 0.5f;
        jitter += vec2(texelcord);

        UV_pixel.xy = ((vec2(jitter) / img_size) * 2.f - 1.f) * camera.FOV;
        UV_pixel.x *= 1.77;
        vec3 ray_dir = normalize(normalize(camera.dir) + UV_pixel.x * camera.right + UV_pixel.y * camera.up);
        vec3 light_dir = normalize(dirlight.dir);
        vec3 fuse_col = vec3(0.f);
        vec3 attenuation = vec3(1.f);
        vec3 current_ray_dir = ray_dir, current_cam_pos = camera.pos;
        for (int l = 0; l < 3; l++) {
            vec3 fragcol = vec3(0.f);
            vec3 closest_norm = vec3(0.f), closest_hit_p = vec3(0.f);
            float closest_dis = 1e20f, closest_index = -1;
            for (int i = 0; i < obj.length() - 1; i++) {
                vec3 omp = vec3(current_cam_pos - obj[i].center);
                float b = dot(current_ray_dir, omp);
                float c = dot(omp, omp) - obj[i].radius * obj[i].radius;
                float Disrc = b * b - c;
                if (Disrc >= 0.f) {
                    float t = -b - sqrt(Disrc);
                    if (t > 0.f) {
                        if (t < closest_dis) {
                            vec3 hit_point = current_cam_pos + current_ray_dir * t;
                            vec3 normal = normalize(hit_point - obj[i].center);
                            float inten = max(dot(normal, -light_dir), 0.f);
                            float spex = pow(max(dot(normalize(-light_dir - current_ray_dir), normal), 0.f), 32) * inten;
                            fragcol = obj[i].color * inten + vec3(0.5f) * spex;
                            closest_dis = t;
                            closest_index = i;
                            closest_norm = normal;
                            closest_hit_p = hit_point;
                        }
                    }
                }
            }
            vec3 norm = normalize(obj[obj.length() - 1].center);
            float dn = dot(norm, current_ray_dir);
            Object oobj = obj[obj.length() - 1];
            if (abs(dn) > 1e-6f) {
                float pla = dot(current_cam_pos, norm);
                float t = (oobj.radius - pla) / dn;
                if (t < closest_dis && t >= 0.f) {
                    if (dn > 0.f) norm = -norm;
                    float diff = max(dot(norm, -light_dir), 0.f);
                    float spex = pow(max(dot(normalize(-light_dir - current_ray_dir), norm), 0.f), 32);
                    closest_hit_p = current_cam_pos + current_ray_dir * t;
                    closest_norm = norm;
                    closest_dis = t;
                    closest_index = obj.length() - 1;
                    fragcol = oobj.color * diff * abs(dn) + vec3(0.5f) * spex;
                    if (int(closest_hit_p.x) % 10 <= 5 && int(closest_hit_p.z) % 10 <= 5) fragcol *= 0.4f;
                }
            }
            if (closest_index == -1) {
                fuse_col += attenuation * vec3(0.005f);
                break;
            }
            for (int i = 0; i < obj.length() - 1; i++) {
                vec3 ray = closest_hit_p + closest_norm * 0.001f;
                vec3 point_to_light = -light_dir;
                vec3 omp = ray - obj[i].center;
                float b = dot(point_to_light, omp);
                float c = dot(omp, omp) - obj[i].radius * obj[i].radius;
                float Disr = b * b - c;
                if (Disr >= 0.f) {
                    float t = -b - sqrt(Disr);
                    if (t > 0.f) {
                        fragcol *= 0.1f;
                        break;
                    }
                }
            }
            fuse_col += fragcol * attenuation;
            attenuation *= 0.9f;
            current_cam_pos = closest_hit_p + closest_norm * 0.001f;
            current_ray_dir = reflect(current_ray_dir, closest_norm);
        }
        final_col += fuse_col;
    }
    final_col /= 4.f;
    final_col = pow(final_col, vec3(1 / 2.2));
    imageStore(image, texelcord, vec4(final_col, 1.f));
}
