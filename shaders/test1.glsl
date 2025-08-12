#version 450

layout(set = 0, binding = 0) buffer InputOutputBuffer {
    float data[];
} bufferData;

layout(set = 0, binding = 1) buffer FirstTempBuffer {
    float data[];
} tempBuffer;

layout(local_size_x = 1) in;
void main() {
    uint idx = gl_GlobalInvocationID.x;
    bufferData.data[idx] *= 5;

    tempBuffer.data[0] = 5;
    tempBuffer.data[1] = 4;
    tempBuffer.data[2] = 3;
    tempBuffer.data[3] = 2;
    tempBuffer.data[4] = 1;

}
