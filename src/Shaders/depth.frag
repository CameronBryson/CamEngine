
#version 460 core
void main()
{
    // Explicitly write depth
    gl_FragDepth = gl_FragCoord.z;
    
    // We don't need color output since we disabled color mask
}
