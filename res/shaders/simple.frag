#version 430 core

#define PI 3.14159265

//	Simplex 3D Noise
//	by Ian McEwan, Ashima Arts
//  https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
float snoise(vec3 v){
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod(i, 289.0 );
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}

// Oren-Nayar diffuse, got it from https://github.com/glslify/glsl-diffuse-oren-nayar
// They mention using Oren-Nayar at first in the talk, before moving over to a modified version
// of Lambert. The reasons they moved away from Oren-Nayar were 1) complexity, and 2) performance.
// Since I don't particularly care about the complexity, and performance isn't as much a concern
// since I'm not making a game, it makes sense to just use Oren-Nayar.
// The major innovation of Oren-Nayar is to take the roughness of the material into account...
// Seeing as sand is coarse and rough, it makes a lot of sense to use Oren-Nayar for diffuse rather
// than Lambert. Still, the modified Lambert gives a more stylised feel, so it's more a matter of taste.
float orenNayarDiffuse(
  vec3 lightDirection,
  vec3 viewDirection,
  vec3 surfaceNormal,
  float roughness,
  float albedo) {

  float LdotV = dot(lightDirection, viewDirection);
  float NdotL = dot(lightDirection, surfaceNormal);
  float NdotV = dot(surfaceNormal, viewDirection);

  float s = LdotV - NdotL * NdotV;
  float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

  float sigma2 = roughness * roughness;
  float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
  float B = 0.45 * sigma2 / (sigma2 + 0.09);

  return albedo * max(0.0, NdotL) * (A + B * s / t) / PI;
}


in layout(location = 0) vec3 normal;
in layout(location = 1) vec2 textureCoordinates;
in layout(location = 2) vec3 world_position;

uniform layout(location = 2) vec3 camera_position;
uniform layout(location = 3) mat4 model;
uniform layout(location = 4) mat4 VP;
uniform layout(location = 5) float N_y;
// is specular on
uniform layout(location = 6) bool specular;
uniform layout(location = 7) int shininess;
uniform layout(location = 8) int dot_degree;
uniform layout(location = 9) int show_normal_map;
uniform layout(location = 10) bool glitter;

layout(binding=0) uniform sampler2D diffuseSand;
layout(binding=1) uniform sampler2D normalShallowX;
layout(binding=2) uniform sampler2D normalShallowZ;
layout(binding=3) uniform sampler2D normalSteepX;
layout(binding=4) uniform sampler2D normalSteepZ;

out vec4 color;

const vec3 sand_color = vec3(0.929f, 0.788f, 0.686f);
const vec3 light_position = vec3(0, 10000, 1000);
const float light_intensity = 1.0f;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }

// Reoriented Normal Mapping for Unity3d
// obviously adapted for GLSL
// http://discourse.selfshadow.com/t/blending-in-detail/21/18
vec3 rnmBlendUnpacked(vec3 n1, vec3 n2) {
    n1 += vec3( 0,  0, 1);
    n2 *= vec3(-1, -1, 1);
    return n1*dot(n1, n2)/n1.z - n2;
}

