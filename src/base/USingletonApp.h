#pragma once

#include <memory>

#include <boost/locale.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>


namespace USingletonApp
{
	namespace bIproc = boost::interprocess;

	class ApplicationSingleton
	{
	public:

		ApplicationSingleton(std::string appName);
		~ApplicationSingleton();

	private:

		static constexpr const char* SM_NAME = "sm";
		static constexpr const char* ID_NAME = "id";

		std::string name;

		bIproc::managed_shared_memory sm;
		std::unique_ptr<bIproc::named_mutex> mtx;
		std::unique_ptr<bIproc::scoped_lock<bIproc::named_mutex>> lock;
	};
}