// Created by Mark "hogsy" Sowden, 2025 <hogsy@oldtimes-software.com>
// https://oldtimes-software.com/jaded/

#include <GL/glew.h>

#include "../MainSharedApp.h"

#include "LINks/LINKmsg.h"

#include "ShaderManager.h"

///////////////////////////////////////////////////////////////////
// Shader Program
///////////////////////////////////////////////////////////////////

jaded::renderer::GLShaderProgramStage::GLShaderProgramStage( GLenum type, const std::string &path ) : type_( type ), path_( path )
{
	id_ = glCreateShader( type );
	if ( id_ == 0 )
	{
		throw std::runtime_error( "failed to create shader stage" );
	}
}

jaded::renderer::GLShaderProgramStage::~GLShaderProgramStage()
{
}

jaded::renderer::GLShaderProgram::GLShaderProgram()
{
	id_ = glCreateProgram();
	if ( id_ == 0 )
	{
		throw std::runtime_error( "failed to create shader program" );
	}
}

jaded::renderer::GLShaderProgram::~GLShaderProgram()
{
	if ( id_ != 0 )
	{
		glDeleteProgram( id_ );
	}
}

int jaded::renderer::GLShaderProgram::GetUniform( const std::string &name )
{
	auto &i = uniforms_.find( name );
	if ( i == uniforms_.end() )
	{
		return -1;
	}

	return i->second.id;
}

bool jaded::renderer::GLShaderProgram::Link()
{
	glLinkProgram( id_ );

	int status;
	glGetProgramiv( id_, GL_LINK_STATUS, &status );
	if ( status == 0 )
	{
		int slength;
		glGetProgramiv( id_, GL_INFO_LOG_LENGTH, &slength );
		if ( slength <= 0 )
		{
			printf( "Unknown link error!\n" );
			return false;
		}

		char *buf = new char[ slength ];
		glGetProgramInfoLog( id_, slength, nullptr, buf );

		printf( "%s\n", buf );

		delete[] buf;

		return false;
	}

	// now fetch all uniforms and attributes

	PopulateUniforms();
	PopulateAttributes();

	return true;
}

void jaded::renderer::GLShaderProgram::PopulateUniforms()
{
	uniforms_.clear();

	int numUniforms;
	glGetProgramiv( id_, GL_ACTIVE_UNIFORMS, &numUniforms );
	if ( numUniforms > 0 )
	{
		printf( "Found %d uniforms in shader\n", numUniforms );

		for ( unsigned int i = 0; i < numUniforms; ++i )
		{
			GLsizei maxUniformNameLength;
			glGetActiveUniformsiv( id_, 1, &i, GL_UNIFORM_NAME_LENGTH, &maxUniformNameLength );

			char *uniformName = new char[ maxUniformNameLength ];

			GLenum  uniformType;
			GLsizei uniformNameLength;
			GLint   uniformSize;
			glGetActiveUniform( id_, i, maxUniformNameLength, &uniformNameLength, &uniformSize, &uniformType, uniformName );

			Uniform uniform = {};
			uniform.id      = glGetUniformLocation( id_, uniformName );
			uniform.type    = uniformType;
			uniform.name    = uniformName;

			delete[] uniformName;

			//TODO: this will absolutely explode with array types...

			switch ( uniform.type )
			{
				default:
					printf( "Unhandled uniform type (%s) (%d)!\n", uniform.name.c_str(), uniform.type );
					break;

				case GL_FLOAT:
					glGetUniformfv( id_, uniform.id, &uniform.defaultValue.f32 );
					break;
				case GL_FLOAT_VEC2:
					glGetUniformfv( id_, uniform.id, uniform.defaultValue.f32v2 );
					break;
				case GL_FLOAT_VEC3:
					glGetUniformfv( id_, uniform.id, uniform.defaultValue.f32v3 );
					break;
				case GL_FLOAT_VEC4:
					glGetUniformfv( id_, uniform.id, uniform.defaultValue.f32v4 );
					break;

				case GL_FLOAT_MAT3:
					glGetUniformfv( id_, uniform.id, ( GLfloat * ) uniform.defaultValue.f32m3 );
					break;
				case GL_FLOAT_MAT4:
					glGetUniformfv( id_, uniform.id, ( GLfloat * ) uniform.defaultValue.f32m4 );
					break;

				case GL_DOUBLE:
					glGetUniformdv( id_, uniform.id, &uniform.defaultValue.f64 );
					break;

				case GL_SAMPLER_1D:
				case GL_SAMPLER_2D:
				case GL_SAMPLER_3D:
				case GL_INT:
				case GL_UNSIGNED_INT:
				case GL_BOOL:
					glGetUniformiv( id_, uniform.id, &uniform.defaultValue.i32 );
					break;
			}

			uniforms_.emplace( uniform.name, uniform );
		}
	}
}

