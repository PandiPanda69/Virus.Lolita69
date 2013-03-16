#ifndef __ANTIVIR_DESCRIPTOR_TRACER_H__
#define __ANTIVIR_DESCRIPTOR_TRACER_H__

#include <map>
#include <exception>

#include "tracer.h"

namespace antivirus
{
	class DescriptorTracerException: public std::exception
	{
		public:
			DescriptorTracerException(const std::string& msg) throw() : _msg(msg) { }
			~DescriptorTracerException() throw() { }

			const char* what() const throw() { return _msg.c_str(); }
		private:
			std::string _msg;
	};

	/**
	* Maintains a descriptor table for each process
	*/
	class DescriptorTracer
	{
		public:
			static void initialize_descriptor_tracer();

			static std::string& get_file_by_desc(pid_t pid, int fd) throw(DescriptorTracerException);
			
		private:
			static std::map<pid_t, std::map<int, std::string> > _descriptors_mapping;

			static bool _open_handler(pid_t pid, struct user_regs_struct& regs);
			static bool _close_handler(pid_t pid, struct user_regs_struct& regs);
	};
}

#endif
