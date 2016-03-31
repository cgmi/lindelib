//Author: Thomas Lindemeier
//Date: 20.08.2014


// http://www.mathworks.com/matlabcentral/fileexchange/14995-anisotropic-diffusion--perona---malik-/content/anisodiff_Perona-Malik/anisodiff2D.m
// Perona-Malik Diffusion
// Arguments:
// kappa controls conduction as a function of gradient.If kappa is low
// small intensity gradients are able to block conduction and hence diffusion
// across step edges.A large value reduces the influence of intensity
// gradients on conduction.
//
// lambda controls speed of diffusion(you usually want it at a maximum of
// 0.25)
//
// Diffusion equation 0 favours high contrast edges over low contrast ones.
// Diffusion equation 1 favours wide regions over smaller ones.
// flux 0 = exp
// flux 1 = sqr


#version 440 core

uniform sampler2D image;

uniform float lambda;
uniform float kappa;
uniform int mode;

in vec4 VertPosition;
in vec4 VertNormal;
in vec4 VertColor;
in vec4 VertTexture;

out vec4 outColor;

float fluxExp(float a, float kappa)
{
    return exp(-pow(a/kappa, 2));
}

float fluxSqr(float a, float kappa)
{
    return 1.0/(1.0 + pow(a/kappa, 2));
}

float sqr(float a)
{
    return a*a;
}


void main()
{

    const float dx = 1;
    const float dy = 1;
    const float dd = sqrt(2.f);

    vec2 size = textureSize(image, 0);
    const vec2 pos = VertTexture.xy;

    // get the image value
    const float v = texture2D(image, pos).x;

    // neighbor values
    const float vN = texture2D(image, pos + ((vec2(0, -1)) / size )).x;
    const float vNE = texture2D(image, pos + ((vec2(1, -1)) / size )).x;
    const float vE = texture2D(image, pos + ((vec2(1, 0)) / size )).x;
    const float vSE = texture2D(image, pos + ((vec2(1, 1)) / size )).x;
    const float vS = texture2D(image, pos + ((vec2(0, 1)) / size )).x;
    const float vSW = texture2D(image, pos + ((vec2(-1, 1)) / size )).x;
    const float vW = texture2D(image, pos + ((vec2(-1, 0)) / size )).x;
    const float vNW = texture2D(image, pos + ((vec2(-1, -1)) / size )).x;

    // derivatives
    const float nablaN 	= vN - v;
    const float nablaNE	= vNE - v;
    const float nablaE	= vE - v;
    const float nablaSE	= vSE - v;
    const float nablaS	= vS - v;
    const float nablaSW	= vSW - v;
    const float nablaW	= vW - v;
    const float nablaNW	= vNW - v;

    // flux magnitudes
    float cN ;
    float cNE;
    float cE ;
    float cSE;
    float cS ;
    float cSW;
    float cW ;
    float cNW;
    if (mode == 0)
    {
        cN 	= fluxExp(nablaN, kappa);
        cNE = fluxExp(nablaNE, kappa);
        cE 	= fluxExp(nablaE, kappa);
        cSE = fluxExp(nablaSE, kappa);
        cS 	= fluxExp(nablaS, kappa);
        cSW = fluxExp(nablaSW, kappa);
        cW 	= fluxExp(nablaW, kappa);
        cNW = fluxExp(nablaNW, kappa);
    }
    else
    {
        cN 	= fluxSqr(nablaN, kappa);
        cNE = fluxSqr(nablaNE, kappa);
        cE 	= fluxSqr(nablaE, kappa);
        cSE = fluxSqr(nablaSE, kappa);
        cS 	= fluxSqr(nablaS, kappa);
        cSW = fluxSqr(nablaSW, kappa);
        cW 	= fluxSqr(nablaW, kappa);
        cNW = fluxSqr(nablaNW, kappa);
    }

    const float diffused = v + lambda * (
                (1.0 / sqr(dy)) * cN	* nablaN + (1.0 / sqr(dy))	*	cS	*	nablaS +
                (1.0 / sqr(dx)) * cW	* nablaW + (1.0 / sqr(dx))	*	cE	*	nablaE +
                (1.0 / sqr(dd)) * cNE	* nablaNE + (1.0 / sqr(dd))	*	cSE	*	nablaSE +
                (1.0 / sqr(dd)) * cSW	* nablaSW + (1.0 / sqr(dd))	*	cNW	*	nablaNW
                );

    outColor = vec4(diffused, diffused, diffused, 1.0);
}
