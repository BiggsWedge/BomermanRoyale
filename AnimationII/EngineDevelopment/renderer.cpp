#include "renderer.h"

#ifdef FSGD_END_USE_D3D
#include "d3d11_renderer_impl.h"
#endif

namespace end
{
	renderer::renderer(native_handle_type window_handle)
	{

		p_impl = new impl(window_handle, default_view);//create and initialize the implementation

	}

	renderer::~renderer()
	{
		// Clean up implementation
		delete(p_impl);
	}

	void renderer::draw()
	{
		// draw views...
		p_impl->draw_view(default_view, totalTimePassed);
		// draw views...
		// draw views...
	}


	float colorLerp(float source, float dest, float ratio)
	{
		float end;
		end = (dest - source) * ratio + source;
		return end;
	}

	void renderer::nextKeyFrame()
	{
		p_impl->currKeyFrame++;
	}

	void renderer::update(float lastFrameTime, float2 mouseMovement, float2 movingTarget, float2 cameraMovement)
	{
		totalTimePassed += lastFrameTime;
		p_impl->frameTime = lastFrameTime;
		if (GetKeyState(VK_RCONTROL) & 1)
		{
			default_view._viewMatrix = MouseLook(default_view._viewMatrix, mouseMovement);
			default_view._viewMatrix = DirectX::XMMatrixTranslation(cameraMovement.x, 0.0f, cameraMovement.y) * default_view._viewMatrix;

		}
		
		if (toggleRenderMode)
		{
			p_impl->toggleFillMode();
			toggleRenderMode = false;
		}

	}

	void renderer::toggleManualInput()
	{
		p_impl->manualInput = !p_impl->manualInput;
	}


	float findRatio(float min, float max, float val)
	{
		float ret;
		ret = max - min;
		ret = val / ret;
		return ret;
	}
}