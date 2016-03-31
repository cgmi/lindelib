//Author: Thomas Lindemeier
//Date: 07 Dez 2015

#version 440

// OpenGL 4.4
//layout (local_size_variable) in;

// OpenGL 4.3
layout (local_size_x = 32, local_size_y = 32) in;

layout (binding = 0, rgba32f) readonly uniform image2D U;
layout (binding = 1, rgba32f) uniform image2D u;

void main()
{
    ivec2 INDEX = ivec2(gl_GlobalInvocationID.xy);

    ivec2 coarseSize = imageSize(U);
    ivec2 fineSize = imageSize(u);

    ivec2 index = 2 * INDEX;

    if (INDEX.x >= coarseSize.x || INDEX.y >= coarseSize.y || INDEX.x < 0  || INDEX.y < 0) return;

    vec4 R = imageLoad(U, INDEX);

    ivec2 x = index + ivec2(1, 0);
    ivec2 y = index + ivec2(0, 1);
    ivec2 xy = index + ivec2(1, 1);

    if (imageLoad(u, index).a < 0.01)
    {
        imageStore(u, index, vec4(R.rgb, 0.0));
    }
    if (imageLoad(u, x).a < 0.01)
    {
        imageStore(u, x, vec4(R.rgb, 0.0));
    }
    if (imageLoad(u, y).a < 0.01)
    {
        imageStore(u, y, vec4(R.rgb, 0.0));
    }
    if (imageLoad(u, xy).a < 0.01)
    {
        imageStore(u, xy, vec4(R.rgb, 0.0));
    }
}
