// File created for Jaded, the community patched Jade engine
#pragma once

#include <functional>
#include <list>

// fuck you Windows!!! >:(
#undef ERROR

namespace core
{
	class Logger
	{
	public:
		struct Channel
		{
			enum Type : unsigned char
			{
				INFO,
				WARN,
				ERROR,
				VERBOSE,

				MAX
			};

		private:
			bool isActive{};

			friend Logger;
		};

	private:
		struct Message
		{
			Message( const std::string &_string, time_t _timestamp, Channel::Type _type ) : string( _string ), timestamp( _timestamp ), type( _type ) {}

			std::string   string;
			time_t        timestamp{};
			Channel::Type type;
		};

		static constexpr unsigned int MAX_MESSAGES = 4096;
		std::list< Message >          messages;

		typedef std::function< void( Channel::Type, bool, const char * ) > Callback;
		std::vector< Callback >                                      callbacks;

		Channel channels[ Channel::Type::MAX ];

		FILE *out{};

		/////////////////////////////////////////////////////////////////////////////////////

		Logger()
		{
			channels[ Channel::Type::INFO ].isActive  = true;
			channels[ Channel::Type::WARN ].isActive  = true;
			channels[ Channel::Type::ERROR ].isActive = true;
#ifndef NDEBUG
			channels[ Channel::Type::VERBOSE ].isActive = true;
#else
			channels[ Channel::Type::VERBOSE ].isActive = false;
#endif
		}

	public:
		static Logger &GetInstance()
		{
			static Logger instance;
			return instance;
		}

	private:
		static void WriteMessage( FILE *file, const std::string &msg, time_t timestamp );

	public:
		/**
		 * Pushes a new message into the logger.
		 * Mind that even if a channel is disabled, the message will still be logged in-case
		 * it's enabled for display later.
		 */
		void PushMessage( Channel::Type type, bool notify, const char *msg, ... );

		/**
		 * Provides an interface for messages to be piped elsewhere, mostly for compatibility
		 * sake with Jade's existing mess.
		 */
		void AddCallback( const Callback &callback );

		/**
		 * Sets an output file to write to as new messages are pumped.
		 */
		bool SetOutputFile( const std::string &path );

		/**
		 * Outputs all current messages to the specified file.
		 */
		bool DumpOutput( const std::string &path ) const;
	};
}// namespace core

#define CORE_LOGGER_INFO( ... )  core::Logger::GetInstance().PushMessage( core::Logger::Channel::Type::INFO, false, __VA_ARGS__ )
#define CORE_LOGGER_WARN( ... )  core::Logger::GetInstance().PushMessage( core::Logger::Channel::Type::WARN, false, __VA_ARGS__ )
#define CORE_LOGGER_ERROR( ... ) core::Logger::GetInstance().PushMessage( core::Logger::Channel::Type::ERROR, false, __VA_ARGS__ )

#define CORE_LOGGER_NOTIF_INFO( ... )  core::Logger::GetInstance().PushMessage( core::Logger::Channel::Type::INFO, true, __VA_ARGS__ )
#define CORE_LOGGER_NOTIF_WARN( ... )  core::Logger::GetInstance().PushMessage( core::Logger::Channel::Type::WARN, true, __VA_ARGS__ )
#define CORE_LOGGER_NOTIF_ERROR( ... ) core::Logger::GetInstance().PushMessage( core::Logger::Channel::Type::ERROR, true, __VA_ARGS__ )
