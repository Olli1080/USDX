#pragma once

#include <memory>
#include <SDL2/SDL_surface.h>

namespace USDX
{
	struct SDL_Surface_Deleter
	{
		void operator()(SDL_Surface* surface) const
		{
			SDL_FreeSurface(surface);
		}
	};

	typedef std::unique_ptr<SDL_Surface, SDL_Surface_Deleter> SDL_Surface_UPtr;
	typedef std::shared_ptr<SDL_Surface> SDL_Surface_SPtr;
	typedef std::weak_ptr<SDL_Surface> SDL_Surface_WPtr;

	inline SDL_Surface_UPtr make_unique(SDL_Surface* surface)
	{
		return { surface, SDL_Surface_Deleter() };
	}

	inline SDL_Surface_SPtr make_shared(SDL_Surface* surface)
	{
		return { surface, SDL_Surface_Deleter() };
	}

	template<typename T>
	struct SurfaceLocker
	{
		SurfaceLocker operator=(const SurfaceLocker&) = delete;
	};

	template<>
	struct SurfaceLocker<SDL_Surface_SPtr>
	{
		SurfaceLocker(SDL_Surface_SPtr surface) : ptr(surface)
		{
			if (!ptr)
				return;

			if (SDL_MUSTLOCK(ptr))
				SDL_LockSurface(ptr.get());
		}

		~SurfaceLocker()
		{
			if (!ptr)
				return;

			if (SDL_MUSTLOCK(ptr))
				SDL_UnlockSurface(ptr.get());
		}

		SDL_Surface_SPtr ptr;
	};
}