// Created by Mark "hogsy" Sowden, 2025 <hogsy@oldtimes-software.com>
// https://oldtimes-software.com/jaded/

#pragma once

namespace jaded
{
	namespace renderer
	{
		// originally the plan here was to make this an interface, and then
		// build API-specific classes on top, but we'll consider this later...

		class GLShaderProgramStage
		{
			int id_{};

			GLenum type_{ 0 };

			std::string path_;

			GLShaderProgramStage( GLenum type, const std::string &path );
			~GLShaderProgramStage();
		};

		union GLShaderProgramValue
		{
			int32_t i32;
			int8_t  i8;

			float  f32;
			double f64;

			float f32v2[ 2 ];
			float f32v3[ 3 ];
			float f32v4[ 4 ];

			float f32m3[ 3 ][ 3 ];
			float f32m4[ 4 ][ 4 ];
		};

		class GLShaderProgram
		{
			int id_{};

			struct Uniform
			{
				int id{};

				GLenum type{};

				std::string name;

				GLShaderProgramValue defaultValue;
				GLShaderProgramValue value;
			};

			struct Attribute
			{
				int id{};

				GLenum type{};

				std::string name;
			};

			std::map< std::string, Uniform >   uniforms_;
			std::map< std::string, Attribute > attributes_;

		public:
			GLShaderProgram();
			~GLShaderProgram();

			void Reload();
			void MakeActive();

			int GetUniform( const std::string &name );
			int GetAttribute( const std::string &name );

		private:
			bool Link();

			void PopulateUniforms();
			void PopulateAttributes();

		public:
		};

		class GLShaderManager
		{
			static constexpr unsigned int HOT_RELOAD_TIMER = 60;

			bool         hotReload_{ false };
			unsigned int incHotReloadTicks_{ HOT_RELOAD_TIMER };
			unsigned int hotReloadTicks_{ HOT_RELOAD_TIMER };

			std::map< std::string, GLShaderProgram * > programs_;

		public:
			void HotReloadPrograms();

			void SetupDefaults();

			GLShaderProgram *GetProgram( const std::string &name );
		};
	}// namespace renderer
}// namespace jaded
