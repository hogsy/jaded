// Created by Mark "hogsy" Sowden, 2025 <hogsy@oldtimes-software.com>
// https://oldtimes-software.com/jaded/

#include <GL/glew.h>

#include "../MainSharedApp.h"
#include "../FileSystem/FileSystem.h"

#include "ShaderManager.h"

///////////////////////////////////////////////////////////////////
// Shader Program Stage
///////////////////////////////////////////////////////////////////

jaded::renderer::GLShaderProgram::Stage::Stage( GLenum type, const std::string &path ) : type_( type ), path_( path )
{
	id_ = glCreateShader( type );
	if ( id_ == 0 )
	{
		throw std::runtime_error( "failed to create shader stage" );
	}
}

jaded::renderer::GLShaderProgram::Stage::~Stage()
{
	if ( id_ != 0 )
	{
		glDeleteShader( id_ );
	}
}

bool jaded::renderer::GLShaderProgram::Stage::Reload()
{
	time_t time = filesystem.GetLocalFileTimestamp( path_ );
	if ( time == ( time_t ) -1 )
	{
		throw std::runtime_error( "failed to get timestamp for " + path_ );
	}

	return Compile();
}

bool jaded::renderer::GLShaderProgram::Stage::Compile()
{
	const char *src = source_.c_str();
	glShaderSource( id_, 1, &src, nullptr );
	glCompileShader( id_ );

	// check if it succeeded or not
	GLint status;
	glGetShaderiv( id_, GL_COMPILE_STATUS, &status );
	if ( !status )
	{
		GLint logLength;
		glGetShaderiv( id_, GL_INFO_LOG_LENGTH, &logLength );
		if ( logLength <= 0 )
		{
			printf( "Unknown compile error!\n" );
			return false;
		}

		std::string log;
		log.resize( logLength );

		glGetShaderInfoLog( id_, logLength, nullptr, &log[ 0 ] );

		printf( "Shader compilation error (%s): %s\n", path_.c_str(), log.c_str() );

		return false;
	}

	return true;
}

bool jaded::renderer::GLShaderProgram::Stage::LoadAndCompile()
{
	return false;
}

///////////////////////////////////////////////////////////////////
// Shader Program
///////////////////////////////////////////////////////////////////

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

void jaded::renderer::GLShaderProgram::Reload()
{
	bool reload = false;
	for ( auto &i : stages_ )
	{
		std::string path = i.path_;
		if ( path.empty() )
		{
			continue;
		}

		time_t time = filesystem.GetLocalFileTimestamp( path );
		if ( time == ( time_t ) -1 )
		{
			// maybe this should be an assert or something?
			// as it shouldn't really fail...
			continue;
		}

		if ( time != i.lastUpdateTime_ )
		{
			reload = true;
			break;
		}
	}

	if ( reload )
	{
		for ( auto &stage : stages_ )
		{
			glDetachShader( id_, stage.id_ );
		}

		//TODO: do reload

		for ( auto &stage : stages_ )
		{
			glAttachShader( id_, stage.id_ );
		}

		if ( !Link() )
		{
			return;
		}

		PopulateUniforms();
		PopulateAttributes();
	}
}

void jaded::renderer::GLShaderProgram::MakeActive()
{
	glUseProgram( id_ );
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

int jaded::renderer::GLShaderProgram::GetAttribute( const std::string &name )
{
	auto &i = attributes_.find( name );
	if ( i == attributes_.end() )
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
		int logLength;
		glGetProgramiv( id_, GL_INFO_LOG_LENGTH, &logLength );
		if ( logLength <= 0 )
		{
			printf( "Unknown link error!\n" );
			return false;
		}

		std::string log;
		log.resize( logLength );

		glGetShaderInfoLog( id_, logLength, nullptr, &log[ 0 ] );

		printf( "Shader link error: %s\n", log.c_str() );

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

	GLint numUniforms;
	glGetProgramiv( id_, GL_ACTIVE_UNIFORMS, &numUniforms );
	if ( numUniforms > 0 )
	{
		printf( "Found %d uniforms in shader\n", numUniforms );

		for ( unsigned int i = 0; i < ( unsigned int ) numUniforms; ++i )
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

	GLint numAttributes;
	glGetProgramiv( id_, GL_ACTIVE_ATTRIBUTES, &numAttributes );
	if ( numAttributes > 0 )
	{
		printf( "Found %d attributes in shader\n", numAttributes );

		// not sure why we can't do this by attribute as we can for uniforms?
		int maxNameLength;
		glGetProgramiv( id_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength );

		std::vector< GLchar > attributeName( maxNameLength );

		for ( int i = 0; i < numAttributes; ++i )
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
	//TODO: replace with something better, this is temp!!!
	hotReloadTicks_++;
	if ( hotReloadTicks_ < HOT_RELOAD_TIMER )
	{
		return;
	}

	for ( auto &i : programs_ )
	{
		i.second->Reload();
	}

	hotReloadTicks_ = 0;
}

void jaded::renderer::GLShaderManager::SetupDefaults()
{
}

jaded::renderer::GLShaderProgram *jaded::renderer::GLShaderManager::CacheProgram( const std::string &vert, const std::string &frag )
{
	//TODO: need to handle the following...
	//	- automatically prefix #version to input
	//	- handle #include ourselves (and discard so driver ignores)

	static constexpr const char BASE_PATH[] = "shaders/glsl/";

	return nullptr;
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

void jaded::renderer::GLShaderManager::SetProgram( const std::string &name )
{
	// an empty name indicates we should clear the active program
	// (this API kinda sucks but I just want to get this crap done)
	if ( name.empty() )
	{
		SetProgram( nullptr );
		return;
	}

	GLShaderProgram *program = GetProgram( name );
	if ( program == nullptr )
	{
		printf( "Failed to set shader program \"%s\"!\n", name.c_str() );
		return;
	}

	SetProgram( program );
}

void jaded::renderer::GLShaderManager::SetProgram( GLShaderProgram *program )
{
	if ( program == activeProgram_ )
	{
		return;
	}

	if ( program != nullptr )
	{
		program->MakeActive();
	}
	else
	{
		glUseProgram( 0 );
	}

	activeProgram_ = program;
}

bool jaded::renderer::GLShaderManager::Initialize()
{
	struct Shader
	{
		const char *name;
		const char *vert;
		const char *frag;
	};

	static constexpr Shader shaders[] = {
	        { "default", "default.frag.glsl", "default.vert.glsl" },
	};

	for ( const auto &i : shaders )
	{
		GLShaderProgram *program = CacheProgram( i.vert, i.frag );
		if ( program == nullptr )
		{
			printf( "Failed to load shader program (%s)!\n", i.name );
			return false;
		}

		programs_.emplace( i.name, program );
	}

	return true;
}
