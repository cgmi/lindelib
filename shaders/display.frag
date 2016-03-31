//Author: Thomas Lindemeier
//Date: 20.08.2014

#version 440 core

uniform sampler2D sampler_0;

in vec4 VertPosition;
in vec4 VertNormal;
in vec4 VertColor;
in vec4 VertTexture;

out vec4 outColor;

void main()
{
	 outColor = texture2D(sampler_0, VertTexture.xy);		
}