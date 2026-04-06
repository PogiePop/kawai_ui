#version 330 core
out vec4 FragColor;
in vec2 vUv;

uniform float radius;
uniform vec4 color;
uniform vec4 shadowColor;
uniform vec2 shadowOffset;
uniform float shadowBlur;

float roundedRectSDF(vec2 uv, float r) {
    vec2 d = abs(uv - 0.5) - 0.5 + r;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r;
}

void main() {
    vec2 uv = vUv;

    // 按钮形状
    float dBtn = roundedRectSDF(uv, radius);
    float aa = fwidth(dBtn) * 0.7;
    float shapeAlpha = 1.0 - smoothstep(-aa, aa, dBtn);

    // 阴影（修复：用 aa + shadowBlur 共同柔和边缘）
    vec2 shadowUV = uv - shadowOffset;
    float dShadow = roundedRectSDF(shadowUV, radius);
    float shadowAA = max(aa, shadowBlur);
    float shadowAlpha = 1.0 - smoothstep(-shadowAA, shadowAA, dShadow);

    // 最终颜色：正确的图层混合
    vec4 finalColor = vec4(0.0);
    
    // 先画阴影
    finalColor = mix(finalColor, shadowColor, shadowAlpha * shadowColor.a);
    
    // 再画按钮（正确覆盖，不会错位）
    finalColor = mix(finalColor, color, shapeAlpha * color.a);

    FragColor = finalColor;
}