void main()
{
    vec3 norm = normalize(normal);
    vec3 map_norm;
    // it's noon ok
    vec3 light_direction = normalize(light_position - world_position);
    vec3 camera_direction = normalize(camera_position - world_position);

    vec3 diffuse_color;
    float specular_intensity;

    // triplanar mapping; thanks to https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
    // in addition, the blending calculation was changed according to https://medium.com/@bgolus/normal-mapping-for-a-triplanar-shader-10bf39dca05a#0c9a
    vec3 blending = pow(abs(norm), vec3(4));
    blending /= dot(blending, vec3(1,1,1));
    vec4 xaxis = texture(diffuseSand, world_position.yz);
    vec4 yaxis = texture(diffuseSand, world_position.xz);
    vec4 zaxis = texture(diffuseSand, world_position.xy);
    // blend the results of the 3 planar projections.
    vec4 tex = xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;

    // triplanar normal maps using a whiteout blend
    // thanks to https://medium.com/@bgolus/normal-mapping-for-a-triplanar-shader-10bf39dca05a
    // sadly, I could never get the blending between steep and shallow normal maps to work well :(
    vec3 steep_norm_x = texture(normalSteepX, world_position.yz).xyz * 2.0 - 1.0;
    vec3 steep_norm_z = texture(normalSteepZ, world_position.xy).xyz * 2.0 - 1.0;
    steep_norm_x = vec3(steep_norm_x.xy + norm.zy, abs(steep_norm_x.z) * norm.x);
    steep_norm_z = vec3(steep_norm_z.xy + norm.xy, abs(steep_norm_z.z) * norm.z);

    float steepness = 1 - abs(norm.y);
    // we don't blend in the y term here...
    vec3 steep_map_norm = normalize(steep_norm_x.zyx * blending.x + /* norm.xyz * blending.y + */ steep_norm_z.xyz * blending.z).xyz;
    // instead, we blend the normal map with the surface normals depending on
    // y like this. This allows us to map exclusively on X and Z, "ignoring Y".
    // I also tried to blend in Y in other ways, but this ended up looking the best
    map_norm = blending.y * norm + (1 - blending.y) * steep_map_norm;

    // diffuse
    // this kind of weird-looking version of lambert is lifted straight from the GDC talk
    // canonically, dot_degree should be 4 and N_y should be 0.3
    // diffuse_color += clamp(dot_degree * dot(light_direction, map_norm * vec3(1,N_y,1)), 0.f, 1.f);
    diffuse_color = vec3(orenNayarDiffuse(light_direction, camera_direction, map_norm, 1, 1));

    // specular
    if (specular) {
        vec3 H = normalize(light_direction + camera_direction);
        // here's blinn-phong specular, for an ocean-y, liquid-y feeling
        specular_intensity = clamp(pow(max(dot(map_norm, H), 0.0), shininess), 0, 1);
    }

    // sparkling! adapted from https://developer.amd.com/wordpress/media/2012/10/Shopf-Procedural.pdf
    float sparkle = 0;
    if (glitter) {
        // We build on good old phong specular
        float spec_base = clamp(dot(reflect(-light_direction, norm), camera_direction), 0, 1);
        /* The slides from the AMD talk don't go into too much detail, so I've worked out most of this
        * by experimentation. It did actually lead to pretty decent understanding all in all.
        * The division by 0.02 increases the noisiness; if we remove it, we start seeing patterns in the noise
        * which we don't want; it does make for a pretty cool line effect, just not the one we want!
        * We multiply by the view vector, instead of add like in the slides. It still leads to some pretty
        * strange effects with the glitter at times, which we might not get by adding it - but at ground level
        * those effects pretty much disappear, and since the glitter is subtle any anomalies aren't noticeable
        * meaning it's fine. The fract is mostly there to make it more random, in addition to making them add
        * together more nicely later.
        * Oh, also, we use the surface normal rather than the one we got from the normal map, just because it
        * doesn't particularly matter for this and it reduces the amount of things to keep track of.
        */
        vec3 fp = fract(snoise(world_position / 0.02) * 0.1 * (camera_position - light_position));
        /*
        * fp *= (1 - fp) basically distributes it evenly. If we remove it, the noise will only be applied to
        * one side of the screen; at least that's what experimentation told me.
        */
        fp *= (1 - fp);
        /*
        * So, not entirely sure, but the multiplied 7 determines the threshold by which something glitters.
        * Setting it to 3 gets you really glittery, any lower and it just starts looking like a very noisy
        * version of Phong specular. 11 removes a lot of the noise and is nice, but 7 is a happy medium if
        * I want to show the sparkle off, in addition to feeling more stylised.
        */
        float glitter = clamp(1 - 11 * (fp.x + fp.y + fp.z), 0, 1);
        // The spec_base actually doesn't add that much, but it's the little things
        sparkle = glitter * pow(spec_base, 1.5);
    }

    vec3 spec = specular_intensity * vec3(0.3);
    vec3 light = (diffuse_color + spec + sparkle) * light_intensity;
    if (show_normal_map == 1)
        color = vec4(abs(norm), 1.0f);
    else if (show_normal_map == 2)
        color = vec4(abs(map_norm), 1.0f);
    else if (show_normal_map == 3)
        color = vec4(vec3(abs(sparkle)), 1.0f);
    else
        color = vec4(tex.xyz * light, 1.0f);
}
