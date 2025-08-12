#version 450

layout(set = 0, binding = 0) buffer InputOutputBuffer {
    float data[];
} bufferData;

layout(local_size_x = 5) in;
void main() {
    uint idx = gl_GlobalInvocationID.x;
    bufferData.data[idx] += 2;
}
