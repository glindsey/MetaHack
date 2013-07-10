#ifndef SHADEREFFECT_H_INCLUDED
#define SHADEREFFECT_H_INCLUDED

// Since SFML can only pass floats to the shader, this is done as a namespace
// of const floats instead of an enum class.
namespace ShaderEffect
{
  const float Default = 0.0f;
  const float Lighting = 1.0f;
  const float Grayscale = 2.0f;
  const float Sepia = 3.0f;
  const float Parchment = 4.0f;
}

#endif // SHADEREFFECT_H_INCLUDED
