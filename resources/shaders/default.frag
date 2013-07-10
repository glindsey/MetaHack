// Default SFML fragment shader

const float EFFECT_LIGHTING = 1.0;
const float EFFECT_GRAYSCALE = 2.0;
const float EFFECT_SEPIA = 3.0;
const float EFFECT_PARCHMENT = 4.0;

uniform sampler2D texture;
uniform float effect;

void main()
{
  vec4 fragment_color;
  float luminance;

  // lookup the pixel in the texture
  vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

  // Get the overall luminance.
  luminance = (gl_Color.r * pixel.r * 0.3) +
              (gl_Color.g * pixel.g * 0.59) +
              (gl_Color.b * pixel.b * 0.11);

  switch (effect)
  {
    case EFFECT_LIGHTING:
      fragment_color = gl_Color * 1.5 * pixel;
      break;

    case EFFECT_GRAYSCALE:
      // Grayscale
      fragment_color.r = luminance;
      fragment_color.g = luminance;
      fragment_color.b = luminance;
      fragment_color.a = gl_Color.a * pixel.a;
      break;

    case EFFECT_SEPIA:
      // Sepia-toned
      luminance *= 0.8;
      luminance += 0.1;
      fragment_color.r = luminance * 0.44;
      fragment_color.g = luminance * 0.26;
      fragment_color.b = luminance * 0.08;
      fragment_color.a = gl_Color.a * pixel.a;
      break;

    case EFFECT_PARCHMENT:
      // Parchment-style
      // Lightest should be (0.945, 0.945, 0.831)
      // Darkest should ideally be some sort of dark ink color, but for
      // the time being it'll be (0, 0, 0) which makes the math much easier.
      fragment_color.r = (1 - luminance) * 0.71;
      fragment_color.g = (1 - luminance) * 0.71;
      fragment_color.b = (1 - luminance) * 0.62;
      fragment_color.a = gl_Color.a * pixel.a;
      break;

    default:
      // Default -- no effect
      // multiply it by the color
      fragment_color = gl_Color * pixel;
      break;
  }

  gl_FragColor = fragment_color;
}
