#pragma once
#include <memory>
#include "view.h"

#define FSGD_END_USE_D3D

namespace end
{
	// APIs/Platforms have their own types for representing a handle to a 'window'
	// They will generally all fit in the 8-bytes of a void*.
	// HWND is actually just a typedef/alias for a void*.
	using native_handle_type = void*;

	// Interface to the renderer
	class renderer
	{
	public:

		renderer(native_handle_type window_handle);

		~renderer();

		void update(float lastFrameTime, float2 mouseMovement, float2 movingTarget, float2 cameraMovement);

		void draw();

		view_t default_view;

		bool toggleRenderMode = false;

		void nextKeyFrame();
		void toggleManualInput();
	private:

		float totalTimePassed;
		// PImpl idiom ("Pointer to implementation")

		// 'impl' will define the implementation for the renderer elsewhere
		struct impl;

		// Pointer to the implementation
		impl* p_impl;
	};

	// The following types just defines scopes for enum values.
	// The enum values can be used as indices in arrays.
	// These enum values can be added to as needed.

	struct VIEWPORT {
		enum { DEFAULT = 0, COUNT };
	};

	struct CONSTANT_BUFFER {
		enum { MVP = 0, DEBUG_RENDERER, LIGHTS, MATERIAL, JOINTS, COUNT };
	};

	struct VERTEX_SHADER {
		enum { COLORED_VERTEX = 0, DEBUG_RENDERER, MENUS, COUNT };
	};

	struct PIXEL_SHADER {
		enum { COLORED_VERTEX = 0, DEBUG_RENDERER, MENUS, COUNT };
	};

	struct VIEW_RENDER_TARGET {
		enum { DEFAULT = 0, COUNT };
	};

	struct INDEX_BUFFER {
		enum { DEFAULT = 0, COUNT };
	};

	struct INPUT_LAYOUT {
		enum { DEBUG_RENDERER = 0, REG_SHADER, COUNT };
	};

	struct STATE_RASTERIZER {
		enum { DEFAULT = 0, COUNT };
	};

	struct VIEW_DEPTH_STENCIL {
		enum { DEFAULT = 0, COUNT };
	};

	struct STATE_DEPTH_STENCIL {
		enum { DEFAULT = 0, COUNT };
	};

	struct VERTEX_BUFFER {
		enum { DEBUG_RENDERER = 0, MODEL, MENUS, COUNT };
	};

	/* Add more as needed...
	enum STATE_SAMPLER{ DEFAULT = 0, COUNT };

	enum STATE_BLEND{ DEFAULT = 0, COUNT };
	*/

	float colorLerp(float source, float dest, float ratio);

	float findRatio(float min, float max, float val);

}