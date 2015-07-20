attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec3 a_normal;
varying float v_fogFactor;              //weight for fog
varying vec2 v_texture_coord;

uniform float u_fogDensity;// For exp and exp2 equation
uniform float u_fogStart; // This is only for linear fog
uniform float u_fogEnd; // This is only for linear fog
uniform int u_fogEquation; // 0 = linear, 1 = exp, 2 = exp2


varying vec3 v_normal;

void main(void)
{
    vec4 ePosition = CC_MVMatrix * a_position;
    v_normal = CC_NormalMatrix * a_normal;
    
    v_texture_coord = a_texCoord;
    v_texture_coord.y = 1.0 - v_texture_coord.y;
    gl_Position = CC_PMatrix * ePosition;
    
    float fogFragCoord = abs(gl_Position.z);                                       //get fog distance
    
    
    if(u_fogEquation == 0)
        v_fogFactor = (u_fogEnd-fogFragCoord )/(u_fogEnd-u_fogStart);                //linear fog
    else if(u_fogEquation == 1)
        v_fogFactor = exp(-u_fogDensity*fogFragCoord  );                         //exp fog
    else if(u_fogEquation == 2)
        v_fogFactor = exp(-pow(u_fogDensity*fogFragCoord , 2.0));                //exp2 fog
    
    
    v_fogFactor = clamp(v_fogFactor, 0.0, 1.0);                                     //clamp 0 to 1
}
