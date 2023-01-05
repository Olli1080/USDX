#include "USingletonApp.h"

#include <boost/interprocess/detail/os_thread_functions.hpp>
#include <boost/process.hpp>

namespace USingletonApp
{
	ApplicationSingleton::ApplicationSingleton(std::string appName)
		:
		name(std::move(appName)),
		sm(bIproc::open_or_create, SM_NAME, 1024), //TODO:: what does 1024 do??
		mtx(std::make_unique<bIproc::named_mutex>(bIproc::open_or_create, name.c_str()))
	{
		lock = std::make_unique<bIproc::scoped_lock<bIproc::named_mutex>>(*mtx, bIproc::try_to_lock);
		if (*lock)
		{
			auto id = bIproc::ipcdetail::get_current_process_id();
			auto idMem = sm.find_or_construct<bIproc::ipcdetail::OS_process_id_t>(ID_NAME)();
			*idMem = id;
		}
		else
		{
			auto id = bIproc::ipcdetail::get_current_process_id();
			auto idMem = sm.find<bIproc::ipcdetail::OS_process_id_t>(ID_NAME).first;

			bool renew = true;
			if (idMem != nullptr)
			{
				try
				{
					boost::process::child c{ *idMem };
					if (c.valid())
					{
						renew = false;
						throw std::exception("Instance already running...");
					}
				}
				catch (const boost::process::process_error& e)
				{}
			}
			else
				idMem = sm.construct<bIproc::ipcdetail::OS_process_id_t>(ID_NAME)();

			if (renew)
			{
				lock.reset();
				mtx.reset();

				bIproc::named_mutex::remove(name.c_str());
				*idMem = id;

				mtx = std::make_unique<bIproc::named_mutex>(bIproc::create_only, name.c_str());
				lock = std::make_unique<bIproc::scoped_lock<bIproc::named_mutex>>(*mtx, bIproc::try_to_lock);

				if (!*lock)
					throw std::exception("Instance already running...");
			}
		}
	}

	ApplicationSingleton::~ApplicationSingleton()
	{
		lock.reset();
		mtx.reset();

		bIproc::named_mutex::remove(name.c_str());
		bIproc::shared_memory_object::remove(SM_NAME);
	}
}