void jaded::renderer::GLShaderProgram::PopulateAttributes()
{
	attributes_.clear();

	int numAttributes;
	glGetProgramiv( id_, GL_ACTIVE_ATTRIBUTES, &numAttributes );
	if ( numAttributes > 0 )
	{
		printf( "Found %d attributes in shader\n", numAttributes );

		// not sure why we can't do this by attribute as we can for uniforms?
		int maxNameLength;
		glGetProgramiv( id_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength );

		std::vector< GLchar > attributeName( maxNameLength );

		for ( unsigned int i = 0; i < numAttributes; ++i )
		{
			attributeName.clear();

			GLenum  attributeType;
			GLint   attributeSize;
			GLsizei attributeNameLength;
			glGetActiveAttrib( id_, i, maxNameLength, &attributeNameLength, &attributeSize, &attributeType, attributeName.data() );

			Attribute attribute = {};
			attribute.id        = glGetAttribLocation( id_, attributeName.data() );
			attribute.type      = attributeType;
			attribute.name      = std::string( attributeName.data(), attributeNameLength );

			//TODO: this will absolutely explode with array types...

#if 0// not bothering for attributes for now...
			switch ( uniform.type )
			{
				default:
					printf( "Unhandled uniform type (%s) (%d)!\n", uniform.name.c_str(), uniform.type );
					break;

				case GL_FLOAT:
					glGetUniformfv( id_, uniform.id, &uniform.defaultValue.f32 );
					break;
				case GL_FLOAT_VEC2:
					glGetUniformfv( id_, uniform.id, uniform.defaultValue.f32v2 );
					break;
				case GL_FLOAT_VEC3:
					glGetUniformfv( id_, uniform.id, uniform.defaultValue.f32v3 );
					break;
				case GL_FLOAT_VEC4:
					glGetUniformfv( id_, uniform.id, uniform.defaultValue.f32v4 );
					break;

				case GL_FLOAT_MAT3:
					glGetUniformfv( id_, uniform.id, ( GLfloat * ) uniform.defaultValue.f32m3 );
					break;
				case GL_FLOAT_MAT4:
					glGetUniformfv( id_, uniform.id, ( GLfloat * ) uniform.defaultValue.f32m4 );
					break;

				case GL_DOUBLE:
					glGetUniformdv( id_, uniform.id, &uniform.defaultValue.f64 );
					break;

				case GL_SAMPLER_1D:
				case GL_SAMPLER_2D:
				case GL_SAMPLER_3D:
				case GL_INT:
				case GL_UNSIGNED_INT:
				case GL_BOOL:
					glGetUniformiv( id_, uniform.id, &uniform.defaultValue.i32 );
					break;
			}
#endif

			attributes_.emplace( attribute.name, attribute );
		}
	}
}

///////////////////////////////////////////////////////////////////
// Shader Manager
// Manages all of the loaded shaders
///////////////////////////////////////////////////////////////////

void jaded::renderer::GLShaderManager::HotReloadPrograms()
{
}

void jaded::renderer::GLShaderManager::SetupDefaults()
{
}

jaded::renderer::GLShaderProgram *jaded::renderer::GLShaderManager::GetProgram( const std::string &name )
{
	auto &i = programs_.find( name );
	if ( i == programs_.end() )
	{
		return nullptr;
	}

	return i->second;
}
