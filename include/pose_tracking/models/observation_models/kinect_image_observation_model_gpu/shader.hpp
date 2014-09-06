#ifndef POSE_TRACKING_MODELS_OBSERVATION_MODELS_SHADER_HPP
#define POSE_TRACKING_MODELS_OBSERVATION_MODELS_SHADER_HPP

#include <vector>
#include <string>
#include <GL/glew.h>

GLuint LoadShaders(std::vector<const char *> shaderFilePaths);
GLuint CreateShader(GLenum eShaderType, const char * strShaderFile);
GLuint CreateProgram(const std::vector<GLuint> &shaderList);

#endif
