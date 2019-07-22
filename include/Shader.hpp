#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {
public:
	GLuint id_;
	GLuint vert_, frag_;

	Shader();

	void Compile(const GLchar *vert_src, const GLchar * frag_src, const GLchar * gmt_src);

    int LoadFromSource(const GLchar *vert_src, const GLchar *frag_src);
	int LoadVertexFromSource(const GLchar *vert_src);
    int LoadFragmentFromSource(const GLchar *frag_src);
    int Link();
    int LinkComp(int n, const GLchar * vars[], GLenum mode);
    Shader & Use();
    // Utility functions
    void SetFloat    (const GLchar *name, GLfloat value, GLboolean useShader = false);
    void SetInteger  (const GLchar *name, GLint value, GLboolean useShader = false);
    void SetVector2f (const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
    void SetVector2f (const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
    void SetVector3f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
    void SetVector3f (const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
    void SetVector4f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
    void SetVector4f (const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
    void SetMatrix4  (const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);
    void SetMatrix3  (const GLchar *name, const glm::mat3 &matrix, GLboolean useShader = false);

	void CheckCompileErrors(GLuint id, std::string type);
};

#endif
