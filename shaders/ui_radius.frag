#version 330 core
out vec4 FragColor;

in vec2 vUv;

uniform vec4 uColor;
uniform float uRadius; // 0~0.5（相对比例）

void main()
{
    // 把中心放在 0
    vec2 p = vUv - 0.5;

    // 圆角 SDF
    vec2 d = abs(p) - (0.5 - uRadius);
    float dist = length(max(d, 0.0)) - uRadius;

    // 抗锯齿
    float edge = fwidth(dist);
    float alpha = 1.0 - smoothstep(0.0, edge, dist);

    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}