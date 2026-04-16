#version 460 core
out vec4 FragColor;

in vec2 vUv;

uniform vec4 uColor = vec4(0.1,0.3,0.6, 1.0);       // 填充颜色
uniform vec4 uBorderColor = vec4(1,0.8,0.2, 1.0); // 边框颜色
uniform float uRadius = 0.05;     // 圆角半径 0~0.5
uniform float uBorderWidth = 0.02;// 边框宽度（建议 0.01 ~ 0.1）

void main()
{
    vec2 p = vUv - 0.5;

    // 外矩形（完整大小 + 圆角）
    vec2 dOuter = abs(p) - 0.5 + uRadius;
    float distOuter = length(max(dOuter, 0.0)) + min(max(dOuter.x, dOuter.y), 0.0) - uRadius;

    // 内矩形（收缩边框宽度）
    vec2 dInner = abs(p) - 0.5 + uRadius + uBorderWidth;
    float distInner = length(max(dInner, 0.0)) + min(max(dInner.x, dInner.y), 0.0) - uRadius;

    // 抗锯齿平滑边缘
    float edge = fwidth(distOuter);
    
    // 外轮廓：整个矩形
    float outerAlpha = 1.0 - smoothstep(-edge, edge, distOuter);
    // 内部区域：去掉边框的部分
    float innerAlpha = 1.0 - smoothstep(-edge, edge, distInner);
    // 最终边框 = 外 - 内
    float borderAlpha = outerAlpha - innerAlpha;

    // 混合颜色：边框颜色 + 填充颜色
    vec3 finalColor = mix(uColor.rgb, uBorderColor.rgb, borderAlpha);
    float finalAlpha = outerAlpha * max(uColor.a, uBorderColor.a);

    FragColor = vec4(finalColor, finalAlpha);
}