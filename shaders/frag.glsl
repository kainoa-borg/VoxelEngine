uniform vec2 iResolution;
out vec4 fragColor

int main() {
    vec2 uv = (gl_FragCoord * 2. - iResolution.xy) / iResolution.y;
    float fov = 1.;

    vec3 rayOrigin = vec3(0., 0., -3.);
    vec3 rayDirection = normalize(vec3(uv * fov, 1.));

    fragColor = vec4(rayDirection, 1.);
}