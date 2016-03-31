//Author: Thomas Lindemeier
//Date: 07 Dez 2015

#version 440

// OpenGL 4.4
//layout (local_size_variable) in;

// OpenGL 4.3
layout (local_size_x = 32, local_size_y = 32) in;

layout (binding = 0, rgba32f) readonly uniform image2D u;
layout (binding = 1, rgba32f) writeonly uniform image2D U;

void main()
{
    ivec2 INDEX = ivec2(gl_GlobalInvocationID.xy);

    ivec2 coarseSize = imageSize(U);
    ivec2 fineSize = imageSize(u);

    ivec2 index = 2 * INDEX;

    if (INDEX.x >= coarseSize.x || INDEX.y >= coarseSize.y || INDEX.x < 0  || INDEX.y < 0) return;

    vec4 a = imageLoad(u, index);
    vec4 b = imageLoad(u, index + ivec2(1, 0));
    vec4 c = imageLoad(u, index + ivec2(0, 1));
    vec4 d = imageLoad(u, index + ivec2(1, 1));

    int n = 0;
    vec4 R = vec4(0.0, 0.0, 0.0, 0.0);
    if (a.a > 0.1)
    {
        R += a;
        n++;
    }
    if (b.a > 0.1)
    {
        R += b;
        n++;
    }
    if (c.a > 0.1)
    {
        R += c;
        n++;
    }
    if (d.a > 0.1)
    {
        R += d;
        n++;
    }

    if(n > 0)
    {
        R = R/float(n);
        //R.a = 1.0;
    }
    else
    {
        R = vec4(0.0,0.0,0.0,0.0);
    }

    imageStore(U, INDEX, R);

}
