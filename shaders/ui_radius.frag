#version 460 core
out vec4 FragColor;

in vec2 vUv;

uniform vec4 uColor;
uniform float uRadius; // 0 ~ 0.5 正常使用

void main()
{
    vec2 p = vUv - 0.5;

    // 【修复】标准圆角矩形 SDF，不收缩、不偏移
    vec2 d = abs(p) - 0.5 + uRadius;
    float dist = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - uRadius;

    // 【修复】双边抗锯齿，边缘最平滑，不会削掉像素
    float edge = fwidth(dist);
    float alpha = 1.0 - smoothstep(-edge, edge, dist);

    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}