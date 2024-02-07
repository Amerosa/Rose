#pragma once
#include <GL/glew.h>

#include <string>
#include <fstream>
#include <iterator>
#include <iostream>

class Shader {
public:
	
	GLuint ID;
	Shader(const std::string& filePath, GLenum shaderType); 

	void compile() { glCompileShader(ID); }

private:

	void checkCompile();
	std::string sourceFile2String(const std::string& filePath) const;

};

Shader::Shader(const std::string& filePath, GLenum shaderType) {
	ID = glCreateShader(shaderType);
	std::string source = sourceFile2String(filePath);
	auto compatibleString = source.c_str();
	glShaderSource(ID, 1, &compatibleString, nullptr);
	glCompileShader(ID);
	checkCompile();
}


std::string Shader::sourceFile2String(const std::string& filePath) const {
	std::ifstream ifs(filePath);
	std::string content( (std::istreambuf_iterator<char>(ifs)),
						 (std::istreambuf_iterator<char>()   ) );
	return content;
}

void Shader::checkCompile() {
	GLint compiled;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei logLength;
		GLchar  message[1024];
		glGetShaderInfoLog(ID, 1024, &logLength, message);
		std::cout << "vertex shader no compile!\n" << message << "\n";
	}
}